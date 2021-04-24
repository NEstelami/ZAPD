#include "EndMarker.h"
#include "../../StringHelper.h"

using namespace std;

EndMarker::EndMarker(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

string EndMarker::GetBodySourceCode()
{
	return "SCENE_CMD_END()";
}

string EndMarker::GetCommandCName()
{
	return "SCmdEndMarker";
}

RoomCommand EndMarker::GetRoomCommand()
{
	return RoomCommand::EndMarker;
}
