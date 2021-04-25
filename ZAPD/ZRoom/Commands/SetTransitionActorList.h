#pragma once

#include "ZRoom/ZRoomCommand.h"

class TransitionActorEntry
{
public:
	TransitionActorEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() const;

protected:
	uint8_t frontObjectRoom;
	uint8_t frontTransitionReaction;
	uint8_t backObjectRoom;
	uint8_t backTransitionReaction;
	uint16_t actorNum;
	int16_t posX, posY, posZ;
	int16_t rotY;
	uint16_t initVar;
};

class SetTransitionActorList : public ZRoomCommand
{
public:
	SetTransitionActorList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
	                       uint32_t rawDataIndex);

	virtual void ParseRawData();
	virtual void DeclareReferences(const std::string& prefix);

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	size_t GetRawDataSize() override;
	std::string GetCommandCName() const override;

private:
	std::vector<TransitionActorEntry> transitionActors;
};
