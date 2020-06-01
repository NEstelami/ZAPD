#include "SetObjectList.h"
#include "../ZRoom.h"
#include "../ObjectList.h"
#include "../../ZFile.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetObjectList::SetObjectList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	objects = vector<uint16_t>();
	uint8_t objectCnt = rawData[rawDataIndex + 1];
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;
	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < objectCnt; i++)
	{
		uint16_t objectIndex = BitConverter::ToInt16BE(rawData, currentPtr);
		objects.push_back(objectIndex);
		currentPtr += 2;
	}

	if (segmentOffset != 0)
		zRoom->parent->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, 0, "");
}

string SetObjectList::GenerateExterns()
{
	return StringHelper::Sprintf("s16 _%s_objectList_%08X[];\n", zRoom->GetName().c_str(), segmentOffset);
}

string SetObjectList::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";

	sourceOutput += StringHelper::Sprintf("%s 0x%02X, (u32)_%s_objectList_%08X };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), objects.size(), zRoom->GetName().c_str(), segmentOffset);

	string declaration = "";
	declaration += StringHelper::Sprintf("s16 _%s_objectList_%08X[] = \n{\n", zRoom->GetName().c_str(), segmentOffset);;

	for (uint16_t objectIndex : objects)
		declaration += StringHelper::Sprintf("\t%s,\n", ObjectList[objectIndex].c_str());

	declaration += "};\n";
	zRoom->parent->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, objects.size() * 2, declaration);

	return sourceOutput;
}

int32_t SetObjectList::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (objects.size() * 2);
}

string SetObjectList::GetCommandCName()
{
	return "SCmdObjectList";
}

RoomCommand SetObjectList::GetRoomCommand()
{
	return RoomCommand::SetObjectList;
}