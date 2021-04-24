#include "SetMinimapChests.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetMinimapChests::SetMinimapChests(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

void SetMinimapChests::ParseRawData()
{
	int numChests = cmdArg1;

	int32_t currentPtr = segmentOffset;

	for (int i = 0; i < numChests; i++)
	{
		MinimapChest chest(rawData, currentPtr);
		chests.push_back(chest);

		currentPtr += 10;
	}
}

void SetMinimapChests::DeclareReferences(const std::string& prefix)
{
	string declaration = "";

	size_t index = 0;
	for (const auto& chest : chests)
	{
		declaration += StringHelper::Sprintf("    { %s },", chest.GetBodySourceCode().c_str());

		if (index < chests.size() - 1)
			declaration += "\n";

		index++;
	}

	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, chests.size() * 10,
		"MinimapChest",
		StringHelper::Sprintf("%sMinimapChests0x%06X", prefix.c_str(), segmentOffset),
		chests.size(), declaration);
}

std::string SetMinimapChests::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENE_CMD_MINIMAP_COMPASS_ICON_INFO(0x%02X, %s)", chests.size(), listName.c_str());
}

string SetMinimapChests::GetCommandCName()
{
	return "SCmdMinimapChests";
}

RoomCommand SetMinimapChests::GetRoomCommand()
{
	return RoomCommand::SetMinimapChests;
}

int32_t SetMinimapChests::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (chests.size() * 10);
}

MinimapChest::MinimapChest(const std::vector<uint8_t>& rawData, int rawDataIndex)
	: unk0(BitConverter::ToUInt16BE(rawData, rawDataIndex + 0)),
	  unk2(BitConverter::ToUInt16BE(rawData, rawDataIndex + 2)),
	  unk4(BitConverter::ToUInt16BE(rawData, rawDataIndex + 4)),
	  unk6(BitConverter::ToUInt16BE(rawData, rawDataIndex + 6)),
	  unk8(BitConverter::ToUInt16BE(rawData, rawDataIndex + 8))
{
}

std::string MinimapChest::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X", unk0, unk2, unk4, unk6, unk8);
}
