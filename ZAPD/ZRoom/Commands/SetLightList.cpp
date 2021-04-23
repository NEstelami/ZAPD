#include "SetLightList.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetLightList::SetLightList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	numLights = cmdArg1;

	ParseRawData();
	DeclareReferences(zRoom->GetName());
}

void SetLightList::ParseRawData()
{
	string declarations = "";

	int32_t currentPtr = segmentOffset;
	for (int i = 0; i < this->numLights; i++)
	{
		LightInfo light(rawData, currentPtr);

		currentPtr += light.GetRawDataSize();
		lights.push_back(light);
	}
}

void SetLightList::DeclareReferences(const std::string& prefix)
{
	if (!lights.empty())
	{
		std::string declarations = "";

		for (int i = 0; i < lights.size(); i++)
		{
			declarations += StringHelper::Sprintf("\t{ %s },", lights.at(i).GetBodySourceCode().c_str());

			if (i < lights.size() - 1)
				declarations += "\n";
		}

		const auto& light = lights.front();

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::None, lights.size() * light.GetRawDataSize(), light.GetSourceTypeName(),
			StringHelper::Sprintf("%sLightInfo0x%06X", prefix.c_str(), segmentOffset),
			lights.size(), declarations);
	}
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

RoomCommand SetLightList::GetRoomCommand()
{
	return RoomCommand::SetLightList;
}

LightInfo::LightInfo(const std::vector<uint8_t>& rawData, int rawDataIndex)
{
	type = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0);
	x = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	y = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	z = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	r = BitConverter::ToUInt8BE(rawData, rawDataIndex + 8);
	g = BitConverter::ToUInt8BE(rawData, rawDataIndex + 9);
	b = BitConverter::ToUInt8BE(rawData, rawDataIndex + 10);
	drawGlow = BitConverter::ToUInt8BE(rawData, rawDataIndex + 11);
	radius = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
}

std::string LightInfo::GetBodySourceCode() const
{
	return StringHelper::Sprintf(
				"0x%02X, { %i, %i, %i, { 0x%02X, 0x%02X, 0x%02X }, 0x%02X, 0x%04X }", type, x,
				y, z, r, g, b, drawGlow, radius);
}

std::string LightInfo::GetSourceTypeName() const
{
	return "LightInfo";
}

size_t LightInfo::GetRawDataSize() const
{
	return 0x0E;
}
