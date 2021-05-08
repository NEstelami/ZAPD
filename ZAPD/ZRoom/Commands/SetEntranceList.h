#pragma once

#include "../ZRoomCommand.h"

class EntranceEntry
{
public:
	uint8_t startPositionIndex;
	uint8_t roomToLoad;

	EntranceEntry(std::vector<uint8_t> rawData, uint32_t rawDataIndex);
};

class SetEntranceList : public ZRoomCommand
{
public:
	SetEntranceList(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);
	~SetEntranceList();

	virtual std::string GenerateSourceCodePass1(std::string roomName,
	                                            uint32_t baseAddress) override;
	virtual std::string GenerateExterns() const override;
	virtual std::string GetCommandCName() const override;
	virtual RoomCommand GetRoomCommand() const override;

private:
	std::vector<EntranceEntry*> entrances;
	uint32_t segmentOffset;
	std::vector<uint8_t> _rawData;
	int32_t _rawDataIndex;
};