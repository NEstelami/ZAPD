#pragma once

#include "ZCamData.h"
#include "ZCollisionPoly.h"
#include "ZFile.h"
#include "ZResource.h"
#include "ZRoom/ZRoom.h"
#include "ZSurfaceType.h"
#include "ZVector.h"
#include "ZWaterbox.h"

class ZCollisionHeader : public ZResource
{
public:
	int16_t absMinX, absMinY, absMinZ;
	int16_t absMaxX, absMaxY, absMaxZ;
	uint16_t numVerts;
	segptr_t vtxAddress;
	uint16_t numPolygons;
	segptr_t polyAddress;
	segptr_t polyTypeDefAddress;
	segptr_t camDataAddress;
	offset_t upperCameraBoundary;

	int32_t numWaterBoxes;
	segptr_t waterBoxAddress;

	offset_t vtxSegmentOffset, polySegmentOffset, polyTypeDefSegmentOffset, camDataSegmentOffset,
		waterBoxSegmentOffset;

	std::vector<ZVector> vertices;
	std::vector<ZCollisionPoly> polygons;
	std::vector<ZSurfaceType> polygonTypes;
	std::vector<ZWaterbox> waterBoxes;
	ZCamData* camData = nullptr;

	ZCollisionHeader(ZFile* nParent);
	~ZCollisionHeader();

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;
	std::string GetDefaultName(const std::string& prefix) const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
};
