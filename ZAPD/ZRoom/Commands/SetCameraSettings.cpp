#include "SetCameraSettings.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

SetCameraSettings::SetCameraSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	cameraMovement = rawData[rawDataIndex + 0x01];
	mapHighlight = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
}

std::string SetCameraSettings::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x%02X, 0x%08X", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(),
		cameraMovement, mapHighlight);
}

std::string SetCameraSettings::GetCommandCName() const
{
	return "SCmdMiscSettings";
}

RoomCommand SetCameraSettings::GetRoomCommand() const
{
	return RoomCommand::SetCameraSettings;
}