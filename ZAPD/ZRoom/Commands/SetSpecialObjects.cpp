#include "SetSpecialObjects.h"
#include "BitConverter.h"
#include "StringHelper.h"

SetSpecialObjects::SetSpecialObjects(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                     uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	elfMessage = cmdArg1;
	globalObject = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
}

std::string SetSpecialObjects::GetBodySourceCode() const
{
	return StringHelper::Sprintf("SCENE_CMD_SPECIAL_FILES(0x%02X, 0x%04X)", elfMessage,
	                             globalObject);
}

std::string SetSpecialObjects::GetCommandCName() const
{
	return "SCmdSpecialFiles";
}

RoomCommand SetSpecialObjects::GetRoomCommand() const
{
	return RoomCommand::SetSpecialObjects;
}
