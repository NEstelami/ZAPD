#pragma once

#include "../ZRoomCommand.h"

class MinimapChest
{
public:
	uint16_t unk0;
	uint16_t unk2;
	uint16_t unk4;
	uint16_t unk6;
	uint16_t unk8;

	MinimapChest(std::vector<uint8_t> rawData, uint32_t rawDataIndex);
};

class SetMinimapChests : public ZRoomCommand
{
public:
	SetMinimapChests(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex);
	~SetMinimapChests();

	std::string GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress) override;
	std::string GenerateSourceCodePass2(std::string roomName, uint32_t baseAddress) override;
	std::string GetCommandCName() const override;
	std::string GenerateExterns() const override;
	RoomCommand GetRoomCommand() const override;
	size_t GetRawDataSize() const override;

private:
	std::vector<MinimapChest*> chests;
	uint32_t segmentOffset;
};