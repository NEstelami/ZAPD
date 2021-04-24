#include "SetWorldMapVisited.h"
#include "../../StringHelper.h"

using namespace std;

SetWorldMapVisited::SetWorldMapVisited(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                       int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

string SetWorldMapVisited::GetBodySourceCode() const
{
	return "SCENECMD_WORLD_MAP_VISITED()";
}

string SetWorldMapVisited::GetCommandCName() const
{
	return "SCmdWorldMapVisited";
}

RoomCommand SetWorldMapVisited::GetRoomCommand() const
{
	return RoomCommand::SetWorldMapVisited;
}
