#pragma once

#include "../ZRoomCommand.h"

class SetSoundSettings : public ZRoomCommand
{
public:
	SetSoundSettings(std::vector<uint8_t> rawData, int rawDataIndex);

	virtual std::string GenerateSourceCode();
	virtual RoomCommand GetRoomCommand();

private:
	uint8_t reverb;
	uint8_t nightTimeSFX;
	uint8_t musicSequence;
};