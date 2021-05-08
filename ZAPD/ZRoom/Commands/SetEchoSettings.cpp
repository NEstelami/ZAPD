#include "SetEchoSettings.h"
#include "../../StringHelper.h"

SetEchoSettings::SetEchoSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	echo = rawData[rawDataIndex + 0x07];
}

std::string SetEchoSettings::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0, { 0 }, 0x%02X", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(),
		echo);
}

std::string SetEchoSettings::GetCommandCName() const
{
	return "SCmdEchoSettings";
}

RoomCommand SetEchoSettings::GetRoomCommand() const
{
	return RoomCommand::SetEchoSettings;
}