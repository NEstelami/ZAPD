#include "Unused09.h"
#include "../../StringHelper.h"

using namespace std;

Unused09::Unused09(ZRoom* nZRoom, const std::vector<uint8_t>& rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
}

std::string Unused09::GetBodySourceCode() const
{
	return "SCENE_CMD_UNUSED_9()";
}

string Unused09::GetCommandCName() const
{
	return "SCmdBase";
}

RoomCommand Unused09::GetRoomCommand() const
{
	return RoomCommand::Unused09;
}
