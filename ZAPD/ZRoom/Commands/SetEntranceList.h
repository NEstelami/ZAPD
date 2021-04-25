#pragma once

#include "../ZRoomCommand.h"

class EntranceEntry
{
public:
	EntranceEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const;

protected:
	uint8_t startPositionIndex;
	uint8_t roomToLoad;
};

class SetEntranceList : public ZRoomCommand
{
public:
	SetEntranceList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	std::vector<EntranceEntry> entrances;
};
