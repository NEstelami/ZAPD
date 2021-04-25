#include "SetRoomBehavior.h"
#include "BitConverter.h"
#include "Globals.h"
#include "StringHelper.h"

SetRoomBehavior::SetRoomBehavior(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                 uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	gameplayFlags = rawData.at(rawDataIndex + 0x01);
	gameplayFlags2 = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x04);
}

std::string SetRoomBehavior::GetBodySourceCode() const
{
	uint8_t currRoomUnk2 = gameplayFlags2 & 0xFF;
	uint8_t currRoomUnk5 = (gameplayFlags2 >> 8) & 1;
	uint8_t msgCtxunk12044 = (gameplayFlags2 >> 10) & 1;
	uint8_t enablePosLights = (gameplayFlags2 >> 11) & 1;
	uint8_t kankyoContextUnkE2 = (gameplayFlags2 >> 12) & 1;
	return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(0x%02X, 0x%02X, %i, %i, %i, %i)",
	                             gameplayFlags, currRoomUnk2, currRoomUnk5, msgCtxunk12044,
	                             enablePosLights, kankyoContextUnkE2);
}

std::string SetRoomBehavior::GetCommandCName() const
{
	return "SCmdRoomBehavior";
}

RoomCommand SetRoomBehavior::GetRoomCommand() const
{
	return RoomCommand::SetRoomBehavior;
}
