#pragma once

#include "../ZRoomCommand.h"

class SetObjectList : public ZRoomCommand
{
public:
	SetObjectList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);

	void ParseRawData() override;
	void DeclareReferences(const std::string& prefix);

	std::string GetBodySourceCode() const override;

	std::string GetCommandCName() const override;
	RoomCommand GetRoomCommand() const override;
	size_t GetRawDataSize() override;

private:
	std::vector<uint16_t> objects;
};
