#include "Globals.h"
#include <Utils/File.h>
#include <Utils/Path.h>
#include <algorithm>
#include "tinyxml2.h"

using namespace tinyxml2;

Globals* Globals::Instance;

Globals::Globals()
{
	Instance = this;

	files = std::vector<ZFile*>();
	segments = std::vector<int32_t>();
	symbolMap = std::map<uint32_t, std::string>();
	segmentRefs = std::map<int32_t, std::string>();
	segmentRefFiles = std::map<int32_t, ZFile*>();
	game = ZGame::OOT_RETAIL;
	genSourceFile = true;
	testMode = false;
	profile = false;
	useLegacyZDList = false;
	useExternalResources = true;
	lastScene = nullptr;
	verbosity = VerbosityLevel::VERBOSITY_SILENT;
	currentExporter = "";
	outputPath = Directory::GetCurrentDirectory();
}

std::string Globals::FindSymbolSegRef(int32_t segNumber, uint32_t symbolAddress)
{
	if (segmentRefs.find(segNumber) != segmentRefs.end())
	{
		if (segmentRefFiles.find(segNumber) == segmentRefFiles.end())
		{
			XMLDocument doc;
			std::string filePath = segmentRefs[segNumber];
			XMLError eResult = doc.LoadFile(filePath.c_str());

			if (eResult != tinyxml2::XML_SUCCESS)
				return "ERROR";

			tinyxml2::XMLNode* root = doc.FirstChild();

			if (root == nullptr)
				return "ERROR";

			for (XMLElement* child = root->FirstChildElement(); child != nullptr;
			     child = child->NextSiblingElement())
			{
				if (std::string(child->Name()) == "File")
				{
					ZFile* file = new ZFile(fileMode, child, "", "", filePath, true);
					file->GeneratePlaceholderDeclarations();
					segmentRefFiles[segNumber] = file;
					break;
				}
			}
		}

		return segmentRefFiles[segNumber]->GetDeclarationName(symbolAddress, "ERROR");
	}

	return "ERROR";
}

enum class ConfigType
{
	SYMBOL_MAP,
	SEGMENT,
	ACTOR_LIST,
	OBJECT_LIST,
	TEXTURE_POOL,
	BG_CONFIG
};

const static std::map<std::string, ConfigType> ConfigTypeDictionary = {
	{"SymbolMap", ConfigType::SYMBOL_MAP},     {"Segment", ConfigType::SEGMENT},
	{"ActorList", ConfigType::ACTOR_LIST},     {"ObjectList", ConfigType::OBJECT_LIST},
	{"TexturePool", ConfigType::TEXTURE_POOL}, {"BGConfig", ConfigType::BG_CONFIG},
};

void Globals::ReadConfigFile(const std::string& configFilePath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError eResult = doc.LoadFile(configFilePath.c_str());

	if (eResult != tinyxml2::XML_SUCCESS)
	{
		throw std::runtime_error("Error: Unable to read config file.");
		return;
	}

	tinyxml2::XMLNode* root = doc.FirstChild();

	if (root == nullptr)
		return;

	for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != nullptr;
	     child = child->NextSiblingElement())
	{
		auto it = ConfigTypeDictionary.find(child->Name());
		if (it != ConfigTypeDictionary.end())
		{
			switch (it->second)
			{
				case ConfigType::SYMBOL_MAP:
				{
					std::string fileName = child->Attribute("File");
					GenSymbolMap(Path::GetDirectoryName(configFilePath) + "/" + fileName);
				}
				break;

				case ConfigType::SEGMENT:
				{
					std::string fileName = child->Attribute("File");
					int32_t segNumber = child->IntAttribute("Number");
					segmentRefs[segNumber] = fileName;
				}
				break;

				case ConfigType::ACTOR_LIST:
				{
					std::string fileName = child->Attribute("File");
					std::vector<std::string> lines =
						File::ReadAllLines(Path::GetDirectoryName(configFilePath) + "/" + fileName);

					for (std::string line : lines)
						cfg.actorList.push_back(line);
				}
				break;

				case ConfigType::OBJECT_LIST:
				{
					std::string fileName = child->Attribute("File");
					std::vector<std::string> lines =
						File::ReadAllLines(Path::GetDirectoryName(configFilePath) + "/" + fileName);

					for (std::string line : lines)
						cfg.objectList.push_back(line);
				}
				break;

				case ConfigType::TEXTURE_POOL:
				{
					std::string fileName = child->Attribute("File");
					ReadTexturePool(Path::GetDirectoryName(configFilePath) + "/" + fileName);
				}
				break;

				case ConfigType::BG_CONFIG:
					cfg.bgScreenWidth = child->IntAttribute("ScreenWidth", 320);
					cfg.bgScreenHeight = child->IntAttribute("ScreenHeight", 240);
					break;
			}
		}
		else
		{
			fprintf(stderr, "Unsupported configuration variable: %s", child->Name());
		}
	}
}

void Globals::ReadTexturePool(const std::string& texturePoolXmlPath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError eResult = doc.LoadFile(texturePoolXmlPath.c_str());

	if (eResult != tinyxml2::XML_SUCCESS)
	{
		fprintf(stderr, "Warning: Unable to read texture pool XML with error code %i\n", eResult);
		return;
	}

	tinyxml2::XMLNode* root = doc.FirstChild();

	if (root == nullptr)
		return;

	for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != nullptr;
	     child = child->NextSiblingElement())
	{
		if (std::string(child->Name()) == "Texture")
		{
			std::string crcStr = std::string(child->Attribute("CRC"));
			fs::path texPath = std::string(child->Attribute("Path"));
			std::string texName = "";

			uint32_t crc = strtoul(crcStr.c_str(), nullptr, 16);

			cfg.texturePool[crc].path = texPath;
		}
	}
}

void Globals::GenSymbolMap(const std::string& symbolMapPath)
{
	auto symbolLines = File::ReadAllLines(symbolMapPath);

	for (std::string symbolLine : symbolLines)
	{
		auto split = StringHelper::Split(symbolLine, " ");
		uint32_t addr = strtoul(split[0].c_str(), nullptr, 16);
		std::string symbolName = split[1];

		symbolMap[addr] = symbolName;
	}
}

void Globals::AddSegment(int32_t segment, ZFile* file)
{
	if (std::find(segments.begin(), segments.end(), segment) == segments.end())
		segments.push_back(segment);

	segmentRefs[segment] = file->GetXmlFilePath().string();
	segmentRefFiles[segment] = file;
}

bool Globals::HasSegment(int32_t segment)
{
	return std::find(segments.begin(), segments.end(), segment) != segments.end();
}

std::map<std::string, ExporterSet*>* Globals::GetExporterMap()
{
	static std::map<std::string, ExporterSet*> exporters;
	return &exporters;
}

void Globals::AddExporter(std::string exporterName, ExporterSet* exporterSet)
{
	auto exporters = GetExporterMap();
	(*exporters)[exporterName] = exporterSet;
}

ZResourceExporter* Globals::GetExporter(ZResourceType resType)
{
	auto exporters = *GetExporterMap();

	if (currentExporter != "" && exporters[currentExporter]->exporters.find(resType) !=
	                                 exporters[currentExporter]->exporters.end())
		return exporters[currentExporter]->exporters[resType];
	else
		return nullptr;
}

ExporterSet* Globals::GetExporterSet()
{
	auto exporters = *GetExporterMap();

	if (currentExporter != "")
		return exporters[currentExporter];
	else
		return nullptr;
}