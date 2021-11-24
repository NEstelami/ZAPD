#include "ZFile.h"

#include <algorithm>
#include <cassert>
#include <string_view>
#include <unordered_set>

#include "Globals.h"
#include "OutputFormatter.h"
#include "Utils/BinaryWriter.h"
#include "Utils/Directory.h"
#include "Utils/File.h"
#include "Utils/MemoryStream.h"
#include "Utils/Path.h"
#include "Utils/StringHelper.h"
#include "ZAnimation.h"
#include "ZArray.h"
#include "ZBackground.h"
#include "ZBlob.h"
#include "ZCollision.h"
#include "ZCutscene.h"
#include "ZDisplayList.h"
#include "ZLimb.h"
#include "ZMtx.h"
#include "ZRoom/ZRoom.h"
#include "ZScalar.h"
#include "ZSkeleton.h"
#include "ZSymbol.h"
#include "ZTexture.h"
#include "ZVector.h"
#include "ZVtx.h"

ZFile::ZFile()
{
	resources = std::vector<ZResource*>();
	basePath = "";
	declarations = std::map<uint32_t, Declaration*>();
	defines = "";
	baseAddress = 0;
	rangeStart = 0x000000000;
	rangeEnd = 0xFFFFFFFF;
}

ZFile::ZFile(const fs::path& nOutPath, const std::string& nName) : ZFile()
{
	name = nName;
	outName = nName;
	outputPath = nOutPath;
}

ZFile::ZFile(ZFileMode nMode, tinyxml2::XMLElement* reader, const fs::path& nBasePath,
             const fs::path& nOutPath, const std::string& filename, const fs::path& nXmlFilePath)
	: ZFile()
{
	xmlFilePath = nXmlFilePath;
	if (nBasePath == "")
		basePath = Directory::GetCurrentDirectory();
	else
		basePath = nBasePath;

	if (nOutPath == "")
		outputPath = Directory::GetCurrentDirectory();
	else
		outputPath = nOutPath;

	mode = nMode;

	ParseXML(reader, filename);
	if (mode != ZFileMode::ExternalFile)
		DeclareResourceSubReferences();
}

ZFile::~ZFile()
{
	for (ZResource* res : resources)
	{
		delete res;
	}

	for (auto d : declarations)
	{
		delete d.second;
	}

	for (auto sym : symbolResources)
	{
		delete sym.second;
	}
}

