#pragma once

#include "../ZRoomCommand.h"

class EndMarker : public ZRoomCommand
{
public:
	EndMarker(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName,
	                                            uint32_t baseAddress) override;
	virtual std::string GetCommandCName() const override;
	virtual RoomCommand GetRoomCommand() const override;

private:
};