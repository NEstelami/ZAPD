#include "SetObjectList.h"
#include "../ObjectList.h"
#include "../../BitConverter.h"

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
}

string SetObjectList::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetObjectList 0x%02X ObjectList_%08X\n", objects.size(), segmentOffset);
	sourceOutput += line;

	return sourceOutput;
}

string SetObjectList::GenerateSourceCodePass2(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "ObjectList_%08X:\n", segmentOffset);
	sourceOutput += line;
	
	for (uint16_t objectIndex : objects)
	{
		//sprintf(line, ".short 0x%04X\n", objectIndex);
		sprintf(line, ".short %s\n", ObjectList[objectIndex].c_str());
		sourceOutput += line;
	}

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