void ZFile::ParseXML(tinyxml2::XMLElement* reader, const std::string& filename)
{
	assert(mode != ZFileMode::Invalid);

	if (filename == "")
		name = reader->Attribute("Name");
	else
		name = filename;

	outName = name;
	const char* outNameXml = reader->Attribute("OutName");
	if (outNameXml != nullptr)
		outName = outNameXml;

	// TODO: This should be a variable on the ZFile, but it is a large change in order to force all
	// ZResource types to have a parent ZFile.
	const char* gameStr = reader->Attribute("Game");
	if (reader->Attribute("Game") != nullptr)
	{
		if (std::string_view(gameStr) == "MM")
			Globals::Instance->game = ZGame::MM_RETAIL;
		else if (std::string_view(gameStr) == "SW97" || std::string_view(gameStr) == "OOTSW97")
			Globals::Instance->game = ZGame::OOT_SW97;
		else if (std::string_view(gameStr) == "OOT")
			Globals::Instance->game = ZGame::OOT_RETAIL;
		else
			throw std::runtime_error(
				StringHelper::Sprintf("Error: Game type %s not supported.", gameStr));
	}

	if (reader->Attribute("BaseAddress") != nullptr)
		baseAddress = StringHelper::StrToL(reader->Attribute("BaseAddress"), 16);

	if (reader->Attribute("RangeStart") != nullptr)
		rangeStart = StringHelper::StrToL(reader->Attribute("RangeStart"), 16);

	if (reader->Attribute("RangeEnd") != nullptr)
		rangeEnd = StringHelper::StrToL(reader->Attribute("RangeEnd"), 16);

	if (rangeStart > rangeEnd)
		throw std::runtime_error("Error: RangeStart must be before than RangeEnd.");

	const char* segmentXml = reader->Attribute("Segment");
	if (segmentXml != nullptr)
	{
		if (!StringHelper::HasOnlyDigits(segmentXml))
		{
			throw std::runtime_error(StringHelper::Sprintf(
				"error: Invalid segment value '%s': must be a decimal between 0 and 15 inclusive",
				segmentXml));
		}

		segment = StringHelper::StrToL(segmentXml, 10);
		if (segment > 15)
		{
			if (segment == 128)
			{
#ifdef DEPRECATION_ON
				fprintf(stderr, "warning: segment 128 is deprecated.\n\tRemove "
				                "'Segment=\"128\"' from the xml to use virtual addresses\n");
#endif
			}
			else
			{
				throw std::runtime_error(
					StringHelper::Sprintf("error: invalid segment value '%s': must be a decimal "
				                          "number between 0 and 15 inclusive",
				                          segmentXml));
			}
		}
	}
	Globals::Instance->AddSegment(segment, this);

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
	{
		if (segment == 0x80)
		{
			printf("File '%s' using virtual addresses.\n", GetName().c_str());
		}
		else
		{
			printf("File '%s' using segment %X.\n", GetName().c_str(), segment);
		}
	}

	if (mode == ZFileMode::Extract || mode == ZFileMode::ExternalFile)
	{
		if (!File::Exists((basePath / name).string()))
			throw std::runtime_error(
				StringHelper::Sprintf("Error! File %s does not exist.", (basePath / name).c_str()));

		rawData = File::ReadAllBytes((basePath / name).string());

		/*
		 * TODO: In OoT repo ovl_Boss_Sst has a wrong RangeEnd (0xAD40 instead of 0xAD70),
		 * so uncommenting the following produces wrong behavior.
		 * If somebody fixes that in OoT repo, uncomment this. I'm too tired of fixing XMLs.
		 */
		// if (reader->Attribute("RangeEnd") == nullptr)
		// rangeEnd = rawData.size();
	}

	std::unordered_set<std::string> nameSet;
	std::unordered_set<std::string> outNameSet;
	std::unordered_set<std::string> offsetSet;

	auto nodeMap = *GetNodeMap();
	uint32_t rawDataIndex = 0;

	for (tinyxml2::XMLElement* child = reader->FirstChildElement(); child != nullptr;
	     child = child->NextSiblingElement())
	{
		const char* nameXml = child->Attribute("Name");
		const char* outNameXml = child->Attribute("OutName");
		const char* offsetXml = child->Attribute("Offset");

		// Check for repeated attributes.
		if (offsetXml != nullptr)
		{
			rawDataIndex = strtol(StringHelper::Split(offsetXml, "0x")[1].c_str(), NULL, 16);

			if (offsetSet.find(offsetXml) != offsetSet.end())
			{
				throw std::runtime_error(StringHelper::Sprintf(
					"ZFile::ParseXML: Error in '%s'.\n\t Repeated 'Offset' attribute: %s \n",
					name.c_str(), offsetXml));
			}
			offsetSet.insert(offsetXml);
		}
		else if (Globals::Instance->warnNoOffset)
		{
			fprintf(stderr, "Warning No offset specified for: %s", nameXml);
		}
		else if (Globals::Instance->errorNoOffset)
		{
			throw std::runtime_error(
				StringHelper::Sprintf("Error no offset specified for %s", nameXml));
		}

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
			printf("%s: 0x%06X\n", nameXml, rawDataIndex);

		if (outNameXml != nullptr)
		{
			if (outNameSet.find(outNameXml) != outNameSet.end())
			{
				throw std::runtime_error(StringHelper::Sprintf(
					"ZFile::ParseXML: Error in '%s'.\n\t Repeated 'OutName' attribute: %s \n",
					name.c_str(), outNameXml));
			}
			outNameSet.insert(outNameXml);
		}
		if (nameXml != nullptr)
		{
			if (nameSet.find(nameXml) != nameSet.end())
			{
				throw std::runtime_error(StringHelper::Sprintf(
					"ZFile::ParseXML: Error in '%s'.\n\t Repeated 'Name' attribute: %s \n",
					name.c_str(), nameXml));
			}
			nameSet.insert(nameXml);
		}

		std::string nodeName = std::string(child->Name());

		if (nodeMap.find(nodeName) != nodeMap.end())
		{
			ZResource* nRes = nodeMap[nodeName](this);

			if (mode == ZFileMode::Extract || mode == ZFileMode::ExternalFile)
				nRes->ExtractFromXML(child, rawDataIndex);

			switch (nRes->GetResourceType())
			{
			case ZResourceType::Texture:
				AddTextureResource(rawDataIndex, static_cast<ZTexture*>(nRes));
				break;

			case ZResourceType::Symbol:
				AddSymbolResource(rawDataIndex, static_cast<ZSymbol*>(nRes));
				break;

			default:
				AddResource(nRes);
				break;
			}

			rawDataIndex += nRes->GetRawDataSize();
		}
		else if (std::string_view(child->Name()) == "File")
		{
			throw std::runtime_error(StringHelper::Sprintf(
				"ZFile::ParseXML: Error in '%s'.\n\t Can't declare a File inside a File.\n",
				name.c_str()));
		}
		else
		{
			throw std::runtime_error(
				StringHelper::Sprintf("ZFile::ParseXML: Error in '%s'.\n\t Unknown element found "
			                          "inside a File element: '%s'.\n",
			                          name.c_str(), nodeName.c_str()));
		}
	}
}

