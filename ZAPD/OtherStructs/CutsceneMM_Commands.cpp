#include "CutsceneMM_Commands.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

CutsceneSubCommandEntry_TextBox::CutsceneSubCommandEntry_TextBox(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	type = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x06);
	textId1 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x08);
	textId2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x0A);
}

std::string CutsceneSubCommandEntry_TextBox::GetBodySourceCode() const
{
	return StringHelper::Sprintf(
		"CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X),", base, startFrame,
		endFrame, type, textId1, textId2);
}

size_t CutsceneSubCommandEntry_TextBox::GetRawSize() const
{
	return 0x0C;
}

CutsceneMMCommand_TextBox::CutsceneMMCommand_TextBox(const std::vector<uint8_t>& rawData,
                                                     uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_TextBox(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_TextBox::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_TEXT_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Unk5A::CutsceneSubCommandEntry_Unk5A(const std::vector<uint8_t>& rawData,
                                                             uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk5A::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X),", base, startFrame);
}

size_t CutsceneSubCommandEntry_Unk5A::GetRawSize() const
{
	return 0x04;
}

CutsceneMMCommand_5A::CutsceneMMCommand_5A(const std::vector<uint8_t>& rawData,
                                           uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	// TODO
	for (size_t i = 0; i < numEntries / 4; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk5A(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_5A::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_CMD_UNK_5A_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Misc::CutsceneSubCommandEntry_Misc(const std::vector<uint8_t>& rawData,
                                                           uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Misc::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_MISC(0x%02X, %i, %i, %i),", base, startFrame, endFrame, pad);
}

CutsceneMMCommand_Misc::CutsceneMMCommand_Misc(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Misc(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Misc::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_MISC_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Lighting::CutsceneSubCommandEntry_Lighting(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Lighting::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_LIGHTING(0x%02X, %i, %i, %i),", base, startFrame, endFrame,
	                             pad);
}

CutsceneMMCommand_Lighting::CutsceneMMCommand_Lighting(const std::vector<uint8_t>& rawData,
                                                       uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Lighting(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Lighting::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_LIGHTING_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_SceneTransFx::CutsceneSubCommandEntry_SceneTransFx(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_SceneTransFx::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_TRANS_FX(0x%02X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_SceneTransFx::CutsceneMMCommand_SceneTransFx(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
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

CutsceneSubCommandEntry_Unk99::CutsceneSubCommandEntry_Unk99(const std::vector<uint8_t>& rawData,
                                                             uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk99::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_99(0x%02X, %i, %i, %i),", base, startFrame, endFrame,
	                             pad);
}

CutsceneMMCommand_Unk99::CutsceneMMCommand_Unk99(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk99(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Unk99::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_99_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_GiveTatl::CutsceneSubCommandEntry_GiveTatl(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_GiveTatl::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_GIVETATL(%i, %i, %i, %i),", base, startFrame, endFrame, pad);
}

CutsceneMMCommand_GiveTatl::CutsceneMMCommand_GiveTatl(const std::vector<uint8_t>& rawData,
                                                       uint32_t rawDataIndex)
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

CutsceneSubCommandEntry_Unk9B::CutsceneSubCommandEntry_Unk9B(const std::vector<uint8_t>& rawData,
                                                             uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_06 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x06);
	unk_07 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x07);
	unk_08 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
	unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
	unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
	unk_0B = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0B);
}

std::string CutsceneSubCommandEntry_Unk9B::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_9B(0x%02X, %i, %i, 0x%02X, 0x%02X, 0x%02X),", base,
	                             startFrame, endFrame, unk_06, unk_07, unk_08);
}

size_t CutsceneSubCommandEntry_Unk9B::GetRawSize() const
{
	return 0x0C;
}

CutsceneMMCommand_Unk9B::CutsceneMMCommand_Unk9B(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk9B(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Unk9B::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_9B_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_FadeSeq::CutsceneSubCommandEntry_FadeSeq(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_08 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
}

std::string CutsceneSubCommandEntry_FadeSeq::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_FADESEQ(0x%02X, %i, %i, 0x%04X, 0x%04X),", base, startFrame,
	                             endFrame, pad, unk_08);
}

size_t CutsceneSubCommandEntry_FadeSeq::GetRawSize() const
{
	return 0x0C;
}

CutsceneMMCommand_FadeSeq::CutsceneMMCommand_FadeSeq(const std::vector<uint8_t>& rawData,
                                                     uint32_t rawDataIndex)
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

CutsceneSubCommandEntry_ActorAction::CutsceneSubCommandEntry_ActorAction(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	rotX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x6);
	rotY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x8);
	rotZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0xA);
	startPosX = BitConverter::ToInt32BE(rawData, rawDataIndex + 0xC);
	startPosY = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x10);
	startPosZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x14);
	endPosX = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x18);
	endPosY = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x1C);
	endPosZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x20);
	normalX = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x24);
	normalY = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x28);
	normalZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x2C);
}

