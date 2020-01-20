#pragma once

#include "../ZRoomCommand.h"

class SetTimeSettings : public ZRoomCommand
{
public:
	SetTimeSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	uint16_t time;
	int8_t speed;
};