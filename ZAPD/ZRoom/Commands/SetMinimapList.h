#pragma once

#include "../ZRoomCommand.h"

class MinimapEntry
{
public:
	int32_t unk0;
	int32_t unk4;
	int16_t unk8;

	MinimapEntry(std::vector<uint8_t> rawData, int rawDataIndex);
};

class SetMinimapList : public ZRoomCommand
{
public:
	SetMinimapList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
	~SetMinimapList();

	virtual std::string GenerateSourceCodePass1(std::string roomName, int baseAddress);
	virtual std::string GenerateSourceCodePass2(std::string roomName, int baseAddress);
	virtual std::string GetCommandCName();
	virtual std::string GenerateExterns();
	virtual RoomCommand GetRoomCommand();
	virtual std::string PreGenSourceFiles();
	virtual std::string Save();

private:
	std::vector<MinimapEntry*> minimaps;
	uint32_t segmentOffset;

	uint32_t listSegmentOffset;
	uint32_t unk4;
};