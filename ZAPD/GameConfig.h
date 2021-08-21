#pragma once

#include <algorithm>
#include <map>
// #include "Globals.h"
#include "Utils/File.h"
#include "Utils/Path.h"
#include "tinyxml2.h"
#include "ZFile.h"

struct TexturePoolEntry
{
	fs::path path = "";  // Path to Shared Texture
};

class GameConfig;

typedef void (GameConfig::*ConfigFunc)(const tinyxml2::XMLElement&, const std::string&);

class GameConfig
{
public:
	std::map<int32_t, std::string> segmentRefs;
	std::map<int32_t, ZFile*> segmentRefFiles;
	std::map<uint32_t, std::string> symbolMap;
	std::vector<std::string> actorList;
	std::vector<std::string> objectList;
	std::map<uint32_t, TexturePoolEntry> texturePool;  // Key = CRC

	// ZBackground
	uint32_t bgScreenWidth = 320, bgScreenHeight = 240;

	GameConfig();// = default;

	void ReadTexturePool(const std::string& texturePoolXmlPath);
	void GenSymbolMap(const std::string& symbolMapPath);

    void ConfigFunc_SymbolMap(const tinyxml2::XMLElement& element, const std::string& configFilePath);
    void ConfigFunc_Segment(const tinyxml2::XMLElement& element, const std::string& configFilePath);
    void ConfigFunc_ActorList(const tinyxml2::XMLElement& element, const std::string& configFilePath);
    void ConfigFunc_ObjectList(const tinyxml2::XMLElement& element, const std::string& configFilePath);
    void ConfigFunc_TexturePool(const tinyxml2::XMLElement& element, const std::string& configFilePath);
    void ConfigFunc_BGConfig(const tinyxml2::XMLElement& element, const std::string& configFilePath);

    void ReadConfigFile(const std::string& configFilePath);
};
