#pragma once

#include "../ZRoomCommand.h"
#include "SetActorList.h"


class SetStartPositionList : public ZRoomCommand
{
public:
	SetStartPositionList(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	std::vector<ActorSpawnEntry*> actors;
};