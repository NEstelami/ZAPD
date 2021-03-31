#pragma once

#include <cstdint>
#include <vector>
#include "ZResource.h"


class ZBackground : public ZResource
{
protected:
	//std::vector<uint8_t> header;
	std::vector<uint8_t> data;
	//std::vector<uint8_t> endMarker;

public:
	ZBackground() = default;
	ZBackground(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, 
				const std::string& nRelPath,
	          ZFile* nParent);
	ZBackground(const std::string& prefix,
	          const std::vector<uint8_t>& nRawData, int nRawDataIndex, const std::string& nRelPath, ZFile* nParent);
	void ParseRawData() override;
	static ZBackground* ExtractFromXML(tinyxml2::XMLElement* reader,
				const std::vector<uint8_t>& nRawData, int nRawDataIndex, const std::string& nRelPath, ZFile* nParent);

	int GetRawDataSize() override;

	void DeclareVar(const std::string& prefix, const std::string& bodyStr);

	bool IsExternalResource() override;
	void Save(const std::string& outFolder) override;

	std::string GetBodySourceCode();
	std::string GetSourceOutputCode(const std::string& prefix) override;
	static std::string GetDefaultName(const std::string& prefix, uint32_t address);

	std::string GetSourceTypeName() override;
	ZResourceType GetResourceType() override;
};
