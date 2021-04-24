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

string SetWind::GetBodySourceCode()
{
	//return StringHelper::Sprintf("SCENE_CMD_WIND_SETTINGS(0x%02X, 0x%02X, 0x%02X, 0x%02X)", windWest, windVertical, windSouth, clothFlappingStrength);
	return StringHelper::Sprintf("SCENE_CMD_WIND_SETTINGS(%i, %i, %i, %i)", windWest, windVertical, windSouth, clothFlappingStrength);
}

string SetWind::GetCommandCName()
{
	return "SCmdWindSettings";
}

RoomCommand SetWind::GetRoomCommand()
{
	return RoomCommand::SetWind;
}
