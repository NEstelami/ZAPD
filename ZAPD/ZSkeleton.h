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

class ZLimbStandard : public ZResource
{
// protected:
public:
	uint32_t address;
	std::string name;

	int16_t transX, transY, transZ;
	uint8_t childIndex, siblingIndex;
	uint32_t dListPtr;

	std::vector<ZLimbStandard*> children;

public:
	ZLimbStandard();
	static ZLimbStandard* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* parent);
	static ZLimbStandard* FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex);
	int GetRawDataSize() override;
	std::string GetSourceOutputCode(const std::string& prefix) override;
	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;

	// protected: // ?
	static std::string MakeLimbDListSourceOutputCode(const std::string& prefix, const std::string& limbPrefix, uint32_t dListPtr, const std::vector<uint8_t>& rawData, ZFile* parent);
};

class ZLimbLOD : public ZLimbStandard
{
// protected:
public:
	uint32_t farDListPtr;

public:
	ZLimbLOD();
	//static ZLimbLOD* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* parent);
	static ZLimbLOD* FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex);
	int GetRawDataSize() override;
	std::string GetSourceOutputCode(const std::string& prefix) override;
	std::string GetSourceTypeName() override;
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

	ZSkeleton();
	static ZSkeleton* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* nParent);
	void Save(const std::string& outFolder) override;
	virtual void GenerateHLIntermediette(HLFileIntermediette& hlFile);

	std::string GetSourceOutputCode(const std::string& prefix) override;

	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;
};
