#include "ZRoom.h"

#include <Path.h>
#include <algorithm>
#include <chrono>
#include <cassert>
#include "File.h"
#include "Globals.h"
#include "StringHelper.h"
#include "ZBlob.h"
#include "Commands/EndMarker.h"
#include "Commands/SetActorCutsceneList.h"
#include "Commands/SetActorList.h"
#include "Commands/SetAlternateHeaders.h"
#include "Commands/SetAnimatedMaterialList.h"
#include "Commands/SetCameraSettings.h"
#include "Commands/SetCollisionHeader.h"
#include "Commands/SetCsCamera.h"
#include "Commands/SetCutscenes.h"
#include "Commands/SetEchoSettings.h"
#include "Commands/SetEntranceList.h"
#include "Commands/SetExitList.h"
#include "Commands/SetLightList.h"
#include "Commands/SetLightingSettings.h"
#include "Commands/SetMesh.h"
#include "Commands/SetMinimapChests.h"
#include "Commands/SetMinimapList.h"
#include "Commands/SetObjectList.h"
#include "Commands/SetPathways.h"
#include "Commands/SetRoomBehavior.h"
#include "Commands/SetRoomList.h"
#include "Commands/SetSkyboxModifier.h"
#include "Commands/SetSkyboxSettings.h"
#include "Commands/SetSoundSettings.h"
#include "Commands/SetSpecialObjects.h"
#include "Commands/SetStartPositionList.h"
#include "Commands/SetTimeSettings.h"
#include "Commands/SetTransitionActorList.h"
#include "Commands/SetWind.h"
#include "Commands/SetWorldMapVisited.h"
#include "Commands/Unused09.h"
#include "Commands/Unused1D.h"
#include "Commands/ZRoomCommandUnk.h"
#include "ZCutscene.h"
#include "ZFile.h"

//using namespace tinyxml2;

REGISTER_ZFILENODE(Room, ZRoom);
REGISTER_ZFILENODE(Scene, ZRoom);
REGISTER_ZFILENODE(RoomAltHeader, ZRoom);
REGISTER_ZFILENODE(SceneAltHeader, ZRoom);

ZRoom::ZRoom(ZFile* nParent) : ZResource(nParent)
{
	roomCount = -1;
	canHaveInner = true;
}

ZRoom::~ZRoom()
{
	for (ZRoomCommand* cmd : commands)
		delete cmd;
}

void ZRoom::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
                           const uint32_t nRawDataIndex)
{
	ZResource::ExtractFromXML(reader, nRawData, nRawDataIndex);

	uint32_t cmdCount = UINT32_MAX;

	if (name == "syotes_room_0" || hackMode == "syotes_room")
	{
		SyotesRoomHack();
		cmdCount = 0;
	}
	else
	{
		/*std::string commandSetName = StringHelper::Sprintf("%sSet_%06X", name.c_str(), rawDataIndex);
		parent->AddDeclarationArray(
			rawDataIndex, DeclarationAlignment::Align16, GetRawDataSize(),
			GetSourceTypeName(), commandSetName, 0, "");*/
		DeclareVar(name, "");
	}
}

void ZRoom::ExtractFromBinary(const std::vector<uint8_t>& nRawData, uint32_t nRawDataIndex, ZResourceType parentType)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;
	name = GetDefaultName(parent->GetName());

	switch (parentType)
	{
	case ZResourceType::Scene:
	case ZResourceType::SceneAltHeader:
		zroomType = ZResourceType::SceneAltHeader;
		break;

	case ZResourceType::Room:
	case ZResourceType::RoomAltHeader:
		zroomType = ZResourceType::RoomAltHeader;
		break;

	default:
		// TODO: error message or something
		assert(false);
		break;
	}

	ParseRawData();
	DeclareVar(name, "");
}

