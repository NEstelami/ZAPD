#include "SetCameraSettings.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetCameraSettings::SetCameraSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	cameraMovement = rawData.at(rawDataIndex + 0x01);
	mapHighlight = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
}

string SetCameraSettings::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s, 0x%02X, 0x%08X", GetCommandHex().c_str(), cameraMovement, mapHighlight);
}

string SetCameraSettings::GetCommandCName()
{
	return "SCmdMiscSettings";
}

RoomCommand SetCameraSettings::GetRoomCommand()
{
	return RoomCommand::SetCameraSettings;
}