void ZFile::DeclareResourceSubReferences()
{
	for (size_t i = 0; i < resources.size(); i++)
	{
		resources.at(i)->DeclareReferences(name);
	}
}

void ZFile::BuildSourceFile()
{
	if (mode == ZFileMode::ExternalFile)
		return;

	if (!Directory::Exists(outputPath))
		Directory::CreateDirectory(outputPath.string());

	GenerateSourceFiles();
}

std::string ZFile::GetName() const
{
	return name;
}

ZFileMode ZFile::GetMode() const
{
	return mode;
}

const fs::path& ZFile::GetXmlFilePath() const
{
	return xmlFilePath;
}

const std::vector<uint8_t>& ZFile::GetRawData() const
{
	return rawData;
}

void ZFile::ExtractResources()
{
	if (mode == ZFileMode::ExternalFile)
		return;

	if (!Directory::Exists(outputPath))
		Directory::CreateDirectory(outputPath.string());

	if (!Directory::Exists(GetSourceOutputFolderPath()))
		Directory::CreateDirectory(GetSourceOutputFolderPath().string());

	for (size_t i = 0; i < resources.size(); i++)
		resources[i]->ParseRawDataLate();
	for (size_t i = 0; i < resources.size(); i++)
		resources[i]->DeclareReferencesLate(name);

	if (Globals::Instance->genSourceFile)
		GenerateSourceFiles();

	MemoryStream* memStream = new MemoryStream();
	BinaryWriter writer = BinaryWriter(memStream);

	ExporterSet* exporterSet = Globals::Instance->GetExporterSet();

	if (exporterSet != nullptr && exporterSet->beginFileFunc != nullptr)
		exporterSet->beginFileFunc(this);

	for (ZResource* res : resources)
	{
		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
			printf("Saving resource %s\n", res->GetName().c_str());

		res->Save(outputPath);

		// Check if we have an exporter "registered" for this resource type
		ZResourceExporter* exporter = Globals::Instance->GetExporter(res->GetResourceType());
		if (exporter != nullptr)
			exporter->Save(res, Globals::Instance->outputPath.string(), &writer);
	}

	if (memStream->GetLength() > 0)
	{
		File::WriteAllBytes(StringHelper::Sprintf("%s%s.bin",
		                                          Globals::Instance->outputPath.string().c_str(),
		                                          GetName().c_str()),
		                    memStream->ToVector());
	}

	writer.Close();

	if (exporterSet != nullptr && exporterSet->endFileFunc != nullptr)
		exporterSet->endFileFunc(this);
}

void ZFile::AddResource(ZResource* res)
{
	resources.push_back(res);
}

ZResource* ZFile::FindResource(uint32_t rawDataIndex)
{
	for (ZResource* res : resources)
	{
		if (res->GetRawDataIndex() == rawDataIndex)
			return res;
	}

	return nullptr;
}

std::vector<ZResource*> ZFile::GetResourcesOfType(ZResourceType resType)
{
	std::vector<ZResource*> resList;

	for (ZResource* res : resources)
	{
		if (res->GetResourceType() == resType)
			resList.push_back(res);
	}

	return resList;
}

Declaration* ZFile::AddDeclaration(offset_t address, DeclarationAlignment alignment, size_t size,
                                   const std::string& varType, const std::string& varName,
                                   const std::string& body)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl = new Declaration(address, alignment, size, varType, varName, false, body);
		declarations[address] = decl;
	}
	else
	{
		decl->alignment = alignment;
		decl->size = size;
		decl->varType = varType;
		decl->varName = varName;
		decl->text = body;
	}
	return decl;
}

Declaration* ZFile::AddDeclarationArray(offset_t address, DeclarationAlignment alignment,
                                        size_t size, const std::string& varType,
                                        const std::string& varName, size_t arrayItemCnt,
                                        const std::string& body)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl =
			new Declaration(address, alignment, size, varType, varName, true, arrayItemCnt, body);
		declarations[address] = decl;
	}
	else
	{
		if (decl->isPlaceholder)
			decl->varName = varName;
		decl->alignment = alignment;
		decl->size = size;
		decl->varType = varType;
		decl->isArray = true;
		decl->arrayItemCnt = arrayItemCnt;
		decl->text = body;
	}

	return decl;
}

