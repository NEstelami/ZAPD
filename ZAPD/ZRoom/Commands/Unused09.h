#pragma once

#include "../ZRoomCommand.h"

class Unused09 : public ZRoomCommand
{
public:
	Unused09(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;
};