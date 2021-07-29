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

	std::string GetSourceOutputCode(const std::string& prefix) override;
	size_t GetRawDataSize() const override;

	ZResourceType GetResourceType() const override;

protected:
	std::vector<ZResource*> resList;
	std::string childName;
	size_t arrayCnt;
};
