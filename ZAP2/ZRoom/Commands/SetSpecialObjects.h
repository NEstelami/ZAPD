#pragma once

#include "../ZRoomCommand.h"

class SetSpecialObjects : public ZRoomCommand
{
public:
	SetSpecialObjects(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t elfMessage;
	uint16_t globalObject;
};