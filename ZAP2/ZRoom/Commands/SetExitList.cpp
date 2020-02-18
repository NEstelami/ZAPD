#include "SetExitList.h"
#include "../ZRoom.h"
#include "../../BitConverter.h"

using namespace std;

SetExitList::SetExitList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;
	exits = vector<uint16_t>();

	if (segmentOffset != 0)
		zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, 0, "");

	_rawData = rawData;
	_rawDataIndex = rawDataIndex;
}

string SetExitList::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x00, (u32)&_%s_exitList_%08X };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), zRoom->GetName().c_str(), segmentOffset);
	sourceOutput = line;

	// Parse Entrances and Generate Declaration
	zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, 0, ""); // Make sure this segment is defined
	int numEntrances = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 2;
	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < numEntrances; i++)
	{
		uint16_t exit = BitConverter::ToInt16BE(_rawData, currentPtr);
		exits.push_back(exit);

		currentPtr += 2;
	}

	string declaration = "";

	sprintf(line, "u16 _%s_exitList_%08X[] = \n{\n", zRoom->GetName().c_str(), segmentOffset);
	declaration += line;

	for (uint16_t exit : exits)
	{
		sprintf(line, "\t0x%04X,\n", exit);
		declaration += line;
	}

	declaration += "};\n";

	zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, exits.size() * 4, declaration);

	return sourceOutput;
}

string SetExitList::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "extern u16 _%s_exitList_%08X[];\n", zRoom->GetName().c_str(), segmentOffset);
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