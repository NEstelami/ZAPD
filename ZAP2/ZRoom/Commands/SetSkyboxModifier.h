#pragma once

#include "../ZRoomCommand.h"

class SetSkyboxModifier : public ZRoomCommand
{
public:
	SetSkyboxModifier(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t disableSky;
	uint8_t disableSunMoon;
};