#pragma once

#include "../ZRoomCommand.h"

class SetObjectList : public ZRoomCommand
{
public:
	SetObjectList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, int32_t baseAddress);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();
	virtual size_t GetRawDataSize();
	virtual std::string GenerateExterns();

private:
	std::vector<uint16_t> objects;
	uint32_t segmentOffset;
};