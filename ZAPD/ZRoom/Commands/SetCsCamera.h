#pragma once

#include "../../Vec3s.h"
#include "../ZRoomCommand.h"

class CsCameraEntry
{
public:
	CsCameraEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

	int16_t GetNumPoints() const;
	int GetSegmentOffset() const;

//protected:
	int baseOffset;
	int16_t type;
	int16_t numPoints;
	uint32_t segmentOffset;
};

class SetCsCamera : public ZRoomCommand
{
public:
	SetCsCamera(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;

private:
	std::vector<CsCameraEntry> cameras;
	std::vector<Vec3s> points;
};
