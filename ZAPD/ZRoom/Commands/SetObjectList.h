#pragma once

#include "../ZRoomCommand.h"

class SetObjectList : public ZRoomCommand
{
public:
	SetObjectList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix);

	std::string GetBodySourceCode() override;

	std::string GetCommandCName() override;
	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;

private:
	std::vector<uint16_t> objects;
};
