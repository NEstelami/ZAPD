#include <algorithm>
#include <chrono>
#include "ZRoom.h"
#include "ZFile.h"
#include "ZCutscene.h"
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
#include "Commands/SetPathways.h"
#include "Commands/Unused09.h"
#include "Commands/SetCutscenes.h"
#include "Commands/EndMarker.h"

using namespace std;
using namespace tinyxml2;

ZRoom* ZRoom::ExtractFromXML(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath, ZFile* nParent, ZRoom* nScene)
{
	ZRoom* room = new ZRoom();

	room->parent = nParent;

	room->commands = vector<ZRoomCommand*>();
	room->extDefines = "";
	room->rawData = nRawData;
	room->name = reader->Attribute("Name");

	//printf("ZRoom: %s\n", name.c_str());

	room->scene = nScene;

	//GenDefinitions();

	int cmdCount = 999999;

	if (room->name == "syotes_room_0")
	{
		room->SyotesRoomHack();
		cmdCount = 0;
	}

	for (XMLElement* child = reader->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
	{
		if (string(child->Name()) == "DListHint")
		{
			string comment = "";
			
			if (child->Attribute("Comment") != NULL)
				comment = "// " + string(child->Attribute("Comment")) + "\n";

			string addressStr = child->Attribute("Address");
			int address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			ZDisplayList* dList = new ZDisplayList(room->rawData, address, ZDisplayList::GetDListLength(room->rawData, address));
			room->parent->declarations[address] = new Declaration(DeclarationAlignment::None, dList->GetRawDataSize(), comment + dList->GetSourceOutputCode(room->name));
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

			ZBlob* blob = new ZBlob(room->rawData, address, size, StringHelper::Sprintf("%s_blob_%08X", room->name.c_str(), address));
			room->parent->declarations[address] = new Declaration(DeclarationAlignment::None, blob->GetRawDataSize(), comment + blob->GetSourceOutputCode(room->name));
		}
		else if (string(child->Name()) == "CutsceneHint")
		{
			string comment = "";

			if (child->Attribute("Comment") != NULL)
				comment = "// " + string(child->Attribute("Comment")) + "\n";

			string addressStr = child->Attribute("Address");
			int address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			ZCutscene* cutscene = new ZCutscene(room->rawData, address, 9999);
			room->parent->declarations[address] = new Declaration(DeclarationAlignment::None, DeclarationPadding::Pad16, cutscene->GetRawDataSize(), comment + cutscene->GetSourceOutputCode(room->name));
		}
		else if (string(child->Name()) == "AltHeaderHint")
		{
			string comment = "";

			if (child->Attribute("Comment") != NULL)
				comment = "// " + string(child->Attribute("Comment")) + "\n";

			string addressStr = child->Attribute("Address");
			int address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			int commandsCount = 99999999;

			if (child->FindAttribute("Count") != NULL)
			{
				string commandCountStr = child->Attribute("Count");
				commandsCount = strtol(commandCountStr.c_str(), NULL, 10);
			}

			room->commandSets.push_back(CommandSet(address, commandsCount));
		}
		else if (string(child->Name()) == "PathHint")
		{
			string comment = "";

			if (child->Attribute("Comment") != NULL)
				comment = "// " + string(child->Attribute("Comment")) + "\n";

			string addressStr = child->Attribute("Address");
			int address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			SetPathways* pathway = new SetPathways(room, room->rawData, address);
			pathway->InitList(address);
			pathway->GenerateSourceCodePass1(room->name, 0);
			pathway->GenerateSourceCodePass2(room->name, 0);

			delete pathway;
		}
		else if (string(child->Name()) == "TextureHint")
		{
			string comment = "";

			if (child->Attribute("Comment") != NULL)
				comment = "// " + string(child->Attribute("Comment")) + "\n";

			string addressStr = child->Attribute("Address");
			int address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			string typeStr = child->Attribute("Type");
			int width = strtol(string(child->Attribute("Width")).c_str(), NULL, 10);
			int height = strtol(string(child->Attribute("Height")).c_str(), NULL, 10);

			ZTexture* tex = ZTexture::FromBinary(ZTexture::GetTextureTypeFromString(typeStr), room->rawData, address, StringHelper::Sprintf("%s_tex_%08X", room->name.c_str(), address), width, height);
			room->parent->declarations[address] = new Declaration(DeclarationAlignment::None, tex->GetRawDataSize(), comment + tex->GetSourceOutputCode(room->name));
		}
	}

	//ParseCommands(rawDataIndex);
	room->commandSets.push_back(CommandSet(rawDataIndex, cmdCount));
	room->ProcessCommandSets();

	return room;
}

void ZRoom::ParseCommands(std::vector<ZRoomCommand*>& commandList, CommandSet commandSet)
{
	bool shouldContinue = true;
	int currentIndex = 0;
	int rawDataIndex = commandSet.address;
	int8_t segmentNumber = rawDataIndex >> 24;

	rawDataIndex &= 0x00FFFFFF;
	int32_t commandsLeft = commandSet.commandCount;

	while (shouldContinue)
	{
		if (commandsLeft <= 0)
			break;

		RoomCommand opcode = (RoomCommand)rawData[rawDataIndex]; 

		ZRoomCommand* cmd = nullptr;

		auto start = chrono::steady_clock::now();

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
		case RoomCommand::SetMesh: cmd = new SetMesh(this, rawData, rawDataIndex, 0); break; // 0x0A
		case RoomCommand::SetObjectList: cmd = new SetObjectList(this, rawData, rawDataIndex); break; // 0x0B
		case RoomCommand::SetPathways: cmd = new SetPathways(this, rawData, rawDataIndex); break; // 0x0D
		case RoomCommand::SetTransitionActorList: cmd = new SetTransitionActorList(this, rawData, rawDataIndex); break; // 0x0E
		case RoomCommand::SetLightingSettings: cmd = new SetLightingSettings(this, rawData, rawDataIndex); break; // 0x0F
		case RoomCommand::SetTimeSettings: cmd = new SetTimeSettings(this, rawData, rawDataIndex); break; // 0x10
		case RoomCommand::SetSkyboxSettings: cmd = new SetSkyboxSettings(this, rawData, rawDataIndex); break; // 0x11
		case RoomCommand::SetSkyboxModifier: cmd = new SetSkyboxModifier(this, rawData, rawDataIndex); break; // 0x12
		case RoomCommand::SetExitList: cmd = new SetExitList(this, rawData, rawDataIndex); break; // 0x13
		case RoomCommand::EndMarker: cmd = new EndMarker(this, rawData, rawDataIndex); break; // 0x14
		case RoomCommand::SetSoundSettings: cmd = new SetSoundSettings(this, rawData, rawDataIndex); break; // 0x15
		case RoomCommand::SetEchoSettings: cmd = new SetEchoSettings(this, rawData, rawDataIndex); break; // 0x16
		case RoomCommand::SetCutscenes: cmd = new SetCutscenes(this, rawData, rawDataIndex); break; // 0x17
		case RoomCommand::SetAlternateHeaders: cmd = new SetAlternateHeaders(this, rawData, rawDataIndex); break; // 0x18
		case RoomCommand::SetCameraSettings: cmd = new SetCameraSettings(this, rawData, rawDataIndex); break; // 0x19
		default: cmd = new ZRoomCommand(this, rawData, rawDataIndex);
		}

		auto end = chrono::steady_clock::now();
		auto diff = chrono::duration_cast<chrono::milliseconds>(end - start).count();

//#if _MSC_VER
		//if (opcode == RoomCommand::SetMesh)
		if (diff > 50)
			printf("OP: %s, TIME: %ims\n", cmd->GetCommandCName().c_str(), diff);
//#endif

		//printf("OP: %s\n", cmd->GetCommandCName().c_str());

		cmd->cmdIndex = currentIndex;
		cmd->cmdSet = rawDataIndex;

		commandList.push_back(cmd);

		if (opcode == RoomCommand::EndMarker)
			shouldContinue = false;

		rawDataIndex += 8;
		currentIndex++;

		commandsLeft--;
	}
}

