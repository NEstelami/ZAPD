#include "SetRoomList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetRoomList::SetRoomList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	ParseRawData();
	DeclareReferences(zRoom->GetName());
}

void SetRoomList::ParseRawData()
{
	int numRooms = cmdArg1;

	int32_t currentPtr = segmentOffset;

	for (int i = 0; i < numRooms; i++)
	{
		RoomEntry entry(rawData, currentPtr);
		rooms.push_back(entry);

		currentPtr += 8;
	}

	zRoom->roomCount = numRooms;
}

void SetRoomList::DeclareReferences(const std::string& prefix)
{
	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, rooms.size() * 8, "RomFile",
		StringHelper::Sprintf("%sRoomList0x%06X", prefix.c_str(), segmentOffset), 0, "");
}

std::string SetRoomList::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("%s, 0x%02X, (u32)%s", GetCommandHex().c_str(), rooms.size(), listName.c_str());
}

string SetRoomList::GetCommandCName()
{
	return "SCmdRoomList";
}

RoomCommand SetRoomList::GetRoomCommand()
{
	return RoomCommand::SetRoomList;
}

std::string SetRoomList::PreGenSourceFiles()
{
	std::string declaration = "";

	for (ZFile* file : Globals::Instance->files)
	{
		for (ZResource* res : file->resources)
		{
			if (res->GetResourceType() == ZResourceType::Room && res != zRoom)
			{
				string roomName = res->GetName();
				declaration += StringHelper::Sprintf(
					"    { (u32)_%sSegmentRomStart, (u32)_%sSegmentRomEnd },\n", roomName.c_str(),
					roomName.c_str());
			}
		}
	}

	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, rooms.size() * 8, "RomFile",
		StringHelper::Sprintf("%sRoomList0x%06X", zRoom->GetName().c_str(), segmentOffset), 0,
		declaration);

	return "";
}

RoomEntry::RoomEntry(int32_t nVAS, int32_t nVAE)
{
	virtualAddressStart = nVAS;
	virtualAddressEnd = nVAE;
}

RoomEntry::RoomEntry(std::vector<uint8_t> rawData, int rawDataIndex)
	: RoomEntry(BitConverter::ToInt32BE(rawData, rawDataIndex + 0),
                BitConverter::ToInt32BE(rawData, rawDataIndex + 4))
{
}
