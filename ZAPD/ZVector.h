#pragma once

#include <vector>
#include <string>
#include <stdint.h>
#include "ZResource.h"
#include "ZScalar.h"
#include "tinyxml2.h"

class ZVector : public ZResource
{
public:
	std::vector<ZScalar*> scalars;
	ZScalarType scalarType;
	uint32_t dimensions;

	ZVector(ZFile* nParent);

	void ParseXML(tinyxml2::XMLElement* reader);
	std::string GetSourceTypeName() override;
	std::string GetSourceValue();
	std::string GetSourceOutputCode(const std::string& prefix);
	void ParseRawData() override;
	int GetRawDataSize();
	bool DoesSupportArray() override;
	ZResourceType GetResourceType();

	void ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, const int nRawDataIndex, const std::string& nRelPath) override;

protected:
};