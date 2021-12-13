#include "ZCutsceneMM.h"

#include <cassert>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"

ZCutsceneMM::ZCutsceneMM(ZFile* nParent) : ZCutsceneBase(nParent)
{
}

ZCutsceneMM::~ZCutsceneMM()
{
	for (CutsceneCommand* cmd : commands)
		delete cmd;
}

void ZCutsceneMM::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	numCommands = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);
	commands = std::vector<CutsceneCommand*>();

	endFrame = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
	uint32_t currentPtr = rawDataIndex + 8;

	for (int32_t i = 0; i < numCommands; i++)
	{
		uint32_t id = BitConverter::ToUInt32BE(rawData, currentPtr);

		if (id == 0xFFFFFFFF)
			break;

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		{
			printf("CutsceneMM Command: 0x%X (%i)\n", id, id);
		}

		CutsceneMMCommands cmdID = static_cast<CutsceneMMCommands>(id);
		currentPtr += 4;

		CutsceneCommand* cmd = nullptr;

		if (((id >= 0x64) && (id < 0x96)) || (id == 0xC9) || ((id >= 0x1C2) && (id < 0x258)))
		{
			cmd = new CutsceneMMCommand_ActorAction(rawData, currentPtr);
		}
		else
		{
			switch (cmdID)
			{
			case CutsceneMMCommands::CS_CMD_TEXTBOX:
				cmd = new CutsceneCommand_TextBox(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_CAMERA:
				cmd = new CutsceneMMCommand_Camera(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_MISC:
				cmd = new CutsceneMMCommand_Misc(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_SET_LIGHTING:
				cmd = new CutsceneMMCommand_Lighting(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_SCENE_TRANS_FX:
				cmd = new CutsceneMMCommand_SceneTransFx(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_MOTIONBLUR:
				cmd = new CutsceneMMCommand_MotionBlur(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_GIVETATL:
				cmd = new CutsceneMMCommand_GiveTatl(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_9B:
				cmd = new CutsceneMMCommand_Unk9B(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_FADESEQ:
				cmd = new CutsceneMMCommand_FadeSeq(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_SETTIME:
				cmd = new CutsceneCommand_SetTime(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION:
				cmd = new CutsceneMMCommand_ActorAction(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_PLAYSEQ:
				cmd = new CutsceneMMCommand_PlaySeq(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_130:
				cmd = new CutsceneMMCommand_Unk130(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_131:
				cmd = new CutsceneMMCommand_Unk131(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_132:
				cmd = new CutsceneMMCommand_Unk132(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_STOPSEQ:
				cmd = new CutsceneMMCommand_StopSeq(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_PLAYAMBIENCE:
				cmd = new CutsceneMMCommand_PlayAmbience(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_FADEAMBIENCE:
				cmd = new CutsceneMMCommand_FadeAmbience(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_TERMINATOR:
				cmd = new CutsceneMMCommand_Terminator(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_15F:
				cmd = new CutsceneMMCommand_Unk15F(rawData, currentPtr);
				break;
			case CutsceneMMCommands::CS_CMD_190:
				cmd = new CutsceneMMCommand_Unk190(rawData, currentPtr);
				break;

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
				cmd = new CutsceneMMCommand_UnknownCommand(rawData, currentPtr);
				break;

			default:
				HANDLE_WARNING_RESOURCE(
					WarningType::NotImplemented, parent, this, rawDataIndex,
					StringHelper::Sprintf("Cutscene command (0x%X) not implemented", cmdID),
					StringHelper::Sprintf(
						"Command ID: 0x%X\nIndex: %d\ncurrentPtr-rawDataIndex: 0x%X", id, i,
						currentPtr - rawDataIndex));
				cmd = new CutsceneMMCommand_NonImplemented(rawData, currentPtr);
				break;
			}
		}

		assert(cmd != nullptr);

		cmd->commandIndex = i;
		cmd->SetCommandID(id);
		currentPtr += cmd->GetCommandSize() - 4;

		commands.push_back(cmd);
	}
}

std::string ZCutsceneMM::GetBodySourceCode() const
{
	std::string output;

	output += StringHelper::Sprintf("    CS_BEGIN_CUTSCENE(%i, %i),\n", numCommands, endFrame);

	uint32_t curPtr = 0;

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];
		output += "    " + cmd->GenerateSourceCode();
		curPtr += cmd->GetCommandSize();
	}

	output += StringHelper::Sprintf("    CS_END(),", commands.size(), endFrame);

	return output;
}

size_t ZCutsceneMM::GetRawDataSize() const
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
	size += 4;

	return size;
}
