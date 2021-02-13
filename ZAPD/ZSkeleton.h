#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include "ZFile.h"

enum class ZLimbType
{
	Standard,
	LOD
};

struct ZLimbStandard : public ZResource
{
	uint32_t address;
	std::string name;

	int16_t transX, transY, transZ;
	uint8_t childIndex, siblingIndex;
	uint32_t dListPtr;

	std::vector<ZLimbStandard*> children;

	ZLimbStandard();
	~ZLimbStandard();
	static std::shared_ptr<ZLimbStandard> FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* parent);
	static std::shared_ptr<ZLimbStandard> FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex);
	std::string GetSourceOutputCode(const std::string& prefix) override;
	int GetRawDataSize() override;
};

struct ZLimbLOD : ZLimbStandard
{
	uint32_t farDListPtr;

	ZLimbLOD();
	//static std::shared_ptr<ZLimbLOD> FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* parent);
	static std::shared_ptr<ZLimbLOD> FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex);
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
	std::vector<std::shared_ptr<ZLimbStandard>> limbs;
	ZLimbStandard* rootLimb;
	uint8_t dListCount; // FLEX SKELETON ONLY

	ZSkeleton();
	~ZSkeleton();
	virtual void GenerateHLIntermediette(HLFileIntermediette& hlFile);
	static std::shared_ptr<ZSkeleton> FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* nParent);
	void Save(const std::string& outFolder) override;
	ZResourceType GetResourceType() override;

	std::string GetSourceOutputCode(const std::string& prefix) override;
};
