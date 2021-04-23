#pragma once

#include "../ZRoomCommand.h"

class SetSpecialObjects : public ZRoomCommand
{
public:
	SetSpecialObjects(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	uint8_t elfMessage;
	uint16_t globalObject;
};
