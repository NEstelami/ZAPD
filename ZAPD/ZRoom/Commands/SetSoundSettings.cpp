#include "SetSoundSettings.h"
#include "../../StringHelper.h"

using namespace std;

SetSoundSettings::SetSoundSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                   int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	reverb = cmdArg1;
	nightTimeSFX = rawData.at(rawDataIndex + 0x06);
	musicSequence = rawData.at(rawDataIndex + 0x07);
}

string SetSoundSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_SOUND_SETTINGS(%i, %i, %i)", reverb, nightTimeSFX,
	                             musicSequence);
}

string SetSoundSettings::GetCommandCName() const
{
	return "SCmdSoundSettings";
}

RoomCommand SetSoundSettings::GetRoomCommand() const
{
	return RoomCommand::SetSoundSettings;
}
