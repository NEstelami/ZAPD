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
	uint8_t childIndex, nextIndex;
	uint32_t dListPtr;

	std::vector<ZLimbStandard*> children;

	ZLimbStandard();
	static ZLimbStandard* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* parent);
	static ZLimbStandard* FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex);
	std::string GetSourceOutputCode(std::string prefix);
	virtual int GetRawDataSize();
};

struct ZLimbLOD : ZLimbStandard
{
	uint32_t farDListPtr;
};

class ZHierarchy : public ZResource
{
public:
	std::vector<ZLimbStandard*> limbs;
	ZLimbStandard* rootLimb;
	uint8_t dListCount;

	ZHierarchy();
	static ZHierarchy* FromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath, ZFile* nParent);
	void Save(std::string outFolder);

	std::string GetSourceOutputCode(std::string prefix);
};