#include "SetSkyboxModifier.h"
#include "../../StringHelper.h"

SetSkyboxModifier::SetSkyboxModifier(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	disableSky = rawData[rawDataIndex + 0x04];
	disableSunMoon = rawData[rawDataIndex + 0x05];
}

std::string SetSkyboxModifier::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0, 0, 0, 0x%02X, 0x%02X",
		ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), disableSky,
		disableSunMoon);
	;
}

std::string SetSkyboxModifier::GetCommandCName() const
{
	return "SCmdSkyboxDisables";
}

RoomCommand SetSkyboxModifier::GetRoomCommand() const
{
	return RoomCommand::SetSkyboxModifier;
}