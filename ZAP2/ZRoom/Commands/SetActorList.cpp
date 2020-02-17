#include "SetActorList.h"
#include "../ZRoom.h"
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

string SetActorList::GetSourceOutputCode(std::string prefix)
{
	string sourceOutput = "";
	char line[2048];


	return "";
}

string SetActorList::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x%02X, (u32)_%s_actorList_%08X };", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), actors.size(), roomName.c_str(), segmentOffset);
	sourceOutput += line;

	string declaration = "";
	sprintf(line, "ActorEntry _%s_actorList_%08X[] = \n{\n", roomName.c_str(), segmentOffset);
	declaration += line;

	for (ActorSpawnEntry* entry : actors)
	{
		sprintf(line, "\t{ %s, %i, %i, %i, %i, %i, %i, 0x%04X }, \n", ActorList[entry->actorNum].c_str(), entry->posX, entry->posY, entry->posZ, entry->rotX, entry->rotY, entry->rotZ, (uint16_t)entry->initVar);
		declaration += line;
	}

	declaration += "};\n\n";

	zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, actors.size() * 16, declaration);

	return sourceOutput;
}


string SetActorList::GenerateSourceCodePass2(string roomName)
{
	return "";
}

int32_t SetActorList::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (actors.size() * 16);
}

string SetActorList::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "ActorEntry _%s_actorList_%08X[];\n", zRoom->GetName().c_str(), segmentOffset);
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