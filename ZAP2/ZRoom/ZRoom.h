#pragma once

#include "../ZResource.h"
#include "ZRoomCommand.h"
#include "ZTexture.h"
#include "../tinyxml2.h"

#include <vector>
#include <map>
#include <string>

class ZRoom : public ZResource
{
protected:
	std::vector<ZRoomCommand*> commands;

	std::string GetSourceOutputHeader(std::string prefix);
	std::string GetSourceOutputCode(std::string prefix);
	void ProcessCommandSets();
	void SyotesRoomHack();

public:
	ZRoom* scene;
	std::map<int32_t, Declaration*> declarations;
	std::map<int32_t, std::string> externs;
	std::map<int32_t, ZTexture*> textures;
	std::vector<CommandSet> commandSets;

	std::string extDefines;

	ZRoom(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZRoom* nScene);

	void ParseCommands(std::vector<ZRoomCommand*>& commandList, CommandSet commandSet);
	size_t GetDeclarationSizeFromNeighbor(int declarationAddress);
	size_t GetCommandSizeFromNeighbor(ZRoomCommand* cmd);
	ZRoomCommand* FindCommandOfType(RoomCommand cmdType);
	std::vector<uint8_t> GetRawData();
	int GetRawDataSize();
};

struct CommandSet
{
	int32_t address;
	int32_t commandCount; // Only used if explicitly specified in the XML

	CommandSet(int32_t nAddress);
	CommandSet(int32_t nAddress, int32_t nCommandCount);
};