#include "SetRoomBehavior.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetRoomBehavior::SetRoomBehavior(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	gameplayFlags = rawData.at(rawDataIndex + 0x01);
	gameplayFlags2 = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x04);
}

std::string SetRoomBehavior::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s, 0x%02X, 0x%08X", GetCommandHex().c_str(), gameplayFlags, gameplayFlags2);
}

string SetRoomBehavior::GetCommandCName()
{
	return "SCmdRoomBehavior";
}

RoomCommand SetRoomBehavior::GetRoomCommand()
{
	return RoomCommand::SetRoomBehavior;
}
