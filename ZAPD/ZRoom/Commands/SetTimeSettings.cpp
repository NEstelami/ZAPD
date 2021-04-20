#include "SetTimeSettings.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetTimeSettings::SetTimeSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	hour = rawData[rawDataIndex + 4];
	min = rawData[rawDataIndex + 5];
	unk = rawData[rawDataIndex + 6];
}

std::string SetTimeSettings::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s, 0x00, 0x00, 0x00, 0x%02X, 0x%02X, 0x%02X", GetCommandHex().c_str(), hour, min, unk);
}

string SetTimeSettings::GetCommandCName()
{
	return "SCmdTimeSettings";
}

RoomCommand SetTimeSettings::GetRoomCommand()
{
	return RoomCommand::SetTimeSettings;
}