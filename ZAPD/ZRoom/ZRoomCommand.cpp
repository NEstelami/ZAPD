#include "ZRoomCommand.h"
#include "StringHelper.h"

ZRoomCommand::ZRoomCommand(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
{
	cmdID = (RoomCommand)rawData[rawDataIndex];
	cmdAddress = rawDataIndex;
	zRoom = nZRoom;
}

ZRoomCommand::~ZRoomCommand()
{
}

std::string ZRoomCommand::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf("0x%02X,", (uint8_t)cmdID);
}

std::string ZRoomCommand::GenerateSourceCodePass2(std::string roomName, uint32_t baseAddress)
{
	return "";
}

std::string ZRoomCommand::GenerateSourceCodePass3(std::string roomName)
{
	return "";
}

std::string ZRoomCommand::GenerateExterns() const
{
	return "";
}

std::string ZRoomCommand::Save()
{
	return std::string();
}

std::string ZRoomCommand::PreGenSourceFiles()
{
	return std::string();
}

size_t ZRoomCommand::GetRawDataSize() const
{
	return 8;
}

std::string ZRoomCommand::GetCommandCName() const
{
	return "SCmdBase";
}

RoomCommand ZRoomCommand::GetRoomCommand() const
{
	return RoomCommand::Error;
}