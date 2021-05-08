#include "EndMarker.h"
#include "../../StringHelper.h"

EndMarker::EndMarker(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

std::string EndMarker::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x00, 0x00", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str());
}

std::string EndMarker::GetCommandCName() const
{
	return "SCmdEndMarker";
}

RoomCommand EndMarker::GetRoomCommand() const
{
	return RoomCommand::EndMarker;
}