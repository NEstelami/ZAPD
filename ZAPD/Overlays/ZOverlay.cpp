#include "ZOverlay.h"

#include <assert.h>

#include <Utils/Directory.h>
#include <Utils/File.h>
#include <Utils/Path.h>
#include <Utils/StringHelper.h>
#include "Globals.h"

using namespace ELFIO;


const char* RelocationEntry::GetSectionName() const
{
	switch (sectionType)
	{
	case SectionType::Text:
		return ".text";
	case SectionType::Data:
		return ".data";
	case SectionType::RoData:
		return ".rodata";
	case SectionType::Bss:
		return ".bss";
	case SectionType::ERROR:
		return ".ERROR";
	default:
		assert(!"Oh no :c");
	}
}

const char* RelocationEntry::GetRelocTypeName() const
{
	switch (relocationType)
	{
	case RelocationType::R_MIPS_32:
		return "R_MIPS_32";
	case RelocationType::R_MIPS_26:
		return "R_MIPS_26";
	case RelocationType::R_MIPS_HI16:
		return "R_MIPS_HI16";
	case RelocationType::R_MIPS_LO16:
		return "R_MIPS_LO16";
	}
}


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

	ZOverlay* ovl = new ZOverlay(StringHelper::Strip(cfgLines[0], "\r"));

	std::vector<std::string> relSections = {".rel.text", ".rel.data", ".rel.rodata"};
	std::vector<std::string> sections = { ".text", ".data", ".rodata", ".rodata.str1.4", ".rodata.cst4" };

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

			if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
			{
				printf("Section: %s \n", pSec->get_name().c_str());
			}

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

					if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
					{
						RelocationEntry reloc(sectionType, (RelocationType)type, offset);
						printf(".word 0x%08X  # %s %s 0x%04X\n", reloc.CalcRelocationWord(), reloc.GetSectionName(), reloc.GetRelocTypeName(), reloc.offset);
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

							Elf_Half sec_num = reader->sections.size();
							for (int32_t j = 0; j < sec_num; j++)
							{
								if (curSymShndx != SHN_UNDEF)
								{
									break;
								}

								auto sectionData = reader->sections[j];

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
					}

					if (curSymShndx != SHN_UNDEF)
					{
						RelocationType typeConverted = (RelocationType)type;
						offset += sectionOffs[static_cast<size_t>(sectionType)];

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
				sectionOffs[static_cast<size_t>(sectionType)] += pSec->get_size();
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

	output += StringHelper::Sprintf("# %sOverlayInfo\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentTextSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentDataSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentRoDataSize\n", name.c_str());
	output += StringHelper::Sprintf(".word _%sSegmentBssSize\n", name.c_str());

	output += "\n";
	output += StringHelper::Sprintf(".word %i # reloc_count\n", entries.size());
	output += "\n";

	for (size_t i = 0; i < entries.size(); i++)
	{
		RelocationEntry* reloc = entries[i];
		output += StringHelper::Sprintf(".word 0x%08X  # %s %s 0x%04X\n", reloc->CalcRelocationWord(), reloc->GetSectionName(), reloc->GetRelocTypeName(), reloc->offset);
	}

	size_t offset = (entries.size() * 4) + 20;

	while (offset % 16 != 12)
	{
		output += ".word 0\n";
		offset += 4;
	}

	output += "\n";
	output += StringHelper::Sprintf(".word 0x%08X # %sOverlayInfoOffset\n", offset + 4);
	return output;
}

SectionType ZOverlay::GetSectionTypeFromStr(std::string sectionName)
{
	if (sectionName == ".rel.text" || sectionName == ".text")
		return SectionType::Text;
	else if (sectionName == ".rel.data" || sectionName == ".data")
		return SectionType::Data;
	else if (sectionName == ".rel.rodata" || sectionName == ".rodata" ||
	         sectionName == ".rodata.str1.4" || sectionName == ".rodata.cst4")
		return SectionType::RoData;
	else if (sectionName == ".rel.bss" || sectionName == ".bss")
		return SectionType::Bss;

	return SectionType::ERROR;
}
