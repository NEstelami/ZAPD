#pragma once

#include "ZCollision.h"
#include "ZRoom/ZRoomCommand.h"

class SetCollisionHeader : public ZRoomCommand
{
public:
	ZCollisionHeader* collisionHeader;

	SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);
    ~SetCollisionHeader();

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;

private:
	uint32_t segmentOffset;
};
