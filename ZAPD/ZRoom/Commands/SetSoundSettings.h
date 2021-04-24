#pragma once

#include "../ZRoomCommand.h"

class SetSoundSettings : public ZRoomCommand
{
public:
	SetSoundSettings(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() const override;

	RoomCommand GetRoomCommand() const override;
	std::string GetCommandCName() const override;

private:
	uint8_t reverb;
	uint8_t nightTimeSFX;
	uint8_t musicSequence;
};
