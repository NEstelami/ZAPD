#pragma once

#include "../ZRoomCommand.h"

class SetObjectList : public ZRoomCommand
{
public:
	SetObjectList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() override;
	std::string GenerateSourceCodePass1(std::string roomName, int baseAddress) override;
	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GenerateExterns() override;

private:
	std::vector<uint16_t> objects;
};