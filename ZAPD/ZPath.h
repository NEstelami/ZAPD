#pragma once

#include "ZResource.h"
#include "ZVector.h"

class PathwayEntry : public ZResource
{
public:
	PathwayEntry(ZFile* nParent);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
	segptr_t GetListAddress() const;

protected:
	std::vector<ZVector> points;
	int32_t numPoints;
	segptr_t listSegmentAddress;
	int16_t unk2;  // (MM Only)
	int8_t unk1;   // (MM Only)
};

class ZPath : public ZResource
{
public:
	ZPath(ZFile* nParent);

	void ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex) override;

	void ParseXML(tinyxml2::XMLElement* reader) override;
	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;
	std::string GetSourceOutputCode(const std::string& prefix) override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;

	size_t GetRawDataSize() const override;
	void SetNumPaths(uint32_t nNumPaths);

protected:
	std::vector<PathwayEntry> pathways;
	uint32_t numPaths;
};