Declaration* ZFile::AddDeclarationArray(offset_t address, DeclarationAlignment alignment,
                                        size_t size, const std::string& varType,
                                        const std::string& varName,
                                        const std::string& arrayItemCntStr, const std::string& body)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl = new Declaration(address, alignment, size, varType, varName, true, arrayItemCntStr,
		                       body);
		declarations[address] = decl;
	}
	else
	{
		decl->alignment = alignment;
		decl->size = size;
		decl->varType = varType;
		decl->varName = varName;
		decl->isArray = true;
		decl->arrayItemCntStr = arrayItemCntStr;
		decl->text = body;
	}
	return decl;
}

Declaration* ZFile::AddDeclarationPlaceholder(offset_t address, const std::string& varName)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl;
	if (declarations.find(address) == declarations.end())
	{
		decl = new Declaration(address, DeclarationAlignment::Align4, 0, "", varName, false, "");
		decl->isPlaceholder = true;
		declarations[address] = decl;
	}
	else
		decl = declarations[address];

	return decl;
}

Declaration* ZFile::AddDeclarationInclude(offset_t address, const std::string& includePath,
                                          size_t size, const std::string& varType,
                                          const std::string& varName)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl = new Declaration(address, includePath, size, varType, varName);
		declarations[address] = decl;
	}
	else
	{
		decl->includePath = includePath;
		decl->size = size;
		decl->varType = varType;
		decl->varName = varName;
	}
	return decl;
}

Declaration* ZFile::AddDeclarationIncludeArray(offset_t address, std::string& includePath,
                                               size_t size, const std::string& varType,
                                               const std::string& varName, size_t arrayItemCnt)
{
	bool validOffset = AddDeclarationChecks(address, varName);
	if (!validOffset)
		return nullptr;

	if (StringHelper::StartsWith(includePath, "assets/extracted/"))
		includePath = "assets/" + StringHelper::Split(includePath, "assets/extracted/")[1];
	if (StringHelper::StartsWith(includePath, "assets/custom/"))
		includePath = "assets/" + StringHelper::Split(includePath, "assets/custom/")[1];

	Declaration* decl = GetDeclaration(address);
	if (decl == nullptr)
	{
		decl = new Declaration(address, includePath, size, varType, varName);

		decl->isArray = true;
		decl->arrayItemCnt = arrayItemCnt;

		declarations[address] = decl;
	}
	else
	{
		decl->includePath = includePath;
		decl->varType = varType;
		decl->varName = varName;
		decl->size = size;
		decl->isArray = true;
		decl->arrayItemCnt = arrayItemCnt;
	}
	return decl;
}

bool ZFile::AddDeclarationChecks(uint32_t address, const std::string& varName)
{
	assert(GETSEGNUM(address) == 0);
	assert(varName != "");
#ifdef DEVELOPMENT
	if (address == 0x0000)
	{
		[[maybe_unused]] int32_t bp = 0;
	}
#endif

	if (!IsOffsetInFileRange(address))
	{
		fprintf(stderr,
		        "%s: Warning in %s\n"
		        "\t Tried to declare a variable outside of this file's range. Ignoring...\n"
		        "\t\t Variable's name: %s\n"
		        "\t\t Variable's offset: 0x%06X\n",
		        __PRETTY_FUNCTION__, name.c_str(), varName.c_str(), address);
		return false;
	}

	return true;
}

bool ZFile::GetDeclarationPtrName(segptr_t segAddress, const std::string& expectedType,
                                  std::string& declName) const
{
	if (segAddress == 0)
	{
		declName = "NULL";
		return true;
	}

	uint32_t offset = Seg2Filespace(segAddress, baseAddress);
	Declaration* decl = GetDeclaration(offset);
	if (GETSEGNUM(segAddress) != segment || decl == nullptr)
	{
		declName = StringHelper::Sprintf("0x%08X", segAddress);
		return false;
	}

	if (expectedType != "" && expectedType != "void*")
	{
		if (expectedType != decl->varType && "static " + expectedType != decl->varType)
		{
			declName = StringHelper::Sprintf("0x%08X", segAddress);
			return false;
		}
	}

	if (!decl->isArray)
		declName = "&" + decl->varName;
	else
		declName = decl->varName;
	return true;
}

bool ZFile::GetDeclarationArrayIndexedName(segptr_t segAddress, size_t elementSize,
                                           const std::string& expectedType,
                                           std::string& declName) const
{
	if (segAddress == 0)
	{
		declName = "NULL";
		return true;
	}

	uint32_t address = Seg2Filespace(segAddress, baseAddress);
	Declaration* decl = GetDeclarationRanged(address);
	if (GETSEGNUM(segAddress) != segment || decl == nullptr || !decl->isArray)
	{
		declName = StringHelper::Sprintf("0x%08X", segAddress);
		return false;
	}

	if (expectedType != "" && expectedType != "void*")
	{
		if (expectedType != decl->varType && "static " + expectedType != decl->varType)
		{
			declName = StringHelper::Sprintf("0x%08X", segAddress);
			return false;
		}
	}

	if (decl->address == address)
	{
		declName = decl->varName;
		return true;
	}

	if ((address - decl->address) % elementSize != 0 || !(address < decl->address + decl->size))
	{
		declName = StringHelper::Sprintf("0x%08X", segAddress);
		return false;
	}

	uint32_t index = (address - decl->address) / elementSize;
	declName = StringHelper::Sprintf("&%s[%u]", decl->varName.c_str(), index);
	return true;
}

