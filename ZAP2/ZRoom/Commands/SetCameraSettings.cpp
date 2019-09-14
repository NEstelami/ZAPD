#include "SetCameraSettings.h"

using namespace std;

SetCameraSettings::SetCameraSettings(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	cameraMovement = rawData[rawDataIndex + 0x01];
	mapHighlight = rawData[rawDataIndex + 0x07];
}

string SetCameraSettings::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetCameraSettings 0x%08X, 0x%08X\n", cameraMovement, mapHighlight);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetCameraSettings::GetRoomCommand()
{
	return RoomCommand::SetCameraSettings;
}