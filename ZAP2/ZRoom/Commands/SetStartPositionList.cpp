#include "SetStartPositionList.h"
#include "../../BitConverter.h"

using namespace std;

SetStartPositionList::SetStartPositionList(std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(rawData, rawDataIndex)
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

string SetStartPositionList::GenerateSourceCode()
{
	string sourceOutput = "";
	char line[2048];

	//sprintf(line, "SetActorList 0x%08X, 0x%08X\n", cameraMovement, mapHighlight);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetStartPositionList::GetRoomCommand()
{
	return RoomCommand::SetStartPositionList;
}