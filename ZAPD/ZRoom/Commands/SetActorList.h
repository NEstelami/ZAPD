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

	ActorSpawnEntry(std::vector<uint8_t> rawData, uint32_t rawDataIndex);
};

class SetActorList : public ZRoomCommand
{
public:
	SetActorList(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);
	~SetActorList();

	std::string GetSourceOutputCode(std::string prefix);
	virtual std::string GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress) override;
	virtual std::string GenerateSourceCodePass2(std::string roomName, uint32_t baseAddress) override;
	virtual RoomCommand GetRoomCommand() const override;
	virtual size_t GetRawDataSize() const override;
	virtual std::string GetCommandCName() const override;
	virtual std::string GenerateExterns() const override;

private:
	size_t GetActorListArraySize() const;
	int32_t numActors;
	std::vector<ActorSpawnEntry*> actors;
	uint32_t segmentOffset;
	std::vector<uint8_t> _rawData;
	uint32_t _rawDataIndex;
};