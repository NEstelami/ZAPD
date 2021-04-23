#include "SetEntranceList.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"
#include "SetStartPositionList.h"

using namespace std;

SetEntranceList::SetEntranceList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	parent->AddDeclarationPlaceholder(segmentOffset); // Make sure this segment is defined
}

void SetEntranceList::ParseRawDataLate()
{
	// Parse Entrances and Generate Declaration
	int numEntrances = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 2;
	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < numEntrances; i++)
	{
		EntranceEntry entry(rawData, currentPtr);
		entrances.push_back(entry);

		currentPtr += 2;
	}
}

void SetEntranceList::DeclareReferencesLate(const std::string& prefix)
{
	if (!entrances.empty()) {
		string declaration = "";

		int index = 0;

		for (const auto& entry : entrances)
		{
			declaration += StringHelper::Sprintf("    { %s }, //0x%06X", entry.GetBodySourceCode().c_str(), segmentOffset + (index * 2));
			if (index + 1 < entrances.size())
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::None, entrances.size() * 2, "EntranceEntry",
			StringHelper::Sprintf("%sEntranceList0x%06X", zRoom->GetName().c_str(), segmentOffset),
			entrances.size(), declaration);
	}
}

std::string SetEntranceList::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("%s, 0x00, (u32)%s", GetCommandHex().c_str(), listName.c_str());
}

string SetEntranceList::GetCommandCName()
{
	return "SCmdEntranceList";
}

RoomCommand SetEntranceList::GetRoomCommand()
{
	return RoomCommand::SetEntranceList;
}

EntranceEntry::EntranceEntry(const std::vector<uint8_t>& rawData, int rawDataIndex)
{
	startPositionIndex = rawData.at(rawDataIndex + 0);
	roomToLoad = rawData.at(rawDataIndex + 1);
}

std::string EntranceEntry::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%02X, 0x%02X", startPositionIndex, roomToLoad);
}
