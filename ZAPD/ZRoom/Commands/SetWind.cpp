#include "SetWind.h"
#include "../../StringHelper.h"

SetWind::SetWind(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	windWest = rawData[rawDataIndex + 0x04];
	windVertical = rawData[rawDataIndex + 0x05];
	windSouth = rawData[rawDataIndex + 0x06];
	clothFlappingStrength = rawData[rawDataIndex + 0x07];
}

std::string SetWind::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x00, 0x00, 0x00, 0x%02X, 0x%02X, 0x%02X, 0x%02X",
		ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), windWest,
		windVertical, windSouth, clothFlappingStrength);
}

std::string SetWind::GetCommandCName() const
{
	return "SCmdWindSettings";
}

RoomCommand SetWind::GetRoomCommand() const
{
	return RoomCommand::SetWind;
}