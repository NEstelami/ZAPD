#include "EndMarker.h"
#include "../../StringHelper.h"

using namespace std;

EndMarker::EndMarker(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

string EndMarker::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s, 0x00, 0x00", GetCommandHex().c_str());
}

string EndMarker::GetCommandCName()
{
	return "SCmdEndMarker";
}

RoomCommand EndMarker::GetRoomCommand()
{
	return RoomCommand::EndMarker;
}
