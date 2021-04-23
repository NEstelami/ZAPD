#pragma once

#include "../ZRoom.h"
#include "../ZRoomCommand.h"

class SetAlternateHeaders : public ZRoomCommand
{
public:
	SetAlternateHeaders(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress);
	virtual size_t GetRawDataSize();
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	int32_t segmentOffset;
	std::vector<uint32_t> headers;
	std::vector<uint8_t> _rawData;
	int32_t _rawDataIndex;
};