#pragma once

#include "../ZRoomCommand.h"

class SetWorldMapVisited : public ZRoomCommand
{
public:
	SetWorldMapVisited(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
};