#pragma once

#include "../ZRoomCommand.h"

class SetWind : public ZRoomCommand
{
public:
	SetWind(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	uint8_t windWest;
	uint8_t windVertical;
	uint8_t windSouth;
	uint8_t clothFlappingStrength;
};
