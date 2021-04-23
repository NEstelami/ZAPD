#pragma once

#include "../ZRoomCommand.h"

class SetEchoSettings : public ZRoomCommand
{
public:
	SetEchoSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;

	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	uint8_t echo;
};
