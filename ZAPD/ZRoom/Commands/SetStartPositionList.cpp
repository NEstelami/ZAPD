#include "SetStartPositionList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZNames.h"
#include "../ZRoom.h"

using namespace std;

SetStartPositionList::SetStartPositionList(ZRoom* nZRoom, std::vector<uint8_t> rawData,
                                           int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	ParseRawData();
	DeclareReferences(zRoom->GetName());
}

void SetStartPositionList::ParseRawData()
{
	uint8_t numActors = cmdArg1;

	if (segmentOffset != 0)
		parent->AddDeclarationPlaceholder(segmentOffset);

	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < numActors; i++)
	{
		actors.push_back(ActorSpawnEntry(rawData, currentPtr));
		currentPtr += 16;
	}
}

void SetStartPositionList::DeclareReferences(const std::string& prefix)
{
	if (!actors.empty())
	{
		string declaration = "";

		for (const auto& entry : actors)
		{
			declaration += StringHelper::Sprintf("    { %s },\n", entry.GetBodySourceCode().c_str());
		}

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::None, actors.size() * 16, "ActorEntry",
			StringHelper::Sprintf("%sStartPositionList0x%06X", prefix.c_str(), segmentOffset),
			0, declaration);
	}
}

std::string SetStartPositionList::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("%s, 0x%02X, (u32)%s", GetCommandHex().c_str(), actors.size(), listName.c_str());
}

string SetStartPositionList::GetCommandCName()
{
	return "SCmdSpawnList";
}

RoomCommand SetStartPositionList::GetRoomCommand()
{
	return RoomCommand::SetStartPositionList;
}
