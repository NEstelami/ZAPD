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
	std::vector<ZScalar*> scalars;
	ZScalarType scalarType;
	uint32_t dimensions;

	ZVector(ZFile* nParent);
	~ZVector();

	void ParseXML(tinyxml2::XMLElement* reader) override;
	std::string GetSourceTypeName() const override;
	std::string GetSourceValue() const;
	std::string GetSourceOutputCode(const std::string& prefix) override;
	void ParseRawData() override;
	size_t GetRawDataSize() const override;
	bool DoesSupportArray() const override;
	ZResourceType GetResourceType() const override;

	void ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
	                    const uint32_t nRawDataIndex) override;

private:
	void ClearScalars();
};