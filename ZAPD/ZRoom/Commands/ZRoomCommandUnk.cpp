#include "ZRoomCommandUnk.h"
#include "../../StringHelper.h"
#include "BitConverter.h"
#include "ZRoom/ZRoom.h"

using namespace std;

ZRoomCommandUnk::ZRoomCommandUnk(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                 int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	data1 = cmdArg1;
	data2 = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
}

string ZRoomCommandUnk::GetBodySourceCode() const
{
	return StringHelper::Sprintf("{ %s, 0x%02X, 0x%06X } /* WARNING: "
	                             "UNIMPLEMENTED ROOM COMMAND */",
	                             GetCommandHex().c_str(), data1, data2);
}
