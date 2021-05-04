#include "SetCsCamera.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

SetCsCamera::SetCsCamera(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	_rawData = rawData;
	_rawDataIndex = rawDataIndex;

	segmentOffset = 0;

	int32_t numCameras = rawData[rawDataIndex + 1];
	segmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4));

	uint32_t currentPtr = segmentOffset;
	int32_t numPoints = 0;

	for (int32_t i = 0; i < numCameras; i++)
	{
		CsCameraEntry* entry = new CsCameraEntry(_rawData, currentPtr);

		cameras.push_back(entry);
		numPoints += entry->numPoints;

		currentPtr += 8;
	}

	if (numPoints > 0)
	{
		uint32_t currentPtr = cameras[0]->segmentOffset;

		for (int32_t i = 0; i < numPoints; i++)
		{
			int16_t x = BitConverter::ToInt16BE(rawData, currentPtr + 0);
			int16_t y = BitConverter::ToInt16BE(rawData, currentPtr + 2);
			int16_t z = BitConverter::ToInt16BE(rawData, currentPtr + 4);
			Vec3s p = {x, y, z};
			points.push_back(p);
			currentPtr += 6;
		}
	}

	if (segmentOffset != 0)
		zRoom->parent->AddDeclarationPlaceholder(segmentOffset);
}

SetCsCamera::~SetCsCamera()
{
	for (CsCameraEntry* entry : cameras)
		delete entry;
}

std::string SetCsCamera::GetSourceOutputCode(std::string prefix)
{
	return "";
}

std::string SetCsCamera::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	return "";
}

std::string SetCsCamera::GenerateSourceCodePass2(std::string roomName, uint32_t baseAddress)
{
	std::string sourceOutput = "";

	sourceOutput +=
		StringHelper::Sprintf("%s %i, (u32)&%sCsCameraList0x%06X };",
	                          ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(),
	                          cameras.size(), roomName.c_str(), segmentOffset);

	if (points.size() > 0)
	{
		std::string declaration = "";
		size_t index = 0;
		for (Vec3s point : points)
		{
			declaration += StringHelper::Sprintf("	{ %i, %i, %i }, //0x%06X", point.x, point.y,
			                                     point.z, cameras[0]->segmentOffset + (index * 6));

			if (index < points.size() - 1)
				declaration += "\n";

			index++;
		}

		zRoom->parent->AddDeclarationArray(cameras[0]->segmentOffset, DeclarationAlignment::None,
		                                   DeclarationPadding::None, points.size() * 6, "Vec3s",
		                                   StringHelper::Sprintf("%sCsCameraPoints0x%06X",
		                                                         roomName.c_str(),
		                                                         cameras[0]->segmentOffset),
		                                   points.size(), declaration);
	}

	{
		std::string declaration = "";

		size_t index = 0;
		size_t pointsIndex = 0;
		for (CsCameraEntry* entry : cameras)
		{
			declaration += StringHelper::Sprintf("    %i, %i, (u32)&%sCsCameraPoints0x%06X[%i],",
			                                     entry->type, entry->numPoints, roomName.c_str(),
			                                     cameras[0]->segmentOffset, pointsIndex);

			if (index < cameras.size() - 1)
				declaration += "\n";

			index++;
			pointsIndex += entry->numPoints;
		}

		zRoom->parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::Align4, DeclarationPadding::Pad16,
			cameras.size() * 8, "CsCameraEntry",
			StringHelper::Sprintf("%sCsCameraList0x%06X", roomName.c_str(), segmentOffset),
			cameras.size(), declaration);
	}

	return sourceOutput;
}

size_t SetCsCamera::GetRawDataSize() const
{
	return ZRoomCommand::GetRawDataSize() + (cameras.size() * 8) + (points.size() * 6);
}

std::string SetCsCamera::GenerateExterns() const
{
	return "";
}

std::string SetCsCamera::GetCommandCName() const
{
	return "SCmdCsCameraList";
}

RoomCommand SetCsCamera::GetRoomCommand() const
{
	return RoomCommand::SetCsCamera;
}

CsCameraEntry::CsCameraEntry(std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: baseOffset(rawDataIndex), type(BitConverter::ToInt16BE(rawData, rawDataIndex + 0)),
	  numPoints(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
	  segmentOffset(GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4)))
{
}
