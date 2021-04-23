#pragma once

#include "../ZRoomCommand.h"
#include "SetActorList.h"

class SetStartPositionList : public ZRoomCommand
{
public:
	SetStartPositionList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	std::vector<ActorSpawnEntry> actors;
};
