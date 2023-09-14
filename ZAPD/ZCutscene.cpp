#include "ZCutscene.h"

#include <cassert>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZResource.h"

REGISTER_ZFILENODE(Cutscene, ZCutscene);

ZCutscene::ZCutscene(ZFile* nParent) : ZResource(nParent)
{
}

ZCutscene::~ZCutscene()
{
	for (CutsceneCommand* cmd : commands)
		delete cmd;
}

std::string ZCutscene::GetBodySourceCode() const
{
	std::string output = "";

	output += StringHelper::Sprintf("    CS_BEGIN_CUTSCENE(%i, %i),\n", commands.size(), endFrame);

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];
		output += "    " + cmd->GenerateSourceCode();
	}

	output += StringHelper::Sprintf("    CS_END(),", commands.size(), endFrame);

	return output;
}

size_t ZCutscene::GetRawDataSize() const
{
	size_t size = 0;

	// Beginning
	size += 8;

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];

		size += cmd->GetCommandSize();
	}

	// End
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		size += 4;
	}
	else
	{
		size += 8;
	}

	return size;
}

void ZCutscene::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();

	numCommands = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);
	commands = std::vector<CutsceneCommand*>();

	endFrame = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
	offset_t currentPtr = rawDataIndex + 8;
	commands.reserve(numCommands);
	for (int32_t i = 0; i < numCommands; i++)
	{
		uint32_t id = BitConverter::ToUInt32BE(rawData, currentPtr);

		if (id == 0xFFFFFFFF)
			break;

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		{
			printf("Cutscene Command: 0x%X (%i)\n", id, id);
		}

		currentPtr += 4;

		CutsceneCommand* cmd = nullptr;

		if (Globals::Instance->game == ZGame::MM_RETAIL)
		{
			cmd = GetCommandMM(id, currentPtr);
		}
		else
		{
			cmd = GetCommandOoT(id, currentPtr);
		}

		if (cmd == nullptr)
		{
			HANDLE_WARNING_RESOURCE(
				WarningType::NotImplemented, parent, this, rawDataIndex,
				StringHelper::Sprintf("Cutscene command not implemented"),
				StringHelper::Sprintf("Command ID: 0x%X\nIndex: %d\ncurrentPtr-rawDataIndex: 0x%X",
			                          id, i, currentPtr - rawDataIndex));
			cmd = new CutsceneMMCommand_NonImplemented(rawData, currentPtr);
		}

		assert(cmd != nullptr);

		cmd->commandIndex = i;
		cmd->SetCommandID(id);
		size_t commmandSize = cmd->GetCommandSize();
		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		{
			printf("\t Command size: 0x%zX (%zu)\n", commmandSize, commmandSize);
		}
		currentPtr += commmandSize - 4;

		commands.push_back(cmd);
	}
}

CutsceneCommand* ZCutscene::GetCommandOoT(uint32_t id, offset_t currentPtr) const
{
	CutsceneCommands cmdID = static_cast<CutsceneCommands>(id);

	const auto& rawData = parent->GetRawData();

	switch (cmdID)
	{
	case CutsceneCommands::CS_CMD_PLAYER_CUE:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_1:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_1:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_2:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_3:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_2:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_6_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_4:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_3:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_1:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_1:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_1:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_5:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_4:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_2:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_2:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_2:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_5_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_6:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_3:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_5:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_7_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_3:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_3:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_6_1:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_4:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_4:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_5_1:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_6_2:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_6_3:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_7_1:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_8_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_5:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_6:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_6:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_7:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_4:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_7:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_5:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_8:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_6:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_7:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_8:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_7:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_5_2:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_9:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_5:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_10:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_8:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_9:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_6:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_5_3:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_8:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_6_4:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_7_2:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_5_4:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_9:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_11:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_10:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_9:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_11:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_10:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_12:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_7_3:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_7_4:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_6_5:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_12:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_10:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_13:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_13:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_14:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_11:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_14:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_15:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_12:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_11:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_7:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_5_5:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_6_6:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_16:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_2_13:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_3_12:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_7_5:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_4_8:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_5_6:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_6_7:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_15:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_16:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_1_17:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_7_6:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_9_0:
	case CutsceneCommands::CS_CMD_ACTOR_CUE_0_17:
		return new CutsceneCommand_ActorCue(rawData, currentPtr);

	case CutsceneCommands::CS_CMD_MISC:
	case CutsceneCommands::CS_CMD_LIGHT_SETTING:
	case CutsceneCommands::CS_CMD_START_SEQ:
	case CutsceneCommands::CS_CMD_STOP_SEQ:
	case CutsceneCommands::CS_CMD_FADE_OUT_SEQ:
		return new CutsceneCommand_GenericCmd(rawData, currentPtr, cmdID);

	case CutsceneCommands::CS_CMD_CAM_EYE_SPLINE:
	case CutsceneCommands::CS_CMD_CAM_AT_SPLINE:
	case CutsceneCommands::CS_CMD_CAM_EYE_SPLINE_REL_TO_PLAYER:
	case CutsceneCommands::CS_CMD_CAM_AT_SPLINE_REL_TO_PLAYER:
		return new CutsceneCommand_GenericCameraCmd(rawData, currentPtr);

	case CutsceneCommands::CS_CMD_RUMBLE_CONTROLLER:
		return new CutsceneCommand_Rumble(rawData, currentPtr);

	case CutsceneCommands::CS_CMD_TEXT:
		return new CutsceneCommand_Text(rawData, currentPtr);

	case CutsceneCommands::CS_CMD_TRANSITION:
		return new CutsceneCommand_Transition(rawData, currentPtr);

	case CutsceneCommands::CS_CMD_TIME:
		return new CutsceneCommand_Time(rawData, currentPtr);

	case CutsceneCommands::CS_CMD_DESTINATION:
		return new CutsceneCommand_Destination(rawData, currentPtr);

	case CutsceneCommands::CS_CMD_CAM_EYE:
	case CutsceneCommands::CS_CMD_CAM_AT:
		break;

	default:
		std::string errorHeader =
			StringHelper::Sprintf("Warning: Invalid cutscene command ID: '0x%04X'", cmdID);
		return new CutsceneCommand_GenericCmd(rawData, currentPtr, cmdID);
	}

	return nullptr;
}

