#pragma once

#include <string>

#include "../ZRoom.h"
#include "../ZRoomCommand.h"
#include "ZFile.h"

class SetLightList : public ZRoomCommand
{
public:
	SetLightList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;
	std::string GenerateExterns() override;

private:
	uint8_t code;
	uint8_t numLights;
};