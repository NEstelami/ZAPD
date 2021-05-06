#pragma once

#include "../ZRoomCommand.h"

class SetSkyboxSettings : public ZRoomCommand
{
public:
	uint8_t unk1;  // (MM Only)
	uint8_t skyboxNumber;
	uint8_t cloudsType;
	uint8_t lightingSettingsControl;

	SetSkyboxSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName,
	                                            uint32_t baseAddress) override;
	virtual std::string GetCommandCName() override;
	virtual RoomCommand GetRoomCommand() override;

private:
};