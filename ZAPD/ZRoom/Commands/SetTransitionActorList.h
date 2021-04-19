#pragma once

#include "../ZRoomCommand.h"

class TransitionActorEntry
{
public:
	uint8_t frontObjectRoom;
	uint8_t frontTransitionReaction;
	uint8_t backObjectRoom;
	uint8_t backTransitionReaction;
	uint16_t actorNum;
	int16_t posX, posY, posZ;
	int16_t rotY;
	uint16_t initVar;

	TransitionActorEntry(std::vector<uint8_t> rawData, int rawDataIndex);
};

class SetTransitionActorList : public ZRoomCommand
{
public:
	SetTransitionActorList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
	~SetTransitionActorList();

	std::string GetBodySourceCode() override;
	std::string GenerateSourceCodePass1(std::string roomName, int baseAddress) override;
	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;
	std::string GenerateExterns() override;

private:
	std::vector<TransitionActorEntry*> transitionActors;
	uint32_t segmentOffset;
};