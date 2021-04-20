#include "SetWorldMapVisited.h"
#include "../../StringHelper.h"

using namespace std;

SetWorldMapVisited::SetWorldMapVisited(ZRoom* nZRoom, std::vector<uint8_t> rawData,
                                       int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

string SetWorldMapVisited::GetBodySourceCode()
{
	return StringHelper::Sprintf(
		"%s 0x00, 0x00", GetCommandHex().c_str());
}

string SetWorldMapVisited::GetCommandCName()
{
	return "SCmdWorldMapVisited";
}

RoomCommand SetWorldMapVisited::GetRoomCommand()
{
	return RoomCommand::SetWorldMapVisited;
}
