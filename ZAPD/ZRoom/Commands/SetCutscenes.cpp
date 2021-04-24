#include "SetCutscenes.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetCutscenes::SetCutscenes(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

void SetCutscenes::ParseRawData()
{
	numCutscenes = cmdArg1;

	uint32_t curPtr = segmentOffset;
	std::string output = "";

	if (Globals::Instance->game == ZGame::OOT_RETAIL || Globals::Instance->game == ZGame::OOT_SW97)
	{
		ZCutscene* cutscene = new ZCutscene(parent);
		cutscene->ExtractFromFile(rawData, segmentOffset, "");

		auto decl = parent->GetDeclaration(segmentOffset);
		if (decl == nullptr)
		{
			cutscene->DeclareVar(zRoom->GetName().c_str(), "");
		}

		cutscenes.push_back(cutscene);
	}
	else
	{
		int32_t currentPtr = segmentOffset;
		string declaration = "";

		for (int i = 0; i < numCutscenes; i++)
		{
			CutsceneEntry entry(rawData, currentPtr);
			cutsceneEntries.push_back(entry);
			currentPtr += 8;

			declaration += StringHelper::Sprintf(
				"    { %sCutsceneData0x%06X, 0x%04X, 0x%02X, 0x%02X },", zRoom->GetName().c_str(),
				entry.segmentOffset, entry.exit, entry.entrance, entry.flag);

			if (i < numCutscenes - 1)
				declaration += "\n";

			ZCutsceneMM* cutscene = new ZCutsceneMM(parent);
			cutscene->ExtractFromFile(rawData, entry.segmentOffset, "");
			cutscenes.push_back(cutscene);
		}

		parent->AddDeclarationArray(segmentOffset, DeclarationAlignment::Align4,
		                            cutsceneEntries.size() * 8, "CutsceneEntry",
		                            StringHelper::Sprintf("%sCutsceneEntryList_%06X",
		                                                  zRoom->GetName().c_str(), segmentOffset),
		                            cutsceneEntries.size(), declaration);
	}

	for (ZCutsceneBase* cutscene : cutscenes)
	{
		if (cutscene->getSegmentOffset() != 0)
		{
			Declaration* decl = parent->GetDeclaration(cutscene->getSegmentOffset());
			if (decl == nullptr)
			{
				cutscene->GetSourceOutputCode(zRoom->GetName());
			}
			else if (decl->text == "")
			{
				decl->text = cutscene->GetBodySourceCode();
			}
		}
	}
}

SetCutscenes::~SetCutscenes()
{
	for (ZCutsceneBase* cutscene : cutscenes)
		delete cutscene;
}

string SetCutscenes::GetBodySourceCode() const
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENECMD_CUTSCENE_LIST(%i, %s)", numCutscenes, listName.c_str());
}

int32_t SetCutscenes::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize();
}

string SetCutscenes::GetCommandCName() const
{
	return "SCmdCutsceneData";
}

RoomCommand SetCutscenes::GetRoomCommand() const
{
	return RoomCommand::SetCutscenes;
}

CutsceneEntry::CutsceneEntry(const std::vector<uint8_t>& rawData, int rawDataIndex)
	: segmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 0))),
	  exit(BitConverter::ToInt16BE(rawData, rawDataIndex + 4)), entrance(rawData[rawDataIndex + 6]),
	  flag(rawData[rawDataIndex + 7])
{
}
