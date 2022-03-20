#include "CutsceneMM_Commands.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

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

	// TODO
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

CutsceneSubCommandEntry_SceneTransFx::CutsceneSubCommandEntry_SceneTransFx(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_SceneTransFx::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_TRANS_FX(%i, %i, %i),", base, startFrame, endFrame);
}

CutsceneMMCommand_SceneTransFx::CutsceneMMCommand_SceneTransFx(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_SceneTransFx(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_SceneTransFx::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_TRANS_FX_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_MotionBlur::CutsceneSubCommandEntry_MotionBlur(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_MotionBlur::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_MOTIONBLUR(%i, %i, %i),", base, startFrame, endFrame);
}

CutsceneMMCommand_MotionBlur::CutsceneMMCommand_MotionBlur(const std::vector<uint8_t>& rawData,
                                                           offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_MotionBlur(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_MotionBlur::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_MOTIONBLUR_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_GiveTatl::CutsceneSubCommandEntry_GiveTatl(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_GiveTatl::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_GIVETATL(%i, %i, %i),", base, startFrame, endFrame);
}

CutsceneMMCommand_GiveTatl::CutsceneMMCommand_GiveTatl(const std::vector<uint8_t>& rawData,
                                                       offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_GiveTatl(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_GiveTatl::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_GIVETATL_LIST(%i)", numEntries);
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
	return StringHelper::Sprintf("CS_FADESEQ(%i, %i, %i),", base, startFrame, endFrame, pad,
	                             unk_08);
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

CutsceneSubCommandEntry_Unk130::CutsceneSubCommandEntry_Unk130(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk130::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_130(0x%04X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_Unk130::CutsceneMMCommand_Unk130(const std::vector<uint8_t>& rawData,
                                                   offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk130(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Unk130::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_130_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Unk131::CutsceneSubCommandEntry_Unk131(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk131::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_131(0x%04X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_Unk131::CutsceneMMCommand_Unk131(const std::vector<uint8_t>& rawData,
                                                   offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk131(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Unk131::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_131_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Unk132::CutsceneSubCommandEntry_Unk132(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk132::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_132(%i, %i, %i),", base, startFrame, endFrame);
}

CutsceneMMCommand_Unk132::CutsceneMMCommand_Unk132(const std::vector<uint8_t>& rawData,
                                                   offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk132(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Unk132::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_132_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_PlayAmbience::CutsceneSubCommandEntry_PlayAmbience(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_PlayAmbience::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_PLAYAMBIENCE(0x%04X, %i, %i, %i),", base, startFrame, endFrame,
	                             pad);
}

CutsceneMMCommand_PlayAmbience::CutsceneMMCommand_PlayAmbience(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_PlayAmbience(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_PlayAmbience::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_PLAYAMBIENCE_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_FadeAmbience::CutsceneSubCommandEntry_FadeAmbience(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_FadeAmbience::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_FADEAMBIENCE(0x%04X, %i, %i, %i),", base, startFrame, endFrame,
	                             pad);
}

CutsceneMMCommand_FadeAmbience::CutsceneMMCommand_FadeAmbience(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_FadeAmbience(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_FadeAmbience::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_FADEAMBIENCE_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Terminator::CutsceneSubCommandEntry_Terminator(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Terminator::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_TERMINATOR(%i, %i, %i),", base, startFrame, endFrame);
}

CutsceneMMCommand_Terminator::CutsceneMMCommand_Terminator(const std::vector<uint8_t>& rawData,
                                                           offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Terminator(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Terminator::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_TERMINATOR_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_ChooseCredits::CutsceneSubCommandEntry_ChooseCredits(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_ChooseCredits::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_CHOOSE_CREDITS_SCENES(%i, %i, %i),", base, startFrame,
	                             endFrame);
}

CutsceneMMCommand_ChooseCredits::CutsceneMMCommand_ChooseCredits(const std::vector<uint8_t>& rawData,
                                                   offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_ChooseCredits(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_ChooseCredits::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_CHOOSE_CREDITS_SCENES_LIST(%i)", numEntries);
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