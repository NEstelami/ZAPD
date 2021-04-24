#include "SetRoomBehavior.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "Globals.h"

using namespace std;

SetRoomBehavior::SetRoomBehavior(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	gameplayFlags = rawData.at(rawDataIndex + 0x01);
	gameplayFlags2 = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x04);
}

std::string SetRoomBehavior::GetBodySourceCode()
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		uint8_t currRoomUnk2 = gameplayFlags2 & 0xFF;
		uint8_t currRoomUnk5 = (gameplayFlags2 >> 8) & 1;
		uint8_t msgCtxunk12044 = (gameplayFlags2 >> 10) & 1;
		uint8_t enablePosLights = (gameplayFlags2 >> 11) & 1;
		uint8_t kankyoContextUnkE2 = (gameplayFlags2 >> 12) & 1;
		return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(0x%02X, 0x%02X, %i, %i, %i, %i)", gameplayFlags, currRoomUnk2, currRoomUnk5, msgCtxunk12044, enablePosLights, kankyoContextUnkE2);
	}
	uint8_t currRoomUnk2 = gameplayFlags2 & 0xFF;
	uint8_t showInvisActors = (gameplayFlags2 >> 8) & 1;
	uint8_t msgCtxUnkE40C = (gameplayFlags2 >> 10) & 1;
	return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(0x%02X, 0x%02X, %i, %i)", gameplayFlags, currRoomUnk2, showInvisActors, msgCtxUnkE40C);
}

string SetRoomBehavior::GetCommandCName()
{
	return "SCmdRoomBehavior";
}

RoomCommand SetRoomBehavior::GetRoomCommand()
{
	return RoomCommand::SetRoomBehavior;
}
