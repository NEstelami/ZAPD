#include "SetAnimatedMaterialList.h"

#include "Utils/BitConverter.h"
#include "Globals.h"
#include "Utils/StringHelper.h"
#include "ZFile.h"
#include "ZRoom/ZRoom.h"
#include "ZTextureAnimation.h"

SetAnimatedMaterialList::SetAnimatedMaterialList(ZFile* nParent) : ZRoomCommand(nParent), textureAnimation(nParent)
{
}

void SetAnimatedMaterialList::ParseRawData()
{
	ZRoomCommand::ParseRawData();
	textureAnimation.SetRawDataIndex(segmentOffset);
	textureAnimation.ParseRawData();
}

void SetAnimatedMaterialList::DeclareReferences(const std::string& prefix)
{
	textureAnimation.SetName(textureAnimation.GetDefaultName(prefix.c_str()));
	textureAnimation.DeclareReferences(prefix);
	textureAnimation.GetSourceOutputCode(prefix);
}

std::string SetAnimatedMaterialList::GetBodySourceCode() const
{
	std::string listName = parent->GetDeclarationPtrName(cmdArg2);
	return StringHelper::Sprintf("SCENE_CMD_ANIMATED_MATERIAL_LIST(%s)", listName.c_str());
}

size_t SetAnimatedMaterialList::GetRawDataSize() const
{
	int32_t size = textureAnimation.GetRawDataSize();

	return ZRoomCommand::GetRawDataSize() + size;
}

std::string SetAnimatedMaterialList::GetCommandCName() const
{
	return "SCmdTextureAnimations";
}

RoomCommand SetAnimatedMaterialList::GetRoomCommand() const
{
	return RoomCommand::SetAnimatedMaterialList;
}
