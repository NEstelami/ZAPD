#pragma once

#include "../ZRoom.h"
#include "../ZRoomCommand.h"

class SetAlternateHeaders : public ZRoomCommand
{
public:
	SetAlternateHeaders(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GenerateSourceCodePass1(std::string roomName, int baseAddress) override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	std::vector<uint32_t> headers;
};
