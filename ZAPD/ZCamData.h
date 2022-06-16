#pragma once

#include "Vec3s.h"
#include "ZFile.h"
#include "ZResource.h"

class ZCollisionHeader;

struct CamDataList
{
	segptr_t camDataListPtr;
	uint16_t cameraSType;
	uint16_t numCameras;
};

class ZCamData : public ZResource
{
public:
	ZCollisionHeader* collisionParent;
	std::vector<CamDataList> dataList;
	std::vector<Vec3s> camPosData;
	size_t count;
	size_t totalCamPoints = 0;

	ZCamData(ZFile* nParent);
	ZCamData(ZFile* nParent, ZCollisionHeader* collisionParent, size_t numElements);
	~ZCamData();

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;
	std::string GetDefaultName(const std::string& prefix) const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	bool DoesSupportArray() const override;

	size_t GetRawDataSize() const override;
};