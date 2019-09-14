#include "SetSkyboxSettings.h"

using namespace std;

SetSkyboxSettings::SetSkyboxSettings(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	skyboxNumber = rawData[rawDataIndex + 0x04];
	cloudsType = rawData[rawDataIndex + 0x05];
	lightingSettingsControl = rawData[rawDataIndex + 0x06];
}

string SetSkyboxSettings::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetSkyboxSettings 0x%08X, 0x%08X, 0x%08X\n", skyboxNumber, cloudsType, lightingSettingsControl);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetSkyboxSettings::GetRoomCommand()
{
	return RoomCommand::SetSkyboxSettings;
}