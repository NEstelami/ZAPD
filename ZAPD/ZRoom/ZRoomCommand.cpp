#include "ZRoomCommand.h"
#include "StringHelper.h"
#include "ZRoom.h"
#include "BitConverter.h"

using namespace std;

ZRoomCommand::ZRoomCommand(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData, int nRawDataIndex)
	: zRoom{nZRoom}, parent{nZRoom->parent}, rawData {nRawData}, rawDataIndex{nRawDataIndex}
{
	cmdID = static_cast<RoomCommand>(rawData.at(rawDataIndex));
	cmdAddress = rawDataIndex;

	cmdArg1 = rawData.at(rawDataIndex + 1);
	segmentOffset = GETSEGOFFSET(BitConverter::ToUInt32BE(rawData, rawDataIndex + 4));
}

void ZRoomCommand::ParseRawData()
{
}

void ZRoomCommand::DeclareReferences()
{
}

std::string ZRoomCommand::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s,", GetCommandHex().c_str());
}

string ZRoomCommand::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	return GetBodySourceCode();
}

string ZRoomCommand::GenerateExterns()
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

int32_t ZRoomCommand::GetRawDataSize()
{
	return 8;
}

string ZRoomCommand::GetCommandCName()
{
	return "SCmdBase";
}

RoomCommand ZRoomCommand::GetRoomCommand()
{
	return RoomCommand::Error;
}

std::string ZRoomCommand::GetCommandHex()
{
	return StringHelper::Sprintf("0x%02X", static_cast<uint8_t>(cmdID));
}
