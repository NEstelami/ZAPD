#include "SetEntranceList.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"
#include "SetStartPositionList.h"

using namespace std;

SetEntranceList::SetEntranceList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4));
}

SetEntranceList::~SetEntranceList()
{
	for (EntranceEntry* entry : entrances)
		delete entry;
}

std::string SetEntranceList::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s, 0x00, (u32)&%sEntranceList0x%06X", GetCommandHex().c_str(), zRoom->GetName().c_str(), segmentOffset);
}

string SetEntranceList::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	// Parse Entrances and Generate Declaration
	zRoom->parent->AddDeclarationPlaceholder(segmentOffset); // Make sure this segment is defined
	int numEntrances = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 2;
	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < numEntrances; i++)
	{
		EntranceEntry* entry = new EntranceEntry(rawData, currentPtr);
		entrances.push_back(entry);

		currentPtr += 2;
	}

	string declaration = "";

	int index = 0;

	for (EntranceEntry* entry : entrances)
	{
		declaration +=
			StringHelper::Sprintf("    { 0x%02X, 0x%02X }, //0x%06X \n", entry->startPositionIndex,
		                          entry->roomToLoad, segmentOffset + (index * 2));
		index++;
	}

	zRoom->parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, entrances.size() * 2, "EntranceEntry",
		StringHelper::Sprintf("%sEntranceList0x%06X", zRoom->GetName().c_str(), segmentOffset),
		entrances.size(), declaration);

	return GetBodySourceCode();
}

string SetEntranceList::GenerateExterns()
{
	return StringHelper::Sprintf("extern EntranceEntry %sEntranceList0x%06X[];\n",
	                             zRoom->GetName().c_str(), segmentOffset);
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