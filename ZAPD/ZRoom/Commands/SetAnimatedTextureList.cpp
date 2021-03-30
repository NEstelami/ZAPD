#include "SetAnimatedTextureList.h"
#include "../../BitConverter.h"
#include "../../Globals.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetAnimatedTextureList::SetAnimatedTextureList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	segmentOffset = GETSEGOFFSET(BitConverter::ToInt32BE(rawData, rawDataIndex + 4));

	string declaration = "";
	
	int32_t currentPtr = segmentOffset;

	AnimatedTexture* lastTexture = nullptr;

	do
	{
		lastTexture = new AnimatedTexture(rawData, currentPtr);
		currentPtr += 8;
		textures.push_back(lastTexture);

        // TODO add params declations
		string textureName = (lastTexture->segmentOffset != 0) ?
			StringHelper::Sprintf("&%sAnimatedTextureParams0x%06X", zRoom->GetName().c_str(), lastTexture->segmentOffset) : "NULL";

		declaration += StringHelper::Sprintf("	{ %i, %i, (u32)%s },", lastTexture->segment, lastTexture->type, textureName.c_str());

		if (lastTexture->segment != 0)
			declaration += "\n";

	} while (lastTexture->segment != 0);

	zRoom->parent->AddDeclarationArray(
		segmentOffset, DeclarationAlignment::None, DeclarationPadding::Pad16,
		textures.size() * 8, "AnimatedTexture",
		StringHelper::Sprintf("%sAnimatedTextureList0x%06X", zRoom->GetName().c_str(), segmentOffset),
		textures.size(), declaration);
}

SetAnimatedTextureList::~SetAnimatedTextureList()
{
	for (AnimatedTexture* texture : textures) 
	    delete texture;
}

string SetAnimatedTextureList::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string pass1 = ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress);
	Declaration* decl = zRoom->parent->GetDeclaration(segmentOffset);
	if (decl != nullptr)
	{
		return StringHelper::Sprintf("%s 0, (u32)%s", pass1.c_str(), decl->varName.c_str());
	}
	return StringHelper::Sprintf("%s 0, (u32)%sAnimatedTextureList0x%06X", pass1.c_str(),
	                             zRoom->GetName().c_str(), segmentOffset);
}

int32_t SetAnimatedTextureList::GetRawDataSize()
{
	return ZRoomCommand::GetRawDataSize() + (0);
}

string SetAnimatedTextureList::GenerateExterns()
{
	return "";
}

string SetAnimatedTextureList::GetCommandCName()
{
	return "SCmdTextureAnimations";
}

RoomCommand SetAnimatedTextureList::GetRoomCommand()
{
	return RoomCommand::SetAnimatedTextureList;
}

string SetAnimatedTextureList::GetSourceOutputCode(std::string prefix)
{
	return "";
}

AnimatedTexture::AnimatedTexture(std::vector<uint8_t> rawData, int rawDataIndex) :
    segment(rawData[rawDataIndex]),
    type(BitConverter::ToInt16BE(rawData, rawDataIndex + 2)),
    segmentOffset(BitConverter::ToInt32BE(rawData, rawDataIndex + 4))
{
}
