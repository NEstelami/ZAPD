#include "CutsceneMM_Commands.h"

#include <cassert>

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

// Specific for command lists where each entry has size 8 bytes
typedef struct CsCommandDescriptorMM {
	const char* commandListFmt;
	const char* commandEntryFmt;
} CsCommandDescriptorMM;

const std::unordered_map<CutsceneMMCommands, CsCommandDescriptorMM> csCommandsDescMM = {
	// { CutsceneMMCommands::CS_CMD_TEXTBOX, { "" } }, // OoT cmd re use
	// { CutsceneMMCommands::CS_CMD_CAMERA, { "" } }, // A bit special..
	// { CutsceneMMCommands::CS_CMD_MISC, { "" } }, // OoT cmd re use
	// { CutsceneMMCommands::CS_CMD_SET_LIGHTING, { "" } }, // OoT cmd re use
	{ CutsceneMMCommands::CS_CMD_SCENE_TRANS_FX, { "CS_SCENE_TRANS_FX_LIST(%i)", "CS_SCENE_TRANS_FX(%i, %i, %i)," } },
	{ CutsceneMMCommands::CS_CMD_MOTIONBLUR, {"CS_MOTIONBLUR_LIST(%i)", "CS_MOTIONBLUR(%i, %i, %i),", } } ,
	{ CutsceneMMCommands::CS_CMD_GIVETATL, {"CS_GIVETATL_LIST(%i)", "CS_GIVETATL(%i, %i, %i),", } } ,
	// { CutsceneMMCommands::CS_CMD_FADESCREEN, {"", "", } } ,
	// { CutsceneMMCommands::CS_CMD_FADESEQ, {"", "", } } ,
	// { CutsceneMMCommands::CS_CMD_SETTIME, {"", "", } } , // OoT cmd re use
	// { CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION, {"", "", } } , // OoT cmd re use
	// { CutsceneMMCommands::CS_CMD_UNK_FA, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_FE, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_FF, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_100, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_101, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_102, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_103, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_104, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_105, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_108, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_UNK_109, {"", "", } } , // unknown cmds
	// { CutsceneMMCommands::CS_CMD_PLAYSEQ, {"", "", } } , // OoT cmd re use
	// { CutsceneMMCommands::CS_CMD_UNK_12D, {"", "", } } , // unknown cmds
	{ CutsceneMMCommands::CS_CMD_130, {"CS_SCENE_UNK_130_LIST(%i)", "CS_SCENE_UNK_130(0x%04X, %i, %i, %i),", } } ,
	{ CutsceneMMCommands::CS_CMD_131, {"CS_SCENE_UNK_131_LIST(%i)", "CS_SCENE_UNK_131(0x%04X, %i, %i, %i),", } } ,
	{ CutsceneMMCommands::CS_CMD_132, {"CS_SCENE_UNK_132_LIST(%i)", "CS_SCENE_UNK_132(%i, %i, %i),", } } ,
	// { CutsceneMMCommands::CS_CMD_STOPSEQ, {"", "", } } , // OoT cmd re use
	{ CutsceneMMCommands::CS_CMD_PLAYAMBIENCE, {"CS_PLAYAMBIENCE_LIST(%i)", "CS_PLAYAMBIENCE(0x%04X, %i, %i, %i),", } } ,
	{ CutsceneMMCommands::CS_CMD_FADEAMBIENCE, {"CS_FADEAMBIENCE_LIST(%i)", "CS_FADEAMBIENCE(0x%04X, %i, %i, %i),", } } ,
	{ CutsceneMMCommands::CS_CMD_TERMINATOR, {"CS_TERMINATOR_LIST(%i)", "CS_TERMINATOR(%i, %i, %i),", } } ,
	{ CutsceneMMCommands::CS_CMD_CHOOSE_CREDITS_SCENES, {"CS_CHOOSE_CREDITS_SCENES_LIST(%i)", "CS_CHOOSE_CREDITS_SCENES(%i, %i, %i),", } } ,
	//{ CutsceneMMCommands::CS_CMD_RUMBLE, {"", "", } } , // OoT cmd re use
};


CutsceneSubCommandEntry_GenericCmd::CutsceneSubCommandEntry_GenericCmd(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex, CutsceneMMCommands cmdId)
	: CutsceneSubCommandEntry(rawData, rawDataIndex), commandId(cmdId)
{
}

std::string CutsceneSubCommandEntry_GenericCmd::GetBodySourceCode() const
{
	return StringHelper::Sprintf(csCommandsDescMM.at(commandId).commandEntryFmt, base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_GenericCmd::CutsceneMMCommand_GenericCmd(const std::vector<uint8_t>& rawData,
                                                   offset_t rawDataIndex, CutsceneMMCommands cmdId)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	commandID = static_cast<uint32_t>(cmdId);

	assert(csCommandsDescMM.find(cmdId) != csCommandsDescMM.end());

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_GenericCmd(rawData, rawDataIndex, cmdId);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_GenericCmd::GetCommandMacro() const
{
	return StringHelper::Sprintf(csCommandsDescMM.at(static_cast<CutsceneMMCommands>(commandID)).commandListFmt, numEntries);
}


CutsceneSubCommandEntry_Camera::CutsceneSubCommandEntry_Camera(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Camera::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X),", base, startFrame);
}

size_t CutsceneSubCommandEntry_Camera::GetRawSize() const
{
	return 0x04;
}

CutsceneMMCommand_Camera::CutsceneMMCommand_Camera(const std::vector<uint8_t>& rawData,
                                                   offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries / 4; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Camera(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Camera::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_CAMERA_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_FadeScreen::CutsceneSubCommandEntry_FadeScreen(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_06 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x06);
	unk_07 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x07);
	unk_08 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
	unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
	unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
	unk_0B = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0B);
}

std::string CutsceneSubCommandEntry_FadeScreen::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_FADESCREEN(0x%02X, %i, %i, %i, %i, %i),", base, startFrame,
	                             endFrame, unk_06, unk_07, unk_08);
}

size_t CutsceneSubCommandEntry_FadeScreen::GetRawSize() const
{
	return 0x0C;
}

CutsceneMMCommand_FadeScreen::CutsceneMMCommand_FadeScreen(const std::vector<uint8_t>& rawData,
                                                           offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_FadeScreen(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_FadeScreen::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_FADESCREEN_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_FadeSeq::CutsceneSubCommandEntry_FadeSeq(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_08 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
}

std::string CutsceneSubCommandEntry_FadeSeq::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_FADESEQ(%i, %i, %i),", base, startFrame, endFrame);
}

size_t CutsceneSubCommandEntry_FadeSeq::GetRawSize() const
{
	return 0x0C;
}

CutsceneMMCommand_FadeSeq::CutsceneMMCommand_FadeSeq(const std::vector<uint8_t>& rawData,
                                                     offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_FadeSeq(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_FadeSeq::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_FADESEQ_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_NonImplemented::CutsceneSubCommandEntry_NonImplemented(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

CutsceneMMCommand_NonImplemented::CutsceneMMCommand_NonImplemented(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_NonImplemented(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}