CutsceneCommand* ZCutscene::GetCommandMM(uint32_t id, offset_t currentPtr) const
{
	CutsceneMMCommands cmdID = static_cast<CutsceneMMCommands>(id);

	const auto& rawData = parent->GetRawData();

	if (((id >= 100) && (id < 150)) || (id == 201) || ((id >= 450) && (id < 600)))
	{
		return new CutsceneMMCommand_ActorCue(rawData, currentPtr);
	}

	switch (cmdID)
	{
	case CutsceneMMCommands::CS_CMD_MISC:
	case CutsceneMMCommands::CS_CMD_SET_LIGHTING:
	case CutsceneMMCommands::CS_CMD_SCENE_TRANS_FX:
	case CutsceneMMCommands::CS_CMD_MOTIONBLUR:
	case CutsceneMMCommands::CS_CMD_GIVETATL:
	case CutsceneMMCommands::CS_CMD_PLAYSEQ:
	case CutsceneMMCommands::CS_CMD_130:
	case CutsceneMMCommands::CS_CMD_131:
	case CutsceneMMCommands::CS_CMD_132:
	case CutsceneMMCommands::CS_CMD_STOPSEQ:
	case CutsceneMMCommands::CS_CMD_PLAYAMBIENCE:
	case CutsceneMMCommands::CS_CMD_FADEAMBIENCE:
	case CutsceneMMCommands::CS_CMD_TERMINATOR:
	case CutsceneMMCommands::CS_CMD_CHOOSE_CREDITS_SCENES:

	case CutsceneMMCommands::CS_CMD_UNK_FA:
	case CutsceneMMCommands::CS_CMD_UNK_FE:
	case CutsceneMMCommands::CS_CMD_UNK_FF:
	case CutsceneMMCommands::CS_CMD_UNK_100:
	case CutsceneMMCommands::CS_CMD_UNK_101:
	case CutsceneMMCommands::CS_CMD_UNK_102:
	case CutsceneMMCommands::CS_CMD_UNK_103:
	case CutsceneMMCommands::CS_CMD_UNK_104:
	case CutsceneMMCommands::CS_CMD_UNK_105:
	case CutsceneMMCommands::CS_CMD_UNK_108:
	case CutsceneMMCommands::CS_CMD_UNK_109:
	case CutsceneMMCommands::CS_CMD_UNK_12D:
		return new CutsceneMMCommand_GenericCmd(rawData, currentPtr, cmdID);

	case CutsceneMMCommands::CS_CMD_TEXTBOX:
		return new CutsceneMMCommand_Text(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_CAMERA:
		return new CutsceneMMCommand_Camera(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_FADESCREEN:
		return new CutsceneMMCommand_FadeScreen(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_FADESEQ:
		return new CutsceneMMCommand_FadeSeq(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_SETTIME:
		return new CutsceneCommand_Time(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION:
		return new CutsceneMMCommand_ActorCue(rawData, currentPtr);
	case CutsceneMMCommands::CS_CMD_RUMBLE:
		return new CutsceneMMCommand_Rumble(rawData, currentPtr);
	}

	return nullptr;
}

Declaration* ZCutscene::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (auxName == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), auxName, 0, bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZCutscene::GetSourceTypeName() const
{
	return "CutsceneData";
}

ZResourceType ZCutscene::GetResourceType() const
{
	return ZResourceType::Cutscene;
}