void ZRoom::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string nodeName = std::string(reader->Name());
	if (nodeName == "Scene")
	{
		Globals::Instance->lastScene = this;
		//isScene = true;
		zroomType = ZResourceType::Scene;
	}
	else if (nodeName == "Room")
		zroomType = ZResourceType::Room;
	else if (nodeName == "RoomAltHeader")
		zroomType = ZResourceType::RoomAltHeader;
	else if (nodeName == "SceneAltHeader")
		zroomType = ZResourceType::SceneAltHeader;

	if (reader->Attribute("HackMode") != nullptr)
		hackMode = std::string(reader->Attribute("HackMode"));

	for (tinyxml2::XMLElement* child = reader->FirstChildElement(); child != NULL;
	     child = child->NextSiblingElement())
	{
		std::string childName =
			child->Attribute("Name") == NULL ? "" : std::string(child->Attribute("Name"));
		std::string childComment = child->Attribute("Comment") == NULL ?
                                       "" :
                                       "// " + std::string(child->Attribute("Comment")) + "\n";

		// TODO: Bunch of repeated code between all of these that needs to be combined.
		if (std::string(child->Name()) == "DListHint")
		{
			std::string addressStr = child->Attribute("Offset");
			int32_t address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			ZDisplayList* dList = new ZDisplayList(
				rawData, address,
				ZDisplayList::GetDListLength(rawData, address,
			                                 Globals::Instance->game == ZGame::OOT_SW97 ?
                                                 DListType::F3DEX :
                                                 DListType::F3DZEX),
				parent);

			dList->GetSourceOutputCode(name);
			delete dList;
		}
		else if (std::string(child->Name()) == "CutsceneHint")
		{
			std::string addressStr = child->Attribute("Offset");
			int32_t address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			// ZCutscene* cutscene = new ZCutscene(rawData, address, 9999, parent);
			ZCutscene* cutscene = new ZCutscene(parent);
			cutscene->ExtractFromXML(child, rawData, address);

			cutscene->GetSourceOutputCode(name);

			delete cutscene;
		}
		else if (std::string(child->Name()) == "AltHeaderHint")
		{
			/*
			std::string addressStr = child->Attribute("Offset");
			int32_t address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			uint32_t commandsCount = UINT32_MAX;

			if (child->FindAttribute("Count") != NULL)
			{
				std::string commandCountStr = child->Attribute("Count");
				commandsCount = strtol(commandCountStr.c_str(), NULL, 10);
			}

			commandSets.push_back(CommandSet(address, commandsCount));
			*/
		}
		else if (std::string(child->Name()) == "PathHint")
		{
			std::string addressStr = child->Attribute("Offset");
			int32_t address = strtol(StringHelper::Split(addressStr, "0x")[1].c_str(), NULL, 16);

			// TODO: add this to command set
			ZPath* pathway = new ZPath(parent);
			pathway->SetRawDataIndex(address);
			pathway->ParseRawData();
			pathway->DeclareReferences(name);
			pathway->GetSourceOutputCode(name);

			delete pathway;
		}

#ifndef DEPRECATION_OFF
		fprintf(stderr,
		        "ZRoom::ExtractFromXML: Deprecation warning in '%s'.\n"
		        "\t The resource '%s' is currently deprecated, and will be removed in a future "
		        "version.\n"
		        "\t Use the non-hint version instead.\n",
		        name.c_str(), child->Name());
#endif
	}
}