Declaration* ZFile::GetDeclaration(uint32_t address) const
{
	if (declarations.find(address) != declarations.end())
		return declarations.at(address);

	return nullptr;
}

Declaration* ZFile::GetDeclarationRanged(uint32_t address) const
{
	for (const auto decl : declarations)
	{
		if (address >= decl.first && address < decl.first + decl.second->size)
			return decl.second;
	}

	return nullptr;
}

bool ZFile::HasDeclaration(uint32_t address)
{
	assert(GETSEGNUM(address) == 0);
	return declarations.find(address) != declarations.end();
}

void ZFile::GenerateSourceFiles()
{
	std::string sourceOutput;

	sourceOutput += "#include \"ultra64.h\"\n";
	sourceOutput += "#include \"z64.h\"\n";
	sourceOutput += "#include \"macros.h\"\n";
	sourceOutput += GetHeaderInclude();

	bool hasZRoom = false;
	for (const auto& res : resources)
	{
		ZResourceType resType = res->GetResourceType();
		if (resType == ZResourceType::Room || resType == ZResourceType::Scene ||
		    resType == ZResourceType::AltHeader)
		{
			hasZRoom = true;
			break;
		}
	}

	if (hasZRoom)
	{
		sourceOutput += GetZRoomHeaderInclude();
	}

	sourceOutput += GetExternalFileHeaderInclude();

	GeneratePlaceholderDeclarations();

	// Generate Code
	for (size_t i = 0; i < resources.size(); i++)
	{
		ZResource* res = resources.at(i);
		res->GetSourceOutputCode(name);
	}

	sourceOutput += ProcessDeclarations();

	fs::path outPath = GetSourceOutputFolderPath() / outName.stem().concat(".c");

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
		printf("Writing C file: %s\n", outPath.c_str());

	OutputFormatter formatter;
	formatter.Write(sourceOutput);

	File::WriteAllText(outPath, formatter.GetOutput());

	GenerateSourceHeaderFiles();
}

void ZFile::GenerateSourceHeaderFiles()
{
	OutputFormatter formatter;

	for (ZResource* res : resources)
	{
		std::string resSrc = res->GetSourceOutputHeader("");
		formatter.Write(resSrc);

		if (resSrc != "")
			formatter.Write("\n");
	}

	for (auto& sym : symbolResources)
	{
		formatter.Write(sym.second->GetSourceOutputHeader(""));
	}

	formatter.Write(ProcessExterns());

	fs::path headerFilename = GetSourceOutputFolderPath() / outName.stem().concat(".h");

	if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_INFO)
		printf("Writing H file: %s\n", headerFilename.c_str());

	File::WriteAllText(headerFilename, formatter.GetOutput());
}

std::string ZFile::GetHeaderInclude() const
{
	std::string headers = StringHelper::Sprintf("#include \"%s.h\"\n",
	                                            (outName.parent_path() / outName.stem()).c_str());

	return headers;
}

std::string ZFile::GetZRoomHeaderInclude() const
{
	std::string headers;
	headers += "#include \"segment_symbols.h\"\n";
	headers += "#include \"command_macros_base.h\"\n";
	headers += "#include \"z64cutscene_commands.h\"\n";
	headers += "#include \"variables.h\"\n";
	return headers;
}

std::string ZFile::GetExternalFileHeaderInclude() const
{
	std::string externalFilesIncludes = "";

	for (ZFile* externalFile : Globals::Instance->files)
	{
		if (externalFile != this)
		{
			fs::path outputFolderPath = externalFile->GetSourceOutputFolderPath();
			if (outputFolderPath == this->GetSourceOutputFolderPath())
			{
				outputFolderPath = externalFile->outName.stem();
			}
			else
			{
				outputFolderPath /= externalFile->outName.stem();
			}

			externalFilesIncludes +=
				StringHelper::Sprintf("#include \"%s.h\"\n", outputFolderPath.c_str());
		}
	}

	return externalFilesIncludes;
}

