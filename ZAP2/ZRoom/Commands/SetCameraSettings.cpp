#include "SetCameraSettings.h"

using namespace std;

SetCameraSettings::SetCameraSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	cameraMovement = rawData[rawDataIndex + 0x01];
	mapHighlight = rawData[rawDataIndex + 0x07];
}

string SetCameraSettings::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x%02X, 0x00, 0x00, 0x00, 0x%02X };", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), cameraMovement, mapHighlight);
	sourceOutput = line;

	return sourceOutput;
}

string SetCameraSettings::GetCommandCName()
{
	return "SCmdMiscSettings";
}

RoomCommand SetCameraSettings::GetRoomCommand()
{
	return RoomCommand::SetCameraSettings;
}