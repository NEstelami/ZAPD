#pragma once

#include <string>

#include "../ZRoom.h"
#include "../ZRoomCommand.h"
#include "ZFile.h"

class SetLightList : public ZRoomCommand
{
public:
	SetLightList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	void ParseRawData() override;

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	uint8_t code;
	uint8_t numLights;
};
