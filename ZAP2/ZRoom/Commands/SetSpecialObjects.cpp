#include "SetSpecialObjects.h"
#include "../../BitConverter.h"

using namespace std;

SetSpecialObjects::SetSpecialObjects(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	elfMessage = rawData[rawDataIndex + 0x01];
	globalObject = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
}

string SetSpecialObjects::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x%02X, 0x%04X};", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), elfMessage, globalObject);
	sourceOutput = line;

	return sourceOutput;
}

string SetSpecialObjects::GetCommandCName()
{
	return "SCmdSpecialFiles";
}

RoomCommand SetSpecialObjects::GetRoomCommand()
{
	return RoomCommand::SetSpecialObjects;
}