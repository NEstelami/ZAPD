#pragma once

#include "../../ZCollision.h"
#include "../ZRoomCommand.h"

class SetCollisionHeader : public ZRoomCommand
{
public:
	SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress) override;
	virtual std::string GenerateSourceCodePass2(std::string roomName, uint32_t baseAddress) override;
	virtual std::string GetCommandCName() const override;
	virtual std::string GenerateExterns() const override;
	virtual RoomCommand GetRoomCommand() const override;

private:
	ZCollisionHeader* collisionHeader;
	uint32_t segmentOffset;
};