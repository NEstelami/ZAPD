#include "ZRoomCommandUnk.h"
#include "../../StringHelper.h"
#include "BitConverter.h"
#include "ZRoom/ZRoom.h"

using namespace std;

ZRoomCommandUnk::ZRoomCommandUnk(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	data1 = rawData.at(rawDataIndex + 1);
	data2 = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
}

string ZRoomCommandUnk::GetBodySourceCode()
{
	return StringHelper::Sprintf("%s %sSet%04XCmd%02X = { %s, 0x%02X, 0x%06X }; /* WARNING: "
	                             "UNIMPLEMENTED ROOM COMMAND */",
	                             GetCommandCName().c_str(), zRoom->GetName().c_str(), rawDataIndex, data2,
	                             GetCommandHex().c_str(), data1, data2);
}
