#include "SetMesh.h"
#include "../../BitConverter.h"

using namespace std;

SetMesh::SetMesh(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	int numActors = rawData[rawDataIndex + 1];
	uint32_t segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;

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

		sprintf(line, "SetMesh0 0x%02X, ", meshHeader0->entries.size());
		sourceOutput += line;

		if (meshHeader0->dListStart != 0)
			sprintf(line, "MeshDListStart_%08X, ", meshHeader0->dListStart);
		else
			sprintf(line, "0, ");

		sourceOutput += line;

		if (meshHeader0->dListStart != 0)
			sprintf(line, "MeshDListEnd_%08X\n", meshHeader0->dListEnd);
		else
			sprintf(line, "0\n");

		sourceOutput += line;
	}
	else
	{
		sourceOutput += "; SetMesh UNIMPLEMENTED HEADER TYPE!\n";
	}

	return sourceOutput;
}


string SetMesh::GenerateSourceCodePass2(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	if (meshHeader->headerType == 0)
	{
		MeshHeader0* meshHeader0 = (MeshHeader0*)meshHeader;

		if (meshHeader0->dListStart != 0)
		{
			sprintf(line, "MeshDListStart_%08X:\n", meshHeader0->dListStart);
			sourceOutput += line;
		}

		for (int i = 0; i < meshHeader0->entries.size(); i++)
		{
			MeshEntry0* entry = meshHeader0->entries[i];

			sprintf(line, ".word MeshOpaque_%08X\n.word MeshTranslucent_%08X\n", entry->opaqueDList, entry->translucentDList);
			sourceOutput += line;
		}
	}

	return sourceOutput;
}

string SetMesh::GenerateSourceCodePass3(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	if (meshHeader->headerType == 0)
	{
		MeshHeader0* meshHeader0 = (MeshHeader0*)meshHeader;

	}

	return sourceOutput;
}

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