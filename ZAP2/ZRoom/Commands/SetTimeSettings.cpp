#include "SetTimeSettings.h"
#include "../../BitConverter.h"

using namespace std;

SetTimeSettings::SetTimeSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	//time = (rawData[rawDataIndex + 5] << 8) + rawData[rawDataIndex + 4];
	time = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	speed = rawData[rawDataIndex + 6];
}

string SetTimeSettings::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, 0x00, 0x00, 0x%04X, 0x%02X };", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), time, speed);
	sourceOutput = line;

	return sourceOutput;
}

string SetTimeSettings::GetCommandCName()
{
	return "SCmdTimeSettings";
}

RoomCommand SetTimeSettings::GetRoomCommand()
{
	return RoomCommand::SetTimeSettings;
}