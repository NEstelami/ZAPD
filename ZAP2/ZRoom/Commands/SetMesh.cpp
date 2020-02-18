#include "SetMesh.h"
#include "../ZRoom.h"
#include "../../BitConverter.h"

using namespace std;

SetMesh::SetMesh(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	data = rawData[rawDataIndex + 1];
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
			entry->opaqueDListAddr = BitConverter::ToInt32BE(rawData, currentPtr + 0) & 0x00FFFFFF;
			entry->translucentDListAddr = BitConverter::ToInt32BE(rawData, currentPtr + 4) & 0x00FFFFFF;

			if (entry->opaqueDListAddr != 0)
			{
				entry->opaqueDList = new ZDisplayList(rawData, entry->opaqueDListAddr, ZDisplayList::GetDListLength(rawData, entry->opaqueDListAddr));
				entry->opaqueDList->scene = zRoom->scene;
				GenDListDeclarations(rawData, entry->opaqueDList);
			}

			if (entry->translucentDListAddr != 0)
			{
				entry->translucentDList = new ZDisplayList(rawData, entry->translucentDListAddr, ZDisplayList::GetDListLength(rawData, entry->translucentDListAddr));
				entry->translucentDList->scene = zRoom->scene;
				GenDListDeclarations(rawData, entry->translucentDList);
			}

			meshHeader0->entries.push_back(entry);

			currentPtr += 8;
		}

		sprintf(line, "MeshHeader0 _%s_meshHeader_%08X = { { 0 }, 0x%02X, ", zRoom->GetName().c_str(), segmentOffset, meshHeader0->entries.size());
		declaration += line;

		if (meshHeader0->dListStart != 0)
			sprintf(line, "(u32)&_%s_meshDListEntry_%08X, ", zRoom->GetName().c_str(), meshHeader0->dListStart);
		else
			sprintf(line, "0, ");

		declaration += line;

		if (meshHeader0->dListEnd != 0)
			sprintf(line, "(u32)&(_%s_meshDListEntry_%08X) + sizeof(_%s_meshDListEntry_%08X) };\n", zRoom->GetName().c_str(), meshHeader0->dListStart, zRoom->GetName().c_str(), meshHeader0->dListStart);
		else
			sprintf(line, "0 };\n");

		declaration += line;

		zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::Align16, 12, declaration);

		declaration = "";

		sprintf(line, "MeshEntry0 _%s_meshDListEntry_%08X[%i] = \n{\n", zRoom->GetName().c_str(), meshHeader0->dListStart, meshHeader0->entries.size());
		declaration += line;

		for (int i = 0; i < meshHeader0->entries.size(); i++)
		{
			if (meshHeader0->entries[i]->opaqueDListAddr != 0)
				sprintf(line, "\t{ (u32)_%s_dlist_%08X, ", zRoom->GetName().c_str(), meshHeader0->entries[i]->opaqueDListAddr);
			else
				sprintf(line, "\t{ 0, ");

			declaration += line;

			if (meshHeader0->entries[i]->translucentDListAddr != 0)
				sprintf(line, "(u32)_%s_dlist_%08X },\n", zRoom->GetName().c_str(), meshHeader0->entries[i]->translucentDListAddr);
			else
				sprintf(line, "0 },\n");

			declaration += line;
		}

		declaration += "};\n\n";

		declaration += "static u32 terminatorMaybe = 0x01000000; // This always appears after the mesh entries. Its purpose is not clear.\n";

		zRoom->declarations[meshHeader0->dListStart] = new Declaration(DeclarationAlignment::None, DeclarationPadding::Pad16, (meshHeader0->entries.size() * 8) + 4, declaration);

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
		
		meshHeader2->entries = vector<MeshEntry2*>();
		meshHeader2->dListStart = BitConverter::ToInt32BE(rawData, segmentOffset + 4) & 0x00FFFFFF;
		meshHeader2->dListEnd = BitConverter::ToInt32BE(rawData, segmentOffset + 8) & 0x00FFFFFF;

		int8_t numEntries = rawData[segmentOffset + 1];

		uint32_t currentPtr = meshHeader2->dListStart;

		for (int i = 0; i < numEntries; i++)
		{
			MeshEntry2* entry = new MeshEntry2();
			entry->playerXMax = BitConverter::ToInt16BE(rawData, currentPtr + 0);
			entry->playerZMax = BitConverter::ToInt16BE(rawData, currentPtr + 2);
			entry->playerXMin = BitConverter::ToInt16BE(rawData, currentPtr + 4);
			entry->playerZMin = BitConverter::ToInt16BE(rawData, currentPtr + 6);

			entry->opaqueDListAddr = BitConverter::ToInt32BE(rawData, currentPtr + 8) & 0x00FFFFFF;
			entry->translucentDListAddr = BitConverter::ToInt32BE(rawData, currentPtr + 12) & 0x00FFFFFF;

			if (entry->opaqueDListAddr != 0)
			{
				entry->opaqueDList = new ZDisplayList(rawData, entry->opaqueDListAddr, ZDisplayList::GetDListLength(rawData, entry->opaqueDListAddr));
				entry->opaqueDList->scene = zRoom->scene;
				GenDListDeclarations(rawData, entry->opaqueDList);
			}

			if (entry->translucentDListAddr != 0)
			{
				entry->translucentDList = new ZDisplayList(rawData, entry->translucentDListAddr, ZDisplayList::GetDListLength(rawData, entry->translucentDListAddr));
				entry->translucentDList->scene = zRoom->scene;
				GenDListDeclarations(rawData, entry->translucentDList);
			}

			meshHeader2->entries.push_back(entry);

			currentPtr += 16;
		}

		sprintf(line, "MeshHeader2 _%s_meshHeader_%08X = { { 2 }, 0x%02X, ", zRoom->GetName().c_str(), segmentOffset, meshHeader2->entries.size());
		declaration += line;

		if (meshHeader2->dListStart != 0)
			sprintf(line, "(u32)&_%s_meshDListEntry_%08X, ", zRoom->GetName().c_str(), meshHeader2->dListStart);
		else
			sprintf(line, "0, ");

		declaration += line;

		if (meshHeader2->dListEnd != 0)
			sprintf(line, "(u32)&(_%s_meshDListEntry_%08X) + sizeof(_%s_meshDListEntry_%08X) };\n", zRoom->GetName().c_str(), meshHeader2->dListStart, zRoom->GetName().c_str(), meshHeader2->dListStart);
		else
			sprintf(line, "0 };\n");

		declaration += line;

		zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, 12, declaration);

		declaration = "";

		sprintf(line, "MeshEntry2 _%s_meshDListEntry_%08X[%i] = \n{\n", zRoom->GetName().c_str(), meshHeader2->dListStart, meshHeader2->entries.size());
		declaration += line;

		for (int i = 0; i < meshHeader2->entries.size(); i++)
		{
			sprintf(line, "\t{ %i, %i, %i, %i, ", meshHeader2->entries[i]->playerXMax, meshHeader2->entries[i]->playerZMax, meshHeader2->entries[i]->playerXMin, meshHeader2->entries[i]->playerZMin);

			declaration += line;

			if (meshHeader2->entries[i]->opaqueDListAddr != 0)
				sprintf(line, "(u32)_%s_dlist_%08X, ", zRoom->GetName().c_str(), meshHeader2->entries[i]->opaqueDListAddr);
			else
				sprintf(line, "0, ");

			declaration += line;

			if (meshHeader2->entries[i]->translucentDListAddr != 0)
				sprintf(line, "(u32)_%s_dlist_%08X },\n", zRoom->GetName().c_str(), meshHeader2->entries[i]->translucentDListAddr);
			else
				sprintf(line, "0 },\n");

			declaration += line;
		}

		declaration += "};\n\n";

		declaration += "static u32 terminatorMaybe = 0x01000000; // This always appears after the mesh entries. Its purpose is not clear.\n";

		zRoom->declarations[meshHeader2->dListStart] = new Declaration(DeclarationAlignment::None, DeclarationPadding::Pad16, (meshHeader2->entries.size() * 16) + 4, declaration);

		meshHeader = meshHeader2;
	}
}

