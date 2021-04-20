#include "SetMinimapChests.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetMinimapChests::SetMinimapChests(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	ParseRawData();
	DeclareReferences();
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

void SetMinimapChests::DeclareReferences()
{
	string declaration = "";

	size_t index = 0;
	for (const auto& chest : chests)
	{
		declaration += StringHelper::Sprintf("    { %s },", chest.GetBodySourceCode());

		if (index < chests.size() - 1)
			declaration += "\n";

		index++;
	}

	zRoom->parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, DeclarationPadding::None, chests.size() * 10,
		"MinimapChest",
		StringHelper::Sprintf("%sMinimapChests0x%06X", zRoom->GetName().c_str(), segmentOffset),
		chests.size(), declaration);
}

std::string SetMinimapChests::GetBodySourceCode()
{
	std::string listName = "NULL";
	if (segmentOffset != 0)
	{
		Declaration* decl = parent->GetDeclaration(segmentOffset);
		if (decl != nullptr)
		{
			listName = "&" + decl->varName;
		}
		else
		{
			listName = StringHelper::Sprintf("0x%08X", segmentOffset);
		}
	}

	return StringHelper::Sprintf("%s, 0x%02X, (u32)%s", GetCommandHex().c_str(), chests.size(), listName.c_str());
}

string SetMinimapChests::GenerateExterns()
{
	return StringHelper::Sprintf("extern MinimapChest %sMinimapChests0x%06X[%i];\n",
	                             zRoom->GetName().c_str(), segmentOffset, chests.size());
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

MinimapChest::MinimapChest(std::vector<uint8_t> rawData, int rawDataIndex)
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
