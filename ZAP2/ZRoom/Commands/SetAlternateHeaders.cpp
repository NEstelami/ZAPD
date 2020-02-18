#include "SetAlternateHeaders.h"
#include "../../BitConverter.h"

using namespace std;

SetAlternateHeaders::SetAlternateHeaders(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;

	if (segmentOffset != 0)
		zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, 0, "");
	
	_rawData = rawData;
	_rawDataIndex = rawDataIndex;
}

string SetAlternateHeaders::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	int numHeaders = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 4;

	for (int i = 0; i < numHeaders; i++)
	{
		int32_t address = BitConverter::ToInt32BE(_rawData, segmentOffset + (i * 4));
		headers.push_back(address);

		if (address != 0)
		{
			zRoom->commandSets.push_back(address);
		}
	}

	sprintf(line, "%s 0, (u32)&_%s_alternateHeaders_%08X};", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), roomName.c_str(), segmentOffset);
	sourceOutput += line;

	string declaration = "";

	sprintf(line, "u32 _%s_alternateHeaders_%08X[] = \n{\n", roomName.c_str(), segmentOffset);
	declaration += line;

	for (int i = 0; i < numHeaders; i++)
	{
		//sprintf(line, "\t0x%08X,\n", headers[i]);

		if (headers[i] == 0)
			sprintf(line, "\t0,\n");
		else
			sprintf(line, "\t(u32)&_%s_set%04X_cmd00,\n", roomName.c_str(), headers[i] & 0x00FFFFFF);
		
		declaration += line;
	}

	declaration += "};\n";

	zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, 0, declaration);

	declaration = "";

	sprintf(line, "extern u32 _%s_alternateHeaders_%08X[];\n", roomName.c_str(), segmentOffset);
	declaration += line;

	zRoom->externs[segmentOffset] = declaration;

	return sourceOutput;
}

int32_t SetAlternateHeaders::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + 0;
}

string SetAlternateHeaders::GetCommandCName()
{
	return "SCmdAltHeaders";
}

RoomCommand SetAlternateHeaders::GetRoomCommand()
{
	return RoomCommand::SetAlternateHeaders;
}