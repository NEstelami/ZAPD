#pragma once

#include "../ZRoomCommand.h"
#include "SetActorList.h"

class SetStartPositionList : public ZRoomCommand
{
public:
	SetStartPositionList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences() override;

	std::string GetBodySourceCode() override;
	std::string GenerateExterns() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	std::vector<ActorSpawnEntry> actors;
};
