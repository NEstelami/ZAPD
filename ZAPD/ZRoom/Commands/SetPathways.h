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

class ZSetPathways : public ZResource, public ZRoomCommand
{
public:
	ZSetPathways(ZFile* nParent);
	ZSetPathways(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData, int nRawDataIndex,
	             bool nIsFromHeader);

	void DeclareVar(const std::string& prefix, const std::string& bodyStr);
	std::string GetSourceOutputCode(const std::string& prefix) override;

	std::string GetBodySourceCode() override;

	void ParseRawDataLate();
	std::string GenerateSourceCodePass1(std::string roomName) override;

	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;

private:
	PathwayList pathwayList;
	bool isFromHeader = false;
};