void ZRoom::ProcessCommandSets()
{
	while (commandSets.size() > 0)
	{
		std::vector<ZRoomCommand*> setCommands = std::vector<ZRoomCommand*>();

		int32_t commandSet = commandSets[0].address;
		int8_t segmentNumber = commandSet >> 24;
		ParseCommands(setCommands, commandSets[0]);
		commandSets.erase(commandSets.begin());

		for (int i = 0; i < setCommands.size(); i++)
		{
			ZRoomCommand* cmd = setCommands[i];
			cmd->commandSet = commandSet & 0x00FFFFFF;
			string pass1 = cmd->GenerateSourceCodePass1(name, cmd->commandSet);

			parent->declarations[cmd->cmdAddress] = new Declaration(i == 0 ? DeclarationAlignment::Align16 : DeclarationAlignment::None, 8, StringHelper::Sprintf("%s // 0x%04X", pass1.c_str(), cmd->cmdAddress));
			parent->externs[cmd->cmdAddress] = StringHelper::Sprintf("extern %s _%s_set%04X_cmd%02X;\n", cmd->GetCommandCName().c_str(), name.c_str(), commandSet & 0x00FFFFFF, cmd->cmdIndex, cmd->cmdID);
		}

		sourceOutput += "\n";

		for (ZRoomCommand* cmd : setCommands)
			commands.push_back(cmd);
	}

	for (ZRoomCommand* cmd : commands)
	{
		string pass2 = cmd->GenerateSourceCodePass2(name, cmd->commandSet);

		if (pass2 != "")
		{
			parent->declarations[cmd->cmdAddress] = new Declaration(DeclarationAlignment::None, 8, StringHelper::Sprintf("%s // 0x%04X", pass2.c_str(), cmd->cmdAddress));
			parent->externs[cmd->cmdAddress] = StringHelper::Sprintf("extern %s _%s_set%04X_cmd%02X;\n", cmd->GetCommandCName().c_str(), name.c_str(), cmd->cmdSet & 0x00FFFFFF, cmd->cmdIndex, cmd->cmdID);
		}
	}
}

