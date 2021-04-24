#include "SetCsCamera.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetCsCamera::SetCsCamera(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

void SetCsCamera::ParseRawData()
{
	int numCameras = cmdArg1;

	uint32_t currentPtr = segmentOffset;
	int numPoints = 0;

	for (int i = 0; i < numCameras; i++)
	{
		CsCameraEntry entry(rawData, currentPtr);
		numPoints += entry.GetNumPoints();

		currentPtr += entry.GetRawDataSize();
		cameras.push_back(entry);
	}

	if (numPoints > 0)
	{
		uint32_t currentPtr = cameras.at(0).GetSegmentOffset();

		for (int i = 0; i < numPoints; i++)
		{
			ZVector vec(parent);
			vec.SetRawData(rawData);
			vec.SetRawDataIndex(currentPtr);
			vec.SetScalarType(ZScalarType::ZSCALAR_S16);
			vec.SetDimensions(3);
			vec.ParseRawData();

			currentPtr += vec.GetRawDataSize();
			points.push_back(vec);
		}
	}

	if (segmentOffset != 0)
		parent->AddDeclarationPlaceholder(segmentOffset);
}

void SetCsCamera::DeclareReferences(const std::string& prefix)
{
	if (points.size() > 0)
	{
		string declaration = "";
		size_t index = 0;
		for (auto& point : points)
		{
			declaration += StringHelper::Sprintf("\t%s, //0x%06X", point.GetSourceValue().c_str(), cameras.at(0).segmentOffset + (index * 6));

			if (index < points.size() - 1)
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(cameras.at(0).GetSegmentOffset(), DeclarationAlignment::None,
		                                   DeclarationPadding::None, points.size() * points.at(0).GetRawDataSize(), points.at(0).GetSourceTypeName().c_str(),
		                                   StringHelper::Sprintf("%sCsCameraPoints0x%06X",
		                                                         prefix.c_str(),
		                                                         cameras.at(0).GetSegmentOffset()),
		                                   points.size(), declaration);
	}

	std::string camPointsName = parent->GetDeclarationPtrName(cameras.at(0).GetSegmentOffset());
	std::string declaration = "";

	size_t index = 0;
	size_t pointsIndex = 0;
	for (const auto& entry : cameras)
	{
		declaration += StringHelper::Sprintf("\t{ %i, %i, %s[%i] },",
												entry.type, entry.numPoints, camPointsName.c_str(), pointsIndex);

		if (index < cameras.size() - 1)
			declaration += "\n";

		index++;
		pointsIndex += entry.GetNumPoints();
	}

	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::Align4, DeclarationPadding::Pad16,
		cameras.size() * 8, "CsCameraEntry",
		StringHelper::Sprintf("%sCsCameraList0x%06X", prefix.c_str(), segmentOffset),
		cameras.size(), declaration);
}

std::string SetCsCamera::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENE_CMD_UNUSED_2(%i, %s)", cameras.size(), listName.c_str());
}

int32_t SetCsCamera::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (cameras.size() * 8) + (points.size() * 6);
}

string SetCsCamera::GetCommandCName()
{
	return "SCmdCsCameraList";
}

RoomCommand SetCsCamera::GetRoomCommand()
{
	return RoomCommand::SetCsCamera;
}

CsCameraEntry::CsCameraEntry(const std::vector<uint8_t>& rawData, int rawDataIndex)
	: baseOffset(rawDataIndex), type(BitConverter::ToInt16BE(rawData, rawDataIndex + 0)),
	  numPoints(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  segmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4)))
{
}

int32_t CsCameraEntry::GetRawDataSize() const
{
	return 8;
}

int16_t CsCameraEntry::GetNumPoints() const
{
	return numPoints;
}

int CsCameraEntry::GetSegmentOffset() const
{
	return segmentOffset;
}
