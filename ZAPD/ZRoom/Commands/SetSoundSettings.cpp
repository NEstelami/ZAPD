#include "Globals.h"
#include "SetSoundSettings.h"
#include "Utils/StringHelper.h"

SetSoundSettings::SetSoundSettings(ZFile* nParent) : ZRoomCommand(nParent)
{
}

void SetSoundSettings::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	reverb = cmdArg1;
	nightTimeSFX = parent->GetRawData().at(rawDataIndex + 0x06);
	musicSequence = parent->GetRawData().at(rawDataIndex + 0x07);
}

std::string SetSoundSettings::GetBodySourceCode() const
{
	EnumData* enumData = &Globals::Instance->cfg.enumData;

	return StringHelper::Sprintf("SCENE_CMD_SOUND_SETTINGS(%i, %s, %s)", reverb,
	                             enumData->ambienceId[nightTimeSFX].c_str(),
	                             enumData->seqId[musicSequence].c_str());
}

std::string SetSoundSettings::GetCommandCName() const
{
	return "SCmdSoundSettings";
}

RoomCommand SetSoundSettings::GetRoomCommand() const
{
	return RoomCommand::SetSoundSettings;
}
