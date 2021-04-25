#include "EndMarker.h"
#include "StringHelper.h"

EndMarker::EndMarker(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

std::string EndMarker::GetBodySourceCode() const
{
	return "SCENE_CMD_END()";
}

std::string EndMarker::GetCommandCName() const
{
	return "SCmdEndMarker";
}

RoomCommand EndMarker::GetRoomCommand() const
{
	return RoomCommand::EndMarker;
}