void SetMesh::GenDListDeclarations(std::vector<uint8_t> rawData, ZDisplayList* dList)
{
	string sourceOutput = dList->GetSourceOutputCode(zRoom->GetName());

	zRoom->declarations[dList->dListAddress] = new Declaration(DeclarationAlignment::None, dList->GetRawDataSize(), sourceOutput);

	for (ZDisplayList* otherDList : dList->otherDLists)
		GenDListDeclarations(rawData, otherDList);

	for (pair<uint32_t, string> vtxEntry : dList->vtxDeclarations)
		zRoom->declarations[vtxEntry.first] = new Declaration(DeclarationAlignment::Align8, dList->vertices[vtxEntry.first].size() * 16, vtxEntry.second);

	for (pair<uint32_t, string> texEntry : dList->texDeclarations)
		zRoom->declarations[texEntry.first] = new Declaration(DeclarationAlignment::None, dList->textures[texEntry.first]->GetRawDataSize(), texEntry.second);
}

std::string SetMesh::GenDListExterns(ZDisplayList* dList)
{
	char line[2048];
	string sourceOutput = "";

	sprintf(line, "extern Gfx _%s_dlist_%08X[];\n", zRoom->GetName().c_str(), dList->dListAddress);
	sourceOutput += line;

	for (ZDisplayList* otherDList : dList->otherDLists)
		sourceOutput += GenDListExterns(otherDList);

	for (pair<uint32_t, string> vtxEntry : dList->vtxDeclarations)
	{
		sprintf(line, "extern Vtx_t _%s_vertices_%08X[%i];\n", zRoom->GetName().c_str(), vtxEntry.first, dList->vertices[vtxEntry.first].size());
		sourceOutput += line;
	}

	for (pair<uint32_t, string> texEntry : dList->texDeclarations)
	{
		sprintf(line, "extern u8 _%s_tex_%08X[];\n", zRoom->GetName().c_str(), texEntry.first);
		sourceOutput += line;
	}

	sourceOutput += dList->defines;

	return sourceOutput;
}

