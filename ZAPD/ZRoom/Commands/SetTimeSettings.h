#pragma once

#include "../ZRoomCommand.h"

class SetTimeSettings : public ZRoomCommand
{
public:
	SetTimeSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, int32_t baseAddress);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	// uint16_t time;
	// int8_t speed;

	uint8_t hour;
	uint8_t min;
	uint8_t unk;
};