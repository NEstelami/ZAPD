#pragma once

#include "../../Vec3s.h"
#include "../ZRoomCommand.h"
#include "ZResource.h"

class PathwayEntry
{
public:
	int16_t x, y, z;

	PathwayEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

	int numPoints;
	int8_t unk1;   // (MM Only)
	int16_t unk2;  // (MM Only)
	uint32_t listSegmentOffset;
	std::vector<Vec3s> points;
};

struct PathwayList
{
public:
	PathwayList() = default;
	PathwayList(ZFile* nParent, const std::vector<uint8_t>& rawData, int rawDataIndex, int length);

	void GetSourceOutputCode(const std::string& prefix);
	int32_t GetRawDataSize();

private:
	ZFile* parent;
	std::vector<PathwayEntry> pathways;
	int _rawDataIndex;
};

class ZSetPathways : public ZRoomCommand
{
public:
	ZSetPathways(ZFile* nParent);
	ZSetPathways(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData, int nRawDataIndex,
	             bool nIsFromHeader=true);

	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	void DeclareVar(const std::string& prefix, const std::string& bodyStr);
	std::string GetSourceOutputCode(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() const override;

private:
	PathwayList pathwayList;
	bool isFromHeader = false;
};