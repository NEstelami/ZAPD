#include "SetRoomBehavior.h"
#include "../../BitConverter.h"

using namespace std;

SetRoomBehavior::SetRoomBehavior(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	gameplayFlags = rawData[rawDataIndex + 0x01];
	gameplayFlags2 = rawData[rawDataIndex + 0x06];
	gameplayFlags3 = rawData[rawDataIndex + 0x07];
}

string SetRoomBehavior::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x%02X, 0x0000, 0x%02X, 0x%02X };", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), gameplayFlags, gameplayFlags2, gameplayFlags3);
	sourceOutput = line;

	return sourceOutput;
}

string SetRoomBehavior::GetCommandCName()
{
	return "SCmdRoomBehavior";
}

RoomCommand SetRoomBehavior::GetRoomCommand()
{
	return RoomCommand::SetRoomBehavior;
}