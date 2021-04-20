#include "SetCutscenes.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetCutscenes::SetCutscenes(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	ParseRawData();
}

void SetCutscenes::ParseRawData()
{
	numCutscenes = rawData[rawDataIndex + 1];

	uint32_t curPtr = segmentOffset;
	string output = "";

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
			cutscene->ExtractFromXML(
				nullptr, rawData, entry.segmentOffset,
				"");  // TODO: Use ExtractFromFile() here when that gets implemented
			cutscenes.push_back(cutscene);
		}

		zRoom->parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::None, DeclarationPadding::None,
			cutsceneEntries.size() * 8, "CutsceneEntry",
			StringHelper::Sprintf("%sCutsceneEntryList0x%06X", zRoom->GetName().c_str(),
		                          segmentOffset),
			cutsceneEntries.size(), declaration);
	}

	for (ZCutsceneBase* cutscene : cutscenes)
	{
		if (cutscene->getSegmentOffset() != 0)
		{
			Declaration* decl = zRoom->parent->GetDeclaration(cutscene->getSegmentOffset());
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

string SetCutscenes::GetBodySourceCode()
{
	Declaration* decl = zRoom->parent->GetDeclaration(segmentOffset);
	if (decl != nullptr)
	{
		return StringHelper::Sprintf("%s, %i, (u32)%s", GetCommandHex().c_str(), numCutscenes,
		                             decl->varName.c_str());
	}

	return StringHelper::Sprintf("%s, %i, (u32)%sCutsceneData0x%06X", GetCommandHex().c_str(), numCutscenes,
	                             zRoom->GetName().c_str(), segmentOffset);
}

int32_t SetCutscenes::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (0);
}

string SetCutscenes::GenerateExterns()
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("extern CutsceneEntry %sCutsceneEntryList0x%06X[];\n",
		                             zRoom->GetName().c_str(), segmentOffset);
	}

	Declaration* decl = zRoom->parent->GetDeclaration(segmentOffset);
	if (decl != nullptr && decl->varName != "")
	{
		return StringHelper::Sprintf("extern s32 %s[];\n", decl->varName.c_str());
	}
	return StringHelper::Sprintf("extern s32 %sCutsceneData0x%06X[];\n", zRoom->GetName().c_str(),
	                             segmentOffset);
}

string SetCutscenes::GetCommandCName()
{
	return "SCmdCutsceneData";
}

RoomCommand SetCutscenes::GetRoomCommand()
{
	return RoomCommand::SetCutscenes;
}

CutsceneEntry::CutsceneEntry(std::vector<uint8_t> rawData, int rawDataIndex)
	: segmentOffset(BitConverter::ToInt32BE(rawData, rawDataIndex + 0) & 0x00FFFFFF),
	  exit(BitConverter::ToInt16BE(rawData, rawDataIndex + 4)), entrance(rawData[rawDataIndex + 6]),
	  flag(rawData[rawDataIndex + 7])
{
}
