#include "EndMarker.h"
#include "../../StringHelper.h"

using namespace std;

EndMarker::EndMarker(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

string EndMarker::GetBodySourceCode() const
{
	return "SCENECMD_END()";
}

string EndMarker::GetCommandCName() const
{
	return "SCmdEndMarker";
}

RoomCommand EndMarker::GetRoomCommand() const
{
	return RoomCommand::EndMarker;
}
