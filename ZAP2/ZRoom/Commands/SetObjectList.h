#pragma once

#include "../ZRoomCommand.h"

class SetObjectList : public ZRoomCommand
{
public:
	SetObjectList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName);
	virtual std::string GenerateSourceCodePass2(std::string roomName);
	virtual std::string GetCommandCName();
	virtual RoomCommand GetRoomCommand();
	virtual int32_t GetRawDataSize();

private:
	std::vector<uint16_t> objects;
	uint32_t segmentOffset;
};