#pragma once

#include "../ZRoomCommand.h"

class Unused1D : public ZRoomCommand
{
public:
	Unused1D(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;
};
