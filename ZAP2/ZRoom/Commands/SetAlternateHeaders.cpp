#include "SetAlternateHeaders.h"
#include "../../BitConverter.h"

using namespace std;

SetAlternateHeaders::SetAlternateHeaders(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;

	string sourceOutput = "";
	char line[2048];

	sprintf(line, "alternateHeaders_%08X:\n ; ALTERNATE HEADER UNIMPLEMENTED!\n", segmentOffset);
	sourceOutput += line;

}

string SetAlternateHeaders::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s alternateHeaders_%08X};", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), segmentOffset);
	sourceOutput += line;

	return sourceOutput;
}

int32_t SetAlternateHeaders::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + 0;
}

string SetAlternateHeaders::GetCommandCName()
{
	return "SCmdAltHeaders";
}

RoomCommand SetAlternateHeaders::GetRoomCommand()
{
	return RoomCommand::SetAlternateHeaders;
}