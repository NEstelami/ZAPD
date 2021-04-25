#pragma once

#include "../ZRoomCommand.h"

class MinimapEntry
{
public:
	MinimapEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

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
	SetMinimapList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	size_t GetRawDataSize() override;
	std::string GetCommandCName() const override;

private:
	std::vector<MinimapEntry> minimaps;

	uint32_t listSegmentOffset;
	uint32_t unk4;
};
