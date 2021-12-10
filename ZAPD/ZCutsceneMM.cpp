#include "ZCutsceneMM.h"

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
	//uint32_t lastData = 0;

	/*
	
	// TODO currently cutscenes aren't being parsed, so just consume words until we see an end
	// marker.
	do
	{
		lastData = BitConverter::ToInt32BE(rawData, currentPtr);
		data.push_back(lastData);
		currentPtr += 4;
	} while (lastData != 0xFFFFFFFF);
	*/


	for (int32_t i = 0; i < numCommands; i++)
	{
		uint32_t id = BitConverter::ToUInt32BE(rawData, currentPtr);

		if (id == 0xFFFFFFFF)
			break;


		CutsceneMMCommands cmdID = static_cast<CutsceneMMCommands>(id);
		currentPtr += 4;


		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		{
			printf("CutsceneMM Command: 0x%X\n", id);
		}

		CutsceneCommand* cmd = nullptr;

		if (((id >= 0x64) && (id < 0x96)) || (id == 0xC9) || ((id >= 0x1C2) && (id < 0x258))) {
			cmd = new CutsceneCommandActorAction(rawData, currentPtr);
		} else {

			switch (cmdID) {
				case CutsceneMMCommands::CS_CMD_TEXTBOX:
					cmd = new CutsceneMMCommand_TextBox(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_5A:
					cmd = new CutsceneMMCommand_5A(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_MISC:
					cmd = new CutsceneMMCommand_Misc(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_SET_LIGHTING:
					// TODO
					cmd = new CutsceneMMCommand_Lighting(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_SCENE_TRANS_FX:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_99:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_GIVETATL:
					// TODO
					cmd = new CutsceneMMCommand_GiveTatl(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_9B:
					cmd = new CutsceneMMCommand_Unk9B(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_FADESEQ:
					cmd = new CutsceneMMCommand_FadeSeq(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_SETTIME:
					// TODO
					cmd = new CutsceneMMCommand_SetTime(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION:
					// stolen from OoT
					cmd = new CutsceneCommandActorAction(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_PLAYSEQ:
					cmd = new CutsceneMMCommand_PlaySeq(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_130:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_131:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_132:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_STOPSEQ:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_PLAYAMBIENCE:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_STOPAMBIENCE:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_15E:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_15F:
					// TODO
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
				case CutsceneMMCommands::CS_CMD_190:
					cmd = new CutsceneMMCommand_Unk190(rawData, currentPtr);
					break;

				default:
					HANDLE_WARNING_RESOURCE(WarningType::NotImplemented, parent, this, rawDataIndex,
											StringHelper::Sprintf("Cutscene command not implemented", cmdID),
											StringHelper::Sprintf("Command ID: 0x%X\nIndex: %d\ncurrentPtr-rawDataIndex: 0x%X", cmdID, i, currentPtr-rawDataIndex));
					cmd = new CutsceneMMCommand(rawData, currentPtr);
					break;
			}
		}

		cmd->commandIndex = i;
		cmd->commandID = id;
		currentPtr += cmd->GetCommandSize();

		commands.push_back(cmd);
	}
}

std::string ZCutsceneMM::GetBodySourceCode() const
{
	std::string output;

	output += StringHelper::Sprintf("    CS_BEGIN_CUTSCENE(%i, %i),\n", numCommands, endFrame);
/*
	for (size_t i = 0; i < data.size(); i++)
	{
		if ((i % 4) == 0)
			output += "\n    ";
		output += StringHelper::Sprintf("0x%08X, ", data[i]);
	}

	return output;
*/

	uint32_t curPtr = 0;

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];
		output += "    " + cmd->GenerateSourceCode(curPtr);
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
		size += 4;
	}

	// End
	size += 4;

	return size;
}
