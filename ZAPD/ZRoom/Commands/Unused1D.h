#pragma once

#include "../ZRoomCommand.h"

class Unused1D : public ZRoomCommand
{
public:
	Unused1D(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;
};