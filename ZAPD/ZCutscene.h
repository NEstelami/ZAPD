#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "tinyxml2.h"

#include "OtherStructs/Cutscene_Commands.h"
#include "ZFile.h"
#include "ZResource.h"

class CutsceneCameraPoint
{
public:
	int8_t continueFlag;
	int8_t cameraRoll;
	int16_t nextPointFrame;
	float viewAngle;
	int16_t posX, posY, posZ;
	int16_t unused;

	CutsceneCameraPoint(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandSetCameraPos : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unused;

	std::vector<CutsceneCameraPoint*> entries;
	~CutsceneCommandSetCameraPos();
	CutsceneCommandSetCameraPos(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class SpecialActionEntry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unused0;
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

class MusicFadeEntry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unknown0;
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
};

class CutsceneCommandFadeBGM : public CutsceneCommand
{
public:
	std::vector<MusicFadeEntry*> entries;

	CutsceneCommandFadeBGM(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandFadeBGM();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class MusicChangeEntry
{
public:
	uint16_t sequence;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unknown0;
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

class EnvLightingEntry
{
public:
	uint16_t setting;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t unused0;
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

class Unknown9Entry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
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

class UnkEntry
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

class DayTimeEntry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint8_t hour;
	uint8_t minute;
	uint8_t unused;

	DayTimeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
};

class CutsceneCommandDayTime : public CutsceneCommand
{
public:
	std::vector<DayTimeEntry*> entries;

	CutsceneCommandDayTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	~CutsceneCommandDayTime();

	std::string GenerateSourceCode() const override;
	size_t GetCommandSize() const override;
};

class TextboxEntry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
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

class ActorAction
{
public:
	uint16_t action;
	uint16_t startFrame;
	uint16_t endFrame;
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

class CutsceneCommandNop : public CutsceneCommand
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;

	CutsceneCommandNop(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	size_t GetCommandSize() const override;
};

class ZCutsceneBase : public ZResource
{
public:
	ZCutsceneBase(ZFile* nParent);

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;
};

class ZCutscene : public ZCutsceneBase
{
public:
	ZCutscene(ZFile* nParent);
	~ZCutscene();

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;

	CutsceneCommands GetCommandFromID(int32_t id);

	int32_t numCommands;
	int32_t endFrame;
	std::vector<CutsceneCommand*> commands;
};
