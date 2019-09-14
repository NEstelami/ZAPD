//#pragma once
//
//#include "../ZRoomCommand.h"
//
//class EntranceEntry
//{
//public:
//	uint8_t startPositionIndex;
//	uint8_t roomToLoad;
//
//	EntranceEntry(std::vector<uint8_t> rawData, int rawDataIndex);
//};
//
//class SetEntranceList : public ZRoomCommand
//{
//public:
//	SetEntranceList(std::vector<uint8_t> rawData, int rawDataIndex);
//
//	virtual std::string GenerateSourceCode();
//	virtual RoomCommand GetRoomCommand();
//
//private:
//	std::vector<EntranceEntry*> entrances;
//};