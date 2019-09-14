#include "SetTimeSettings.h"
#include "../../BitConverter.h"

using namespace std;

SetTimeSettings::SetTimeSettings(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	//time = (rawData[rawDataIndex + 5] << 8) + rawData[rawDataIndex + 4];
	time = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	speed = rawData[rawDataIndex + 6];
}

string SetTimeSettings::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetTimeSettings 0x%16X, 0x%08X\n", time, speed);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetTimeSettings::GetRoomCommand()
{
	return RoomCommand::SetTimeSettings;
}