void ZFile::GeneratePlaceholderDeclarations()
{
	// Generate placeholder declarations
	for (ZResource* res : resources)
	{
		if (GetDeclaration(res->GetRawDataIndex()) != nullptr)
		{
			continue;
		}

		Declaration* decl = res->DeclareVar(GetName(), "");
		decl->staticConf = res->GetStaticConf();
		if (res->GetResourceType() == ZResourceType::Symbol)
		{
			decl->staticConf = StaticConfig::Off;
		}
	}
}

void ZFile::AddTextureResource(uint32_t offset, ZTexture* tex)
{
	for (auto res : resources)
		assert(res->GetRawDataIndex() != offset);

	resources.push_back(tex);
	texturesResources[offset] = tex;
}

ZTexture* ZFile::GetTextureResource(uint32_t offset) const
{
	auto tex = texturesResources.find(offset);
	if (tex != texturesResources.end())
		return tex->second;

	return nullptr;
}

void ZFile::AddSymbolResource(uint32_t offset, ZSymbol* sym)
{
	symbolResources[offset] = sym;
}

ZSymbol* ZFile::GetSymbolResource(uint32_t offset) const
{
	auto sym = symbolResources.find(offset);
	if (sym != symbolResources.end())
		return sym->second;

	return nullptr;
}

ZSymbol* ZFile::GetSymbolResourceRanged(uint32_t offset) const
{
	for (const auto decl : symbolResources)
	{
		if (offset >= decl.first && offset < decl.first + decl.second->GetRawDataSize())
			return decl.second;
	}

	return nullptr;
}

fs::path ZFile::GetSourceOutputFolderPath() const
{
	return outputPath / outName.parent_path();
}

bool ZFile::IsOffsetInFileRange(uint32_t offset) const
{
	if (!(offset < rawData.size()))
		return false;

	return rangeStart <= offset && offset < rangeEnd;
}
bool ZFile::IsSegmentedInFilespaceRange(segptr_t segAddress) const
{
	uint8_t segnum = GETSEGNUM(segAddress);
	uint32_t offset = Seg2Filespace(segAddress, baseAddress);

	if (segment != segnum)
		return false;

	return IsOffsetInFileRange(offset);
}

std::map<std::string, ZResourceFactoryFunc*>* ZFile::GetNodeMap()
{
	static std::map<std::string, ZResourceFactoryFunc*> nodeMap;
	return &nodeMap;
}

void ZFile::RegisterNode(std::string nodeName, ZResourceFactoryFunc* nodeFunc)
{
	std::map<std::string, ZResourceFactoryFunc*>* nodeMap = GetNodeMap();
	(*nodeMap)[nodeName] = nodeFunc;
}

std::string ZFile::ProcessDeclarations()
{
	std::string output;

	if (declarations.size() == 0)
		return output;

	defines += ProcessTextureIntersections(name);

	// Account for padding/alignment
	uint32_t lastAddr = 0;

	// printf("RANGE START: 0x%06X - RANGE END: 0x%06X\n", rangeStart, rangeEnd);

	// Optimization: See if there are any arrays side by side that can be merged...
	std::vector<std::pair<int32_t, Declaration*>> declarationKeys(declarations.begin(),
	                                                              declarations.end());

	std::pair<int32_t, Declaration*> lastItem = declarationKeys.at(0);

	for (size_t i = 1; i < declarationKeys.size(); i++)
	{
		std::pair<int32_t, Declaration*> curItem = declarationKeys[i];

		if (curItem.second->isArray && lastItem.second->isArray)
		{
			if (curItem.second->varType == lastItem.second->varType)
			{
				if (!curItem.second->declaredInXml && !lastItem.second->declaredInXml)
				{
					// TEST: For now just do Vtx declarations...
					if (lastItem.second->varType == "Vtx")
					{
						int32_t sizeDiff = curItem.first - (lastItem.first + lastItem.second->size);

						// Make sure there isn't an unaccounted inbetween these two
						if (sizeDiff == 0)
						{
							lastItem.second->size += curItem.second->size;
							lastItem.second->arrayItemCnt += curItem.second->arrayItemCnt;
							lastItem.second->text += "\n" + curItem.second->text;
							declarations.erase(curItem.first);
							declarationKeys.erase(declarationKeys.begin() + i);
							delete curItem.second;
							i--;
							continue;
						}
					}
				}
			}
		}

		lastItem = curItem;
	}

	for (std::pair<uint32_t, Declaration*> item : declarations)
		ProcessDeclarationText(item.second);

	for (std::pair<uint32_t, Declaration*> item : declarations)
	{
		while (item.second->size % 4 != 0)
			item.second->size++;

		if (lastAddr != 0)
		{
			if (item.second->alignment == DeclarationAlignment::Align16)
			{
				int32_t curPtr = lastAddr + declarations[lastAddr]->size;

				while (curPtr % 4 != 0)
				{
					declarations[lastAddr]->size++;
					curPtr++;
				}
			}
			else if (item.second->alignment == DeclarationAlignment::Align8)
			{
				size_t curPtr = lastAddr + declarations[lastAddr]->size;

				while (curPtr % 4 != 0)
				{
					declarations[lastAddr]->size++;
					curPtr++;
				}

				while (curPtr % 8 != 0)
				{
					char buffer[2048];

					sprintf(buffer, "u32 %s_align%02zX = 0;\n", name.c_str(), curPtr);
					item.second->preText = buffer + item.second->preText;

					declarations[lastAddr]->size += 4;
					curPtr += 4;
				}
			}
		}

		lastAddr = item.first;
	}

	HandleUnaccountedData();

	// Go through include declarations
	// First, handle the prototypes (static only for now)
	for (std::pair<uint32_t, Declaration*> item : declarations)
	{
		output += item.second->GetStaticForwardDeclarationStr();
	}

	output += "\n";

	// Next, output the actual declarations
	for (const auto& item : declarations)
	{
		if (!IsOffsetInFileRange(item.first))
			continue;

		if (item.second->includePath != "")
		{
			if (item.second->isExternal)
			{
				// HACK
				std::string extType;

				if (item.second->varType == "Gfx")
					extType = "dlist";
				else if (item.second->varType == "Vtx")
					extType = "vtx";

				auto filepath = outputPath / item.second->varName;
				File::WriteAllText(
					StringHelper::Sprintf("%s.%s.inc", filepath.c_str(), extType.c_str()),
					item.second->text);
			}

			output += item.second->GetExternalDeclarationStr();
		}
		else if (item.second->varType != "")
		{
			output += item.second->GetNormalDeclarationStr();
		}
	}

	output += "\n";

	return output;
}

