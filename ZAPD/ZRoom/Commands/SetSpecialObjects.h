#pragma once

#include "../ZRoomCommand.h"

class SetSpecialObjects : public ZRoomCommand
{
public:
	uint8_t elfMessage;
	uint16_t globalObject;

	SetSpecialObjects(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);

	virtual std::string GenerateSourceCodePass1(std::string roomName,
	                                            uint32_t baseAddress) override;
	virtual std::string GetCommandCName() override;
	virtual RoomCommand GetRoomCommand() override;

private:
};