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
	void ParseRawData() override;

	std::string GetBodySourceCode() const override;
	std::string GetSourceOutputCode(const std::string& prefix) override;

	bool DoesSupportArray() const override;
	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;

private:
	std::vector<ZScalar*> scalars;
	ZScalarType scalarType;
	uint32_t dimensions;

	void ClearScalars();
};