void ZRoom::ParseRawData()
{

	if (hackMode == "syotes_room")
		return;

	bool shouldContinue = true;
	uint32_t currentIndex = 0;
	uint32_t currentPtr = rawDataIndex;

	while (shouldContinue)
	{
		RoomCommand opcode = static_cast<RoomCommand>(rawData.at(currentPtr));

		ZRoomCommand* cmd = nullptr;

		auto start = std::chrono::steady_clock::now();

		switch (opcode)
		{
		case RoomCommand::SetStartPositionList:
			cmd = new SetStartPositionList(parent);
			break;  // 0x00
		case RoomCommand::SetActorList:
			cmd = new SetActorList(parent);
			break;  // 0x01
		case RoomCommand::SetCsCamera:
			cmd = new SetCsCamera(parent);
			break;  // 0x02 (MM-ONLY)
		case RoomCommand::SetCollisionHeader:
			cmd = new SetCollisionHeader(parent);
			break;  // 0x03
		case RoomCommand::SetRoomList:
			cmd = new SetRoomList(parent);
			break;  // 0x04
		case RoomCommand::SetWind:
			cmd = new SetWind(parent);
			break;  // 0x05
		case RoomCommand::SetEntranceList:
			cmd = new SetEntranceList(parent);
			break;  // 0x06
		case RoomCommand::SetSpecialObjects:
			cmd = new SetSpecialObjects(parent);
			break;  // 0x07
		case RoomCommand::SetRoomBehavior:
			cmd = new SetRoomBehavior(parent);
			break;  // 0x08
		case RoomCommand::Unused09:
			cmd = new Unused09(parent);
			break;  // 0x09
		case RoomCommand::SetMesh:
			cmd = new SetMesh(parent);
			break;  // 0x0A
		case RoomCommand::SetObjectList:
			cmd = new SetObjectList(parent);
			break;  // 0x0B
		case RoomCommand::SetLightList:
			cmd = new SetLightList(parent);
			break;  // 0x0C (MM-ONLY)
		case RoomCommand::SetPathways:
			cmd = new SetPathways(parent);
			break;  // 0x0D
		case RoomCommand::SetTransitionActorList:
			cmd = new SetTransitionActorList(parent);
			break;  // 0x0E
		case RoomCommand::SetLightingSettings:
			cmd = new SetLightingSettings(parent);
			break;  // 0x0F
		case RoomCommand::SetTimeSettings:
			cmd = new SetTimeSettings(parent);
			break;  // 0x10
		case RoomCommand::SetSkyboxSettings:
			cmd = new SetSkyboxSettings(parent);
			break;  // 0x11
		case RoomCommand::SetSkyboxModifier:
			cmd = new SetSkyboxModifier(parent);
			break;  // 0x12
		case RoomCommand::SetExitList:
			cmd = new SetExitList(parent);
			break;  // 0x13
		case RoomCommand::EndMarker:
			cmd = new EndMarker(parent);
			break;  // 0x14
		case RoomCommand::SetSoundSettings:
			cmd = new SetSoundSettings(parent);
			break;  // 0x15
		case RoomCommand::SetEchoSettings:
			cmd = new SetEchoSettings(parent);
			break;  // 0x16
		case RoomCommand::SetCutscenes:
			cmd = new SetCutscenes(parent);
			break;  // 0x17
		case RoomCommand::SetAlternateHeaders:
			cmd = new SetAlternateHeaders(parent);
			break;  // 0x18
		case RoomCommand::SetCameraSettings:
			if (Globals::Instance->game == ZGame::MM_RETAIL)
				cmd = new SetWorldMapVisited(parent);
			else
				cmd = new SetCameraSettings(parent);
			break;  // 0x19
		case RoomCommand::SetAnimatedMaterialList:
			cmd = new SetAnimatedMaterialList(parent);
			break;  // 0x1A (MM-ONLY)
		case RoomCommand::SetActorCutsceneList:
			cmd = new SetActorCutsceneList(parent);
			break;  // 0x1B (MM-ONLY)
		case RoomCommand::SetMinimapList:
			cmd = new SetMinimapList(parent);
			break;  // 0x1C (MM-ONLY)
		case RoomCommand::Unused1D:
			cmd = new Unused1D(parent);
			break;  // 0x1D
		case RoomCommand::SetMinimapChests:
			cmd = new SetMinimapChests(parent);
			break;  // 0x1E (MM-ONLY)
		default:
			cmd = new ZRoomCommandUnk(parent);
		}

		cmd->commandSet = rawDataIndex;
		cmd->ExtractCommandFromRoom(this, currentPtr);
		//cmd->DeclareReferences(GetName());

		auto end = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		if (Globals::Instance->profile)
		{
			if (diff > 50)
				printf("OP: %s, TIME: %lims\n", cmd->GetCommandCName().c_str(), diff);
		}

		cmd->cmdIndex = currentIndex;

		commands.push_back(cmd);

		if (opcode == RoomCommand::EndMarker)
			shouldContinue = false;

		currentPtr += 8;
		currentIndex++;
	}
}

void ZRoom::DeclareReferences(const std::string& prefix)
{
	for (auto& cmd : commands)
		cmd->DeclareReferences(prefix);
}

void ZRoom::ParseRawDataLate()
{
	for (auto& cmd : commands)
		cmd->ParseRawDataLate();
}

void ZRoom::DeclareReferencesLate(const std::string& prefix)
{
	for (auto& cmd : commands)
		cmd->DeclareReferencesLate(prefix);
}

void ZRoom::DeclareVar(const std::string& prefix, const std::string body)
{
	std::string auxName = name;
	if (auxName == "")
		auxName = GetDefaultName(prefix);
	if (zroomType == ZResourceType::Scene || zroomType == ZResourceType::Room)
		auxName = StringHelper::Sprintf("%sCommands", name.c_str());

	parent->AddDeclarationArray(
		rawDataIndex, DeclarationAlignment::Align16, GetRawDataSize(),
		GetSourceTypeName(), auxName, 0, body);
}