string SetMesh::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s %i, (u32)&_%s_meshHeader_%08X };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), data, zRoom->GetName().c_str(), segmentOffset);
	sourceOutput += line;

	/*if (meshHeader->headerType == 0)
	{
		MeshHeader0* meshHeader0 = (MeshHeader0*)meshHeader;

	}
	else
	{
		sourceOutput += "// SetMesh UNIMPLEMENTED HEADER TYPE!\n";
	}
*/
	return sourceOutput;
}

string SetMesh::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	if (meshHeader->headerType == 0)
	{
		MeshHeader0* meshHeader0 = (MeshHeader0*)meshHeader;

		sprintf(line, "extern MeshHeader0 _%s_meshHeader_%08X;\n", zRoom->GetName().c_str(), segmentOffset);
		sourceOutput += line;

		sprintf(line, "extern MeshEntry0 _%s_meshDListEntry_%08X[%i];\n", zRoom->GetName().c_str(), meshHeader0->dListStart, meshHeader0->entries.size());
		sourceOutput += line;
	
		for (MeshEntry0* entry : meshHeader0->entries)
		{
			if (entry->opaqueDList != nullptr)
			{
				sourceOutput += GenDListExterns(entry->opaqueDList);
			}

			if (entry->translucentDList != nullptr)
			{
				sourceOutput += GenDListExterns(entry->translucentDList);
			}
		}
	}
	else if (meshHeader->headerType == 2)
	{
		MeshHeader2* meshHeader2 = (MeshHeader2*)meshHeader;

		sprintf(line, "extern MeshHeader2 _%s_meshHeader_%08X;\n", zRoom->GetName().c_str(), segmentOffset);
		sourceOutput += line;

		sprintf(line, "extern MeshEntry2 _%s_meshDListEntry_%08X[%i];\n", zRoom->GetName().c_str(), meshHeader2->dListStart, meshHeader2->entries.size());
		sourceOutput += line;

		for (MeshEntry2* entry : meshHeader2->entries)
		{
			if (entry->opaqueDList != nullptr)
			{
				sourceOutput += GenDListExterns(entry->opaqueDList);
			}

			if (entry->translucentDList != nullptr)
			{
				sourceOutput += GenDListExterns(entry->translucentDList);
			}
		}
	}
	else
	{
		//sourceOutput += "// SetMesh UNIMPLEMENTED HEADER TYPE!\n";
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