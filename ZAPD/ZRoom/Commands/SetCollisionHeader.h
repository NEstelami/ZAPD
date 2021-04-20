#pragma once

#include "../../ZCollision.h"
#include "../ZRoomCommand.h"

class SetCollisionHeader : public ZRoomCommand
{
public:
	SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, int32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, int32_t baseAddress);
	virtual std::string GenerateSourceCodePass2(std::string roomName, int32_t baseAddress);
	virtual std::string GetCommandCName();
	virtual std::string GenerateExterns();
	virtual RoomCommand GetRoomCommand();

private:
	ZCollisionHeader* collisionHeader;
	uint32_t segmentOffset;
};