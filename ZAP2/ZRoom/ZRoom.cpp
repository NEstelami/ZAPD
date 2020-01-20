#include <algorithm>
#include "ZRoom.h"
#include "ObjectList.h"
#include "../File.h"
#include "Commands/SetRoomList.h"
#include "Commands/SetEchoSettings.h"
#include "Commands/SetSoundSettings.h"
#include "Commands/SetWind.h"
#include "Commands/SetTimeSettings.h"
#include "Commands/SetSpecialObjects.h"
#include "Commands/SetSkyboxSettings.h"
#include "Commands/SetSkyboxModifier.h"
#include "Commands/SetRoomBehavior.h"
#include "Commands/SetCameraSettings.h"
#include "Commands/SetStartPositionList.h"
#include "Commands/SetActorList.h"
#include "Commands/SetEntranceList.h"
#include "Commands/SetExitList.h"
#include "Commands/SetAlternateHeaders.h"
#include "Commands/SetCollisionHeader.h"
#include "Commands/SetObjectList.h"
#include "Commands/SetMesh.h"
#include "Commands/SetLightingSettings.h"
#include "Commands/Unused09.h"
#include "Commands/EndMarker.h"

using namespace std;
using namespace tinyxml2;

ZRoom::ZRoom(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath)
{
	commands = vector<ZRoomCommand*>();
	declarations = map<int32_t, string>();
	rawData = nRawData;
	name = reader->Attribute("Name");

	GenDefinitions();

	bool shouldContinue = true;

	int currentIndex = 0;

	while (shouldContinue)
	{
		RoomCommand opcode = (RoomCommand)rawData[rawDataIndex];

		ZRoomCommand* cmd = nullptr;

		switch (opcode)
		{
			case RoomCommand::SetStartPositionList: cmd = new SetStartPositionList(this, rawData, rawDataIndex); break; // 0x00
			case RoomCommand::SetActorList: cmd = new SetActorList(this, rawData, rawDataIndex); break; // 0x01
			case RoomCommand::SetCollisionHeader: cmd = new SetCollisionHeader(this, rawData, rawDataIndex); break; // 0x03
			case RoomCommand::SetRoomList: cmd = new SetRoomList(this, rawData, rawDataIndex); break; // 0x04
			case RoomCommand::SetWind: cmd =new SetWind(this, rawData, rawDataIndex); break; // 0x05
			case RoomCommand::SetEntranceList: cmd = new SetEntranceList(this, rawData, rawDataIndex); break; // 0x06
			case RoomCommand::SetSpecialObjects: cmd = new SetSpecialObjects(this, rawData, rawDataIndex); break; // 0x07
			case RoomCommand::SetRoomBehavior: cmd = new SetRoomBehavior(this, rawData, rawDataIndex); break; // 0x08
			case RoomCommand::Unused09: cmd = new Unused09(this, rawData, rawDataIndex); break; // 0x09
			case RoomCommand::SetMesh: cmd = new SetMesh(this, rawData, rawDataIndex); break; // 0x0A
			case RoomCommand::SetObjectList: cmd = new SetObjectList(this, rawData, rawDataIndex); break; // 0x0B
			case RoomCommand::SetLightingSettings: cmd = new SetLightingSettings(this, rawData, rawDataIndex); break; // 0x0F
			case RoomCommand::SetTimeSettings: cmd = new SetTimeSettings(this, rawData, rawDataIndex); break; // 0x10
			case RoomCommand::SetSkyboxSettings: cmd = new SetSkyboxSettings(this, rawData, rawDataIndex); break; // 0x11
			case RoomCommand::SetSkyboxModifier: cmd = new SetSkyboxModifier(this, rawData, rawDataIndex); break; // 0x12
			case RoomCommand::EndMarker: cmd = new EndMarker(this, rawData, rawDataIndex); break; // 0x14
			case RoomCommand::SetSoundSettings: cmd = new SetSoundSettings(this, rawData, rawDataIndex); break; // 0x15
			case RoomCommand::SetEchoSettings: cmd = new SetEchoSettings(this, rawData, rawDataIndex); break; // 0x16
			case RoomCommand::SetAlternateHeaders: cmd = new SetAlternateHeaders(this, rawData, rawDataIndex); break; // 0x18
			case RoomCommand::SetCameraSettings: cmd = new SetCameraSettings(this, rawData, rawDataIndex); break; // 0x19
			default: cmd = new ZRoomCommand(this, rawData, rawDataIndex);
		}
		
		cmd->cmdIndex = currentIndex;

		commands.push_back(cmd);

		if (opcode == RoomCommand::EndMarker)
			shouldContinue = false;

		rawDataIndex += 8;
		currentIndex++;
	}
}

