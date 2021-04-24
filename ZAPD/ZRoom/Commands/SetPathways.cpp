#include "SetPathways.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

REGISTER_ZFILENODE(Path, ZSetPathways);

using namespace std;

ZSetPathways::ZSetPathways(ZFile* nParent) : ZRoomCommand(nParent)
{
}

ZSetPathways::ZSetPathways(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData, int nRawDataIndex,
                           bool nIsFromHeader)
	: ZRoomCommand(nZRoom, nRawData, nRawDataIndex)
{
	ZResource::rawData = nRawData;
	ZResource::rawDataIndex = nRawDataIndex;
	isFromHeader = nIsFromHeader;

	if (segmentOffset != 0)
		ZResource::parent->AddDeclarationPlaceholder(segmentOffset);
}

void ZSetPathways::ParseRawDataLate()
{
	if (isFromHeader)
		segmentOffset =
			GETSEGOFFSET(BitConverter::ToUInt32BE(ZResource::rawData, ZResource::rawDataIndex + 4));
	else
		segmentOffset = ZResource::rawDataIndex;

	if (segmentOffset != 0)
		ZResource::parent->AddDeclarationPlaceholder(segmentOffset);

	int numPaths = 1;
	if (Globals::Instance->game == ZGame::MM_RETAIL)
		numPaths = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 8;

	pathwayList = PathwayList(ZResource::parent, ZResource::rawData, segmentOffset, numPaths);
}

void ZSetPathways::DeclareReferencesLate(const std::string& prefix)
{
	pathwayList.GetSourceOutputCode(prefix);
}

void ZSetPathways::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	ZResource::parent->AddDeclaration(
		cmdAddress, DeclarationAlignment::None, 8,
		StringHelper::Sprintf("static %s", GetCommandCName().c_str()),
		StringHelper::Sprintf("%sSet%04XCmd%02X", name.c_str(), commandSet & 0x00FFFFFF, cmdIndex,
	                          cmdID),
		StringHelper::Sprintf("%s // 0x%04X", bodyStr.c_str(), cmdAddress));
}

string ZSetPathways::GetSourceOutputCode(const std::string& prefix)
{
	pathwayList.GetSourceOutputCode(ZResource::parent->GetName());

	return "";
}

std::string ZSetPathways::GetBodySourceCode() const
{
	std::string listName = ZResource::parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENECMD_PATH_LIST(%s)", listName.c_str());
}

int32_t ZSetPathways::GetRawDataSize()
{
	int32_t size = pathwayList.GetRawDataSize();

	return ZRoomCommand::GetRawDataSize() + size;
}

string ZSetPathways::GetCommandCName() const
{
	return "SCmdPathList";
}

RoomCommand ZSetPathways::GetRoomCommand() const
{
	return RoomCommand::SetPathways;
}

PathwayEntry::PathwayEntry(const std::vector<uint8_t>& rawData, int rawDataIndex)
	: numPoints(rawData[rawDataIndex + 0]), unk1(rawData[rawDataIndex + 1]),
	  unk2(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  listSegmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4)))
{
	uint32_t currentPtr = listSegmentOffset;

	for (int i = 0; i < numPoints; i++)
	{
		x = BitConverter::ToInt16BE(rawData, currentPtr + 0);
		y = BitConverter::ToInt16BE(rawData, currentPtr + 2);
		z = BitConverter::ToInt16BE(rawData, currentPtr + 4);

		Vec3s point = Vec3s(x, y, z);
		points.push_back(point);

		currentPtr += 6;
	}

	if (numPoints == 0)  // Hack for SharpOcarina
	{
		for (int i = 0; i < 3; i++)
		{
			Vec3s point = Vec3s(0, 0, 0);
			points.push_back(point);
		}
	}
}

PathwayList::PathwayList(ZFile* nParent, const std::vector<uint8_t>& rawData, int rawDataIndex,
                         int length)
{
	parent = nParent;
	_rawDataIndex = rawDataIndex;

	uint32_t currentPtr = rawDataIndex;

	for (int pathIndex = 0; pathIndex < length; pathIndex++)
	{
		PathwayEntry path(rawData, currentPtr);
		currentPtr += 8;

		if (path.listSegmentOffset == 0)
			break;

		pathways.push_back(path);
	}
}

void PathwayList::GetSourceOutputCode(const std::string& prefix)
{
	{
		string declaration = "";
		size_t index = 0;
		for (const auto& entry : pathways)
		{
			if (Globals::Instance->game == ZGame::MM_RETAIL)
				declaration += StringHelper::Sprintf("	{ %i, %i, %i, (u32)%sPathwayList0x%06X },",
				                                     entry.numPoints, entry.unk1, entry.unk2,
				                                     prefix.c_str(), entry.listSegmentOffset);
			else
				declaration +=
					StringHelper::Sprintf("	{ %i, (u32)%sPathwayList0x%06X },", entry.numPoints,
				                          prefix.c_str(), entry.listSegmentOffset);

			if (index < pathways.size() - 1)
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(
			_rawDataIndex, DeclarationAlignment::None, DeclarationPadding::None,
			pathways.size() * 8, "Path",
			StringHelper::Sprintf("%sPathway0x%06X", prefix.c_str(), _rawDataIndex),
			pathways.size(), declaration);
	}

	for (const auto& entry : pathways)
	{
		string declaration = "";

		size_t index = 0;
		for (const auto& point : entry.points)
		{
			declaration += StringHelper::Sprintf("	{ %i, %i, %i }, //0x%06X", point.x, point.y,
			                                     point.z, entry.listSegmentOffset + (index * 6));

			if (index < entry.points.size() - 1)
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(
			entry.listSegmentOffset, DeclarationAlignment::Align4, DeclarationPadding::Pad4,
			entry.points.size() * 6, "Vec3s",
			StringHelper::Sprintf("%sPathwayList0x%06X", prefix.c_str(), entry.listSegmentOffset),
			entry.points.size(), declaration);
	}
}

int32_t PathwayList::GetRawDataSize()
{
	int32_t pointsSize = 0;

	for (const auto& entry : pathways)
	{
		pointsSize += entry.points.size() * 6;
	}

	return pathways.size() * 8 + pointsSize;
}
