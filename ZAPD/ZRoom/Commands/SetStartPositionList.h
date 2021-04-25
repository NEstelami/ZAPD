#pragma once

#include "../ZRoomCommand.h"
#include "SetActorList.h"

class SetStartPositionList : public ZRoomCommand
{
public:
	SetStartPositionList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	std::vector<ActorSpawnEntry> actors;
};
