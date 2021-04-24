#include "SetStartPositionList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZNames.h"
#include "../ZRoom.h"

using namespace std;

SetStartPositionList::SetStartPositionList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                           int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
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
		std::string declaration = "";

		size_t index = 0;
		for (const auto& entry : actors)
		{
			declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode().c_str());
			if (index + 1 < actors.size())
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::Align4, actors.size() * 16, "ActorEntry",
			StringHelper::Sprintf("%sStartPositionList0x%06X", prefix.c_str(), segmentOffset), 0,
			declaration);
	}
}

std::string SetStartPositionList::GetBodySourceCode() const
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENE_CMD_SPAWN_LIST(%i, %s)", actors.size(), listName.c_str());
}

string SetStartPositionList::GetCommandCName() const
{
	return "SCmdSpawnList";
}

RoomCommand SetStartPositionList::GetRoomCommand() const
{
	return RoomCommand::SetStartPositionList;
}
