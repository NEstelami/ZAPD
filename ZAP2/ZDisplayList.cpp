#include "ZDisplayList.h"

using namespace std;
using namespace tinyxml2;

// EXTRACT MODE
ZDisplayList::ZDisplayList(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath)
{
	relativePath = nRelPath;
	rawData = vector<uint8_t>(rawData.data() + rawDataIndex, rawData.data() + rawDataIndex + GetRawDataSize());
	ParseRawData();
}


void ZDisplayList::ParseRawData()
{
	int numInstructions = rawData.size() / 8;

	for (int i = 0; i < numInstructions; i++)
	{

		uint64_t data = *(uint64_t*)(rawData.data() + (i * 8));
		instructions.push_back(data);
	}
}

void ZDisplayList::PrepareSourceOutput()
{
	sourceOutput = "";

	for (int i = 0; i < instructions.size(); i++) 
	{
		F3DZEXOpcode opcode = (F3DZEXOpcode)rawData[(i * 8) + 0];
		uint64_t data = instructions[i];

		char line[512];

		switch (opcode)
		{
		case F3DZEXOpcode::G_NOOP:
			sprintf(line, "gsDPNoOpTag 0x%08X,", data >> 32);
			break;
		default:
			sprintf(line, "# Opcode 0x%02X unimplemented!", opcode);
			break;
		}

		sourceOutput += line;
		sourceOutput += "\n";
	}
}

void ZDisplayList::Save(string outFolder)
{

}

vector<uint8_t> ZDisplayList::GetRawData()
{
	return rawData;
}

int ZDisplayList::GetRawDataSize()
{
	return instructions.size() * 8;
}