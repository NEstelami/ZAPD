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

string SetEntranceList::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, (u32)&_%s_entranceList_%08X };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), zRoom->GetName().c_str(), segmentOffset);
	sourceOutput = line;

	// Parse Entrances and Generate Declaration
	zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, 0, ""); // Make sure this segment is defined
	int numEntrances = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 2;
	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < numEntrances; i++)
	{
		EntranceEntry* entry = new EntranceEntry(_rawData, currentPtr);
		entrances.push_back(entry);

		currentPtr += 2;
	}

	string declaration = "";

	sprintf(line, "EntranceEntry _%s_entranceList_%08X[] = \n{\n", zRoom->GetName().c_str(), segmentOffset);
	declaration += line;

	int index = 0;

	for (EntranceEntry* entry : entrances)
	{
		sprintf(line, "\t{ 0x%02X, 0x%02X }, //0x%08X \n", entry->startPositionIndex, entry->roomToLoad, segmentOffset + (index * 2));
		declaration += line;

		index++;
	}

	declaration += "};\n";

	zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, entrances.size() * 4, declaration);

	return sourceOutput;
}

string SetEntranceList::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "extern EntranceEntry _%s_entranceList_%08X[];\n", zRoom->GetName().c_str(), segmentOffset);
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