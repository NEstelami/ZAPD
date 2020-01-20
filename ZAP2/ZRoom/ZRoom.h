#pragma once

#include "../ZResource.h"
#include "ZRoomCommand.h"
#include "../tinyxml2.h"

#include <vector>
#include <map>
#include <string>

class ZRoom : public ZResource
{
protected:
	std::vector<ZRoomCommand*> commands;

	std::string GetSourceOutputHeader();
	std::string GetSourceOutputCode();
	void GenDefinitions();

	void ParseRawData();
	void PrepareSourceOutput();
public:
	std::map<int32_t, std::string> declarations;

	ZRoom(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath);

	int32_t GetDeclarationSizeFromNeighbor(int declarationAddress);
	int32_t GetCommandSizeFromNeighbor(ZRoomCommand* cmd);
	ZRoomCommand* FindCommandOfType(RoomCommand cmdType);
	std::vector<uint8_t> GetRawData();
	int GetRawDataSize();
	//void Save(std::string outFolder);
};