#include "SetEntranceList.h"
#include "SetStartPositionList.h"
#include "../ZRoom.h"
#include "../../BitConverter.h"

using namespace std;

SetEntranceList::SetEntranceList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;
	entrances = vector<EntranceEntry*>();

	_rawData = rawData;
	_rawDataIndex = rawDataIndex;
}

string SetEntranceList::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, (u32)&entranceList_%08X };", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), segmentOffset);
	sourceOutput = line;

	// Parse Entrances and Generate Declaration
	zRoom->declarations[segmentOffset] = ""; // Make sure this segment is defined
	int numEntrances = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 2;
	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < numEntrances; i++)
	{
		EntranceEntry* entry = new EntranceEntry(_rawData, currentPtr);
		entrances.push_back(entry);

		currentPtr += 2;
	}

	string declaration = "";

	sprintf(line, "EntranceEntry entranceList_%08X[] = \n{\n", segmentOffset);
	declaration += line;

	for (EntranceEntry* entry : entrances)
	{
		sprintf(line, "\t{ %02X, %02X },\n", entry->startPositionIndex, entry->roomToLoad);
		declaration += line;
	}

	declaration += "};\n";

	zRoom->declarations[segmentOffset] = declaration;

	return sourceOutput;
}

string SetEntranceList::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "extern EntranceEntry entranceList_%08X[];\n", segmentOffset);
	sourceOutput = line;

	return sourceOutput;
}

string SetEntranceList::GetCommandCName()
{
	return "SCmdEntranceList";
}

RoomCommand SetEntranceList::GetRoomCommand()
{
	return RoomCommand::SetEntranceList;
}

EntranceEntry::EntranceEntry(std::vector<uint8_t> rawData, int rawDataIndex)
{
	startPositionIndex = rawData[rawDataIndex + 0];
	roomToLoad = rawData[rawDataIndex + 1];
}