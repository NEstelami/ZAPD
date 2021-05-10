#include "SetWorldMapVisited.h"
#include "../../StringHelper.h"

SetWorldMapVisited::SetWorldMapVisited(ZRoom* nZRoom, std::vector<uint8_t> rawData,
                                       uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

std::string SetWorldMapVisited::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x00, 0x00", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str());
}

std::string SetWorldMapVisited::GetCommandCName() const
{
	return "SCmdWorldMapVisited";
}

RoomCommand SetWorldMapVisited::GetRoomCommand() const
{
	return RoomCommand::SetWorldMapVisited;
}