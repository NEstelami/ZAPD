#include "SetMinimapList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetMinimapList::SetMinimapList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	listSegmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, segmentOffset + 0));
	unk4 = BitConverter::ToInt32BE(rawData, segmentOffset + 4);
}

void SetMinimapList::ParseRawData()
{
	int32_t currentPtr = listSegmentOffset;

	for (int i = 0; i < zRoom->roomCount; i++)
	{
		MinimapEntry entry(rawData, currentPtr);
		minimaps.push_back(entry);

		currentPtr += 10;
	}
}

void SetMinimapList::DeclareReferences(const std::string& prefix)
{
	{
		std::string declaration = "";

		size_t index = 0;
		for (const auto& entry : minimaps)
		{
			declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode().c_str());

			if (index < minimaps.size() - 1)
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(
			listSegmentOffset, DeclarationAlignment::Align4,
			minimaps.size() * 10, "MinimapEntry",
			StringHelper::Sprintf("%sMinimapEntryList0x%06X", prefix.c_str(), listSegmentOffset),
			minimaps.size(), declaration);
	}


	{
		std::string listName = parent->GetDeclarationPtrName(listSegmentOffset);

		std::string declaration = StringHelper::Sprintf("(u32)%s, 0x%08X",
													listName.c_str(), unk4);

		parent->AddDeclaration(
			segmentOffset, DeclarationAlignment::Align4, 8, "MinimapList",
			StringHelper::Sprintf("%sMinimapList0x%06X", prefix.c_str(), segmentOffset),
			declaration);

	}
}

string SetMinimapList::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENE_CMD_MINIMAP_INFO(%s)", listName.c_str());
}


string SetMinimapList::GetCommandCName()
{
	return "SCmdMinimapSettings";
}

RoomCommand SetMinimapList::GetRoomCommand()
{
	return RoomCommand::SetMinimapList;
}

int32_t SetMinimapList::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (minimaps.size() * 10);
}

MinimapEntry::MinimapEntry(const std::vector<uint8_t>& rawData, int rawDataIndex)
	: unk0(BitConverter::ToUInt16BE(rawData, rawDataIndex + 0)),
	  unk2(BitConverter::ToUInt16BE(rawData, rawDataIndex + 2)),
	  unk4(BitConverter::ToUInt16BE(rawData, rawDataIndex + 4)),
	  unk6(BitConverter::ToUInt16BE(rawData, rawDataIndex + 6)),
	  unk8(BitConverter::ToUInt16BE(rawData, rawDataIndex + 8))
{
}

std::string MinimapEntry::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X", unk0, unk2, unk4, unk6, unk8);
}
