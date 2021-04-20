#include "SetActorList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZNames.h"
#include "../ZRoom.h"

using namespace std;

SetActorList::SetActorList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	numActors = rawData.at(rawDataIndex + 1);
	segmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4));

	if (segmentOffset != 0)
		zRoom->parent->AddDeclarationPlaceholder(segmentOffset);
}

string SetActorList::GenerateSourceCodePass2(string roomName, int baseAddress)
{
	string sourceOutput = "";
	size_t numActorsReal = zRoom->GetDeclarationSizeFromNeighbor(segmentOffset) / 16;
	uint32_t currentPtr = segmentOffset;

	// TODO: hack
	actors.clear();

	for (size_t i = 0; i < numActorsReal; i++)
	{
		ActorSpawnEntry entry(rawData, currentPtr);
		actors.push_back(entry);

		currentPtr += 16;
	}

	sourceOutput +=
		StringHelper::Sprintf("\n    %s 0x%02X, (u32)%sActorList_%06X \n};",
	                          ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(),
	                          numActors, roomName.c_str(), segmentOffset);

	// zRoom->parent->AddDeclaration(segmentOffset, DeclarationAlignment::None,
	// DeclarationPadding::None, GetRawDataSize(), "SCmdActorList",
	// ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress), sourceOutput);

	string declaration = "";

	size_t index = 0;
	for (const auto& entry : actors)
	{
		declaration += StringHelper::Sprintf("    { %s }, // 0x%06X", entry.GetBodySourceCode().c_str(), segmentOffset + (index * 16));

		if (index < actors.size() - 1)
			declaration += "\n";

		index++;
	}

	DeclarationPadding padding = DeclarationPadding::Pad16;
	if (Globals::Instance->game == ZGame::MM_RETAIL)
		padding = DeclarationPadding::None;

	zRoom->parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::Align4, padding, actors.size() * 16, "ActorEntry",
		StringHelper::Sprintf("%sActorList_%06X", roomName.c_str(), segmentOffset),
		GetActorListArraySize(), declaration);

	return sourceOutput;
}

int32_t SetActorList::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + ((int)actors.size() * 16);
}

size_t SetActorList::GetActorListArraySize()
{
	size_t actorCount = 0;

	// Doing an else-if here so we only do the loop when the game is SW97.
	// Actor 0x22 is removed from SW97, so we need to ensure that we don't increment the actor count
	// for it.
	if (Globals::Instance->game == ZGame::OOT_SW97)
	{
		actorCount = 0;

		for (const auto& entry : actors)
			if (entry.GetActorId() != 0x22)
				actorCount++;
	}
	else
	{
		actorCount = actors.size();
	}

	return actorCount;
}

string SetActorList::GenerateExterns()
{
	Declaration* decl = parent->GetDeclaration(segmentOffset);
	if (decl == nullptr)
	{
		return "";
	}

	return StringHelper::Sprintf("extern %s %s[];\n", actors.at(0).GetSourceTypeName().c_str(), decl->varName.c_str());
}

string SetActorList::GetCommandCName()
{
	return "SCmdActorList";
}

RoomCommand SetActorList::GetRoomCommand()
{
	return RoomCommand::SetActorList;
}

ActorSpawnEntry::ActorSpawnEntry(const std::vector<uint8_t>& rawData, int rawDataIndex)
{
	actorNum = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);
	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 6);
	rotX = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	rotY = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	rotZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);
	initVar = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
}

std::string ActorSpawnEntry::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf(
			"%s, %i, %i, %i, SPAWN_ROT_FLAGS(%i, 0x%04X), SPAWN_ROT_FLAGS(%i, 0x%04X), "
			"SPAWN_ROT_FLAGS(%i, 0x%04X), 0x%04X",
			ZNames::GetActorName(actorNum).c_str(), posX, posY, posZ,
			(rotX >> 7) & 0b111111111, rotX & 0b1111111,
			(rotY >> 7) & 0b111111111, rotY & 0b1111111,
			(rotZ >> 7) & 0b111111111, rotZ & 0b1111111, initVar);
	}

	return StringHelper::Sprintf("%s, %i, %i, %i, %i, %i, %i, 0x%04X", ZNames::GetActorName(actorNum).c_str(), posX, posY, posZ, rotX, rotY, rotZ, initVar);
}

std::string ActorSpawnEntry::GetSourceTypeName() const
{
	return "ActorEntry";
}

uint16_t ActorSpawnEntry::GetActorId() const
{
	return actorNum;
}
