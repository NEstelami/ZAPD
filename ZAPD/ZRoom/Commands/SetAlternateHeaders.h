#pragma once

#include "../ZRoom.h"
#include "../ZRoomCommand.h"

class SetAlternateHeaders : public ZRoomCommand
{
public:
	SetAlternateHeaders(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GenerateSourceCodePass1(std::string roomName, int baseAddress) override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;

private:
	int32_t segmentOffset;
	std::vector<uint32_t> headers;
};
