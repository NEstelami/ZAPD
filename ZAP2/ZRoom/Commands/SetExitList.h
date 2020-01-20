#pragma once

#include "../ZRoomCommand.h"

class SetExitList : public ZRoomCommand
{
public:
	SetExitList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	std::vector<uint16_t> exits;
};