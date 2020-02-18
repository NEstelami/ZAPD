#include "SetWind.h"

using namespace std;

SetWind::SetWind(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	windWest = rawData[rawDataIndex + 0x04];
	windVertical = rawData[rawDataIndex + 0x05];
	windSouth = rawData[rawDataIndex + 0x06];
	clothFlappingStrength = rawData[rawDataIndex + 0x07];
}

string SetWind::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x%02X, 0x%02X, 0x%02X, 0x%02X};", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), windWest, windVertical, windSouth, clothFlappingStrength);
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