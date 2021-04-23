#pragma once

#include "../../ZCutscene.h"
#include "../../ZCutsceneMM.h"
#include "../ZRoomCommand.h"

class CutsceneEntry
{
public:
	CutsceneEntry(const std::vector<uint8_t>& rawData, int rawDataIndex);

	uint32_t segmentOffset;
	uint16_t exit;
	uint8_t entrance;
	uint8_t flag;
};

class SetCutscenes : public ZRoomCommand
{
public:
	SetCutscenes(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex);
	~SetCutscenes();

	void ParseRawData() override;

	std::string GetBodySourceCode() override;

	RoomCommand GetRoomCommand() override;
	int32_t GetRawDataSize() override;
	std::string GetCommandCName() override;

private:
	std::vector<ZCutsceneBase*> cutscenes;
	std::vector<CutsceneEntry> cutsceneEntries;  // (MM Only)
	uint8_t numCutscenes;  // (MM Only)
};
