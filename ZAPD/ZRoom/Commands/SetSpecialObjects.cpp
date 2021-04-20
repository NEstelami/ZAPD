#include "SetSpecialObjects.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetSpecialObjects::SetSpecialObjects(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	elfMessage = cmdArg1;
	globalObject = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
}

std::string SetSpecialObjects::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s, 0x%02X, 0x%04X", GetCommandHex().c_str(), elfMessage, globalObject);
}

string SetSpecialObjects::GetCommandCName()
{
	return "SCmdSpecialFiles";
}

RoomCommand SetSpecialObjects::GetRoomCommand()
{
	return RoomCommand::SetSpecialObjects;
}
