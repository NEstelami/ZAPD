#include "SetCollisionHeader.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetCollisionHeader::SetCollisionHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData,
                                       int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	collisionHeader = new ZCollisionHeader(nZRoom->parent);
	collisionHeader->SetRawData(rawData);
	collisionHeader->SetRawDataIndex(segmentOffset);
	collisionHeader->SetName(
		StringHelper::Sprintf("%sCollisionHeader0x%06X", nZRoom->GetName().c_str(), segmentOffset));
	collisionHeader->ParseRawData();
}

SetCollisionHeader::~SetCollisionHeader()
{
	delete collisionHeader;
}

string SetCollisionHeader::GetBodySourceCode()
{
	std::string listName = "NULL";
	if (segmentOffset != 0)
	{
		Declaration* decl = parent->GetDeclaration(segmentOffset);
		if (decl != nullptr)
		{
			listName = "&" + decl->varName;
		}
		else
		{
			listName = StringHelper::Sprintf("0x%08X", segmentOffset);
		}
	}

	return StringHelper::Sprintf("%s, 0, (u32)%s", GetCommandHex().c_str(), listName.c_str());
}

string SetCollisionHeader::GetCommandCName()
{
	return "SCmdColHeader";
}

RoomCommand SetCollisionHeader::GetRoomCommand()
{
	return RoomCommand::SetCollisionHeader;
}