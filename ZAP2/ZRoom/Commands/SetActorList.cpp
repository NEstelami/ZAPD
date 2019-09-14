#include "SetActorList.h"
#include "../../BitConverter.h"

using namespace std;

SetActorList::SetActorList(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
{
	int numActors = rawData[rawDataIndex + 1];
	int actorListPtr = BitConverter::ToInt32BE(rawData, rawDataIndex + 3) & 0x00FFFFFF;

	actors = vector<ActorSpawnEntry*>();

	for (int i = 0; i < numActors; i++)
	{
		ActorSpawnEntry* entry = new ActorSpawnEntry(rawData, (rawDataIndex + actorListPtr) + (i * 16));
		actors.push_back(entry);
	}
}

string SetActorList::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	//sprintf(line, "SetActorList 0x%08X, 0x%08X\n", cameraMovement, mapHighlight);
	sourceOutput = line;

	return sourceOutput;
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