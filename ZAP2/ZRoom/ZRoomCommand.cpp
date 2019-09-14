#include "ZRoomCommand.h"

using namespace std;

ZRoomCommand::ZRoomCommand(std::vector<uint8_t> rawData, int rawDataIndex)
{
}

string ZRoomCommand::GenerateSourceCode()
{
	return "";
}

string ZRoomCommand::GenerateSourceCodeDeferred()
{
	return "";
}

RoomCommand ZRoomCommand::GetRoomCommand()
{
	return RoomCommand::Error;
}