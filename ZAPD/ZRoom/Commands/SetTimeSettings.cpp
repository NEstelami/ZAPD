#include "SetTimeSettings.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetTimeSettings::SetTimeSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                 int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	hour = rawData.at(rawDataIndex + 4);
	min = rawData.at(rawDataIndex + 5);
	unk = rawData.at(rawDataIndex + 6);
}

std::string SetTimeSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_TIME_SETTINGS(%i, %i, %i)", hour, min, unk);
}

string SetTimeSettings::GetCommandCName() const
{
	return "SCmdTimeSettings";
}

RoomCommand SetTimeSettings::GetRoomCommand() const
{
	return RoomCommand::SetTimeSettings;
}
