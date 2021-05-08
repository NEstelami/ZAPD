#pragma once

#include "Vec3s.h"
#include "ZResource.h"
#include "ZRoom/ZRoomCommand.h"

class PathwayEntry
{
public:
	int16_t x, y, z;

	PathwayEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	int32_t numPoints;
	int8_t unk1;   // (MM Only)
	int16_t unk2;  // (MM Only)
	uint32_t listSegmentOffset;
	std::vector<Vec3s> points;
};

class PathwayList
{
public:
	PathwayList() = default;
	PathwayList(ZFile* nParent, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex,
	            size_t length);

	void GetSourceOutputCode(const std::string& prefix);
	size_t GetRawDataSize() const;

private:
	ZFile* parent;
	std::vector<PathwayEntry> pathways;
	int rawDataIndex;
};

class ZSetPathways : public ZRoomCommand
{
public:
	ZSetPathways(ZFile* nParent);
	ZSetPathways(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData, uint32_t nRawDataIndex,
	             bool nIsFromHeader = true);

	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	void DeclareVar(const std::string& prefix, const std::string& bodyStr);
	std::string GetSourceOutputCode(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	size_t GetRawDataSize() const override;
	std::string GetCommandCName() const override;

private:
	PathwayList pathwayList;
	bool isFromHeader = false;
};