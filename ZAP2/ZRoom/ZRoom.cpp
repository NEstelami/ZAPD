#include <algorithm>
#include "ZRoom.h"
#include "../ZBlob.h"
#include "ObjectList.h"
#include "../File.h"
#include "../StringHelper.h"
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
#include "Commands/SetTransitionActorList.h"
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

ZRoom::ZRoom(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath, ZRoom* nScene)
{
	commands = vector<ZRoomCommand*>();
	declarations = map<int32_t, Declaration*>();
	rawData = nRawData;
	name = reader->Attribute("Name");

	//printf("ZRoom: %s\n", name.c_str());

	scene = nScene;

	//GenDefinitions();

	for (XMLElement* child = reader->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
	{
		if (string(child->Name()) == "DListHint")
		{
			string comment = "";
			
			if (child->Attribute("Comment") != NULL)
				comment = "// " + string(child->Attribute("Comment")) + "\n";

			string addressStr = child->Attribute("Address");
			int address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			ZDisplayList* dList = new ZDisplayList(rawData, address, ZDisplayList::GetDListLength(rawData, address));
			declarations[address] = new Declaration(DeclarationAlignment::None, dList->GetRawDataSize(), comment + dList->GetSourceOutputCode(name));
		}
		else if (string(child->Name()) == "BlobHint")
		{
			string comment = "";

			if (child->Attribute("Comment") != NULL)
				comment = "// " + string(child->Attribute("Comment")) + "\n";

			string addressStr = child->Attribute("Address");
			int address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			string sizeStr = child->Attribute("Size");
			int size = strtol(StringHelper::Split(sizeStr, "0x")[1].c_str(), NULL, 16);

			char buffer[2048];
			sprintf(buffer, "%s_blob_%08X", name.c_str(), address);

			ZBlob* blob = new ZBlob(rawData, address, size, buffer);
			declarations[address] = new Declaration(DeclarationAlignment::None, blob->GetRawDataSize(), comment + blob->GetSourceOutputCode(name));
		}
	}

	//ParseCommands(rawDataIndex);
	commandSets.push_back(rawDataIndex);
	ProcessCommandSets();
}

void ZRoom::ParseCommands(std::vector<ZRoomCommand*>& commandList, int rawDataIndex)
{
	bool shouldContinue = true;

	int currentIndex = 0;

	int8_t segmentNumber = rawDataIndex >> 24;

	rawDataIndex = rawDataIndex & 0x00FFFFFF;

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
		case RoomCommand::SetWind: cmd = new SetWind(this, rawData, rawDataIndex); break; // 0x05
		case RoomCommand::SetEntranceList: cmd = new SetEntranceList(this, rawData, rawDataIndex); break; // 0x06
		case RoomCommand::SetSpecialObjects: cmd = new SetSpecialObjects(this, rawData, rawDataIndex); break; // 0x07
		case RoomCommand::SetRoomBehavior: cmd = new SetRoomBehavior(this, rawData, rawDataIndex); break; // 0x08
		case RoomCommand::Unused09: cmd = new Unused09(this, rawData, rawDataIndex); break; // 0x09
		case RoomCommand::SetMesh: cmd = new SetMesh(this, rawData, rawDataIndex); break; // 0x0A
		case RoomCommand::SetObjectList: cmd = new SetObjectList(this, rawData, rawDataIndex); break; // 0x0B
		case RoomCommand::SetTransitionActorList: cmd = new SetTransitionActorList(this, rawData, rawDataIndex); break; // 0x0E
		case RoomCommand::SetLightingSettings: cmd = new SetLightingSettings(this, rawData, rawDataIndex); break; // 0x0F
		case RoomCommand::SetTimeSettings: cmd = new SetTimeSettings(this, rawData, rawDataIndex); break; // 0x10
		case RoomCommand::SetSkyboxSettings: cmd = new SetSkyboxSettings(this, rawData, rawDataIndex); break; // 0x11
		case RoomCommand::SetSkyboxModifier: cmd = new SetSkyboxModifier(this, rawData, rawDataIndex); break; // 0x12
		case RoomCommand::SetExitList: cmd = new SetExitList(this, rawData, rawDataIndex); break; // 0x13
		case RoomCommand::EndMarker: cmd = new EndMarker(this, rawData, rawDataIndex); break; // 0x14
		case RoomCommand::SetSoundSettings: cmd = new SetSoundSettings(this, rawData, rawDataIndex); break; // 0x15
		case RoomCommand::SetEchoSettings: cmd = new SetEchoSettings(this, rawData, rawDataIndex); break; // 0x16
		case RoomCommand::SetAlternateHeaders: cmd = new SetAlternateHeaders(this, rawData, rawDataIndex); break; // 0x18
		case RoomCommand::SetCameraSettings: cmd = new SetCameraSettings(this, rawData, rawDataIndex); break; // 0x19
		default: cmd = new ZRoomCommand(this, rawData, rawDataIndex);
		}

		//printf("OP: %s\n", cmd->GetCommandCName().c_str());

		cmd->cmdIndex = currentIndex;

		commandList.push_back(cmd);

		if (opcode == RoomCommand::EndMarker)
			shouldContinue = false;

		rawDataIndex += 8;
		currentIndex++;
	}
}

