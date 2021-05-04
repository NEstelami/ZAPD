#pragma once

#include "../ZRoomCommand.h"

class RoomEntry
{
public:
	uint32_t virtualAddressStart;
	uint32_t virtualAddressEnd;

	RoomEntry(uint32_t nVAS, uint32_t nVAE);
	RoomEntry(std::vector<uint8_t> rawData, uint32_t rawDataIndex);
};

class SetRoomList : public ZRoomCommand
{
public:
	SetRoomList(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);
	~SetRoomList();

	virtual std::string GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress) override;
	virtual std::string GenerateSourceCodePass2(std::string roomName, uint32_t baseAddress) override;
	virtual std::string GetCommandCName() const override;
	virtual std::string GenerateExterns() const override;
	virtual RoomCommand GetRoomCommand() const override;
	virtual std::string PreGenSourceFiles() override;
	virtual std::string Save() override;

private:
	std::vector<RoomEntry*> rooms;
	uint32_t segmentOffset;
};