std::string ZRoom::GetBodySourceCode() const
{
	std::string declaration = "";

	for (size_t i = 0; i < commands.size(); i++)
	{
		ZRoomCommand* cmd = commands[i];
		declaration += StringHelper::Sprintf("\t%s,", cmd->GetBodySourceCode().c_str());

		if (i + 1 < commands.size())
			declaration += "\n";
	}

	return declaration;
}

std::string ZRoom::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sSet_%06X", prefix.c_str(), rawDataIndex);
}

/*
 * There is one room in Ocarina of Time that lacks a header. Room 120, "Syotes", dates back to very
 * early in the game's development. Since this room is a special case, this hack adds back a header
 * so that the room can be processed properly.
 */
void ZRoom::SyotesRoomHack()
{
	PolygonType2 poly(parent, parent->GetRawData(), 0, this);

	poly.ParseRawData();
	poly.DeclareReferences(GetName());
	parent->AddDeclaration(0, DeclarationAlignment::Align4, poly.GetRawDataSize(),
	                       poly.GetSourceTypeName(), poly.GetDefaultName(GetName()),
	                       poly.GetBodySourceCode());
}

ZRoomCommand* ZRoom::FindCommandOfType(RoomCommand cmdType)
{
	for (size_t i = 0; i < commands.size(); i++)
	{
		if (commands[i]->GetRoomCommand() == cmdType)
			return commands[i];
	}

	return nullptr;
}

size_t ZRoom::GetDeclarationSizeFromNeighbor(uint32_t declarationAddress)
{
	auto currentDecl = parent->declarations.find(declarationAddress);
	if (currentDecl == parent->declarations.end())
		return 0;

	auto nextDecl = currentDecl;
	std::advance(nextDecl, 1);
	if (nextDecl == parent->declarations.end())
		return rawData.size() - currentDecl->first;

	return nextDecl->first - currentDecl->first;
}

size_t ZRoom::GetCommandSizeFromNeighbor(ZRoomCommand* cmd)
{
	int32_t cmdIndex = -1;

	for (size_t i = 0; i < commands.size(); i++)
	{
		if (commands[i] == cmd)
		{
			cmdIndex = i;
			break;
		}
	}

	if (cmdIndex != -1)
	{
		if (cmdIndex + 1 < (int32_t)commands.size())
			return commands[cmdIndex + 1]->cmdAddress - commands[cmdIndex]->cmdAddress;
		else
			return rawData.size() - commands[cmdIndex]->cmdAddress;
	}

	return 0;
}

std::string ZRoom::GetSourceOutputHeader(const std::string& prefix)
{
	return "\n" + extDefines + "\n\n";
}

std::string ZRoom::GetSourceOutputCode(const std::string& prefix)
{
	sourceOutput = "";

	if (zroomType == ZResourceType::Scene || zroomType == ZResourceType::Room)
	{
		sourceOutput += "#include \"segment_symbols.h\"\n";
		sourceOutput += "#include \"command_macros_base.h\"\n";
		sourceOutput += "#include \"z64cutscene_commands.h\"\n";
		sourceOutput += "#include \"variables.h\"\n";

		if (Globals::Instance->lastScene != nullptr)
			sourceOutput += Globals::Instance->lastScene->parent->GetHeaderInclude();
	}

	DeclareVar(prefix, GetBodySourceCode());

	return sourceOutput;
}

size_t ZRoom::GetRawDataSize() const
{
	size_t size = 0;

	for (ZRoomCommand* cmd : commands)
		size += cmd->GetRawDataSize();

	return size;
}

std::string ZRoom::GetSourceTypeName() const
{
	return "SCmdBase";
}

ZResourceType ZRoom::GetResourceType() const
{
	assert(zroomType == ZResourceType::Scene || zroomType == ZResourceType::Room || zroomType == ZResourceType::SceneAltHeader || zroomType == ZResourceType::RoomAltHeader);
	return zroomType;
}

/* CommandSet */

CommandSet::CommandSet(uint32_t nAddress, uint32_t nCommandCount)
{
	address = nAddress;
	commandCount = nCommandCount;
}
