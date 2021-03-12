#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include "ZFile.h"
#include "ZDisplayList.h"

enum class ZLimbType
{
	Standard,
	LOD
};

class ZSkeleton;

struct ZLimbStandard : public ZResource
{
	ZSkeleton* skeleton;
	std::string skelName;
	uint32_t address;
	std::string name;

	int16_t transX, transY, transZ;
	uint8_t childIndex, siblingIndex;
	uint32_t dListPtr;
	ZDisplayList* dList;

	std::vector<ZLimbStandard*> children;

	ZLimbStandard(ZFile* nParent);
	static ZLimbStandard* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* parent);
	static ZLimbStandard* FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex, ZFile* nParent);
	std::string GetSourceOutputCode(const std::string& prefix) override;
	int GetRawDataSize() override;
};

struct ZLimbLOD : ZLimbStandard
{
	uint32_t farDListPtr;

	ZLimbLOD(ZFile* nParent);
	//static ZLimbLOD* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* parent);
	static ZLimbLOD* FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex, ZFile* nParent);
	std::string GetSourceOutputCode(const std::string& prefix) override;
	int GetRawDataSize() override;
};

enum ZSkeletonType
{
	Normal,
	Flex,
	Skin
};

class ZSkeleton : public ZResource
{
public:
	ZSkeletonType type;
	std::vector<ZLimbStandard*> limbs;
	ZLimbStandard* rootLimb;
	uint8_t dListCount; // FLEX SKELETON ONLY

	ZSkeleton(ZFile* nParent);
	virtual void GenerateHLIntermediette(HLFileIntermediette& hlFile);
	static ZSkeleton* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* nParent);
	void Save(const std::string& outFolder) override;
	ZResourceType GetResourceType() override;

	std::string GetSourceOutputCode(const std::string& prefix) override;
};
