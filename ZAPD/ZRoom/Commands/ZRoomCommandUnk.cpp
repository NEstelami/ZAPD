#include "ZRoomCommandUnk.h"
#include "../../StringHelper.h"
#include "BitConverter.h"

ZRoomCommandUnk::ZRoomCommandUnk(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	cmdID = (RoomCommand)rawData[rawDataIndex];
	cmdAddress = rawDataIndex;
	zRoom = nZRoom;

	data1 = rawData[rawDataIndex + 1];
	data2 = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
}

std::string ZRoomCommandUnk::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf("%s %sSet%04XCmd%02X = { 0x%02X, 0x%02X, 0x%06X }; /* WARNING: "
	                             "UNIMPLEMENTED ROOM COMMAND */",
	                             GetCommandCName().c_str(), roomName.c_str(), baseAddress, data2,
	                             cmdID, data1, data2);
}

std::string ZRoomCommandUnk::GenerateSourceCodePass2(std::string roomName, uint32_t baseAddress)
{
	return "";
}

std::string ZRoomCommandUnk::GenerateSourceCodePass3(std::string roomName)
{
	return "";
}

std::string ZRoomCommandUnk::GenerateExterns() const
{
	return "";
}

size_t ZRoomCommandUnk::GetRawDataSize() const
{
	return 8;
}

std::string ZRoomCommandUnk::GetCommandCName() const
{
	return "SCmdBase";
}

RoomCommand ZRoomCommandUnk::GetRoomCommand() const
{
	return RoomCommand::Error;
}