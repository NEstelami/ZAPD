#include "SetWind.h"

using namespace std;

SetWind::SetWind(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	windWest = rawData[rawDataIndex + 0x04];
	windVertical = rawData[rawDataIndex + 0x05];
	windSouth = rawData[rawDataIndex + 0x06];
	clothFlappingStrength = rawData[rawDataIndex + 0x07];
}

string SetWind::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetWind 0x%08X, 0x%08X, 0x%08X, 0x%08X\n", windWest, windVertical, windSouth, clothFlappingStrength);
	sourceOutput = line;

	return sourceOutput;
}

string SetWind::GetCommandCName()
{
	return "SCmdWindSettings";
}

RoomCommand SetWind::GetRoomCommand()
{
	return RoomCommand::SetWind;
}