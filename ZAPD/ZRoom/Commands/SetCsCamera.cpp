#include "SetCsCamera.h"

#include "BitConverter.h"
#include "StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"

SetCsCamera::SetCsCamera(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

void SetCsCamera::ParseRawData()
{
	int numCameras = cmdArg1;

	uint32_t currentPtr = segmentOffset;
	int32_t numPoints = 0;

	for (int32_t i = 0; i < numCameras; i++)
	{
		CsCameraEntry entry(rawData, currentPtr);
		numPoints += entry.GetNumPoints();

		currentPtr += entry.GetRawDataSize();
		cameras.push_back(entry);
	}

	if (numPoints > 0)
	{
		uint32_t currentPtr = cameras.at(0).GetSegmentOffset();

		for (int32_t i = 0; i < numPoints; i++)
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
		std::string declaration = "";
		size_t index = 0;
		for (auto& point : points)
		{
			declaration += StringHelper::Sprintf("\t%s, //0x%06X", point.GetSourceValue().c_str(),
			                                     cameras.at(0).segmentOffset + (index * 6));

			if (index < points.size() - 1)
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(
			cameras.at(0).GetSegmentOffset(), DeclarationAlignment::None, DeclarationPadding::None,
			points.size() * points.at(0).GetRawDataSize(), points.at(0).GetSourceTypeName().c_str(),
			StringHelper::Sprintf("%sCsCameraPoints_%06X", prefix.c_str(),
		                          cameras.at(0).GetSegmentOffset()),
			points.size(), declaration);
	}

	if (!cameras.empty())
	{
		std::string camPointsName = parent->GetDeclarationPtrName(cameras.at(0).GetSegmentOffset());
		std::string declaration = "";

		size_t index = 0;
		size_t pointsIndex = 0;
		for (const auto& entry : cameras)
		{
			declaration +=
				StringHelper::Sprintf("\t{ %i, %i, %s[%i] },", entry.type, entry.numPoints,
			                          camPointsName.c_str(), pointsIndex);

			if (index < cameras.size() - 1)
				declaration += "\n";

			index++;
			pointsIndex += entry.GetNumPoints();
		}

		const auto& entry = cameras.front();
		std::string camTypename = entry.GetSourceTypeName();

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::Align4, DeclarationPadding::Pad16,
			cameras.size() * entry.GetRawDataSize(), camTypename,
			StringHelper::Sprintf("%s%s_%06X", prefix.c_str(), camTypename.c_str(), segmentOffset),
			cameras.size(), declaration);
	}
}

std::string SetCsCamera::GetBodySourceCode() const
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("SCENE_CMD_CAM_LIST(%i, %s)", cameras.size(), listName.c_str());
}

size_t SetCsCamera::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (cameras.size() * 8) + (points.size() * 6);
}

std::string SetCsCamera::GetCommandCName() const
{
	return "SCmdCsCameraList";
}

RoomCommand SetCsCamera::GetRoomCommand() const
{
	return RoomCommand::SetCsCamera;
}

CsCameraEntry::CsCameraEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: baseOffset(rawDataIndex), type(BitConverter::ToInt16BE(rawData, rawDataIndex + 0)),
	  numPoints(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  segmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4)))
{
}

std::string CsCameraEntry::GetSourceTypeName() const
{
	return "CsCameraEntry";
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
