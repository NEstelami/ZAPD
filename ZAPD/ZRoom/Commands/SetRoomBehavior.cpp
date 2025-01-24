#include "SetRoomBehavior.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

SetRoomBehavior::SetRoomBehavior(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetRoomBehavior::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	gameplayFlags = cmdArg1;
	gameplayFlags2 = BitConverter::ToInt32BE(parent->GetRawData(), rawDataIndex + 0x04);

	currRoomUnk2 = gameplayFlags2 & 0xFF;

	currRoomUnk5 = showInvisActors = (gameplayFlags2 >> 8) & 1;

	msgCtxUnk = (gameplayFlags2 >> 10) & 1;

	enablePosLights = (gameplayFlags2 >> 11) & 1;
	kankyoContextUnkE2 = (gameplayFlags2 >> 12) & 1;
}

std::string SetRoomBehavior::GetBodySourceCode() const
{
	EnumData* enumData = &Globals::Instance->cfg.enumData;

	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		std::string enableLights = StringHelper::BoolStr(enablePosLights);
		return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(%s, %s, %s, %i, %s, %s)",
		                             enumData->roomType[gameplayFlags].c_str(),
		                             enumData->environmentType[currRoomUnk2].c_str(),
		                             enumData->lensMode[currRoomUnk5].c_str(), msgCtxUnk,
		                             enableLights.c_str(),
		                             enumData->stormState[kankyoContextUnkE2].c_str());
	}

	std::string disableWarps = StringHelper::BoolStr(msgCtxUnk);

	return StringHelper::Sprintf("SCENE_CMD_ROOM_BEHAVIOR(%s, %s, %s, %s)",
	                             enumData->roomType[gameplayFlags].c_str(),
	                             enumData->environmentType[currRoomUnk2].c_str(),
	                             enumData->lensMode[showInvisActors].c_str(), disableWarps.c_str());
}

std::string SetRoomBehavior::GetCommandCName() const
{
	return "SCmdRoomBehavior";
}

RoomCommand SetRoomBehavior::GetRoomCommand() const
{
	return RoomCommand::SetRoomBehavior;
}