/*
 * There is one room in Ocarina of Time that lacks a header. Room 120, "Syotes", dates back to very early in the game's development.
 * Since this room is a special case, this hack adds back a header so that the room can be processed properly.
 */
void ZRoom::SyotesRoomHack()
{
	char headerData[] = 
	{
		0x0A, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x08
	};

	for (int i = 0; i < sizeof(headerData); i++)
		rawData.insert(rawData.begin() + i, headerData[i]);

	SetMesh* cmdSetMesh = new SetMesh(this, rawData, 0, -8);

	for (int i = 0; i < sizeof(headerData); i++)
		rawData.erase(rawData.begin());

	cmdSetMesh->cmdIndex = 0;
	cmdSetMesh->cmdSet = 0;

	commands.push_back(cmdSetMesh);
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

size_t ZRoom::GetDeclarationSizeFromNeighbor(int declarationAddress)
{
	int declarationIndex = -1;

	// Copy it into a vector.
	vector<pair<int32_t, Declaration*>> declarationKeysSorted(parent->declarations.begin(), parent->declarations.end());

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

size_t ZRoom::GetCommandSizeFromNeighbor(ZRoomCommand* cmd)
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
	sourceOutput = "";
	
	for (ZRoomCommand* cmd : commands)
		sourceOutput += cmd->GenerateExterns();

	sourceOutput += "\n";

	// Copy it into a vector.
	vector<pair<int32_t, string>> externsKeysSorted(parent->externs.begin(), parent->externs.end());

	// Sort the vector according to the word count in descending order.
	sort(externsKeysSorted.begin(), externsKeysSorted.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs.first < rhs.first;
	});

	// Print out the vector.
	for (pair<int32_t, string> item : externsKeysSorted)
		sourceOutput += item.second;

	sourceOutput += "\n" + extDefines + "\n";
	sourceOutput += "\n";

	return sourceOutput;
}

string ZRoom::GetSourceOutputCode(std::string prefix)
{
	sourceOutput = "";

	sourceOutput += "#include <z64.h>\n";
	sourceOutput += "#include <segment_symbols.h>\n";
	sourceOutput += "#include <command_macros_base.h>\n";
	sourceOutput += "#include <z64cutscene_commands.h>\n";
	sourceOutput += "#include <variables.h>\n";

	//sourceOutput += StringHelper::Sprintf("#include \"%s.h\"\n", name.c_str());
	
	if (scene != nullptr)
	{
		sourceOutput += StringHelper::Sprintf("#include \"%s.h\"\n", scene->GetName().c_str());
	}

	sourceOutput += "\n";

	ProcessCommandSets();

	// Check for texture intersections
	{
		string defines = "";
		if (textures.size() != 0)
		{
			vector<pair<uint32_t, ZTexture*>> texturesSorted(textures.begin(), textures.end());

			sort(texturesSorted.begin(), texturesSorted.end(), [](const auto& lhs, const auto& rhs)
			{
				return lhs.first < rhs.first;
			});

			for (int i = 0; i < texturesSorted.size() - 1; i++)
			{
				int texSize = textures[texturesSorted[i].first]->GetRawDataSize();

				if ((texturesSorted[i].first + texSize) > texturesSorted[i + 1].first)
				{
					int intersectAmt = (texturesSorted[i].first + texSize) - texturesSorted[i + 1].first;

					defines += StringHelper::Sprintf("#define _%s_tex_%08X ((u32)_%s_tex_%08X + 0x%08X)\n", prefix.c_str(), texturesSorted[i + 1].first, prefix.c_str(),
						texturesSorted[i].first, texturesSorted[i + 1].first - texturesSorted[i].first);

					//int nSize = textures[texturesSorted[i].first]->GetRawDataSize();

					parent->declarations.erase(texturesSorted[i + 1].first);
					parent->externs.erase(texturesSorted[i + 1].first);
					textures.erase(texturesSorted[i + 1].first);
					texturesSorted.erase(texturesSorted.begin() + i + 1);

					//textures.erase(texturesSorted[i + 1].first);

					i--;
				}
			}
		}

		parent->externs[0xFFFFFFFF] = defines;
	}

	for (pair<int32_t, ZTexture*> item : textures)
	{
		string declaration = "";

		declaration += item.second->GetSourceOutputCode(prefix);
		parent->declarations[item.first] = new Declaration(DeclarationAlignment::None, item.second->GetRawDataSize(), item.second->GetSourceOutputCode(name));
	}

	sourceOutput += "\n";

	return sourceOutput;
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

CommandSet::CommandSet(int32_t nAddress)
{
	address = nAddress;
	commandCount = 9999999;
}

CommandSet::CommandSet(int32_t nAddress, int32_t nCommandCount)
{
	address = nAddress;
	commandCount = nCommandCount;
}