#pragma once

#include "ZRoom/ZRoomCommand.h"

class SetRoomBehavior : public ZRoomCommand
{
public:
	SetRoomBehavior(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

protected:
	uint8_t gameplayFlags;
	uint32_t gameplayFlags2;

	uint8_t currRoomUnk2;

	uint8_t showInvisActors;
	uint8_t currRoomUnk5;

	uint8_t msgCtxUnk;

	uint8_t enablePosLights;
	uint8_t kankyoContextUnkE2;
};
