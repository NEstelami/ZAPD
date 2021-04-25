#pragma once
#include "../ZRoomCommand.h"

class ZRoomCommandUnk : public ZRoomCommand
{
public:
	ZRoomCommandUnk(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	std::string GetBodySourceCode() const override;

protected:
	uint8_t data1;
	uint32_t data2;
};
