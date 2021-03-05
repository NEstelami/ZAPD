#pragma once

#include "ZResource.h"
#include "tinyxml2.h"

class ZString : public ZResource
{
public:
	static ZString* ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int rawDataIndex, std::string nRelPath);

	//std::string GetSourceOutputCode(const std::string& prefix) override;
	std::string GetSourceOutputHeader(const std::string& prefix) override;
	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;

private:
	ZString();
};
