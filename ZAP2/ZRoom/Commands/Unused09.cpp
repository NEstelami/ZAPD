#include "Unused09.h"

using namespace std;

Unused09::Unused09(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

string Unused09::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, 0x00};", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress));
	sourceOutput = line;

	return sourceOutput;
}

string Unused09::GetCommandCName()
{
	return "SCmdBase";
}

RoomCommand Unused09::GetRoomCommand()
{
	return RoomCommand::Unused09;
}