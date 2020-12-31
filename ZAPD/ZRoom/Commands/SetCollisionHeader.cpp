#include "SetCollisionHeader.h"
#include "../ZRoom.h"
#include "../../ZFile.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"

using namespace std;

SetCollisionHeader::SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = SEG2FILESPACE(BitConverter::ToInt32BE(rawData, rawDataIndex + 4));
	collisionHeader = ZCollisionHeader(nZRoom->parent, StringHelper::Sprintf("%s_collisionHeader", nZRoom->GetName().c_str()), rawData, segmentOffset);
}

string SetCollisionHeader::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	return StringHelper::Sprintf("%s 0x00, (u32)&%s_collisionHeader", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), zRoom->GetName().c_str());
}

string SetCollisionHeader::GenerateSourceCodePass2(string roomName, int baseAddress)
{
	return "";
}

string SetCollisionHeader::GetCommandCName()
{
	return "SCmdColHeader";
}

string SetCollisionHeader::GenerateExterns()
{
	return StringHelper::Sprintf("extern CollisionHeader %s_collisionHeader_%08X;\n", zRoom->GetName().c_str(), segmentOffset);
}

RoomCommand SetCollisionHeader::GetRoomCommand()
{
	return RoomCommand::SetCollisionHeader;
}