#pragma once

#include "../ZRoomCommand.h"

class SetEchoSettings : public ZRoomCommand
{
public:
	SetEchoSettings(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t echo;
};