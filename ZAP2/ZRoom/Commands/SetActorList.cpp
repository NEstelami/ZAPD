#include "SetActorList.h"
#include "../ActorList.h"
#include "../../BitConverter.h"

using namespace std;

SetActorList::SetActorList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	int numActors = rawData[rawDataIndex + 1];
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;

	actors = vector<ActorSpawnEntry*>();

	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < numActors; i++)
	{
		ActorSpawnEntry* entry = new ActorSpawnEntry(rawData, currentPtr);
		actors.push_back(entry);

		currentPtr += 16;
	}
}

string SetActorList::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "SetActorList 0x%02X ActorList_%08X\n", actors.size(), segmentOffset);
	sourceOutput += line;

	return sourceOutput;
}


string SetActorList::GenerateSourceCodePass2(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "ActorList_%08X:\n", segmentOffset);
	sourceOutput += line;

	for (ActorSpawnEntry* entry : actors)
	{
		sprintf(line, "ActorSpawnEntry %s, %i, %i, %i, %i, %i, %i, 0x%04X\n", ActorList[entry->actorNum].c_str(), entry->posX, entry->posY, entry->posZ, entry->rotX, entry->rotY, entry->rotZ, (uint16_t)entry->initVar);
		sourceOutput += line;
	}

	return sourceOutput;
}

int32_t SetActorList::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (actors.size() * 16);
}

string SetActorList::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "const ActorSpawnEntry ActorList_%08X[];\n", segmentOffset);
	sourceOutput = line;

	return sourceOutput;
}

string SetActorList::GetCommandCName()
{
	return "SCmdActorList";
}

RoomCommand SetActorList::GetRoomCommand()
{
	return RoomCommand::SetActorList;
}

ActorSpawnEntry::ActorSpawnEntry(std::vector<uint8_t> rawData, int rawDataIndex)
{
	actorNum = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	rotX = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	rotY = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	rotZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
	initVar = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
}