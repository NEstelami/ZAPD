#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "ZFile.h"
#include "ZDisplayList.h"
#include "ZLimb.h"

enum class ZSkeletonType
{
	Normal,
	Flex
};

class ZSkeleton : public ZResource
{
public:
	ZSkeletonType type;
	ZLimbType limbType;
	std::vector<ZLimb*> limbs;
	ZLimb* rootLimb;
	segptr_t limbsArrayAddress;
	uint8_t limbCount;
	uint8_t dListCount; // FLEX SKELETON ONLY

	ZSkeleton(ZFile* nParent);
	//ZSkeleton(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZFile* nParent);
	~ZSkeleton();
	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void ExtractFromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int nRawDataIndex, std::string nRelPath) override;
	void Save(const std::string& outFolder) override;
	void GenerateHLIntermediette(HLFileIntermediette& hlFile) override;

	int GetRawDataSize() override;
	std::string GetSourceOutputCode(const std::string& prefix) override;

	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;

	segptr_t GetAddress();
};
