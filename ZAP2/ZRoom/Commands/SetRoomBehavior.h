#pragma once

#include "../ZRoomCommand.h"

class SetRoomBehavior : public ZRoomCommand
{
public:
	SetRoomBehavior(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t gameplayFlags;
	uint8_t gameplayFlags2;
	uint8_t gameplayFlags3;
};