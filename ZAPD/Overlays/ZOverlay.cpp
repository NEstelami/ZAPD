#include "ZOverlay.h"
#include <Utils/Directory.h>
#include <Utils/File.h>
#include <Utils/Path.h>
#include <Utils/StringHelper.h>

using namespace ELFIO;

ZOverlay::ZOverlay()
{
	name = "";
	entries = std::vector<RelocationEntry*>();
}

ZOverlay::ZOverlay(std::string nName) : ZOverlay()
{
	name = nName;
}

ZOverlay::~ZOverlay()
{
	for (auto entry : entries)
		if (entry)
			delete entry;
	entries.clear();
}

ZOverlay* ZOverlay::FromBuild(std::string buildPath, std::string cfgFolderPath)
{
	std::string cfgText = File::ReadAllText(cfgFolderPath + "/overlay.cfg");
	std::vector<std::string> cfgLines = StringHelper::Split(cfgText, "\n");

	ZOverlay* ovl = new ZOverlay(cfgLines[0]);

	std::vector<std::string> relSections = {".rel.text", ".rel.data", ".rel.rodata"};
	std::vector<std::string> sections = {".text", ".data", ".rodata"};

	int32_t sectionOffs[5] = {0};
	std::vector<RelocationEntry*> textRelocs;
	std::vector<RelocationEntry*> dataRelocs;
	std::vector<RelocationEntry*> rodataRelocs;

	// get the elf files
	std::vector<elfio*> readers;
	for (size_t i = 1; i < cfgLines.size(); i++)
	{
		std::string elfPath =
			buildPath + "/" + cfgLines[i].substr(0, cfgLines[i].size() - 2) + ".o";
		elfio* reader = new elfio();

		if (!reader->load(elfPath))
		{
			// not all files were compiled
			for (auto r : readers)
				delete r;
			readers.clear();

			delete ovl;
			return nullptr;
		}

		readers.push_back(reader);
	}

	for (auto curReader : readers)
	{
		Elf_Half sec_num = curReader->sections.size();
		for (int32_t i = 0; i < sec_num; i++)
		{
			section* pSec = curReader->sections[i];

			if (pSec->get_type() == SHT_REL && std::find(relSections.begin(), relSections.end(),
			                                             pSec->get_name()) != relSections.end())
			{
				SectionType sectionType = GetSectionTypeFromStr(pSec->get_name());

				if (sectionType == SectionType::ERROR)
					fprintf(stderr, "WARNING: One of the section types returned ERROR\n");

				relocation_section_accessor relocs(*curReader, pSec);
				for (Elf_Xword j = 0; j < relocs.get_entries_num(); j++)
				{
					Elf64_Addr offset = 0;
					Elf_Word symbol = 0;
					Elf_Word type = 0;
					{
						Elf_Sxword addend = 0;
						relocs.get_entry(j, offset, symbol, type, addend);
					}

					std::string curSymName;
					Elf_Half curSymShndx = SHN_UNDEF;
					{
						symbol_section_accessor symbols(
							*curReader, curReader->sections[(Elf_Half)pSec->get_link()]);
						Elf64_Addr value;
						Elf_Xword size;
						unsigned char bind;
						unsigned char type;
						unsigned char other;
						symbols.get_symbol(symbol, curSymName, value, size, bind, type, curSymShndx,
						                   other);
					}

					// check symbols outside the elf but within the overlay
					if (curSymShndx == SHN_UNDEF)
					{
						for (auto reader : readers)
						{
							if (curSymShndx != SHN_UNDEF)
								break;

							if (reader == curReader)
								continue;

							auto sectionData = reader->sections[(Elf_Half)pSec->get_link()];

							if (sectionData == nullptr)
								continue;

							symbol_section_accessor symbols(*reader, sectionData);

							for (Elf_Xword symIdx = 0; symIdx < symbols.get_symbols_num(); symIdx++)
							{
								Elf_Half shndx = SHN_UNDEF;
								Elf64_Addr value;
								std::string name;
								Elf_Xword size;
								unsigned char bind;
								unsigned char type;
								unsigned char other;

								symbols.get_symbol(symIdx, name, value, size, bind, type, shndx,
								                   other);

								if (name == curSymName)
								{
									curSymShndx = shndx;
									break;
								}
							}
						}
					}

					if (curSymShndx != SHN_UNDEF)
					{
						RelocationType typeConverted = (RelocationType)type;
						offset += sectionOffs[sectionType];

						RelocationEntry* reloc =
							new RelocationEntry(sectionType, typeConverted, offset);

						// this is to keep the correct reloc entry order
						if (sectionType == SectionType::Text)
							textRelocs.push_back(reloc);
						if (sectionType == SectionType::Data)
							dataRelocs.push_back(reloc);
						if (sectionType == SectionType::RoData)
							rodataRelocs.push_back(reloc);
					}
				}
			}
		}

		// increase section offsets
		for (int32_t i = 0; i < sec_num; i++)
		{
			section* pSec = curReader->sections[i];
			if (pSec->get_type() == SHT_PROGBITS &&
			    std::find(sections.begin(), sections.end(), pSec->get_name()) != sections.end())
			{
				SectionType sectionType = GetSectionTypeFromStr(pSec->get_name());
				sectionOffs[sectionType] += pSec->get_size();
			}
		}
	}

	for (auto reloc : textRelocs)
		ovl->entries.push_back(reloc);
	for (auto reloc : dataRelocs)
		ovl->entries.push_back(reloc);
	for (auto reloc : rodataRelocs)
		ovl->entries.push_back(reloc);

	for (auto r : readers)
		delete r;
	readers.clear();

	return ovl;
}

std::string ZOverlay::GetSourceOutputCode(const std::string& prefix)
{
	std::string output = "";

	output += ".section .ovl\n";

	output += StringHelper::Sprintf(".word _%sSegmentTextSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentDataSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentRoDataSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentBssSize\n", name.c_str());

	output += StringHelper::Sprintf(".word %i\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		RelocationEntry* reloc = entries[i];
		output += StringHelper::Sprintf(".word 0x%08X\n", reloc->CalcRelocationWord());
	}

	size_t offset = (entries.size() * 4) + 20;

	while (offset % 16 != 12)
	{
		output += ".word 0\n";
		offset += 4;
	}

	output += StringHelper::Sprintf(".word 0x%08X\n", offset + 4);
	return output;
}

SectionType ZOverlay::GetSectionTypeFromStr(std::string sectionName)
{
	const static std::map<std::string, SectionType> SectionTypeDictionary = {
		{".text", SectionType::Text},
		{".rel.text", SectionType::Text},
		{".data", SectionType::Data},
		{".rel.data", SectionType::Data},
		{".rodata", SectionType::RoData},
		{".rel.rodata", SectionType::RoData},
		{".rodata.str1.4", SectionType::RoData},
		{".rodata.cst4", SectionType::RoData},
		{".bss", SectionType::Bss},
		{".rel.bss", SectionType::Bss},
	};

	auto it = SectionTypeDictionary.find(sectionName);
	if (it != SectionTypeDictionary.end())
	{
		return it->second;
	}
	return SectionType::ERROR;
}