void ZRoom::ProcessCommandSets()
{
	char line[2048];

	while (commandSets.size() > 0)
	{
		std::vector<ZRoomCommand*> setCommands = std::vector<ZRoomCommand*>();

		int32_t commandSet = commandSets[0];
		int8_t segmentNumber = commandSet >> 24;
		ParseCommands(setCommands, commandSet);
		commandSets.erase(commandSets.begin());

		for (ZRoomCommand* cmd : setCommands)
		{
			string pass1 = cmd->GenerateSourceCodePass1(name, commandSet & 0x00FFFFFF);
			sprintf(line, "%s // 0x%04X", pass1.c_str(), cmd->cmdAddress);

			declarations[cmd->cmdAddress] = new Declaration(DeclarationAlignment::None, 8, line);

			sprintf(line, "extern %s _%s_set%04X_cmd%02X;\n", cmd->GetCommandCName().c_str(), name.c_str(), commandSet & 0x00FFFFFF, cmd->cmdIndex, cmd->cmdID);
			externs[cmd->cmdAddress] = line;
		}

		sourceOutput += "\n";

		for (ZRoomCommand* cmd : setCommands)
			commands.push_back(cmd);
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
	vector<pair<int32_t, Declaration*>> declarationKeysSorted(declarations.begin(), declarations.end());

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

string ZRoom::GetSourceOutputHeader(string prefix)
{
	char line[2048];
	sourceOutput = "";
	
	for (ZRoomCommand* cmd : commands)
		sourceOutput += cmd->GenerateExterns();

	sourceOutput += "\n";

	// Copy it into a vector.
	vector<pair<int32_t, string>> externsKeysSorted(externs.begin(), externs.end());

	// Sort the vector according to the word count in descending order.
	sort(externsKeysSorted.begin(), externsKeysSorted.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs.first < rhs.first;
	});

	// Print out the vector.
	for (pair<int32_t, string> item : externsKeysSorted)
	{
		sourceOutput += item.second;
	}

	sourceOutput += "\n";

	return sourceOutput;
}

string ZRoom::GetSourceOutputCode(std::string prefix)
{
	char line[2048];
	sourceOutput = "";

	sourceOutput += "#include <z64.h>\n";
	sourceOutput += "#include <segment_symbols.h>\n";

	sprintf(line, "#include \"%s.h\"\n", name.c_str());
	sourceOutput += line;

	if (scene != nullptr)
	{
		sprintf(line, "#include \"%s.h\"\n", scene->GetName().c_str());
		sourceOutput += line;
	}

	sourceOutput += "\n";

	ProcessCommandSets();

	// Copy it into a vector.
	vector<pair<int32_t, Declaration*>> declarationKeysSorted(declarations.begin(), declarations.end());

	// Sort the vector according to the word count in descending order.
	sort(declarationKeysSorted.begin(), declarationKeysSorted.end(), [](const auto& lhs, const auto& rhs) 
	{ 
		return lhs.first < rhs.first;
	});

	int lastPtr = 0;

	// Print out the vector.
	int lastAddr = 0;

	for (pair<int32_t, Declaration*> item : declarationKeysSorted)
	{
		if (lastAddr != 0)
		{
			if (item.second->alignment == DeclarationAlignment::Align16)
			{
				int lastAddrSizeTest = declarations[lastAddr]->size;
				int curPtr = lastAddr + declarations[lastAddr]->size;

				while (curPtr % 4 != 0)
				{
					item.second->size++;
					declarations[item.first]->size++;
					curPtr++;
				}

				while (curPtr % 16 != 0)
				{
					char buffer[2048];

					sprintf(buffer, "static u32 pad%02X = 0;\n", curPtr);
					sourceOutput += buffer;

					item.second->size += 4;
					declarations[item.first]->size += 4;
					curPtr += 4;
				}
			}
			else if (item.second->alignment == DeclarationAlignment::Align8)
				{
					int curPtr = lastAddr + declarations[lastAddr]->size;

					while (curPtr % 4 != 0)
					{
						item.second->size++;
						declarations[item.first]->size++;
						curPtr++;
					}

					while (curPtr % 8 != 0)
					{
						char buffer[2048];

						sprintf(buffer, "static u32 pad%02X = 0;\n", curPtr);
						sourceOutput += buffer;

						item.second->size += 4;
						declarations[item.first]->size += 4;
						curPtr += 4;
					}
				}
		}

		sourceOutput += item.second->text + "\n";

		if (item.second->padding == DeclarationPadding::Pad16)
		{
			int curPtr = item.first + item.second->size;

			while (curPtr % 4 != 0)
			{
				item.second->size++;
				curPtr++;
			}

			while (curPtr % 16 != 0)
			{
				char buffer[2048];

				sprintf(buffer, "static u32 pad%02X = 0;\n", curPtr);
				sourceOutput += buffer;

				item.second->size += 4;
				curPtr += 4;
			}
		}

		lastAddr = item.first;
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

	//File::WriteAllText("objectlist.inc", sourceOutput);
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

Declaration::Declaration(DeclarationAlignment nAlignment, uint32_t nSize, string nText)
{
	alignment = nAlignment;
	padding = DeclarationPadding::None;
	size = nSize;
	text = nText;
}

Declaration::Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, uint32_t nSize, string nText)
{
	alignment = nAlignment;
	padding = nPadding;
	size = nSize;
	text = nText;
}