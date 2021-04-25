#include "ZRoomCommand.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "ZRoom.h"

ZRoomCommand::ZRoomCommand(ZFile* nParent) : ZResource(nParent)
{
}

ZRoomCommand::ZRoomCommand(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData,
                           uint32_t nRawDataIndex)
	: ZResource(nZRoom->parent), zRoom{nZRoom}
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;
	cmdID = static_cast<RoomCommand>(rawData.at(rawDataIndex));
	cmdAddress = rawDataIndex;

	cmdArg1 = rawData.at(rawDataIndex + 1);
	segmentOffset = GETSEGOFFSET(BitConverter::ToUInt32BE(rawData, rawDataIndex + 4));
}

void ZRoomCommand::DeclareReferences(const std::string& prefix)
{
}

void ZRoomCommand::ParseRawDataLate()
{
}

void ZRoomCommand::DeclareReferencesLate(const std::string& prefix)
{
}

std::string ZRoomCommand::GetCommandCName() const
{
	return "SCmdBase";
}

RoomCommand ZRoomCommand::GetRoomCommand() const
{
	return RoomCommand::Error;
}

std::string ZRoomCommand::GetCommandHex() const
{
	return StringHelper::Sprintf("0x%02X", static_cast<uint8_t>(cmdID));
}
