#include "SetRoomBehavior.h"
#include "../../BitConverter.h"

using namespace std;

SetRoomBehavior::SetRoomBehavior(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	gameplayFlags = rawData[rawDataIndex + 0x01];
	gameplayFlags2 = rawData[rawDataIndex + 0x06];
	gameplayFlags3 = rawData[rawDataIndex + 0x07];
}

string SetRoomBehavior::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetRoomBehavior 0x%08X, 0x%08X, 0x%08X\n", gameplayFlags, gameplayFlags2, gameplayFlags3);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetRoomBehavior::GetRoomCommand()
{
	return RoomCommand::SetRoomBehavior;
}