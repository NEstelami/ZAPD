#include "SetEchoSettings.h"

using namespace std;

SetEchoSettings::SetEchoSettings(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	echo = rawData[rawDataIndex + 0x07];
}

string SetEchoSettings::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetEchoSettings 0x%08X\n", echo);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetEchoSettings::GetRoomCommand()
{
	return RoomCommand::SetEchoSettings;
}