void ZFile::ProcessDeclarationText(Declaration* decl)
{
	size_t refIndex = 0;

	if (!(decl->references.size() > 0))
		return;

	for (size_t i = 0; i < decl->text.size() - 1; i++)
	{
		char c = decl->text[i];
		char c2 = decl->text[i + 1];

		if (c == '@' && c2 == 'r')
		{
			std::string vtxName;
			Globals::Instance->GetSegmentedArrayIndexedName(decl->references[refIndex], 0x10, this,
			                                                "Vtx", vtxName);
			decl->text.replace(i, 2, vtxName);

			refIndex++;

			if (refIndex >= decl->references.size())
				break;
		}
	}
}

std::string ZFile::ProcessExterns()
{
	std::string output;

	for (const auto& item : declarations)
	{
		if (!IsOffsetInFileRange(item.first))
		{
			continue;
		}

		output += item.second->GetExternStr();
	}

	output += "\n";

	output += defines;

	return output;
}

std::string ZFile::ProcessTextureIntersections([[maybe_unused]] const std::string& prefix)
{
	if (texturesResources.empty())
		return "";

	std::string defines;
	std::vector<std::pair<uint32_t, ZTexture*>> texturesSorted(texturesResources.begin(),
	                                                           texturesResources.end());

	for (size_t i = 0; i < texturesSorted.size() - 1; i++)
	{
		uint32_t currentOffset = texturesSorted[i].first;
		uint32_t nextOffset = texturesSorted[i + 1].first;
		auto& currentTex = texturesResources.at(currentOffset);
		int texSize = currentTex->GetRawDataSize();

		if (currentTex->WasDeclaredInXml())
		{
			// We believe the user is right.
			continue;
		}

		if ((currentOffset + texSize) > nextOffset)
		{
			uint32_t offsetDiff = nextOffset - currentOffset;
			if (currentTex->isPalette)
			{
				// Shrink palette so it doesn't overlap
				currentTex->SetDimensions(offsetDiff / currentTex->GetPixelMultiplyer(), 1);
				declarations.at(currentOffset)->size = currentTex->GetRawDataSize();
				currentTex->DeclareVar(GetName(), "");
			}
			else
			{
				std::string texName;
				std::string texNextName;
				GetDeclarationPtrName(currentOffset, "", texName);

				Declaration* nextDecl = GetDeclaration(nextOffset);
				if (nextDecl == nullptr)
					texNextName = texturesResources.at(nextOffset)->GetName();
				else
					texNextName = nextDecl->varName;

				defines += StringHelper::Sprintf("#define %s ((u32)%s + 0x%06X)\n",
				                                 texNextName.c_str(), texName.c_str(), offsetDiff);

				delete declarations[nextOffset];
				declarations.erase(nextOffset);
				texturesResources.erase(nextOffset);
				texturesSorted.erase(texturesSorted.begin() + i + 1);

				i--;
			}
		}
	}

	return defines;
}

