#pragma once

#include "ZFile.h"
#include "ZResource.h"
#include "Vec3s.h"

class ZCollisionHeader;

class ZCamData : public ZResource
{
public:
	ZCollisionHeader* collisionParent;
	segptr_t camDataList;
	uint16_t cameraSType;
	int16_t numCameras;
	std::vector<Vec3s> camPosData;

	//Vec3s* camPosData;

	ZCamData(ZFile* nParent);
	ZCamData(ZFile* nParent, ZCollisionHeader* collisionParent);
	~ZCamData();

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;
	std::string GetDefaultName(const std::string& prefix) const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	bool DoesSupportArray() const override;

	size_t GetRawDataSize() const override;
};