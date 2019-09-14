#pragma once

#include "../ZRoomCommand.h"

class SetWind : public ZRoomCommand
{
public:
	SetWind(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t windWest;
	uint8_t windVertical;
	uint8_t windSouth;
	uint8_t clothFlappingStrength;
};