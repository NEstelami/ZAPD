#pragma once

#include "../ZRoomCommand.h"

class SetTimeSettings : public ZRoomCommand
{
public:
	SetTimeSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	uint8_t hour;
	uint8_t min;
	uint8_t unk;
};
