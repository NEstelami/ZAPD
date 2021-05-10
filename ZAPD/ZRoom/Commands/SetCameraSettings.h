#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetCameraSettings : public ZRoomCommand
{
public:
	SetCameraSettings(ZFile* nParent);

	void ParseRawData() override;

	uint8_t cameraMovement;
	uint32_t mapHighlight;
private:
};
