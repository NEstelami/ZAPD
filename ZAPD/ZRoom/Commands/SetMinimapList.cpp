#include "SetMinimapList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetMinimapList::SetMinimapList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	listSegmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, segmentOffset + 0));
	unk4 = BitConverter::ToInt32BE(rawData, segmentOffset + 4);

	ParseRawData();
	DeclareReferences();
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

void SetMinimapList::DeclareReferences()
{
	std::string declaration = StringHelper::Sprintf("(u32)%sMinimapEntryList0x%06X, 0x%08X",
		                                           zRoom->GetName().c_str(), listSegmentOffset, unk4);

	zRoom->parent->AddDeclaration(
		segmentOffset, DeclarationAlignment::Align4, 8, "MinimapList",
		StringHelper::Sprintf("%sMinimapList0x%06X", zRoom->GetName().c_str(), segmentOffset),
		declaration);


	declaration = "";

	size_t index = 0;
	for (const auto& entry : minimaps)
	{
		declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode());

		if (index < minimaps.size() - 1)
			declaration += "\n";

		index++;
	}

	zRoom->parent->AddDeclarationArray(
		listSegmentOffset, DeclarationAlignment::Align4,
		minimaps.size() * 10, "MinimapEntry",
		StringHelper::Sprintf("%sMinimapEntryList0x%06X", zRoom->GetName().c_str(), listSegmentOffset),
		minimaps.size(), declaration);
}

/*
string SetMinimapList::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x%02X, (u32)&%sMinimapList0x%06X",
		ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), minimaps.size(),
		zRoom->GetName().c_str(), segmentOffset);
}
*/

string SetMinimapList::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s 0, (u32)&%sMinimapList0x%06X",
	                          GetCommandHex().c_str(),
	                          zRoom->GetName().c_str(), segmentOffset, unk4);
}

string SetMinimapList::GenerateExterns()
{
	return StringHelper::Sprintf("extern MinimapList %sMinimapList0x%06X;\n",
	                             zRoom->GetName().c_str(), listSegmentOffset);
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
