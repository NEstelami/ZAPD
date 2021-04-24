#include "SetSkyboxSettings.h"
#include "../../Globals.h"
#include "../../StringHelper.h"

using namespace std;

SetSkyboxSettings::SetSkyboxSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                     int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	unk1 = cmdArg1;
	skyboxNumber = rawData.at(rawDataIndex + 0x04);
	cloudsType = rawData.at(rawDataIndex + 0x05);
	lightingSettingsControl = rawData.at(rawDataIndex + 0x06);
}

std::string SetSkyboxSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_SKYBOX_SETTINGS(0x%02X, %i, %i, %i)", unk1,
	                             skyboxNumber, cloudsType, lightingSettingsControl);
}

string SetSkyboxSettings::GetCommandCName() const
{
	return "SCmdSkyboxSettings";
}

RoomCommand SetSkyboxSettings::GetRoomCommand() const
{
	return RoomCommand::SetSkyboxSettings;
}
