#pragma once

#include "../ZRoomCommand.h"

class SetSkyboxSettings : public ZRoomCommand
{
public:
	SetSkyboxSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, int32_t baseAddress);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t unk1;  // (MM Only)
	uint8_t skyboxNumber;
	uint8_t cloudsType;
	uint8_t lightingSettingsControl;
};