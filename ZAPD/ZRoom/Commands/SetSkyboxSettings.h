#pragma once

#include "../ZRoomCommand.h"

class SetSkyboxSettings : public ZRoomCommand
{
public:
	SetSkyboxSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;

	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	uint8_t unk1;  // (MM Only)
	uint8_t skyboxNumber;
	uint8_t cloudsType;
	uint8_t lightingSettingsControl;
};
