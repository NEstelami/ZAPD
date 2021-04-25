#include "Unused09.h"
#include "StringHelper.h"

Unused09::Unused09(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

std::string Unused09::GetBodySourceCode() const
{
	return StringHelper::Sprintf("{ %s, 0x00, 0x00 }", GetCommandHex().c_str());
}

std::string Unused09::GetCommandCName() const
{
	return "SCmdBase";
}

RoomCommand Unused09::GetRoomCommand() const
{
	return RoomCommand::Unused09;
}
