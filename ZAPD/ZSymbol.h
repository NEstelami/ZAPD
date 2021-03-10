#pragma once

#include "ZResource.h"
#include "tinyxml2.h"

class ZSymbol : public ZResource
{
protected:
    std::string type;
    uint32_t typeSize;

public:
	ZSymbol(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZFile* nParent);

	static ZSymbol* ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZFile* parent);

	void ParseXML(tinyxml2::XMLElement* reader) override;

	bool DoesSupportArray() override;
	int GetRawDataSize() override;

	std::string GetSourceOutputHeader(const std::string& prefix) override;

	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;
};
