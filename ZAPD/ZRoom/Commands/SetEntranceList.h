#pragma once

#include "../ZRoomCommand.h"

class EntranceEntry
{
public:
	uint8_t startPositionIndex;
	uint8_t roomToLoad;

	EntranceEntry(std::vector<uint8_t> rawData, int rawDataIndex);
};

class SetEntranceList : public ZRoomCommand
{
public:
	SetEntranceList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
	~SetEntranceList();

	std::string GetBodySourceCode() override;
	std::string GenerateSourceCodePass1(std::string roomName, int baseAddress) override;
	std::string GenerateExterns() override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	std::vector<EntranceEntry*> entrances;
};