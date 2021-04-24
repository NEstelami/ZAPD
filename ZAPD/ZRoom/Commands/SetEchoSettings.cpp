#include "SetEchoSettings.h"
#include "../../StringHelper.h"

using namespace std;

SetEchoSettings::SetEchoSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	echo = rawData[rawDataIndex + 0x07];
}

std::string SetEchoSettings::GetBodySourceCode()
{
	return StringHelper::Sprintf("SCENE_CMD_ECHO_SETTINGS(0x%02X)", echo);
}

string SetEchoSettings::GetCommandCName()
{
	return "SCmdEchoSettings";
}

RoomCommand SetEchoSettings::GetRoomCommand()
{
	return RoomCommand::SetEchoSettings;
}
