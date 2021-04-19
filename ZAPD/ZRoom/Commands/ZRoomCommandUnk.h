#pragma once
#include "../ZRoomCommand.h"

class ZRoomCommandUnk : public ZRoomCommand
{
public:
	RoomCommand cmdID;
	int32_t cmdAddress;
	int32_t cmdIndex;
	int32_t cmdSet;
	uint32_t commandSet;
	uint8_t data1;
	uint32_t data2;

	ZRoomCommandUnk(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName, int baseAddress);
	virtual RoomCommand GetRoomCommand();
	virtual std::string GetCommandCName();

protected:
	ZRoom* zRoom;
};