#pragma once

#include "../ZRoomCommand.h"

class SetSkyboxModifier : public ZRoomCommand
{
public:
	SetSkyboxModifier(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	uint8_t disableSky;
	uint8_t disableSunMoon;
};