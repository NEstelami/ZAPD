#include "SetCameraSettings.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetCameraSettings::SetCameraSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	cameraMovement = cmdArg1;
	mapHighlight = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
}

string SetCameraSettings::GetBodySourceCode()
{
	return StringHelper::Sprintf("SCENE_CMD_MISC_SETTINGS(0x%02X, 0x%08X)", cameraMovement, mapHighlight);
}

string SetCameraSettings::GetCommandCName()
{
	return "SCmdMiscSettings";
}

RoomCommand SetCameraSettings::GetRoomCommand()
{
	return RoomCommand::SetCameraSettings;
}
