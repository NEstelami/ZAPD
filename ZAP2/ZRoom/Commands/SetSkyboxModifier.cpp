#include "SetSkyboxModifier.h"

using namespace std;

SetSkyboxModifier::SetSkyboxModifier(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	disableSky = rawData[rawDataIndex + 0x04];
	disableSunMoon = rawData[rawDataIndex + 0x05];
}

string SetSkyboxModifier::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetSkyboxModifier 0x%08X, 0x%08X\n", disableSky, disableSunMoon);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetSkyboxModifier::GetRoomCommand()
{
	return RoomCommand::SetSkyboxModifier;
}