#pragma once

#include "../ZRoomCommand.h"

class MinimapEntry
{
public:
	MinimapEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() const;

protected:
	uint16_t unk0;
	uint16_t unk2;
	uint16_t unk4;
	uint16_t unk6;
	uint16_t unk8;
};

class SetMinimapList : public ZRoomCommand
{
public:
	SetMinimapList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;

private:
	std::vector<MinimapEntry> minimaps;

	uint32_t listSegmentOffset;
	uint32_t unk4;
};
