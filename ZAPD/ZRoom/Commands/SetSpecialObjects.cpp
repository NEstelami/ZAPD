#include "SetSpecialObjects.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetSpecialObjects::SetSpecialObjects(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	elfMessage = cmdArg1;
	globalObject = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
}

std::string SetSpecialObjects::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_SPECIAL_FILES(0x%02X, 0x%04X)", elfMessage, globalObject);
}

string SetSpecialObjects::GetCommandCName() const
{
	return "SCmdSpecialFiles";
}

RoomCommand SetSpecialObjects::GetRoomCommand() const
{
	return RoomCommand::SetSpecialObjects;
}
