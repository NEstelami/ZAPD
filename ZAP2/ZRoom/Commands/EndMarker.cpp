#include "EndMarker.h"
#include "../../StringHelper.h"

using namespace std;

EndMarker::EndMarker(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

string EndMarker::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";

	sourceOutput = StringHelper::Sprintf("%s 0x00, 0x00 };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str());
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