#include "ZCamData.h"
#include "ZCollision.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

REGISTER_ZFILENODE(CamData, ZCamData);

ZCamData::ZCamData(ZFile* nParent) : ZResource(nParent)
{
	collisionParent = nullptr;
}

ZCamData::ZCamData(ZFile* nParent, ZCollisionHeader* collisionParent)
	: ZResource(nParent), collisionParent(collisionParent)
{
}

ZCamData::~ZCamData()
{
}

void ZCamData::ParseRawData()
{
	const auto& rawData = parent->GetRawData();

	cameraSType = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	numCameras = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	camDataList = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);

	segptr_t camDataListPtr = GETSEGOFFSET(camDataList);
	if (camDataListPtr == SEGMENTED_NULL)
	{
		return;
	}
	for (size_t i = 0; i < numCameras; i++)
	{
		Vec3s entry;
		entry.x = BitConverter::ToUInt16BE(rawData, camDataListPtr + ((i* 6) + 0));
		entry.y = BitConverter::ToUInt16BE(rawData, camDataListPtr + ((i* 6) + 2));
		entry.z = BitConverter::ToUInt16BE(rawData, camDataListPtr + ((i* 6) + 4));
		camPosData.push_back(entry);
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

	//parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	//                       GetSourceTypeName(), name.c_str(), GetBodySourceCode());
	if (collisionParent != nullptr)
	{
		name = StringHelper::Sprintf("%s%s", collisionParent->GetName().c_str(),
		                          "CamData");
		arrayName =
			StringHelper::Sprintf("%sCamDataPos_0x%06X", collisionParent->GetName().c_str(), GETSEGOFFSET(camDataList));

	} 

	if (camDataList == SEGMENTED_NULL)
	{
		return;
	}

	for (const auto& data : camPosData)
	{
		declaration += StringHelper::Sprintf("\t{%i, %i, %i},\n", data.x, data.y, data.z);
	}

	

	//enrui_sceneCollisionHeader_0003B4CamPosData
	parent->AddDeclarationArray(GETSEGOFFSET(camDataList), DeclarationAlignment::Align4, numCameras * 6, "Vec3s", arrayName.c_str(), numCameras,
	                            declaration);
}

std::string ZCamData::GetBodySourceCode() const
{
	std::string camPosDataString;

	if (camDataList == SEGMENTED_NULL)
	{
		camPosDataString = "NULL";
	}
	else
	{
		Globals::Instance->GetSegmentedPtrName(camDataList, parent, "Vec3s", camPosDataString);
	}

	return StringHelper::Sprintf("{0x%04X, %i, %s}", cameraSType, numCameras, camPosDataString.c_str());
}

std::string ZCamData::GetDefaultName(const std::string& prefix) const
{
	return StringHelper::Sprintf("%sCamData_%_06X", prefix.c_str(), rawDataIndex);
}

ZResourceType ZCamData::GetResourceType() const
{
	return ZResourceType::CamData;
}

size_t ZCamData::GetRawDataSize() const
{
	return 8;
}

std::string ZCamData::GetSourceTypeName() const
{
	return "CamData";
}

bool ZCamData::DoesSupportArray() const
{
	return true;
}
