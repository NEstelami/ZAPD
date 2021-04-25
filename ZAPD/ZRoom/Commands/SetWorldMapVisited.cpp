#include "SetWorldMapVisited.h"
#include "StringHelper.h"

SetWorldMapVisited::SetWorldMapVisited(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                       uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

std::string SetWorldMapVisited::GetBodySourceCode() const
{
	return "SCENECMD_WORLD_MAP_VISITED()";
}

std::string SetWorldMapVisited::GetCommandCName() const
{
	return "SCmdWorldMapVisited";
}

RoomCommand SetWorldMapVisited::GetRoomCommand() const
{
	return RoomCommand::SetWorldMapVisited;
}
