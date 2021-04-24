#include "SetEchoSettings.h"
#include "../../StringHelper.h"

using namespace std;

SetEchoSettings::SetEchoSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	echo = rawData[rawDataIndex + 0x07];
}

std::string SetEchoSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_ECHO_SETTINGS(%i)", echo);
}

string SetEchoSettings::GetCommandCName() const
{
	return "SCmdEchoSettings";
}

RoomCommand SetEchoSettings::GetRoomCommand() const
{
	return RoomCommand::SetEchoSettings;
}
