#pragma once

#include "../ZRoomCommand.h"
#include "../ZRoom.h"

class AlternateHeader
{
public:
	uint32_t unknown;

	AlternateHeader(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
};

class SetAlternateHeaders : public ZRoomCommand
{
public:
	SetAlternateHeaders(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName);
	virtual int32_t GetRawDataSize();
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	int32_t segmentOffset;
	std::vector<AlternateHeader*> headers;
};