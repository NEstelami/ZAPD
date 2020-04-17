#include "SetPathways.h"
#include "../ZRoom.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetPathways::SetPathways(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	_rawData = rawData;
	_rawDataIndex = rawDataIndex;

	segmentOffset = 0;
	listSegmentOffset = 0;

	InitList(BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF);

	uint32_t currentPtr = listSegmentOffset;

	if (segmentOffset != 0)
		zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, 0, "");

	//if (listSegmentOffset != 0)
		//zRoom->declarations[listSegmentOffset] = new Declaration(DeclarationAlignment::None, 0, "");
}

void SetPathways::InitList(uint32_t address)
{
	segmentOffset = address;
	listSegmentOffset = BitConverter::ToInt32BE(_rawData, address + 4) & 0x00FFFFFF;
	numPoints = _rawData[address + 0];
}

string SetPathways::GetSourceOutputCode(std::string prefix)
{
	string sourceOutput = "";
	char line[2048];


	return "";
}

string SetPathways::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	//int numPathsReal = zRoom->GetDeclarationSizeFromNeighbor(listSegmentOffset) / 6;
	uint32_t currentPtr = listSegmentOffset;

	for (int i = 0; i < numPoints; i++)
	{
		PathwayEntry* entry = new PathwayEntry();
		entry->x = BitConverter::ToInt16BE(_rawData, currentPtr + 0);
		entry->y = BitConverter::ToInt16BE(_rawData, currentPtr + 2);
		entry->z = BitConverter::ToInt16BE(_rawData, currentPtr + 4);

		pathways.push_back(entry);

		currentPtr += 6;
	}

	return "";
}


string SetPathways::GenerateSourceCodePass2(string roomName, int baseAddress)
{
	string sourceOutput = "";

	sourceOutput += StringHelper::Sprintf("%s 0, (u32)&_%s_pathway_%08X };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), roomName.c_str(), segmentOffset);

	{
		string declaration = StringHelper::Sprintf("Path _%s_pathway_%08X = { %i, (u32)_%s_pathwayList_%08X };\n", roomName.c_str(), segmentOffset, numPoints, roomName.c_str(), listSegmentOffset);
		zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, DeclarationPadding::None, 8, declaration);
		zRoom->externs[segmentOffset] = StringHelper::Sprintf("extern Path _%s_pathway_%08X;\n", roomName.c_str(), segmentOffset);
	}

	{
		string declaration = "";
		declaration += StringHelper::Sprintf("Vec3s _%s_pathwayList_%08X[] = \n{\n", roomName.c_str(), listSegmentOffset);

		int index = 0;
		for (PathwayEntry* entry : pathways)
		{
			declaration += StringHelper::Sprintf("\t{ %i, %i, %i }, //0x%08X \n", entry->x, entry->y, entry->z, listSegmentOffset + (index * 6));
			index++;
		}

		declaration += "};\n\n";


		zRoom->declarations[listSegmentOffset] = new Declaration(DeclarationAlignment::None, DeclarationPadding::None, pathways.size() * 6, declaration);
		zRoom->externs[listSegmentOffset] = StringHelper::Sprintf("extern Vec3s _%s_pathwayList_%08X[];\n", roomName.c_str(), listSegmentOffset);
	}

	return sourceOutput;
}

int32_t SetPathways::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (pathways.size() * 6);
}

string SetPathways::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sourceOutput += StringHelper::Sprintf("extern Vec3s _%s_pathwayList_%08X[];\n", zRoom->GetName().c_str(), segmentOffset);
	
	return sourceOutput;
}

string SetPathways::GetCommandCName()
{
	return "SCmdPathList";
}

RoomCommand SetPathways::GetRoomCommand()
{
	return RoomCommand::SetPathways;
}