ZRoomCommand* ZRoom::FindCommandOfType(RoomCommand cmdType)
{
	for (int i = 0; i < commands.size(); i++)
	{
		if (commands[i]->cmdID == cmdType)
			return commands[i];
	}

	return nullptr;
}

int32_t ZRoom::GetDeclarationSizeFromNeighbor(int declarationAddress)
{
	int declarationIndex = -1;

	// Copy it into a vector.
	vector<pair<int32_t, string>> declarationKeysSorted(declarations.begin(), declarations.end());

	// Sort the vector according to the word count in descending order.
	sort(declarationKeysSorted.begin(), declarationKeysSorted.end(), [](const auto& lhs, const auto& rhs) { return lhs.first < rhs.first; });

	for (int i = 0; i < declarationKeysSorted.size(); i++)
	{
		if (declarationKeysSorted[i].first == declarationAddress)
		{
			declarationIndex = i;
			break;
		}
	}

	if (declarationIndex != -1)
	{
		if (declarationIndex + 1 < declarationKeysSorted.size())
			return declarationKeysSorted[declarationIndex + 1].first - declarationKeysSorted[declarationIndex].first;
		else
			return rawData.size() - declarationKeysSorted[declarationIndex].first;
	}

	return 0;
}

int32_t ZRoom::GetCommandSizeFromNeighbor(ZRoomCommand* cmd)
{
	int cmdIndex = -1;

	for (int i = 0; i < commands.size(); i++)
	{
		if (commands[i] == cmd)
		{
			cmdIndex = i;
			break;
		}
	}

	if (cmdIndex != -1)
	{
		if (cmdIndex + 1 < commands.size())
			return commands[cmdIndex + 1]->cmdAddress - commands[cmdIndex]->cmdAddress;
		else
			return rawData.size() - commands[cmdIndex]->cmdAddress;
	}

	return 0;
}

string ZRoom::GetSourceOutputHeader()
{
	return "";
}

string ZRoom::GetSourceOutputCode()
{
	char line[2048];
	sourceOutput = "";

	sourceOutput += "#include <segment_symbols.h>\n";
	sourceOutput += "#include <z64scene.h>\n";
	sourceOutput += "#include <z64actor.h>\n\n";

	//sourceOutput += "_" + name + ":\n";
	//sourceOutput += "const SceneCmd _" + name + "[] = \n{\n";

	for (ZRoomCommand* cmd : commands)
		sourceOutput += cmd->GenerateExterns();

	sourceOutput += "\n";

	//sourceOutput += "// BEGIN COMMANDS\n";

	for (ZRoomCommand* cmd : commands)
	{
		string pass1 = cmd->GenerateSourceCodePass1(name);
		sprintf(line, "%s // 0x%04X\n", pass1.c_str(), cmd->cmdAddress);
		sourceOutput += line;
	}

	//sourceOutput += "// END COMMANDS\n";
	sourceOutput += "\n";


	// Copy it into a vector.
	vector<pair<int32_t, string>> declarationKeysSorted(declarations.begin(), declarations.end());

	// Sort the vector according to the word count in descending order.
	sort(declarationKeysSorted.begin(), declarationKeysSorted.end(), [](const auto& lhs, const auto& rhs) 
	{ 
		return lhs.first < rhs.first;
	});

	// Print out the vector.
	for (pair<int32_t, string> item : declarationKeysSorted)
	{
		sourceOutput += item.second + "\n";
	}

	sourceOutput += "\n";

	for (ZRoomCommand* cmd : commands)
		sourceOutput += cmd->GenerateSourceCodePass2(name) + "\n";

	//sourceOutput += "};\n";

	//sprintf(line, "_%s:\n", name.c_str());
	//sourceOutput += line;

	return sourceOutput;
}

void ZRoom::GenDefinitions()
{
	char line[2048];
	string sourceOutput = "";

	for (int i = 0; i < sizeof(ObjectList) / sizeof(ObjectList[0]); i++)
	{
		sprintf(line, ".set %s 0x%04X\n", ObjectList[i].c_str(), i);
		sourceOutput += line;
	}

	File::WriteAllText("objectlist.inc", sourceOutput);
}

vector<uint8_t> ZRoom::GetRawData()
{
	return rawData;
}

int ZRoom::GetRawDataSize()
{
	int32_t size = 0;

	for (ZRoomCommand* cmd : commands)
		size += cmd->GetRawDataSize();

	return size;
}