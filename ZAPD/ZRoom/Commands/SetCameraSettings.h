#pragma once

#include "../ZRoomCommand.h"

class SetCameraSettings : public ZRoomCommand
{
public:
	SetCameraSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;

private:
	uint8_t cameraMovement;
	uint32_t mapHighlight;
};
