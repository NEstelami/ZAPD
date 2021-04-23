#pragma once

#include "../../ZCollision.h"
#include "../ZRoomCommand.h"

class SetCollisionHeader : public ZRoomCommand
{
public:
	SetCollisionHeader(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);
	~SetCollisionHeader();

	void ParseRawData() override;

	std::string GetBodySourceCode() override;

	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	ZCollisionHeader* collisionHeader;
};
