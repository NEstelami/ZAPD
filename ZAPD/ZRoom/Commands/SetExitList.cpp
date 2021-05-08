#include "SetExitList.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

SetExitList::SetExitList(ZRoom* nZRoom, std::vector<uint8_t> rawData, uint32_t rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4));
	exits = std::vector<uint16_t>();

	if (segmentOffset != 0)
		zRoom->parent->AddDeclarationPlaceholder(segmentOffset);

	_rawData = rawData;
	_rawDataIndex = rawDataIndex;
}

std::string SetExitList::GenerateSourceCodePass1(std::string roomName, uint32_t baseAddress)
{
	std::string sourceOutput =
		StringHelper::Sprintf("%s 0x00, (u32)&%sExitList0x%06X",
	                          ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(),
	                          zRoom->GetName().c_str(), segmentOffset);

	// Parse Entrances and Generate Declaration
	zRoom->parent->AddDeclarationPlaceholder(segmentOffset);  // Make sure this segment is defined
	int32_t numEntrances = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 2;
	uint32_t currentPtr = segmentOffset;

	for (int32_t i = 0; i < numEntrances; i++)
	{
		uint16_t exit = BitConverter::ToInt16BE(_rawData, currentPtr);
		exits.push_back(exit);

		currentPtr += 2;
	}

	std::string declaration = "";

	for (uint16_t exit : exits)
		declaration += StringHelper::Sprintf("    0x%04X,\n", exit);
	;

	zRoom->parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, exits.size() * 2, "u16",
		StringHelper::Sprintf("%sExitList0x%06X", zRoom->GetName().c_str(), segmentOffset),
		exits.size(), declaration);

	return sourceOutput;
}

std::string SetExitList::GenerateExterns() const
{
	return StringHelper::Sprintf("extern u16 %sExitList0x%06X[];\n", zRoom->GetName().c_str(),
	                             segmentOffset);
	;
}

std::string SetExitList::GetCommandCName() const
{
	return "SCmdExitList";
}

RoomCommand SetExitList::GetRoomCommand() const
{
	return RoomCommand::SetExitList;
}