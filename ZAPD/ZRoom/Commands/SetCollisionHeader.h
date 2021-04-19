#pragma once

#include "../../ZCollision.h"
#include "../ZRoomCommand.h"

class SetCollisionHeader : public ZRoomCommand
{
public:
	SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
	~SetCollisionHeader();

	virtual std::string GenerateSourceCodePass1(std::string roomName, int baseAddress);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	ZCollisionHeader* collisionHeader;
	uint32_t segmentOffset;
};