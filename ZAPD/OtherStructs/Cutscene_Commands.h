#pragma once

#include <cstdint>
#include <string>
#include <vector>


enum class CutsceneCommands
{
	Cmd00 = 0x0000,
	SetCameraPos = 0x0001,
	SetCameraFocus = 0x0002,
	SpecialAction = 0x0003,
	SetLighting = 0x0004,
	SetCameraPosLink = 0x0005,
	SetCameraFocusLink = 0x0006,
	Cmd07 = 0x0007,
	Cmd08 = 0x0008,
	Cmd09 = 0x0009,
	Unknown = 0x001A,
	Textbox = 0x0013,
	SetActorAction0 = 0x000A,
	SetActorAction1 = 0x000F,
	SetActorAction2 = 0x000E,
	SetActorAction3 = 0x0019,
	SetActorAction4 = 0x001D,
	SetActorAction5 = 0x001E,
	SetActorAction6 = 0x002C,
	SetActorAction7 = 0x001F,
	SetActorAction8 = 0x0031,
	SetActorAction9 = 0x003E,
	SetActorAction10 = 0x008F,
	SetSceneTransFX = 0x002D,
	PlayBGM = 0x0056,
	StopBGM = 0x0057,
	FadeBGM = 0x007C,
	SetTime = 0x008C,
	Terminator = 0x03E8,
	End = 0xFFFF,
	Error = 0xFEAF,
};


class CutsceneSubCommandEntry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t pad;

	uint32_t commandID;

	CutsceneSubCommandEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	virtual ~CutsceneSubCommandEntry() = default;

	virtual std::string GetBodySourceCode() const;

	virtual size_t GetRawSize() const;
};

class CutsceneCommand
{
public:
	uint32_t commandID;
	uint32_t commandIndex;

	uint32_t numEntries;
	std::vector<CutsceneSubCommandEntry*> entries;

	CutsceneCommand(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	virtual ~CutsceneCommand();

	virtual std::string GetCommandMacro() const;
	virtual std::string GenerateSourceCode() const;
	virtual size_t GetCommandSize() const;

    virtual void SetCommandID(uint32_t nCommandID);
};




class CutsceneCameraPoint : public CutsceneSubCommandEntry
{
public:
	int8_t continueFlag;
	int8_t cameraRoll;
	int16_t nextPointFrame;
	float viewAngle;
	int16_t posX, posY, posZ;
	int16_t unused;

	CutsceneCameraPoint(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

    std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommandSetCameraPos : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unused;

	CutsceneCommandSetCameraPos(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

    std::string GetCommandMacro() const override;

	size_t GetCommandSize() const override;
};

class SpecialActionEntry : public CutsceneSubCommandEntry
{
public:
	uint32_t unused1;
	uint32_t unused2;
	uint32_t unused3;
	uint32_t unused4;
	uint32_t unused5;
	uint32_t unused6;
	uint32_t unused7;
	uint32_t unused8;
	uint32_t unused9;
	uint32_t unused10;

	SpecialActionEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandSpecialAction : public CutsceneCommand
{
public:
	std::vector<SpecialActionEntry*> entries;

	CutsceneCommandSpecialAction(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandSpecialAction();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class MusicFadeEntry : public CutsceneSubCommandEntry
{
public:
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
	uint32_t unknown5;
	uint32_t unknown6;
	uint32_t unknown7;
	uint32_t unknown8;
	uint32_t unknown9;
	uint32_t unknown10;

	MusicFadeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

    std::string GetBodySourceCode() const override;
    size_t GetRawSize() const override;
};

class CutsceneCommandFadeBGM : public CutsceneCommand
{
public:
	CutsceneCommandFadeBGM(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

    std::string GetCommandMacro() const override;
};

class MusicChangeEntry : public CutsceneSubCommandEntry
{
public:
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
	uint32_t unknown5;
	uint32_t unknown6;
	uint32_t unknown7;

	MusicChangeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandPlayBGM : public CutsceneCommand
{
public:
	std::vector<MusicChangeEntry*> entries;

	CutsceneCommandPlayBGM(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandPlayBGM();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class CutsceneCommandStopBGM : public CutsceneCommand
{
public:
	std::vector<MusicChangeEntry*> entries;

	CutsceneCommandStopBGM(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandStopBGM();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class EnvLightingEntry : public CutsceneSubCommandEntry
{
public:
	uint32_t unused1;
	uint32_t unused2;
	uint32_t unused3;
	uint32_t unused4;
	uint32_t unused5;
	uint32_t unused6;
	uint32_t unused7;

	EnvLightingEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandEnvLighting : public CutsceneCommand
{
public:
	std::vector<EnvLightingEntry*> entries;

	CutsceneCommandEnvLighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandEnvLighting();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class CutsceneCommandSceneTransFX : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	~CutsceneCommandSceneTransFX();
	CutsceneCommandSceneTransFX(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class Unknown9Entry : public CutsceneSubCommandEntry
{
public:
    // TODO:check
	uint16_t unk2;
	uint16_t unk3;
	uint16_t unk4;
	uint8_t unused0;
	uint8_t unused1;

	Unknown9Entry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandUnknown9 : public CutsceneCommand
{
public:
	std::vector<Unknown9Entry*> entries;

	CutsceneCommandUnknown9(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandUnknown9();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class UnkEntry : public CutsceneSubCommandEntry
{
public:
	uint32_t unused0;
	uint32_t unused1;
	uint32_t unused2;
	uint32_t unused3;
	uint32_t unused4;
	uint32_t unused5;
	uint32_t unused6;
	uint32_t unused7;
	uint32_t unused8;
	uint32_t unused9;
	uint32_t unused10;
	uint32_t unused11;

	UnkEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandUnknown : public CutsceneCommand
{
public:
	std::vector<UnkEntry*> entries;

	CutsceneCommandUnknown(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandUnknown();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class CutsceneSubCommandEntry_SetTime : public CutsceneSubCommandEntry
{
public:
	uint8_t hour;
	uint8_t minute;
	uint32_t unk_08;

	CutsceneSubCommandEntry_SetTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

	size_t GetRawSize() const override;
};

class CutsceneCommand_SetTime : public CutsceneCommand
{
public:
	CutsceneCommand_SetTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetCommandMacro() const override;
};

class TextboxEntry : public CutsceneSubCommandEntry
{
public:
	uint16_t type;
	uint16_t textID1;
	uint16_t textID2;

	TextboxEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandTextbox : public CutsceneCommand
{
public:
	std::vector<TextboxEntry*> entries;

	CutsceneCommandTextbox(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandTextbox();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class ActorAction : public CutsceneSubCommandEntry
{
public:
	int16_t rotX, rotY, rotZ;
	int32_t startPosX, startPosY, startPosZ;
	int32_t endPosX, endPosY, endPosZ;
	float normalX, normalY, normalZ;

	ActorAction(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandActorAction : public CutsceneCommand
{
public:
	std::vector<ActorAction*> entries;

	CutsceneCommandActorAction(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandActorAction();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class CutsceneCommandTerminator : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unknown;

	CutsceneCommandTerminator(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class CutsceneCommandEnd : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;

	CutsceneCommandEnd(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};
