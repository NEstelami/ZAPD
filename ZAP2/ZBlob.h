#pragma once

#include "ZResource.h"
#include "tinyxml2.h"

class ZBlob : public ZResource
{
public:
	ZBlob(tinyxml2::XMLElement* reader, std::vector<uint8_t> nRawData, int rawDataIndex);
	ZBlob(tinyxml2::XMLElement* reader, std::string inFolder);
	void Save(std::string outFolder);
};