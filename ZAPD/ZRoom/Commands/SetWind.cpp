#include "SetWind.h"
#include "../../StringHelper.h"

using namespace std;

SetWind::SetWind(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	windWest = rawData[rawDataIndex + 0x04];
	windVertical = rawData[rawDataIndex + 0x05];
	windSouth = rawData[rawDataIndex + 0x06];
	clothFlappingStrength = rawData[rawDataIndex + 0x07];
}

string SetWind::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_WIND_SETTINGS(%i, %i, %i, %i)", windWest, windVertical,
	                             windSouth, clothFlappingStrength);
}

string SetWind::GetCommandCName() const
{
	return "SCmdWindSettings";
}

RoomCommand SetWind::GetRoomCommand() const
{
	return RoomCommand::SetWind;
}
