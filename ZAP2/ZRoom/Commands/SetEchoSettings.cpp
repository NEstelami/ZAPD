#include "SetEchoSettings.h"

using namespace std;

SetEchoSettings::SetEchoSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	echo = rawData[rawDataIndex + 0x07];
}

string SetEchoSettings::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0, { 0 }, 0x%02X };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), echo);
	sourceOutput = line;

	return sourceOutput;
}

string SetEchoSettings::GetCommandCName()
{
	return "SCmdEchoSettings";
}


RoomCommand SetEchoSettings::GetRoomCommand()
{
	return RoomCommand::SetEchoSettings;
}