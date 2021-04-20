#include "SetSoundSettings.h"
#include "../../StringHelper.h"

using namespace std;

SetSoundSettings::SetSoundSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	reverb = cmdArg1;
	nightTimeSFX = rawData.at(rawDataIndex + 0x06);
	musicSequence = rawData.at(rawDataIndex + 0x07);
}

string SetSoundSettings::GetBodySourceCode()
{
	return StringHelper::Sprintf(
		"%s, 0x%02X, 0x00, 0x00, 0x00, 0x00, 0x%02X, 0x%02X",
		GetCommandHex().c_str(), reverb, nightTimeSFX,
		musicSequence);
}

string SetSoundSettings::GetCommandCName()
{
	return "SCmdSoundSettings";
}

RoomCommand SetSoundSettings::GetRoomCommand()
{
	return RoomCommand::SetSoundSettings;
}
