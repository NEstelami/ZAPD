#include "SetWorldMapVisited.h"
#include "StringHelper.h"

SetWorldMapVisited::SetWorldMapVisited(ZFile* nParent) : ZRoomCommand(nParent)
{
}

std::string SetWorldMapVisited::GetBodySourceCode() const
{
	return "SCENE_CMD_WORLD_MAP_VISITED()";
}

std::string SetWorldMapVisited::GetCommandCName() const
{
	return "SCmdWorldMapVisited";
}

RoomCommand SetWorldMapVisited::GetRoomCommand() const
{
	return RoomCommand::SetWorldMapVisited;
}
