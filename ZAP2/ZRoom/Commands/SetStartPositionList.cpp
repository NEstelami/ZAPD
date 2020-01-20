#include "SetStartPositionList.h"
#include "../ZRoom.h"
#include "../ActorList.h"
#include "../../BitConverter.h"

using namespace std;

SetStartPositionList::SetStartPositionList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
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

string SetStartPositionList::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x%02X, (u32)&startPositionList_%08X};", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), actors.size(), segmentOffset);
	sourceOutput = line;

	string declaration = "";

	sprintf(line, "ActorSpawnEntry startPositionList_%08X[] = \n{\n", segmentOffset);
	declaration += line;

	for (ActorSpawnEntry* entry : actors)
	{
		sprintf(line, "\t{ %s, %i, %i, %i, %i, %i, %i, 0x%04X },\n", ActorList[entry->actorNum].c_str(), entry->posX, entry->posY, entry->posZ, 
			entry->rotX, entry->rotY, entry->rotZ, entry->initVar);
		declaration += line;
	}

	declaration += "};\n";

	zRoom->declarations[segmentOffset] = declaration;

	return sourceOutput;
}

string SetStartPositionList::GenerateSourceCodePass2(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	return sourceOutput;
}

string SetStartPositionList::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "extern ActorSpawnEntry startPositionList_%08X[];\n", segmentOffset);
	sourceOutput = line;

	return sourceOutput;
}

string SetStartPositionList::GetCommandCName()
{
	return "SCmdSpawnList";
}

RoomCommand SetStartPositionList::GetRoomCommand()
{
	return RoomCommand::SetStartPositionList;
}