#pragma once

#include "../../Vec3s.h"
#include "../ZRoomCommand.h"
#include "ZResource.h"

class PathwayEntry
{
public:
	int16_t x, y, z;

	PathwayEntry(std::vector<uint8_t> rawData, int rawDataIndex);

	int numPoints;
	int8_t unk1;   // (MM Only)
	int16_t unk2;  // (MM Only)
	uint32_t listSegmentOffset;
	std::vector<Vec3s> points;
};

struct PathwayList
{
public:
	PathwayList(ZFile* nParent, std::vector<uint8_t> rawData, int rawDataIndex, int length);
	~PathwayList();

	void GetSourceOutputCode(const std::string& prefix);
	int32_t GetRawDataSize();
	std::string GenerateExterns(const std::string& prefix);

private:
	ZFile* parent;
	std::vector<PathwayEntry*> pathways;
	std::vector<uint8_t> _rawData;
	int _rawDataIndex;
};

class ZSetPathways : public ZResource, public ZRoomCommand
{
public:
	ZSetPathways(ZFile* nParent);
	ZSetPathways(ZRoom* nZRoom, const std::vector<uint8_t>& nRawData, int nRawDataIndex, bool nIsFromHeader);
	~ZSetPathways();

	void ParseRawData() override;

	void DeclareVar(const std::string& prefix, const std::string& bodyStr);
	std::string GetSourceOutputCode(const std::string& prefix) override;

	std::string GenerateSourceCodePass1(std::string roomName, int baseAddress) override;
	std::string GenerateSourceCodePass2(std::string roomName, int baseAddress) override;
	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;
	std::string GenerateExterns() override;

private:
	uint32_t segmentOffset;
	PathwayList* pathwayList;
	bool isFromHeader = false;
};