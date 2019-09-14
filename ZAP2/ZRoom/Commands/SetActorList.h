#pragma once

#include "../ZRoomCommand.h"

class ActorSpawnEntry
{
public:
	uint16_t actorNum;
	int16_t posX;
	int16_t posY;
	int16_t posZ;
	int16_t rotX;
	int16_t rotY;
	int16_t rotZ;
	uint16_t initVar;

	ActorSpawnEntry(std::vector<uint8_t> rawData, int rawDataIndex);
};

class SetActorList : public ZRoomCommand
{
public:
	SetActorList(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	std::vector<ActorSpawnEntry*> actors;
};