#include "EndMarker.h"

using namespace std;

EndMarker::EndMarker(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

string EndMarker::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, 0x00 };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str());
	sourceOutput = line;

	return sourceOutput;
}

string EndMarker::GetCommandCName()
{
	return "SCmdEndMarker";
}

RoomCommand EndMarker::GetRoomCommand()
{
	return RoomCommand::EndMarker;
}