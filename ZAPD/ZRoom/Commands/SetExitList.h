#pragma once

#include "../ZRoomCommand.h"

class SetExitList : public ZRoomCommand
{
public:
	SetExitList(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress);
	virtual std::string GenerateExterns();
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();

private:
	std::vector<uint16_t> exits;
	uint32_t segmentOffset;
	std::vector<uint8_t> _rawData;
	int32_t _rawDataIndex;
};