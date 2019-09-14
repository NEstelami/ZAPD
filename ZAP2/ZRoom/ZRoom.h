#pragma once

#include "../ZResource.h"
#include "ZRoomCommand.h"
#include "../tinyxml2.h"

#include <vector>
#include <string>

class ZRoom : public ZResource
{
protected:
	std::vector<ZRoomCommand*> commands;

	void ParseRawData();
	void PrepareSourceOutput();
public:
	ZRoom(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath);

	std::vector<uint8_t> GetRawData();
	int GetRawDataSize();
	//void Save(std::string outFolder);
};