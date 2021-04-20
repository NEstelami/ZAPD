#include "SetSkyboxModifier.h"
#include "../../StringHelper.h"

using namespace std;

SetSkyboxModifier::SetSkyboxModifier(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	disableSky = rawData.at(rawDataIndex + 0x04);
	disableSunMoon = rawData.at(rawDataIndex + 0x05);
}

std::string SetSkyboxModifier::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s, 0, 0, 0, 0x%02X, 0x%02X", GetCommandHex().c_str(), disableSky, disableSunMoon);
}

string SetSkyboxModifier::GetCommandCName()
{
	return "SCmdSkyboxDisables";
}

RoomCommand SetSkyboxModifier::GetRoomCommand()
{
	return RoomCommand::SetSkyboxModifier;
}
