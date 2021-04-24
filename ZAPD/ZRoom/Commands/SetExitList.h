#pragma once

#include "../ZRoomCommand.h"

class SetExitList : public ZRoomCommand
{
public:
	SetExitList(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	void ParseRawDataLate() override;
	void DeclareReferencesLate(const std::string& prefix) override;

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	std::vector<uint16_t> exits;
};
