#include "SetObjectList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZNames.h"
#include "../ZRoom.h"

using namespace std;

SetObjectList::SetObjectList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

void SetObjectList::ParseRawData()
{
	uint8_t objectCnt = rawData[rawDataIndex + 1];
	uint32_t currentPtr = segmentOffset;

	for (uint8_t i = 0; i < objectCnt; i++)
	{
		uint16_t objectIndex = BitConverter::ToInt16BE(rawData, currentPtr);
		objects.push_back(objectIndex);
		currentPtr += 2;
	}

	if (segmentOffset != 0)
		parent->AddDeclarationPlaceholder(segmentOffset);
}

void SetObjectList::DeclareReferences(const std::string& prefix)
{
	if (!objects.empty())
	{
		std::string declaration = "";

		for (size_t i = 0; i < objects.size(); i++)
		{
			uint16_t objectIndex = objects[i];
			declaration +=
				StringHelper::Sprintf("    %s,", ZNames::GetObjectName(objectIndex).c_str());

			if (i < objects.size() - 1)
				declaration += "\n";
		}

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::None, objects.size() * 2, "s16",
			StringHelper::Sprintf("%sObjectList_%06X", prefix.c_str(), segmentOffset),
			objects.size(), declaration);
	}
}

std::string SetObjectList::GetBodySourceCode() const
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENE_CMD_OBJECT_LIST(%i, %s)", objects.size(), listName.c_str());
}

int32_t SetObjectList::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (objects.size() * 2);
}

string SetObjectList::GetCommandCName() const
{
	return "SCmdObjectList";
}

RoomCommand SetObjectList::GetRoomCommand() const
{
	return RoomCommand::SetObjectList;
}
