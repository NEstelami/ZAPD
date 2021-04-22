#include "SetLightList.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetLightList::SetLightList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	numLights = cmdArg1;

	ParseRawData();
}

void SetLightList::ParseRawData()
{
	string declarations = "";

	int32_t currentPtr = segmentOffset;
	for (int i = 0; i < this->numLights; i++)
	{
		uint8_t type = rawData[currentPtr + 0];
		int16_t x = BitConverter::ToInt16BE(rawData, currentPtr + 2);
		int16_t y = BitConverter::ToInt16BE(rawData, currentPtr + 4);
		int16_t z = BitConverter::ToInt16BE(rawData, currentPtr + 6);
		uint8_t r = rawData[currentPtr + 8];
		uint8_t g = rawData[currentPtr + 9];
		uint8_t b = rawData[currentPtr + 10];
		uint8_t drawGlow = rawData[currentPtr + 11];
		int16_t radius = BitConverter::ToInt16BE(rawData, currentPtr + 12);

		currentPtr += 14;

		declarations += StringHelper::Sprintf(
			"    { 0x%02X, { %i, %i, %i, { 0x%02X, 0x%02X, 0x%02X }, 0x%02X, 0x%04X } },", type, x,
			y, z, r, g, b, drawGlow, radius);

		if (i < this->numLights - 1)
			declarations += "\n";
	}

	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, this->numLights * 0xE, "LightInfo",
		StringHelper::Sprintf("%sLightInfo0x%06X", zRoom->GetName().c_str(), segmentOffset),
		this->numLights, declarations);
}

std::string SetLightList::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("%s, %i, %s", GetCommandHex().c_str(), numLights, listName.c_str());
}

string SetLightList::GetCommandCName()
{
	return "SCmdLightList";
}

string SetLightList::GenerateExterns()
{
	return StringHelper::Sprintf("extern LightInfo %sLightInfo0x%06X[];\n",
	                             zRoom->GetName().c_str(), segmentOffset);
}

RoomCommand SetLightList::GetRoomCommand()
{
	return RoomCommand::SetLightList;
}
