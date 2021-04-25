#include "SetCameraSettings.h"
#include "BitConverter.h"
#include "StringHelper.h"

SetCameraSettings::SetCameraSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                     uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	cameraMovement = cmdArg1;
	mapHighlight = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
}

std::string SetCameraSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_MISC_SETTINGS(0x%02X, 0x%08X)", cameraMovement,
	                             mapHighlight);
}

std::string SetCameraSettings::GetCommandCName() const
{
	return "SCmdMiscSettings";
}

RoomCommand SetCameraSettings::GetRoomCommand() const
{
	return RoomCommand::SetCameraSettings;
}
