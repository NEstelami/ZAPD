#include "SetSpecialObjects.h"
#include "../../BitConverter.h"

using namespace std;

SetSpecialObjects::SetSpecialObjects(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	elfMessage = rawData[rawDataIndex + 0x01];
	globalObject = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
}

string SetSpecialObjects::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetSpecialObjects 0x%08X, 0x%16X\n", elfMessage, globalObject);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetSpecialObjects::GetRoomCommand()
{
	return RoomCommand::SetSpecialObjects;
}