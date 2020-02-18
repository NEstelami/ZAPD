#include "SetSkyboxModifier.h"

using namespace std;

SetSkyboxModifier::SetSkyboxModifier(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	disableSky = rawData[rawDataIndex + 0x04];
	disableSunMoon = rawData[rawDataIndex + 0x05];
}

string SetSkyboxModifier::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0, 0, 0, 0x%02X, 0x%02X };", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), disableSky, disableSunMoon);
	sourceOutput = line;

	return sourceOutput;
}

string SetSkyboxModifier::GetCommandCName()
{
	return "SCmdSkyboxDisables";
}

RoomCommand SetSkyboxModifier::GetRoomCommand()
{
	return RoomCommand::SetSkyboxModifier;
}