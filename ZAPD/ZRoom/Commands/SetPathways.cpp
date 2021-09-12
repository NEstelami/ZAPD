#include "SetPathways.h"
#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetPathways::SetPathways(ZFile* nParent) : ZRoomCommand(nParent), pathwayList(nParent)
{
}

void SetPathways::DeclareReferences([[maybe_unused]] const std::string& prefix)
{
	if (segmentOffset != 0)
	{
		std::string varName =
			StringHelper::Sprintf("%sPathway_%06X", prefix.c_str(), segmentOffset);
		parent->AddDeclarationPlaceholder(segmentOffset, varName);
	}
}

void SetPathways::ParseRawDataLate()
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		auto numPaths = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 8;
		pathwayList.SetNumPaths(numPaths);
	}

	pathwayList.SetRawDataIndex(segmentOffset);
	pathwayList.ParseRawData();
}

void SetPathways::DeclareReferencesLate(const std::string& prefix)
{
	std::string varName = StringHelper::Sprintf("%sPathway_%06X", prefix.c_str(), segmentOffset);
	pathwayList.SetName(varName);
	pathwayList.DeclareReferences(prefix);
	pathwayList.GetSourceOutputCode(prefix);
}

std::string SetPathways::GetBodySourceCode() const
{
	std::string listName;
	Globals::Instance->GetSegmentedPtrName(cmdArg2, parent, "Path", listName);
	return StringHelper::Sprintf("SCENE_CMD_PATH_LIST(%s)", listName.c_str());
}

size_t SetPathways::GetRawDataSize() const
{
	int32_t size = pathwayList.GetRawDataSize();

	return ZRoomCommand::GetRawDataSize() + size;
}

std::string SetPathways::GetCommandCName() const
{
	return "SCmdPathList";
}

RoomCommand SetPathways::GetRoomCommand() const
{
	return RoomCommand::SetPathways;
}
