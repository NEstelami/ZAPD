#include "ZCamData.h"
#include "ZCollision.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

REGISTER_ZFILENODE(CamData, ZCamData);

ZCamData::ZCamData(ZFile* nParent) : ZResource(nParent)
{
	collisionParent = nullptr;
	RegisterRequiredAttribute("Count");
}

ZCamData::ZCamData(ZFile* nParent, ZCollisionHeader* collisionParent, size_t numElements)
	: ZResource(nParent), collisionParent(collisionParent)
{
	count = numElements;
}

ZCamData::~ZCamData()
{
}

void ZCamData::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);
	count = reader->IntAttribute("Count", 0);
}

void ZCamData::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	dataList.reserve(count);

	for (size_t i = 0; i < count; i++)
	{
		CamDataList element;
		element.cameraSType = BitConverter::ToUInt16BE(rawData, (rawDataIndex + 0) + (i * 8));
		element.numCameras = BitConverter::ToUInt16BE(rawData, (rawDataIndex + 2) + (i * 8));
		element.camDataListPtr = BitConverter::ToUInt32BE(rawData, (rawDataIndex + 4) + (i * 8));
		dataList.push_back(element);

		//totalCamPoints += element.numCameras;

		segptr_t camDataListPtr = GETSEGOFFSET(element.camDataListPtr);

		if (camDataListPtr == SEGMENTED_NULL)
		{
			continue;
		}
		for (size_t i = 0; i < element.numCameras; i++)
		{
			Vec3s entry;
			entry.x = BitConverter::ToUInt16BE(rawData, camDataListPtr + ((i * 6) + 0));
			entry.y = BitConverter::ToUInt16BE(rawData, camDataListPtr + ((i * 6) + 2));
			entry.z = BitConverter::ToUInt16BE(rawData, camDataListPtr + ((i * 6) + 4));
			camPosData.push_back(entry);
		}
	}
	if (count != 0)
	{	//Get the total number of cam points not including the last cam and then add the final number of points

		totalCamPoints =
			((dataList.back().camDataListPtr - dataList.front().camDataListPtr) / 0x06) +
			dataList.back().numCameras;
	}
}

void ZCamData::DeclareReferences(const std::string& prefix)
{
	std::string declaration;
	std::string auxName = name;
	std::string name;
	std::string arrayName;

	if (name == "")
		name = GetDefaultName(prefix);

	if (collisionParent != nullptr)
	{
		name = StringHelper::Sprintf("%s%s", collisionParent->GetName().c_str(), "CamData");
		arrayName = StringHelper::Sprintf("%sCamDataPos_0x%06X", collisionParent->GetName().c_str(),
		                                  GETSEGOFFSET(dataList[0].camDataListPtr));
	}
	else
	{
		arrayName = name;  // GetDefaultName(prefix);
	}
	if (totalCamPoints != 0)
	{
		for (const auto& data : camPosData)
		{
			declaration += StringHelper::Sprintf("\t{%i, %i, %i},\n", data.x, data.y, data.z);
		}

		// enrui_sceneCollisionHeader_0003B4CamPosData
		parent->AddDeclarationArray(GETSEGOFFSET(dataList[0].camDataListPtr),
		                            DeclarationAlignment::Align4, totalCamPoints * 6, "Vec3s",
		                            arrayName.c_str(), totalCamPoints, declaration);
	}
}

std::string ZCamData::GetBodySourceCode() const
{
	std::string camPosDataString;
	std::string body;

	for (const auto& element : dataList)
	{
		if (element.camDataListPtr == SEGMENTED_NULL)
		{
			camPosDataString = "NULL";
		}
		else
		{
			if (collisionParent != nullptr)
			{
				camPosDataString = StringHelper::Sprintf(
					"&%sCamDataPos_0x%06X[%i]", collisionParent->GetName().c_str(),
					GETSEGOFFSET(dataList[0].camDataListPtr),
					((element.camDataListPtr) - (dataList[0].camDataListPtr)) / 0x6);
			}
			else
			{
				std::string temp;

				Globals::Instance->GetSegmentedPtrName(dataList[0].camDataListPtr, parent, "Vec3s",
				                                       temp);

				camPosDataString = StringHelper::Sprintf(
					"&%s[%i]", temp.c_str(),
					((element.camDataListPtr) - (dataList[0].camDataListPtr)) / 0x06);
			}
		}

		body += StringHelper::Sprintf("{0x%04X, %i, %s},", element.cameraSType, element.numCameras,
		                              camPosDataString.c_str());
	}
	return body;
}

std::string ZCamData::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sCamData_%06X", prefix.c_str(), rawDataIndex);
}

ZResourceType ZCamData::GetResourceType() const
{
	return ZResourceType::CamData;
}

size_t ZCamData::GetRawDataSize() const
{
	return 8 * count;
}

std::string ZCamData::GetSourceTypeName() const
{
	return "CamData";
}

bool ZCamData::DoesSupportArray() const
{
	return false;
}
