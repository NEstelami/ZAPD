#include "SetSpecialObjects.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

SetSpecialObjects::SetSpecialObjects(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	elfMessage = rawData[rawDataIndex + 0x01];
	globalObject = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
}

std::string SetSpecialObjects::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x%02X, 0x%04X", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(),
		elfMessage, globalObject);
}

std::string SetSpecialObjects::GetCommandCName() const
{
	return "SCmdSpecialFiles";
}

RoomCommand SetSpecialObjects::GetRoomCommand() const
{
	return RoomCommand::SetSpecialObjects;
}