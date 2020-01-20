#include "SetMesh.h"
#include "../ZRoom.h"
#include "../../BitConverter.h"

using namespace std;

SetMesh::SetMesh(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;

	string declaration = "";
	char line[2048];

	int8_t meshHeaderType = rawData[segmentOffset + 0];

	if (meshHeaderType == 0)
	{
		MeshHeader0* meshHeader0 = new MeshHeader0();
		meshHeader0->headerType = 0;
		meshHeader0->entries = vector<MeshEntry0*>();

		meshHeader0->dListStart = BitConverter::ToInt32BE(rawData, segmentOffset + 4) & 0x00FFFFFF;
		meshHeader0->dListEnd = BitConverter::ToInt32BE(rawData, segmentOffset + 8) & 0x00FFFFFF;

		int8_t numEntries = rawData[segmentOffset + 1];

		uint32_t currentPtr = meshHeader0->dListStart;

		for (int i = 0; i < numEntries; i++)
		{
			MeshEntry0* entry = new MeshEntry0();
			entry->opaqueDList = BitConverter::ToInt32BE(rawData, currentPtr + 0) & 0x00FFFFFF;
			entry->translucentDList = BitConverter::ToInt32BE(rawData, currentPtr + 4) & 0x00FFFFFF;

			meshHeader0->entries.push_back(entry);

			currentPtr += 8;
		}

		
		sprintf(line, "MeshHeader0 meshHeader_%08X = { { 0 }, 0x%02X, ", segmentOffset, meshHeader0->entries.size());
		declaration += line;

		if (meshHeader0->dListStart != 0)
			sprintf(line, "meshDListStart_%08X, ", meshHeader0->dListStart);
		else
			sprintf(line, "0, ");

		declaration += line;

		if (meshHeader0->dListStart != 0)
			sprintf(line, "meshDListEnd_%08X };", meshHeader0->dListEnd);
		else
			sprintf(line, "0 };");

		declaration += line;

		zRoom->declarations[segmentOffset] = declaration;

		meshHeader = meshHeader0;
	}
	else if (meshHeaderType == 1)
	{
		MeshHeader1Base* meshHeader1 = new MeshHeader1Base();
		meshHeader1->headerType = 1;

		meshHeader = meshHeader1;
	}
	else if (meshHeaderType == 2)
	{
		MeshHeader2* meshHeader2 = new MeshHeader2();
		meshHeader2->headerType = 2;

		meshHeader = meshHeader2;
	}
}

string SetMesh::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	if (meshHeader->headerType == 0)
	{
		MeshHeader0* meshHeader0 = (MeshHeader0*)meshHeader;

	}
	else
	{
		sourceOutput += "// SetMesh UNIMPLEMENTED HEADER TYPE!\n";
	}

	return sourceOutput;
}


//string SetMesh::GenerateSourceCodePass2(string roomName)
//{
//	string sourceOutput = "";
//	char line[2048];
//
//	if (meshHeader->headerType == 0)
//	{
//		MeshHeader0* meshHeader0 = (MeshHeader0*)meshHeader;
//
//		if (meshHeader0->dListStart != 0)
//		{
//			sprintf(line, "MeshDListStart_%08X:\n", meshHeader0->dListStart);
//			sourceOutput += line;
//		}
//
//		for (int i = 0; i < meshHeader0->entries.size(); i++)
//		{
//			MeshEntry0* entry = meshHeader0->entries[i];
//
//			sprintf(line, ".word MeshOpaque_%08X\n.word MeshTranslucent_%08X\n", entry->opaqueDList, entry->translucentDList);
//			sourceOutput += line;
//		}
//	}
//
//	return sourceOutput;
//}

//string SetMesh::GenerateSourceCodePass3(string roomName)
//{
//	string sourceOutput = "";
//	char line[2048];
//
//	if (meshHeader->headerType == 0)
//	{
//		MeshHeader0* meshHeader0 = (MeshHeader0*)meshHeader;
//
//	}
//
//	return sourceOutput;
//}

int32_t SetMesh::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize();
}

string SetMesh::GetCommandCName()
{
	return "SCmdMesh";
}

RoomCommand SetMesh::GetRoomCommand()
{
	return RoomCommand::SetMesh;
}