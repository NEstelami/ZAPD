#pragma once

#include "../ZRoomCommand.h"

class RoomEntry
{
public:
	RoomEntry(int32_t nVAS, int32_t nVAE);
	RoomEntry(std::vector<uint8_t> rawData, int rawDataIndex);

protected:
	int32_t virtualAddressStart;
	int32_t virtualAddressEnd;
};

class SetRoomList : public ZRoomCommand
{
public:
	SetRoomList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	void ParseRawData() override;

	std::string GetBodySourceCode() override;
	std::string GenerateExterns() override;
	std::string PreGenSourceFiles() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	std::vector<RoomEntry> rooms;
};
