#include "SetCollisionHeader.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetCollisionHeader::SetCollisionHeader(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                       int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

void SetCollisionHeader::ParseRawData()
{
	collisionHeader = new ZCollisionHeader(parent);
	collisionHeader->SetRawData(rawData);
	collisionHeader->SetRawDataIndex(segmentOffset);
	collisionHeader->SetName(
		StringHelper::Sprintf("%sCollisionHeader_%06X", parent->GetName().c_str(), segmentOffset));
	collisionHeader->ParseRawData();
}

SetCollisionHeader::~SetCollisionHeader()
{
	delete collisionHeader;
}

string SetCollisionHeader::GetBodySourceCode() const
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENE_CMD_COL_HEADER(%s)", listName.c_str());
}

string SetCollisionHeader::GetCommandCName() const
{
	return "SCmdColHeader";
}

RoomCommand SetCollisionHeader::GetRoomCommand() const
{
	return RoomCommand::SetCollisionHeader;
}
