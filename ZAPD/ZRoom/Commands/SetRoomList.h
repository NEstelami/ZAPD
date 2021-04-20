#pragma once

#include "../ZRoomCommand.h"

class RoomEntry
{
public:
	int32_t virtualAddressStart;
	int32_t virtualAddressEnd;

	RoomEntry(int32_t nVAS, int32_t nVAE);
	RoomEntry(std::vector<uint8_t> rawData, int rawDataIndex);
};

class SetRoomList : public ZRoomCommand
{
public:
	SetRoomList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
	~SetRoomList();

	std::string GetBodySourceCode() override;
	std::string GetCommandCName() override;
	std::string GenerateExterns() override;
	RoomCommand GetRoomCommand() override;
	std::string PreGenSourceFiles() override;
	std::string Save() override;

private:
	std::vector<RoomEntry*> rooms;
};