std::string CutsceneSubCommandEntry_ActorAction::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_ACTOR_ACTION(0x%04X, %i, %i, 0x%04X, 0x%04X, 0x%04X, %i, %i, "
	                             "%i, %i, %i, %i, %i, %i, %i),",
	                             base, startFrame, endFrame, rotX, rotY, rotZ, startPosX, startPosY,
	                             startPosZ, endPosX, endPosY, endPosZ, normalX, normalY, normalZ);
}

size_t CutsceneSubCommandEntry_ActorAction::GetRawSize() const
{
	return 0x30;
}

CutsceneMMCommand_ActorAction::CutsceneMMCommand_ActorAction(const std::vector<uint8_t>& rawData,
                                                             uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_ActorAction(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_ActorAction::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_ACTOR_ACTION_LIST(0x%04X, %i)", commandID, numEntries);
}

CutsceneSubCommandEntry_PlaySeq::CutsceneSubCommandEntry_PlaySeq(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_PlaySeq::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_PLAYSEQ(0x%04X, %i, %i, %i),", base, startFrame, endFrame,
	                             pad);
}

CutsceneMMCommand_PlaySeq::CutsceneMMCommand_PlaySeq(const std::vector<uint8_t>& rawData,
                                                     uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_PlaySeq(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_PlaySeq::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_PLAYSEQ_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Unk130::CutsceneSubCommandEntry_Unk130(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk130::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_130(0x%04X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_Unk130::CutsceneMMCommand_Unk130(const std::vector<uint8_t>& rawData,
                                                   uint32_t rawDataIndex)
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
                                                               uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk131::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_131(0x%04X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_Unk131::CutsceneMMCommand_Unk131(const std::vector<uint8_t>& rawData,
                                                   uint32_t rawDataIndex)
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
                                                               uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk132::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_132(0x%04X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_Unk132::CutsceneMMCommand_Unk132(const std::vector<uint8_t>& rawData,
                                                   uint32_t rawDataIndex)
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

CutsceneSubCommandEntry_StopSeq::CutsceneSubCommandEntry_StopSeq(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_StopSeq::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_STOPSEQ(0x%04X, %i, %i, %i),", base, startFrame, endFrame,
	                             pad);
}

CutsceneMMCommand_StopSeq::CutsceneMMCommand_StopSeq(const std::vector<uint8_t>& rawData,
                                                     uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_StopSeq(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_StopSeq::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_STOPSEQ_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_PlayAmbience::CutsceneSubCommandEntry_PlayAmbience(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_PlayAmbience::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_PLAYAMBIENCE(0x%04X, %i, %i, %i),", base, startFrame, endFrame,
	                             pad);
}

CutsceneMMCommand_PlayAmbience::CutsceneMMCommand_PlayAmbience(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
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
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_FadeAmbience::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_FADEAMBIENCE(0x%04X, %i, %i, %i),", base, startFrame, endFrame,
	                             pad);
}

CutsceneMMCommand_FadeAmbience::CutsceneMMCommand_FadeAmbience(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
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

CutsceneSubCommandEntry_Unk15E::CutsceneSubCommandEntry_Unk15E(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk15E::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_15E(0x%04X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_Unk15E::CutsceneMMCommand_Unk15E(const std::vector<uint8_t>& rawData,
                                                   uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk15E(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Unk15E::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_15E_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Unk15F::CutsceneSubCommandEntry_Unk15F(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_Unk15F::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_15F(0x%04X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_Unk15F::CutsceneMMCommand_Unk15F(const std::vector<uint8_t>& rawData,
                                                   uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk15F(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Unk15F::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_15F_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Unk190::CutsceneSubCommandEntry_Unk190(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_06 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x06);
	unk_07 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x07);
	unk_08 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
	unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
	unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
	unk_0B = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0B);
}

std::string CutsceneSubCommandEntry_Unk190::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_190(0x%04X, %i, %i, 0x%02X, 0x%02X, 0x%02X),", base,
	                             startFrame, endFrame, unk_06, unk_07, unk_08);
}

size_t CutsceneSubCommandEntry_Unk190::GetRawSize() const
{
	return 0x0C;
}

CutsceneMMCommand_Unk190::CutsceneMMCommand_Unk190(const std::vector<uint8_t>& rawData,
                                                   uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Unk190(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_Unk190::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNK_190_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_UnknownCommand::CutsceneSubCommandEntry_UnknownCommand(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

std::string CutsceneSubCommandEntry_UnknownCommand::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_UNKNOWN_CMD(0x%02X, %i, %i, %i),", base, startFrame,
	                             endFrame, pad);
}

CutsceneMMCommand_UnknownCommand::CutsceneMMCommand_UnknownCommand(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_UnknownCommand(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneMMCommand_UnknownCommand::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_SCENE_UNKNOWN_CMD_LIST(CS_CMD_UNK_%X, %i)", commandID,
	                             numEntries);
}

CutsceneSubCommandEntry_NonImplemented::CutsceneSubCommandEntry_NonImplemented(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}

CutsceneMMCommand_NonImplemented::CutsceneMMCommand_NonImplemented(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
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