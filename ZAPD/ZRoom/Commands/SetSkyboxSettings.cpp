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
	isIndoors = rawData.at(rawDataIndex + 0x06);
}

std::string SetSkyboxSettings::GetBodySourceCode() const
{
	std::string indoors = StringHelper::BoolStr(isIndoors);
	if (Globals::Instance->game == ZGame::MM_RETAIL)
		return StringHelper::Sprintf("SCENE_CMD_SKYBOX_SETTINGS(0x%02X, %i, %i, %s)", unk1, skyboxNumber,
	                             cloudsType, indoors.c_str());
	return StringHelper::Sprintf("SCENE_CMD_SKYBOX_SETTINGS(%i, %i, %s)", skyboxNumber,
	                             cloudsType, indoors.c_str());
}

std::string SetSkyboxSettings::GetCommandCName() const
{
	return "SCmdSkyboxSettings";
}

RoomCommand SetSkyboxSettings::GetRoomCommand() const
{
	return RoomCommand::SetSkyboxSettings;
}
