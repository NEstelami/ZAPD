#pragma once

#include "../ZRoomCommand.h"

class MinimapChest
{
public:
	MinimapChest(std::vector<uint8_t> rawData, int rawDataIndex);

	std::string GetBodySourceCode() const;

protected:
	uint16_t unk0;
	uint16_t unk2;
	uint16_t unk4;
	uint16_t unk6;
	uint16_t unk8;
};

class SetMinimapChests : public ZRoomCommand
{
public:
	SetMinimapChests(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences() override;

	std::string GetBodySourceCode() override;
	std::string GenerateExterns() override;

	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;

private:
	std::vector<MinimapChest> chests;
};
