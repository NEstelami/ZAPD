#pragma once

#include "../ZRoomCommand.h"

class SetSkyboxModifier : public ZRoomCommand
{
public:
	SetSkyboxModifier(ZRoom* nZRoom, std::vector<uint8_t> rawData, int32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, int32_t baseAddress);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t disableSky;
	uint8_t disableSunMoon;
};