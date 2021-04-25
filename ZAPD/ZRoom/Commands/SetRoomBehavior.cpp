#include "SetRoomBehavior.h"
#include "BitConverter.h"
#include "Globals.h"
#include "StringHelper.h"

SetRoomBehavior::SetRoomBehavior(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                 uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

void SetRoomBehavior::ParseRawData()
{
	gameplayFlags = cmdArg1;
	gameplayFlags2 = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x04);

	currRoomUnk2 = gameplayFlags2 & 0xFF;

	currRoomUnk5 = showInvisActors = (gameplayFlags2 >> 8) & 1;

	msgCtxUnk = (gameplayFlags2 >> 10) & 1;

	enablePosLights = (gameplayFlags2 >> 11) & 1;
	kankyoContextUnkE2 = (gameplayFlags2 >> 12) & 1;
}

std::string SetRoomBehavior::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(0x%02X, 0x%02X, %i, %i, %i, %i)",
		                             gameplayFlags, currRoomUnk2, currRoomUnk5, msgCtxUnk,
		                             enablePosLights, kankyoContextUnkE2);
	}
	return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(0x%02X, 0x%02X, %i, %i)", gameplayFlags,
	                             currRoomUnk2, showInvisActors, msgCtxUnk);
}

std::string SetRoomBehavior::GetCommandCName() const
{
	return "SCmdRoomBehavior";
}

RoomCommand SetRoomBehavior::GetRoomCommand() const
{
	return RoomCommand::SetRoomBehavior;
}
