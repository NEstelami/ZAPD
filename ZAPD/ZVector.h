#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "ZResource.h"
#include "ZScalar.h"
#include "tinyxml2.h"

class ZVector : public ZResource
{
public:
	ZVector(ZFile* nParent);
	~ZVector();

	void ParseXML(tinyxml2::XMLElement* reader) override;
	std::string GetSourceTypeName() override;
	std::string GetSourceValue();
	std::string GetSourceOutputCode(const std::string& prefix) override;
	void ParseRawData() override;
	size_t GetRawDataSize() override;
	bool DoesSupportArray() override;
	ZResourceType GetResourceType() override;

	void SetScalarType(ZScalarType type);
	void SetDimensions(uint32_t dim);

	void ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
	                    const uint32_t nRawDataIndex, const std::string& nRelPath) override;

protected:
	std::vector<ZScalar*> scalars;
	ZScalarType scalarType;
	uint32_t dimensions;

	void ClearScalars();
};
