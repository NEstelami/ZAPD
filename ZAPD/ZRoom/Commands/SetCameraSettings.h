#pragma once

#include "../ZRoomCommand.h"

class SetCameraSettings : public ZRoomCommand
{
public:
	SetCameraSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;

	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	uint8_t cameraMovement;
	uint32_t mapHighlight;
};
