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
	SetActorList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName);
	virtual std::string GenerateSourceCodePass2(std::string roomName);
	virtual RoomCommand GetRoomCommand();
	virtual int32_t GetRawDataSize();
	virtual std::string GetCommandCName();
	virtual std::string GenerateExterns();

private:
	std::vector<ActorSpawnEntry*> actors;
	uint32_t segmentOffset;
};