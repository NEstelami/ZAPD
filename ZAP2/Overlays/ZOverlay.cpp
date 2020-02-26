#include "ZOverlay.h"
#include "../File.h"
#include "../Path.h"
#include "../Directory.h"
#include "../StringHelper.h"

using namespace std;
using namespace ELFIO;

ZOverlay::ZOverlay(string nName)
{
	name = nName;
	entries = vector<RelocationEntry*>();
}

ZOverlay* ZOverlay::FromELF(string elfFilePath, string cfgFolderPath)
{
	/*string basePath = Path::GetDirectoryName(elfFilePath);

	if (basePath == "")
		basePath = Directory::GetCurrentDirectory();*/

	string cfgText = File::ReadAllText(cfgFolderPath + "/overlay.cfg");
	vector<string> cfgLines = StringHelper::Split(cfgText, "\n");

	ZOverlay* ovl = new ZOverlay(StringHelper::Strip(cfgLines[0], "\r"));
	elfio reader;

	if (!reader.load(elfFilePath))
		return NULL;
	

	Elf_Half sec_num = reader.sections.size();

	vector<string> sections = {".rel.text", ".rel.data" , ".rel.rodata"};

	for (auto sectionSearched : sections)
	{
		for (int i = 0; i < sec_num; ++i)
		{
			section* pSec = reader.sections[i];

			string sectionName = pSec->get_name();

			if (pSec->get_type() == SHT_REL && sectionName == sectionSearched)
			{
				relocation_section_accessor* symbols = new relocation_section_accessor(reader, pSec);

				for (Elf_Xword j = 0; j < symbols->get_entries_num(); j++)
				{
					std::string name;
					Elf64_Addr offset;
					Elf64_Addr symbolValue;
					Elf_Xword size;
					Elf_Sxword addend;
					Elf_Sxword calcValue;
					Elf_Word type;

					symbols->get_entry(j, offset, symbolValue, name, type, addend, calcValue);

					RelocationType typeConverted = (RelocationType)type;
					SectionType sectionType = GetSectionTypeFromStr(sectionName);

					if (name == ".text" || name == ".data" || name == ".bss" || name == ".rodata")
					{
						RelocationEntry* reloc = new RelocationEntry(sectionType, typeConverted, offset);
						ovl->entries.push_back(reloc);
					}
				}
				break;
			}
		}
	}

	return ovl;
}

string ZOverlay::GetSourceOutputCode(std::string prefix)
{
	string output = "";
	char buffer[4096];

	output += ".section .ovl\n";

	sprintf(buffer, ".word _%sSegmentTextSize\n", name.c_str());
	output += buffer;
	sprintf(buffer, ".word _%sSegmentDataSize\n", name.c_str());
	output += buffer;
	sprintf(buffer, ".word _%sSegmentRoDataSize\n", name.c_str());
	output += buffer;
	sprintf(buffer, ".word _%sSegmentBssSize\n", name.c_str());
	output += buffer;

	sprintf(buffer, ".word %i\n", entries.size());
	output += buffer;

	for (int i = 0; i < entries.size(); i++)
	{
		RelocationEntry* reloc = entries[i];

		sprintf(buffer, ".word 0x%08X\n", reloc->CalcRelocationWord());
		output += buffer;
	}

	int offset = (entries.size() * 4) + 20;

	while (offset % 16 != 12)
	{
		output += ".word 0\n";
		offset += 4;
	}

	sprintf(buffer, ".word 0x%08X\n", offset + 4);
	output += buffer;

	return output;
}

SectionType ZOverlay::GetSectionTypeFromStr(string sectionName)
{
	if (sectionName == ".rel.text")
		return SectionType::Text;
	else if (sectionName == ".rel.data")
		return SectionType::Data;
	else if (sectionName == ".rel.rodata")
		return SectionType::RoData;
	else if (sectionName == ".rel.bss")
		return SectionType::Bss;
}