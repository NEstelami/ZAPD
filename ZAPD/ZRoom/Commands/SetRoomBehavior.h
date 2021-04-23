#pragma once

#include "../ZRoomCommand.h"

class SetRoomBehavior : public ZRoomCommand
{
public:
	SetRoomBehavior(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t gameplayFlags;
	uint32_t gameplayFlags2;
};