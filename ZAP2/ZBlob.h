#pragma once

#include "ZResource.h"
#include "tinyxml2.h"

class ZBlob : public ZResource
{
public:
	ZBlob(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex, std::string nRelPath);
	ZBlob(tinyxml2::XMLElement* reader, std::string inFolder);
	std::string GetSourceOutputCode();
	void Save(std::string outFolder);
};