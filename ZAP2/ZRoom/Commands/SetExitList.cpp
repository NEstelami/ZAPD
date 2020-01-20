#include "SetExitList.h"
#include "../../BitConverter.h"

using namespace std;

SetExitList::SetExitList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	int32_t segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);

	exits = vector<uint16_t>();

	/*for (int i = 0; i < numActors; i++)
	{
		ActorSpawnEntry* entry = new ActorSpawnEntry(rawData, (rawDataIndex + actorListPtr) + (i * 16));
		actors.push_back(entry);
	}*/
}

string SetExitList::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	//sprintf(line, "SetActorList 0x%08X, 0x%08X\n", cameraMovement, mapHighlight);
	sourceOutput = line;

	return sourceOutput;
}

string SetExitList::GetCommandCName()
{
	return "SCmdExitList";
}

RoomCommand SetExitList::GetRoomCommand()
{
	return RoomCommand::SetExitList;
}

//SetEntranceList::SetEntranceList(std::vector<uint8_t> rawData, int rawDataIndex)
//{
//	actorNum = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
//	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
//	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
//	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
//	rotX = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
//	rotY = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
//	rotZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
//	initVar = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
//}