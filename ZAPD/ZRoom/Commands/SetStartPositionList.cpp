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
	uint8_t numActors = rawData.at(rawDataIndex + 1);
	segmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4));

	if (segmentOffset != 0)
		zRoom->parent->AddDeclarationPlaceholder(segmentOffset);

	uint32_t currentPtr = segmentOffset;

	for (int i = 0; i < numActors; i++)
	{
		actors.push_back(ActorSpawnEntry(rawData, currentPtr));
		currentPtr += 16;
	}
}

std::string SetStartPositionList::GetBodySourceCode()
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

	return StringHelper::Sprintf("%s, 0x%02X, (u32)%s", GetCommandHex().c_str(), actors.size(), listName.c_str());
}

string SetStartPositionList::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	if (!actors.empty())
	{
		string declaration = "";

		for (const auto& entry : actors)
		{
			declaration += StringHelper::Sprintf("    { %s },\n", entry.GetBodySourceCode().c_str());
		}

		zRoom->parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::None, actors.size() * 16, "ActorEntry",
			StringHelper::Sprintf("%sStartPositionList0x%06X", zRoom->GetName().c_str(), segmentOffset),
			0, declaration);
	}

	return GetBodySourceCode();
}

string SetStartPositionList::GenerateExterns()
{
	return StringHelper::Sprintf("extern ActorEntry %sStartPositionList0x%06X[];\n",
	                             zRoom->GetName().c_str(), segmentOffset);
}

string SetStartPositionList::GetCommandCName()
{
	return "SCmdSpawnList";
}

RoomCommand SetStartPositionList::GetRoomCommand()
{
	return RoomCommand::SetStartPositionList;
}