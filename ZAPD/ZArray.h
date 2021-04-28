#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "ZResource.h"
#include "tinyxml2.h"

class ZArray : public ZResource
{
public:
	ZArray(ZFile* nParent);
	~ZArray();

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;

	std::string GetSourceOutputCode(const std::string& prefix) override;
	size_t GetRawDataSize() override;

	ZResourceType GetResourceType() const override;

protected:
	size_t arrayCnt;
	std::string childName;
	std::vector<ZResource*> resList;
};