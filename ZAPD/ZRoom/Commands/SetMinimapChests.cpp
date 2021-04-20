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
	int numChests = rawData[rawDataIndex + 1];

	int32_t currentPtr = segmentOffset;

	for (int i = 0; i < numChests; i++)
	{
		MinimapChest* chest = new MinimapChest(rawData, currentPtr);
		chests.push_back(chest);

		currentPtr += 10;
	}
}

SetMinimapChests::~SetMinimapChests()
{
	for (MinimapChest* chest : chests)
		delete chest;
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

	return StringHelper::Sprintf("%s, 0x%02X, (u32)%s };", GetCommandHex().c_str(), chests.size(), listName.c_str());
}

string SetMinimapChests::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	return "";
}

string SetMinimapChests::GenerateSourceCodePass2(string roomName, int baseAddress)
{
	string declaration = "";

	size_t index = 0;
	for (MinimapChest* chest : chests)
	{
		declaration += StringHelper::Sprintf("    { 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X },",
												chest->unk0, chest->unk2, chest->unk4, chest->unk6,
												chest->unk8);

		if (index < chests.size() - 1)
			declaration += "\n";

		index++;
	}

	zRoom->parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, DeclarationPadding::None, chests.size() * 10,
		"MinimapChest",
		StringHelper::Sprintf("%sMinimapChests0x%06X", roomName.c_str(), segmentOffset),
		chests.size(), declaration);

	return GetBodySourceCode();
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