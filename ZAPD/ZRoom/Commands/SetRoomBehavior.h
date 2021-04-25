#pragma once

#include "../ZRoomCommand.h"

class SetRoomBehavior : public ZRoomCommand
{
public:
	SetRoomBehavior(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	uint8_t gameplayFlags;
	uint32_t gameplayFlags2;
};
