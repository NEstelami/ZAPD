#pragma once

#include "../ZRoomCommand.h"

class SetSkyboxSettings : public ZRoomCommand
{
public:
	SetSkyboxSettings(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t skyboxNumber;
	uint8_t cloudsType;
	uint8_t lightingSettingsControl;
};