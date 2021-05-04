#include "SetRoomBehavior.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

SetRoomBehavior::SetRoomBehavior(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	gameplayFlags = rawData[rawDataIndex + 0x01];
	gameplayFlags2 = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x04);
}

std::string SetRoomBehavior::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x%02X, 0x%08X", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(),
		gameplayFlags, gameplayFlags2);
	;
}

std::string SetRoomBehavior::GetCommandCName() const
{
	return "SCmdRoomBehavior";
}

RoomCommand SetRoomBehavior::GetRoomCommand() const
{
	return RoomCommand::SetRoomBehavior;
}