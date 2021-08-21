#include "GameConfig.h"


GameConfig::GameConfig()
{
	symbolMap = std::map<uint32_t, std::string>();
	segmentRefs = std::map<int32_t, std::string>();
	segmentRefFiles = std::map<int32_t, ZFile*>();


}

void ReadTexturePool(GameConfig& cfg, const std::string& texturePoolXmlPath)
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

void GenSymbolMap(GameConfig& cfg, const std::string& symbolMapPath)
{
	auto symbolLines = File::ReadAllLines(symbolMapPath);

	for (std::string symbolLine : symbolLines)
	{
		auto split = StringHelper::Split(symbolLine, " ");
		uint32_t addr = strtoul(split[0].c_str(), nullptr, 16);
		std::string symbolName = split[1];

		cfg.symbolMap[addr] = symbolName;
	}
}

void ConfigFunc_SymbolMap(GameConfig& cfg, tinyxml2::XMLElement& element, const std::string& configFilePath)
{
	std::string fileName = element.Attribute("File");
	GenSymbolMap(cfg, Path::GetDirectoryName(configFilePath) + "/" + fileName);
}

void ConfigFunc_Segment(GameConfig& cfg, tinyxml2::XMLElement& element, const std::string& configFilePath)
{
	std::string fileName = element.Attribute("File");
	int32_t segNumber = element.IntAttribute("Number");
	cfg.segmentRefs[segNumber] = fileName;
}

void ConfigFunc_ActorList(GameConfig& cfg, tinyxml2::XMLElement& element, const std::string& configFilePath)
{
	std::string fileName = element.Attribute("File");
	std::vector<std::string> lines =
		File::ReadAllLines(Path::GetDirectoryName(configFilePath) + "/" + fileName);

	for (auto& line : lines)
		cfg.actorList.emplace_back(std::move(line));
}

void ConfigFunc_ObjectList(GameConfig& cfg, tinyxml2::XMLElement& element, const std::string& configFilePath)
{
	std::string fileName = element.Attribute("File");
	std::vector<std::string> lines =
		File::ReadAllLines(Path::GetDirectoryName(configFilePath) + "/" + fileName);

	for (auto& line : lines)
		cfg.objectList.emplace_back(std::move(line));
}

void ConfigFunc_TexturePool(GameConfig& cfg, tinyxml2::XMLElement& element, const std::string& configFilePath)
{
	std::string fileName = element.Attribute("File");
	ReadTexturePool(cfg, Path::GetDirectoryName(configFilePath) + "/" + fileName);
}

void ConfigFunc_BGConfig(GameConfig& cfg, tinyxml2::XMLElement& element, const std::string& configFilePath)
{
	cfg.bgScreenWidth =  element.IntAttribute("ScreenWidth", 320);
	cfg.bgScreenHeight = element.IntAttribute("ScreenHeight", 240);
}