void ZFile::HandleUnaccountedData()
{
	uint32_t lastAddr = 0;
	uint32_t lastSize = 0;
	std::vector<uint32_t> declsAddresses;
	for (const auto& item : declarations)
	{
		declsAddresses.push_back(item.first);
	}

	bool breakLoop = false;
	for (uint32_t currentAddress : declsAddresses)
	{
		if (currentAddress >= rangeEnd)
		{
			breakLoop = true;
			break;
		}

		if (currentAddress < rangeStart)
		{
			lastAddr = currentAddress;
			continue;
		}

		breakLoop = HandleUnaccountedAddress(currentAddress, lastAddr, lastSize);
		if (breakLoop)
			break;

		lastAddr = currentAddress;
	}

	if (!breakLoop)
	{
		// TODO: change rawData.size() to rangeEnd
		// HandleUnaccountedAddress(rangeEnd, lastAddr, lastSize);
		HandleUnaccountedAddress(rawData.size(), lastAddr, lastSize);
	}
}

bool ZFile::HandleUnaccountedAddress(uint32_t currentAddress, uint32_t lastAddr, uint32_t& lastSize)
{
	if (currentAddress != lastAddr && declarations.find(lastAddr) != declarations.end())
	{
		Declaration* lastDecl = declarations.at(lastAddr);
		lastSize = lastDecl->size;

		if (lastAddr + lastSize > currentAddress)
		{
			Declaration* currentDecl = declarations.at(currentAddress);

			fprintf(stderr,
			        "WARNING: Intersection detected from 0x%06X:0x%06X (%s), conflicts with "
			        "0x%06X (%s)\n",
			        lastAddr, lastAddr + lastSize, lastDecl->varName.c_str(), currentAddress,
			        currentDecl->varName.c_str());
		}
	}

	uint32_t unaccountedAddress = lastAddr + lastSize;

	if (unaccountedAddress != currentAddress && lastAddr >= rangeStart &&
	    unaccountedAddress < rangeEnd)
	{
		int diff = currentAddress - unaccountedAddress;
		bool nonZeroUnaccounted = false;

		std::string src = "    ";

		if (currentAddress > rawData.size())
		{
			throw std::runtime_error(StringHelper::Sprintf(
				"ZFile::ProcessDeclarations(): Fatal error while processing XML '%s'.\n"
				"\t Offset '0x%X' is outside of the limits of file '%s', which has a size of "
				"'0x%X'.\n"
				"\t Aborting...",
				xmlFilePath.c_str(), currentAddress, name.c_str(), rawData.size()));
		}

		for (int i = 0; i < diff; i++)
		{
			uint8_t val = rawData.at(unaccountedAddress + i);
			src += StringHelper::Sprintf("0x%02X, ", val);
			if (val != 0x00)
			{
				nonZeroUnaccounted = true;
			}

			if (Globals::Instance->verboseUnaccounted)
			{
				if ((i % 4 == 3))
				{
					src += StringHelper::Sprintf(" // 0x%06X", unaccountedAddress + i - 3);
					if (i != (diff - 1))
					{
						src += "\n\t";
					}
				}
			}
			else
			{
				if ((i % 16 == 15) && (i != (diff - 1)))
					src += "\n    ";
			}
		}

		if (declarations.find(unaccountedAddress) == declarations.end() && diff > 0)
		{
			std::string unaccountedPrefix = "unaccounted";

			if (diff < 16 && !nonZeroUnaccounted)
			{
				unaccountedPrefix = "possiblePadding";

				// Strip unnecessary padding at the end of the file.
				if (unaccountedAddress + diff >= rawData.size())
					return true;
			}

			Declaration* decl = AddDeclarationArray(
				unaccountedAddress, DeclarationAlignment::Align4, diff, "u8",
				StringHelper::Sprintf("%s_%s_%06X", name.c_str(), unaccountedPrefix.c_str(),
			                          unaccountedAddress),
				diff, src);
			decl->isUnaccounted = true;

			if (Globals::Instance->warnUnaccounted)
			{
				if (nonZeroUnaccounted)
				{
					fprintf(stderr,
					        "Warning in file: %s (%s)\n"
					        "\t A non-zero unaccounted block was found at offset '0x%06X'.\n"
					        "\t Block size: '0x%X'.\n",
					        xmlFilePath.c_str(), name.c_str(), unaccountedAddress, diff);
				}
				else if (diff >= 16)
				{
					fprintf(stderr,
					        "Warning in file: %s (%s)\n"
					        "\t A big (size>=0x10) zero-only unaccounted block was found "
					        "at offset '0x%06X'.\n"
					        "\t Block size: '0x%X'.\n",
					        xmlFilePath.c_str(), name.c_str(), unaccountedAddress, diff);
				}
			}
		}
	}

	return false;
}
