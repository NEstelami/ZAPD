#include "SetTimeSettings.h"
#include "../../BitConverter.h"

using namespace std;

SetTimeSettings::SetTimeSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	//time = (rawData[rawDataIndex + 5] << 8) + rawData[rawDataIndex + 4];
	//time = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	//speed = rawData[rawDataIndex + 6];

	hour = rawData[rawDataIndex + 4];
	min = rawData[rawDataIndex + 5];
	unk = rawData[rawDataIndex + 6];
}

string SetTimeSettings::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, 0x00, 0x00, 0x%02X, 0x%02X, 0x%02X };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), hour, min, unk);
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