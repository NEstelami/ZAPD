#pragma once

#include "../ZRoomCommand.h"

class SetEchoSettings : public ZRoomCommand
{
public:
	SetEchoSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;

private:
	uint8_t echo;
};
