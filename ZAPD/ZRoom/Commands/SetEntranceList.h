#pragma once

#include "../ZRoomCommand.h"

class EntranceEntry
{
public:
	EntranceEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() const;

protected:
	uint8_t startPositionIndex;
	uint8_t roomToLoad;
};

class SetEntranceList : public ZRoomCommand
{
public:
	SetEntranceList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	std::vector<EntranceEntry> entrances;
};
