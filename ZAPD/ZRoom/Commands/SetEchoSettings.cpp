#include "SetEchoSettings.h"
#include "StringHelper.h"

SetEchoSettings::SetEchoSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                 uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	echo = rawData[rawDataIndex + 0x07];
}

std::string SetEchoSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENECMD_ECHO_SETTINGS(%i)", echo);
}

std::string SetEchoSettings::GetCommandCName() const
{
	return "SCmdEchoSettings";
}

RoomCommand SetEchoSettings::GetRoomCommand() const
{
	return RoomCommand::SetEchoSettings;
}
