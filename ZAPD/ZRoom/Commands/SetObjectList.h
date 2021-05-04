#pragma once

#include "../ZRoomCommand.h"

class SetObjectList : public ZRoomCommand
{
public:
	SetObjectList(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress) override;
	virtual std::string GetCommandCName() const override;
	virtual RoomCommand GetRoomCommand() const override;
	virtual size_t GetRawDataSize() const override;
	virtual std::string GenerateExterns() const override;

private:
	std::vector<uint16_t> objects;
	uint32_t segmentOffset;
};