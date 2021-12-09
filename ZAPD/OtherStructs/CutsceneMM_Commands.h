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
    /* 0x12F */ CS_CMD_STOPAMBIENCE,
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

	std::string GetBodySourceCode() const;

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

