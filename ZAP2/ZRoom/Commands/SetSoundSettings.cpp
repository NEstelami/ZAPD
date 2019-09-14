#include "SetSoundSettings.h"

using namespace std;

SetSoundSettings::SetSoundSettings(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	reverb = rawData[rawDataIndex + 0x01];
	nightTimeSFX = rawData[rawDataIndex + 0x06];
	musicSequence = rawData[rawDataIndex + 0x07];
}

string SetSoundSettings::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetSoundSettings 0x%08X, 0x%08X, 0x%08X\n", reverb, nightTimeSFX, musicSequence);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetSoundSettings::GetRoomCommand()
{
	return RoomCommand::SetSoundSettings;
}