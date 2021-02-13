#pragma once

#include "../ZResource.h"
#include "ZRoomCommand.h"
#include "ZTexture.h"
#include <tinyxml2.h>

#include <vector>
#include <map>
#include <string>

class ZRoom : public ZResource
{
protected:
	std::vector<std::shared_ptr<ZRoomCommand>> commands;

	std::string GetSourceOutputHeader(const std::string& prefix);
	std::string GetSourceOutputCode(const std::string& prefix);
	void ProcessCommandSets();
	void SyotesRoomHack();

public:
	std::shared_ptr<ZRoom> scene;
	std::map<int32_t, std::shared_ptr<ZTexture>> textures;
	std::vector<CommandSet> commandSets;

	std::string extDefines;

	ZRoom();

	static std::shared_ptr<ZRoom> ExtractFromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* nParent, std::shared_ptr<ZRoom>& nScene);
	void ParseCommands(std::vector<std::shared_ptr<ZRoomCommand>>& commandList, CommandSet commandSet);
	size_t GetDeclarationSizeFromNeighbor(int declarationAddress);
	size_t GetCommandSizeFromNeighbor(ZRoomCommand* cmd);
	std::shared_ptr<ZRoomCommand> FindCommandOfType(RoomCommand cmdType);
	std::vector<uint8_t> GetRawData();
	int GetRawDataSize();
	virtual ZResourceType GetResourceType();
	virtual void Save(const std::string& outFolder);
	virtual void PreGenSourceFiles();
};

struct CommandSet
{
	int32_t address;
	int32_t commandCount; // Only used if explicitly specified in the XML

	CommandSet(int32_t nAddress);
	CommandSet(int32_t nAddress, int32_t nCommandCount);
};
