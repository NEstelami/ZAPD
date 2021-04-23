#include "SetAlternateHeaders.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"

using namespace std;

SetAlternateHeaders::SetAlternateHeaders(ZRoom* nZRoom, const std::vector<uint8_t>& rawData,
                                         int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	if (segmentOffset != 0)
		parent->AddDeclarationPlaceholder(segmentOffset);
}

std::string SetAlternateHeaders::GetBodySourceCode()
{
	std::string listName = parent->GetDeclarationPtrName(segmentOffset);
	return StringHelper::Sprintf("%s, 0, (u32)%s", GetCommandHex().c_str(), listName.c_str());
}

string SetAlternateHeaders::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	int numHeaders = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 4;

	for (int i = 0; i < numHeaders; i++)
	{
		int32_t address = BitConverter::ToInt32BE(rawData, segmentOffset + (i * 4));
		headers.push_back(address);

		if (address != 0)
			zRoom->commandSets.push_back(CommandSet(address));
	}

	string declaration = "";

	for (int i = 0; i < numHeaders; i++)
	{
		// sprintf(line, "    0x%06X,\n", headers[i]);

		if (headers[i] == 0)
			declaration += StringHelper::Sprintf("    0,\n");
		else
			declaration += StringHelper::Sprintf("    (u32)&%sSet%04XCmd00,\n", roomName.c_str(),
			                                     headers[i] & 0x00FFFFFF);
	}

	parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, headers.size() * 4, "u32",
		StringHelper::Sprintf("%sAlternateHeaders0x%06X", roomName.c_str(), segmentOffset), 0,
		declaration);

	return GetBodySourceCode();
}

string SetAlternateHeaders::GetCommandCName()
{
	return "SCmdAltHeaders";
}

RoomCommand SetAlternateHeaders::GetRoomCommand()
{
	return RoomCommand::SetAlternateHeaders;
}
