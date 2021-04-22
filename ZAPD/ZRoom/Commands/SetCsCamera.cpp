#include "SetCsCamera.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetCsCamera::SetCsCamera(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	ParseRawData();
	DeclareReferences();
}

void SetCsCamera::ParseRawData()
{
	int numCameras = cmdArg1;

	uint32_t currentPtr = segmentOffset;
	int numPoints = 0;

	for (int i = 0; i < numCameras; i++)
	{
		CsCameraEntry entry(rawData, currentPtr);

		cameras.push_back(entry);
		numPoints += entry.GetNumPoints();

		currentPtr += 8;
	}

	if (numPoints > 0)
	{
		uint32_t currentPtr = cameras[0].GetSegmentOffset();

		for (int i = 0; i < numPoints; i++)
		{
			int16_t x = BitConverter::ToInt16BE(rawData, currentPtr + 0);
			int16_t y = BitConverter::ToInt16BE(rawData, currentPtr + 2);
			int16_t z = BitConverter::ToInt16BE(rawData, currentPtr + 4);
			points.push_back(Vec3s(x, y, z));
			currentPtr += 6;
		}
	}

	if (segmentOffset != 0)
		parent->AddDeclarationPlaceholder(segmentOffset);
}

void SetCsCamera::DeclareReferences()
{
	if (points.size() > 0)
	{
		string declaration = "";
		size_t index = 0;
		for (Vec3s point : points)
		{
			declaration += StringHelper::Sprintf("	{ %i, %i, %i }, //0x%06X", point.x, point.y,
			                                     point.z, cameras[0].segmentOffset + (index * 6));

			if (index < points.size() - 1)
				declaration += "\n";

			index++;
		}

		parent->AddDeclarationArray(cameras[0].GetSegmentOffset(), DeclarationAlignment::None,
		                                   DeclarationPadding::None, points.size() * 6, "Vec3s",
		                                   StringHelper::Sprintf("%sCsCameraPoints0x%06X",
		                                                         zRoom->GetName().c_str(),
		                                                         cameras[0].GetSegmentOffset()),
		                                   points.size(), declaration);
	}

	string declaration = "";

	size_t index = 0;
	size_t pointsIndex = 0;
	for (const auto& entry : cameras)
	{
		declaration += StringHelper::Sprintf("    %i, %i, (u32)&%sCsCameraPoints0x%06X[%i],",
												entry.type, entry.numPoints, zRoom->GetName().c_str(),
												cameras[0].GetSegmentOffset(), pointsIndex);

		if (index < cameras.size() - 1)
			declaration += "\n";

		index++;
		pointsIndex += entry.GetNumPoints();
	}

	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::Align4, DeclarationPadding::Pad16,
		cameras.size() * 8, "CsCameraEntry",
		StringHelper::Sprintf("%sCsCameraList0x%06X", zRoom->GetName().c_str(), segmentOffset),
		cameras.size(), declaration);
}

std::string SetCsCamera::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("%s, %i, (u32)%s", GetCommandHex().c_str(), cameras.size(), listName.c_str());
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

CsCameraEntry::CsCameraEntry(std::vector<uint8_t> rawData, int rawDataIndex)
	: baseOffset(rawDataIndex), type(BitConverter::ToInt16BE(rawData, rawDataIndex + 0)),
	  numPoints(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  segmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4)))
{
}

int16_t CsCameraEntry::GetNumPoints() const
{
	return numPoints;
}

int CsCameraEntry::GetSegmentOffset() const
{
	return segmentOffset;
}
