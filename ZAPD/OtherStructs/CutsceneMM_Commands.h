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
    virtual ~CutsceneSubCommandEntry() = default;

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

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};

class CutsceneMMCommand_TextBox : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_TextBox(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_TextBox();

	std::string GetCommandMacro() const override;
};



class CutsceneSubCommandEntry_Unk5A : public CutsceneSubCommandEntry
{
public:
    uint32_t unk_08;

    CutsceneSubCommandEntry_Unk5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};

class CutsceneMMCommand_5A : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_5A();

	std::string GetCommandMacro() const override;
};




class CutsceneSubCommandEntry_Misc : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_Misc(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
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
    CutsceneSubCommandEntry_Lighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Lighting : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Lighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Lighting();

	std::string GetCommandMacro() const override;
};







class CutsceneSubCommandEntry_SceneTransFx : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_SceneTransFx(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_SceneTransFx : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_SceneTransFx(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_SceneTransFx();

	std::string GetCommandMacro() const override;
};







class CutsceneSubCommandEntry_Unk99 : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_Unk99(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk99 : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Unk99(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Unk99();

	//std::string GetCommandMacro() const override;
};



class CutsceneSubCommandEntry_GiveTatl : public CutsceneSubCommandEntry
{
public:

    CutsceneSubCommandEntry_GiveTatl(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_GiveTatl : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_GiveTatl(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_GiveTatl();

	std::string GetCommandMacro() const override;
};



class CutsceneSubCommandEntry_Unk9B : public CutsceneSubCommandEntry
{
public:
    uint8_t unk_08;
    uint8_t unk_09;
    uint8_t unk_0A;
    uint8_t unk_0B;

    CutsceneSubCommandEntry_Unk9B(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

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



class CutsceneSubCommandEntry_FadeSeq : public CutsceneSubCommandEntry
{
public:
    uint32_t unk_08;

    CutsceneSubCommandEntry_FadeSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

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




class CutsceneSubCommandEntry_SetTime : public CutsceneSubCommandEntry
{
public:
    uint8_t hour;
    uint8_t minute;
    uint32_t unk_08;

    CutsceneSubCommandEntry_SetTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

    size_t GetRawSize() override;
};

class CutsceneMMCommand_SetTime : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_SetTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_SetTime();

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

    size_t GetRawSize() override;
};

class CutsceneMMCommand_ActorAction : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_ActorAction(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_ActorAction();

	std::string GetCommandMacro() const override;
};




class CutsceneSubCommandEntry_PlaySeq : public CutsceneSubCommandEntry
{
public:

    CutsceneSubCommandEntry_PlaySeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

};

class CutsceneMMCommand_PlaySeq : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_PlaySeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_PlaySeq();

	//std::string GetCommandMacro() const override;
};








class CutsceneSubCommandEntry_Unk130 : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_Unk130(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk130 : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Unk130(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Unk130();

	//std::string GetCommandMacro() const override;
};






class CutsceneSubCommandEntry_Unk131 : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_Unk131(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk131 : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Unk131(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Unk131();

	//std::string GetCommandMacro() const override;
};


class CutsceneSubCommandEntry_Unk132 : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_Unk132(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk132 : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Unk132(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Unk132();

	//std::string GetCommandMacro() const override;
};




class CutsceneSubCommandEntry_StopSeq : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_StopSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_StopSeq : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_StopSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_StopSeq();

	//std::string GetCommandMacro() const override;
};







class CutsceneSubCommandEntry_PlayAmbience : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_PlayAmbience(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_PlayAmbience : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_PlayAmbience(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_PlayAmbience();

	//std::string GetCommandMacro() const override;
};





class CutsceneSubCommandEntry_FadeAmbience : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_FadeAmbience(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_FadeAmbience : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_FadeAmbience(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_FadeAmbience();

	//std::string GetCommandMacro() const override;
};








class CutsceneSubCommandEntry_Unk15E : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_Unk15E(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk15E : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Unk15E(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Unk15E();

	//std::string GetCommandMacro() const override;
};



class CutsceneSubCommandEntry_Unk15F : public CutsceneSubCommandEntry
{
public:
    CutsceneSubCommandEntry_Unk15F(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	//std::string GetBodySourceCode() const override;
};

class CutsceneMMCommand_Unk15F : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_Unk15F(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_Unk15F();

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





class CutsceneSubCommandEntry_NonImplemented : public CutsceneSubCommandEntry
{
public:

    CutsceneSubCommandEntry_NonImplemented(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneMMCommand_NonImplemented : public CutsceneMMCommand
{
public:
    CutsceneMMCommand_NonImplemented(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
    virtual ~CutsceneMMCommand_NonImplemented();
};
