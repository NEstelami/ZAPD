#include "SetActorCutsceneList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetActorCutsceneList::SetActorCutsceneList(ZRoom* nZRoom, std::vector<uint8_t> rawData,
                                           int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	ParseRawData();
}

void SetActorCutsceneList::ParseRawData()
{
	int numCutscenes = cmdArg1;

	if (numCutscenes > 0) {
		int32_t currentPtr = segmentOffset;

		for (int i = 0; i < numCutscenes; i++)
		{
			ActorCutsceneEntry entry(rawData, currentPtr);
			cutscenes.push_back(entry);

			currentPtr += 16;
		}

		string declaration = "";

		for (size_t i = 0; i < cutscenes.size(); i++)
		{
			const auto& entry = cutscenes.at(i);
			declaration += StringHelper::Sprintf("    { %s },", entry.GetBodySourceCode().c_str());

			if (i + 1 < cutscenes.size())
			{
				declaration += "\n";
			}
		}

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::Align4, cutscenes.size() * 16, cutscenes.at(0).GetSourceTypeName(),
			StringHelper::Sprintf("%sActorCutsceneList_%06X", zRoom->GetName().c_str(), segmentOffset),
			0, declaration);
	}
}

string SetActorCutsceneList::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("%s, %3i, (u32)%s", GetCommandHex().c_str(), cutscenes.size(), listName.c_str());
}

int32_t SetActorCutsceneList::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (cutscenes.size() * 16);
}

string SetActorCutsceneList::GetCommandCName()
{
	return "SCmdCutsceneActorList";
}

RoomCommand SetActorCutsceneList::GetRoomCommand()
{
	return RoomCommand::SetActorCutsceneList;
}

ActorCutsceneEntry::ActorCutsceneEntry(std::vector<uint8_t> rawData, int rawDataIndex)
	: priority(BitConverter::ToInt16BE(rawData, rawDataIndex + 0)),
	  length(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  unk4(BitConverter::ToInt16BE(rawData, rawDataIndex + 4)),
	  unk6(BitConverter::ToInt16BE(rawData, rawDataIndex + 6)),
	  additionalCutscene(BitConverter::ToInt16BE(rawData, rawDataIndex + 8)),
	  sound(rawData[rawDataIndex + 0xA]), unkB(rawData[rawDataIndex + 0xB]),
	  unkC(BitConverter::ToInt16BE(rawData, rawDataIndex + 0xC)), unkE(rawData[rawDataIndex + 0xE]),
	  letterboxSize(rawData[rawDataIndex + 0xF])
{
}

std::string ActorCutsceneEntry::GetBodySourceCode() const
{
	return StringHelper::Sprintf("%i, %i, %i, %i, %i, %i, %i, %i, %i, %i", priority, length, unk4, unk6, additionalCutscene, sound, unkB, unkC, unkE, letterboxSize);
}

std::string ActorCutsceneEntry::GetSourceTypeName() const
{
	return "ActorCutscene";
}
