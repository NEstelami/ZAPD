#include "SetCollisionHeader.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

SetCollisionHeader::SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData,
                                       uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4));
	collisionHeader = new ZCollisionHeader(nZRoom->parent);
	collisionHeader->SetRawData(rawData);
	collisionHeader->SetRawDataIndex(segmentOffset);
	collisionHeader->SetName(
		StringHelper::Sprintf("%sCollisionHeader0x%06X", nZRoom->GetName().c_str(), segmentOffset));
	collisionHeader->ParseRawData();
}

std::string SetCollisionHeader::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return StringHelper::Sprintf(
		"%s 0x00, (u32)&%sCollisionHeader0x%06X",
		ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(),
		zRoom->GetName().c_str(), segmentOffset);
}

std::string SetCollisionHeader::GenerateSourceCodePass2(std::string roomName, uint32_t baseAddress)
{
	return "";
}

std::string SetCollisionHeader::GetCommandCName() const
{
	return "SCmdColHeader";
}

std::string SetCollisionHeader::GenerateExterns() const
{
	return "";
}

RoomCommand SetCollisionHeader::GetRoomCommand() const
{
	return RoomCommand::SetCollisionHeader;
}