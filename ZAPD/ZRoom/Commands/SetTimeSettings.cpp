#include "SetTimeSettings.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

SetTimeSettings::SetTimeSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	hour = rawData[rawDataIndex + 4];
	min = rawData[rawDataIndex + 5];
	unk = rawData[rawDataIndex + 6];
}

std::string SetTimeSettings::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x00, 0x00, 0x00, 0x%02X, 0x%02X, 0x%02X",
		ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), hour, min, unk);
}

std::string SetTimeSettings::GetCommandCName() const
{
	return "SCmdTimeSettings";
}

RoomCommand SetTimeSettings::GetRoomCommand() const
{
	return RoomCommand::SetTimeSettings;
}