#include "SetSkyboxSettings.h"
#include "Globals.h"
#include "StringHelper.h"

SetSkyboxSettings::SetSkyboxSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                     uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	unk1 = cmdArg1;
	skyboxNumber = rawData.at(rawDataIndex + 0x04);
	cloudsType = rawData.at(rawDataIndex + 0x05);
	lightingSettingsControl = rawData.at(rawDataIndex + 0x06);
}

std::string SetSkyboxSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENECMD_SKYBOX_SETTINGS(0x%02X, %i, %i, %i)", unk1, skyboxNumber,
	                             cloudsType, lightingSettingsControl);
}

std::string SetSkyboxSettings::GetCommandCName() const
{
	return "SCmdSkyboxSettings";
}

RoomCommand SetSkyboxSettings::GetRoomCommand() const
{
	return RoomCommand::SetSkyboxSettings;
}
