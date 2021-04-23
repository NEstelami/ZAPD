#pragma once

#include "../ZRoomCommand.h"

class ActorSpawnEntry
{
protected:
	uint16_t actorNum;
	int16_t posX;
	int16_t posY;
	int16_t posZ;
	int16_t rotX;
	int16_t rotY;
	int16_t rotZ;
	uint16_t initVar;

public:
	ActorSpawnEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() const;
	std::string GetSourceTypeName() const;

	uint16_t GetActorId() const;
};

class SetActorList : public ZRoomCommand
{
public:
	SetActorList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;

protected:
	size_t GetActorListArraySize();

	int numActors;
	std::vector<ActorSpawnEntry> actors;
};
