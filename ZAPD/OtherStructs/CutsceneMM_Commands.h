#pragma once

#include "Cutscene_Commands.h"


enum class CutsceneMMCommands {
    ///* 0x000 */ CS_CMD_00,
    /* 0x00A */ CS_CMD_TEXTBOX = 0xA,
    /* 0x05A */ CS_CMD_5A = 0x5A,
    /* 0x096 */ CS_CMD_MISC = 0x96,
    /* 0x097 */ CS_CMD_SET_LIGHTING,
    /* 0x098 */ CS_CMD_SCENE_TRANS_FX,
    /* 0x098 */ CS_CMD_99 = 0x99,
    /* 0x09A */ CS_CMD_GIVETATL = 0x9A,
    /* 0x09B */ CS_CMD_9B = 0x9B,
    /* 0x09C */ CS_CMD_FADESEQ = 0x9C,
    /* 0x09D */ CS_CMD_SETTIME = 0x9D,
    /* 0x0C8 */ CS_CMD_SET_PLAYER_ACTION = 0xC8,
    /* 0x12C */ CS_CMD_PLAYSEQ = 0x12C,
    /* 0x130 */ CS_CMD_130 = 0x130,
    /* 0x131 */ CS_CMD_131 = 0x131,
    /* 0x132 */ CS_CMD_132 = 0x132,
    /* 0x12D */ CS_CMD_STOPSEQ,
    /* 0x12E */ CS_CMD_PLAYAMBIENCE,
    /* 0x12F */ CS_CMD_FADEAMBIENCE,
    /* 0x15E */ CS_CMD_15E = 0x15E,
    /* 0x15F */ CS_CMD_15F,
    /* 0x190 */ CS_CMD_190 = 0x190,
};

class CutsceneSubCommandEntry {
public:
    uint16_t base;
    uint16_t startFrame;
    uint16_t endFrame;
    uint16_t pad;

    CutsceneSubCommandEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry();

	virtual std::string GetBodySourceCode() const;

    virtual size_t GetRawSize();
};



class CutsceneMMCommand : public CutsceneCommand
{
public:
    uint32_t numEntries;
    std::vector<CutsceneSubCommandEntry*> entries;

	CutsceneMMCommand(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	virtual ~CutsceneMMCommand();

	virtual std::string GetCName() override;
    virtual std::string GetCommandMacro() const;
	virtual std::string GenerateSourceCode(uint32_t baseAddress) override;
	virtual size_t GetCommandSize()  override;
};




class CutsceneSubCommandEntry_TextBox : public CutsceneSubCommandEntry
{
public:
    uint16_t type;
    uint16_t textId1;
    uint16_t textId2;

    CutsceneSubCommandEntry_TextBox(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_TextBox();

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};

class CutsceneMMCommand_TextBox : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_TextBox(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_TextBox();

	//std::string GetCommandMacro() const override;
};



class CutsceneSubCommandEntry_Unk5A : public CutsceneSubCommandEntry
{
public:
    uint32_t unk_08;

    CutsceneSubCommandEntry_Unk5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_Unk5A();

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};

class CutsceneMMCommand_5A : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_5A();

	//std::string GetCommandMacro() const override;
	virtual size_t GetCommandSize()  override;
};



class CutsceneMMCommand_Misc : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Misc(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Misc();

	std::string GetCommandMacro() const override;
};




class CutsceneSubCommandEntry_Lighting : public CutsceneSubCommandEntry
{
public:
    uint16_t type;
    uint16_t textId1;
    uint16_t textId2;

    CutsceneSubCommandEntry_Lighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_Lighting();

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Lighting : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Lighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Lighting();

	//std::string GetCommandMacro() const override;
};





class CutsceneSubCommandEntry_GiveTatl : public CutsceneSubCommandEntry
{
public:

    CutsceneSubCommandEntry_GiveTatl(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_GiveTatl();

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_GiveTatl : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_GiveTatl(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_GiveTatl();

	std::string GetCommandMacro() const override;
};



class CutsceneSubCommandEntry_FadeSeq : public CutsceneSubCommandEntry
{
public:
    uint32_t unk_08;

    CutsceneSubCommandEntry_FadeSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_FadeSeq();

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};

class CutsceneMMCommand_FadeSeq : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_FadeSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_FadeSeq();

	//std::string GetCommandMacro() const override;
};




class CutsceneSubCommandEntry_PlaySeq : public CutsceneSubCommandEntry
{
public:

    CutsceneSubCommandEntry_PlaySeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_PlaySeq();

};

class CutsceneMMCommand_PlaySeq : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_PlaySeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_PlaySeq();

	//std::string GetCommandMacro() const override;
};



class CutsceneSubCommandEntry_Unk9B : public CutsceneSubCommandEntry
{
public:
    uint8_t unk_08;
    uint8_t unk_09;
    uint8_t unk_0A;
    uint8_t unk_0B;

    CutsceneSubCommandEntry_Unk9B(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_Unk9B();

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};

class CutsceneMMCommand_Unk9B : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Unk9B(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Unk9B();

	//std::string GetCommandMacro() const override;
};



class CutsceneSubCommandEntry_SetTime : public CutsceneSubCommandEntry
{
public:
    uint8_t hour;
    uint8_t minute;
    uint32_t unk_08;

    CutsceneSubCommandEntry_SetTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_SetTime();

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};

class CutsceneMMCommand_SetTime : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_SetTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_SetTime();

	//std::string GetCommandMacro() const override;
};




class CutsceneSubCommandEntry_Unk190 : public CutsceneSubCommandEntry
{
public:
    uint8_t unk_08;
    uint8_t unk_09;
    uint8_t unk_0A;
    uint8_t unk_0B;

    CutsceneSubCommandEntry_Unk190(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneSubCommandEntry_Unk190();

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};


class CutsceneMMCommand_Unk190 : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Unk190(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Unk190();

	//std::string GetCommandMacro() const override;
};

