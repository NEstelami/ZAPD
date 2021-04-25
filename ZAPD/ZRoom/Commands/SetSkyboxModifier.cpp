#include "SetSkyboxModifier.h"
#include "StringHelper.h"

SetSkyboxModifier::SetSkyboxModifier(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                     uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	disableSky = rawData.at(rawDataIndex + 0x04);
	disableSunMoon = rawData.at(rawDataIndex + 0x05);
}

std::string SetSkyboxModifier::GetBodySourceCode() const
{
	std::string sky = StringHelper::BoolStr(disableSky);
	std::string soonMoon = StringHelper::BoolStr(disableSunMoon);
	return StringHelper::Sprintf("SCENE_CMD_SKYBOX_DISABLES(%s, %s)", sky.c_str(), soonMoon.c_str());
}

std::string SetSkyboxModifier::GetCommandCName() const
{
	return "SCmdSkyboxDisables";
}

RoomCommand SetSkyboxModifier::GetRoomCommand() const
{
	return RoomCommand::SetSkyboxModifier;
}
