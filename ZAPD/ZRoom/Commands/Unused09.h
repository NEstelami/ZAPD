#pragma once

#include "../ZRoomCommand.h"

class Unused09 : public ZRoomCommand
{
public:
	Unused09(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;
};
