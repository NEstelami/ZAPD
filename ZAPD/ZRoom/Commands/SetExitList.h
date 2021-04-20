#pragma once

#include "../ZRoomCommand.h"

class SetExitList : public ZRoomCommand
{
public:
	SetExitList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GenerateSourceCodePass1(std::string roomName, int baseAddress) override;
	std::string GenerateExterns() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	std::vector<uint16_t> exits;
};
