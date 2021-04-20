#pragma once
#include "../ZRoomCommand.h"

class ZRoomCommandUnk : public ZRoomCommand
{
public:
	ZRoomCommandUnk(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;

protected:
	uint8_t data1;
	uint32_t data2;
};
