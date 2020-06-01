#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include "ZResource.h"
#include "Vec3s.h"
#include "tinyxml2.h"

struct RotationIndex
{
	//uint16_t transX, transY, transZ;
	uint16_t x, y, z;

	RotationIndex(uint16_t nX, uint16_t nY, uint16_t nZ) { x = nX; y = nY; z = nZ; };
};

class ZAnimation : public ZResource
{
public:
	ZAnimation();

	static ZAnimation* ExtractFromXML(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath);

protected:
	int16_t frameCount;
	std::vector<uint16_t> rotationValues;
	std::vector<RotationIndex> rotationIndices;
	uint32_t rotationValuesSeg;
	uint32_t rotationIndicesSeg;
	int16_t limit;
	int16_t limbCount;

	void ParseRawData();
	void ParseXML(tinyxml2::XMLElement* reader);
	std::string GetSourceOutputCode(std::string prefix);
};