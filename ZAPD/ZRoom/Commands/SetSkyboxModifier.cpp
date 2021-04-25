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
	return StringHelper::Sprintf("SCENECMD_SKYBOX_DISABLES(%i, %i)", disableSky, disableSunMoon);
}

std::string SetSkyboxModifier::GetCommandCName() const
{
	return "SCmdSkyboxDisables";
}

RoomCommand SetSkyboxModifier::GetRoomCommand() const
{
	return RoomCommand::SetSkyboxModifier;
}
