#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "OtherStructs/SkinLimbStructs.h"
#include "ZDisplayList.h"
#include "ZFile.h"

enum class ZLimbType
{
	Standard,
	LOD,
	Skin,
	Curve,
};

class ZLimb : public ZResource
{
public:
	ZLimb(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	size_t GetRawDataSize() const override;
	std::string GetSourceOutputCode(const std::string& prefix) override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	ZLimbType GetLimbType();
	void SetLimbType(ZLimbType value);
	static const char* GetSourceTypeName(ZLimbType limbType);

	uint32_t GetFileAddress();
	void SetFileAddress(uint32_t nAddress);

protected:
	ZLimbType type = ZLimbType::Standard;

	int16_t transX, transY, transZ;
	uint8_t childIndex, siblingIndex;
	segptr_t dListPtr = 0;
	segptr_t dList2Ptr = 0;                                     // LOD and Curve Only
	ZLimbSkinType skinSegmentType = ZLimbSkinType::SkinType_0;  // Skin only
	segptr_t skinSegment = 0;                                   // Skin only
	Struct_800A5E28 segmentStruct;                              // Skin only

	void DeclareDList(segptr_t dListSegmentedPtr, const std::string& prefix,
	                  const std::string& limbSuffix);
};
