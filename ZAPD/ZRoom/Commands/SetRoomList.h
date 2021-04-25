#pragma once

#include "../ZRoomCommand.h"

class RoomEntry
{
public:
	RoomEntry(int32_t nVAS, int32_t nVAE);
	RoomEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

protected:
	int32_t virtualAddressStart;
	int32_t virtualAddressEnd;
};

class SetRoomList : public ZRoomCommand
{
public:
	SetRoomList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	void ParseRawData() override;
	virtual void DeclareReferences(const std::string& prefix);

	std::string GetBodySourceCode() const override;
	void PreGenSourceFiles() override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	std::vector<RoomEntry> rooms;
};
