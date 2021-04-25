#pragma once

#include "ZRoom/ZRoomCommand.h"
#include "ZVector.h"

class CsCameraEntry
{
public:
	CsCameraEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetSourceTypeName() const;
	int32_t GetRawDataSize() const;

	int16_t GetNumPoints() const;
	int GetSegmentOffset() const;

	int baseOffset;
	int16_t type;
	int16_t numPoints;
	uint32_t segmentOffset;
};

class SetCsCamera : public ZRoomCommand
{
public:
	SetCsCamera(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	size_t GetRawDataSize() override;
	std::string GetCommandCName() const override;

private:
	std::vector<CsCameraEntry> cameras;
	std::vector<ZVector> points;
};
