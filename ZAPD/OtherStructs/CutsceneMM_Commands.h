#pragma once

#include "Cutscene_Commands.h"

enum class CutsceneMMCommands
{
	/* 0x00A */ CS_CMD_TEXTBOX = 0xA,
	/* 0x05A */ CS_CMD_5A = 0x5A,
	/* 0x096 */ CS_CMD_MISC = 0x96,
	/* 0x097 */ CS_CMD_SET_LIGHTING,
	/* 0x098 */ CS_CMD_SCENE_TRANS_FX,
	/* 0x099 */ CS_CMD_99,
	/* 0x09A */ CS_CMD_GIVETATL,
	/* 0x09B */ CS_CMD_9B,
	/* 0x09C */ CS_CMD_FADESEQ,
	/* 0x09D */ CS_CMD_SETTIME,
	/* 0x0C8 */ CS_CMD_SET_PLAYER_ACTION = 0xC8,
	/* 0x0FA */ CS_CMD_UNK_FA = 0xFA,
	/* 0x0FE */ CS_CMD_UNK_FE = 0xFE,
	/* 0x0FF */ CS_CMD_UNK_FF,
	/* 0x100 */ CS_CMD_UNK_100,
	/* 0x101 */ CS_CMD_UNK_101,
	/* 0x102 */ CS_CMD_UNK_102,
	/* 0x103 */ CS_CMD_UNK_103,
	/* 0x104 */ CS_CMD_UNK_104,
	/* 0x105 */ CS_CMD_UNK_105,
	/* 0x108 */ CS_CMD_UNK_108 = 0x108,
	/* 0x109 */ CS_CMD_UNK_109,
	/* 0x12C */ CS_CMD_PLAYSEQ = 0x12C,
	/* 0x12D */ CS_CMD_UNK_12D,
	/* 0x130 */ CS_CMD_130 = 0x130,
	/* 0x131 */ CS_CMD_131 = 0x131,
	/* 0x132 */ CS_CMD_132 = 0x132,
	/* 0x133 */ CS_CMD_STOPSEQ,
	/* 0x134 */ CS_CMD_PLAYAMBIENCE,
	/* 0x135 */ CS_CMD_FADEAMBIENCE,
	/* 0x15E */ CS_CMD_15E = 0x15E,
	/* 0x15F */ CS_CMD_15F,
	/* 0x190 */ CS_CMD_190 = 0x190,
};

class CutsceneSubCommandEntry_TextBox : public CutsceneSubCommandEntry
{
public:
	uint16_t type;
	uint16_t textId1;
	uint16_t textId2;

	CutsceneSubCommandEntry_TextBox(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneMMCommand_TextBox : public CutsceneCommand
{
public:
	CutsceneMMCommand_TextBox(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk5A : public CutsceneSubCommandEntry
{
public:
	uint32_t unk_08;

	CutsceneSubCommandEntry_Unk5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneMMCommand_5A : public CutsceneCommand
{
public:
	CutsceneMMCommand_5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Misc : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_Misc(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Misc : public CutsceneCommand
{
public:
	CutsceneMMCommand_Misc(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Lighting : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_Lighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Lighting : public CutsceneCommand
{
public:
	CutsceneMMCommand_Lighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_SceneTransFx : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_SceneTransFx(const std::vector<uint8_t>& rawData,
	                                     uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_SceneTransFx : public CutsceneCommand
{
public:
	CutsceneMMCommand_SceneTransFx(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk99 : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_Unk99(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk99 : public CutsceneCommand
{
public:
	CutsceneMMCommand_Unk99(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_GiveTatl : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_GiveTatl(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_GiveTatl : public CutsceneCommand
{
public:
	CutsceneMMCommand_GiveTatl(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk9B : public CutsceneSubCommandEntry
{
public:
	uint8_t unk_06;
	uint8_t unk_07;
	uint8_t unk_08;
	uint8_t unk_09;
	uint8_t unk_0A;
	uint8_t unk_0B;

	CutsceneSubCommandEntry_Unk9B(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneMMCommand_Unk9B : public CutsceneCommand
{
public:
	CutsceneMMCommand_Unk9B(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_FadeSeq : public CutsceneSubCommandEntry
{
public:
	uint32_t unk_08;

	CutsceneSubCommandEntry_FadeSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneMMCommand_FadeSeq : public CutsceneCommand
{
public:
	CutsceneMMCommand_FadeSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_ActorAction : public CutsceneSubCommandEntry
{
public:
	uint16_t rotX, rotY, rotZ;
	int32_t startPosX, startPosY, startPosZ;
	int32_t endPosX, endPosY, endPosZ;
	int32_t normalX, normalY, normalZ;

	CutsceneSubCommandEntry_ActorAction(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneMMCommand_ActorAction : public CutsceneCommand
{
public:
	CutsceneMMCommand_ActorAction(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_PlaySeq : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_PlaySeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_PlaySeq : public CutsceneCommand
{
public:
	CutsceneMMCommand_PlaySeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk130 : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_Unk130(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk130 : public CutsceneCommand
{
public:
	CutsceneMMCommand_Unk130(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk131 : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_Unk131(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk131 : public CutsceneCommand
{
public:
	CutsceneMMCommand_Unk131(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk132 : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_Unk132(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk132 : public CutsceneCommand
{
public:
	CutsceneMMCommand_Unk132(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_StopSeq : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_StopSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_StopSeq : public CutsceneCommand
{
public:
	CutsceneMMCommand_StopSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_PlayAmbience : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_PlayAmbience(const std::vector<uint8_t>& rawData,
	                                     uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_PlayAmbience : public CutsceneCommand
{
public:
	CutsceneMMCommand_PlayAmbience(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_FadeAmbience : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_FadeAmbience(const std::vector<uint8_t>& rawData,
	                                     uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_FadeAmbience : public CutsceneCommand
{
public:
	CutsceneMMCommand_FadeAmbience(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk15E : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_Unk15E(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk15E : public CutsceneCommand
{
public:
	CutsceneMMCommand_Unk15E(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk15F : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_Unk15F(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk15F : public CutsceneCommand
{
public:
	CutsceneMMCommand_Unk15F(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_Unk190 : public CutsceneSubCommandEntry
{
public:
	uint8_t unk_06;
	uint8_t unk_07;
	uint8_t unk_08;
	uint8_t unk_09;
	uint8_t unk_0A;
	uint8_t unk_0B;

	CutsceneSubCommandEntry_Unk190(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneMMCommand_Unk190 : public CutsceneCommand
{
public:
	CutsceneMMCommand_Unk190(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_UnknownCommand : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_UnknownCommand(const std::vector<uint8_t>& rawData,
	                                       uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_UnknownCommand : public CutsceneCommand
{
public:
	CutsceneMMCommand_UnknownCommand(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class CutsceneSubCommandEntry_NonImplemented : public CutsceneSubCommandEntry
{
public:
	CutsceneSubCommandEntry_NonImplemented(const std::vector<uint8_t>& rawData,
	                                       uint32_t rawDataIndex);
};

class CutsceneMMCommand_NonImplemented : public CutsceneCommand
{
public:
	CutsceneMMCommand_NonImplemented(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};
