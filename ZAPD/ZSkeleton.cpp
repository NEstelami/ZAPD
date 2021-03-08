#include "ZSkeleton.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "Globals.h"
#include "HighLevel/HLModelIntermediette.h"
#include <typeinfo>
#include <cassert>

using namespace std;
using namespace tinyxml2;

ZLimb::ZLimb(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int rawDataIndex, ZFile* parent)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	this->rawDataIndex = rawDataIndex;
	this->parent = parent;

	segAddress = rawDataIndex;

	ParseXML(reader);
	ParseRawData();
}

ZLimb::~ZLimb()
{
	for (auto& child: children) {
		delete child;
	}
}

void ZLimb::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	const char* limbType = reader->Attribute("LimbType");
	if (limbType == nullptr) {
		fprintf(stderr, "ZLimb::ParseXML: Error in '%s'.\n\t Missing 'LimbType' attribute in xml. Defaulting to 'Standard'.\n", name.c_str());
		type = ZLimbType::Standard;
	}
	else {
		string limbTypeStr(limbType);
		if (limbTypeStr == "Standard") {
			type = ZLimbType::Standard;
		}
		else if(limbTypeStr == "LOD") {
			type = ZLimbType::LOD;
		}
		else if(limbTypeStr == "Skin") {
			type = ZLimbType::Skin;
		}
		else {
			fprintf(stderr, "ZLimb::ParseXML: Error in '%s'.\n\t Invalid LimbType found: '%s'. Defaulting to 'Standard'.\n", name.c_str(), limbType);
			type = ZLimbType::Standard;
		}
	}
}

void ZLimb::ParseRawData()
{
	transX = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	transY = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	transZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);

	childIndex = rawData.at(rawDataIndex + 6);
	siblingIndex = rawData.at(rawDataIndex + 7);

	switch (type) {
	case ZLimbType::LOD:
		farDListPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
	case ZLimbType::Standard:
		dListPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
		break;
	case ZLimbType::Skin:
		skinSegmentType = static_cast<ZLimbSkinType>(BitConverter::ToInt32BE(rawData, rawDataIndex + 8));
		skinSegment = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
		/*if (skinSegmentType == ZLimbSkinType::SkinType_4 || skinSegmentType == ZLimbSkinType::SkinType_DList) {
			printf("Type: %i\nsegment: 0x%08X\n\n", skinSegmentType, skinSegment);
		}
		else {
			printf("Type: %i\nsegment: %X\n\n", skinSegmentType, skinSegment);
		}*/
		break;
	}
}

ZLimb* ZLimb::FromXML(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath, ZFile* parent)
{
	ZLimb* limb = new ZLimb(reader, nRawData, rawDataIndex, parent);

	limb->relativePath = nRelPath;
	limb->parent->AddDeclaration(
		limb->GetFileAddress(), DeclarationAlignment::None, limb->GetRawDataSize(), 
		limb->GetSourceTypeName(), limb->name.c_str(), "");

	return limb;
}


int ZLimb::GetRawDataSize()
{
	switch (type) {
	case ZLimbType::Standard:
		return 0x0C;
	case ZLimbType::LOD:
	case ZLimbType::Skin:
		return 0x10;
	}
	return 0x0C;
}

string ZLimb::GetSourceOutputCode(const std::string& prefix)
{
	string dListStr = "NULL";
	string dListStr2 = "NULL";

	if (dListPtr != 0) {
		dListStr = ZLimb::MakeLimbDListSourceOutputCode(prefix, "", dListPtr, rawData, parent);
	}
	if (farDListPtr != 0) {
		dListStr2 = ZLimb::MakeLimbDListSourceOutputCode(prefix, "Far", farDListPtr, rawData, parent);
	}

	string entryStr = StringHelper::Sprintf("\n    { %i, %i, %i },\n    0x%02X, 0x%02X,\n",
			transX, transY, transZ, childIndex, siblingIndex);

	switch (type) {
	case ZLimbType::Standard:
		entryStr += StringHelper::Sprintf("    %s\n", dListStr.c_str());
		break;
	case ZLimbType::LOD:
		entryStr += StringHelper::Sprintf("    { %s, %s }\n",
			dListStr.c_str(), dListStr2.c_str());
		break;
	case ZLimbType::Skin:
		entryStr += GetSourceOutputCodeSkin(prefix);
		break;
	}

	Declaration* decl = parent->GetDeclaration(GetFileAddress());
	if (decl == nullptr) {
		parent->AddDeclaration(GetFileAddress(), DeclarationAlignment::None, GetRawDataSize(), GetSourceTypeName(), name, entryStr);
	}
	else {
		decl->text = entryStr;
	}

	return "";
}

std::string ZLimb::GetSourceTypeName()
{
	return GetSourceTypeName(type);
}

ZResourceType ZLimb::GetResourceType()
{
	return ZResourceType::Limb;
}

ZLimbType ZLimb::GetLimbType()
{
	return type;
}

const char* ZLimb::GetSourceTypeName(ZLimbType limbType)
{
	switch (limbType) {
	case ZLimbType::Standard:
		return "StandardLimb";
	case ZLimbType::LOD:
		return "LodLimb";
	case ZLimbType::Skin:
		return "SkinLimb";
	}
	return "StandardLimb";
}

uint32_t ZLimb::GetFileAddress()
{
	return Seg2Filespace(segAddress, parent->baseAddress);
}

std::string ZLimb::MakeLimbDListSourceOutputCode(const std::string& prefix, const std::string& limbPrefix, segptr_t dListPtr, const std::vector<uint8_t> &rawData, ZFile* parent)
{
	if (dListPtr == 0) {
		return "NULL";
	}

	uint32_t dListOffset = Seg2Filespace(dListPtr, parent->baseAddress);

	string dListStr;
	Declaration* decl = parent->GetDeclaration(dListOffset);
	if (decl == nullptr) {
		dListStr = StringHelper::Sprintf("%s%sLimbDL_%06X", prefix.c_str(), limbPrefix.c_str(), dListOffset);

		int dlistLength = ZDisplayList::GetDListLength(rawData, dListOffset, Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX);
		// Does this need to be a pointer?
		ZDisplayList* dList = new ZDisplayList(rawData, dListOffset, dlistLength);
		dList->parent = parent;
		dList->SetName(dListStr);
		dList->GetSourceOutputCode(prefix);
	}
	else {
		dListStr = decl->varName;
	}

	return dListStr;
}

std::string ZLimb::GetSourceOutputCodeSkin(const std::string& prefix)
{
	assert(type == ZLimbType::Skin);

	string skinSegmentStr = "NULL";

	if (skinSegment != 0) {
		switch (skinSegmentType) {
		case ZLimbSkinType::SkinType_4:
			skinSegmentStr = "&" + GetSourceOutputCodeSkin_Type_4(prefix);
			break;
		case ZLimbSkinType::SkinType_DList:
			skinSegmentStr = ZLimb::MakeLimbDListSourceOutputCode(prefix, "Skin", skinSegment, rawData, parent);
			break;
		default:
			fprintf(stderr, "ZLimb::GetSourceOutputCodeSkinType: Error in '%s'.\n\t Unknown segment type for SkinLimb: '%i'. \n\tPlease report this.\n", name.c_str(), static_cast<int32_t>(skinSegmentType));
		case ZLimbSkinType::SkinType_0:
		case ZLimbSkinType::SkinType_5:
			fprintf(stderr, "ZLimb::GetSourceOutputCodeSkinType: Error in '%s'.\n\t Segment type for SkinLimb not implemented: '%i'.\n", name.c_str(), static_cast<int32_t>(skinSegmentType));
			skinSegmentStr = StringHelper::Sprintf("0x%08X", skinSegment);
			break;
		}
	}

	string entryStr = StringHelper::Sprintf("    0x%02X, %s\n",
		skinSegmentType, skinSegmentStr.c_str());

	return entryStr;
}

// Should this be a class on it's own?
std::string ZLimb::GetSourceOutputCodeSkin_Type_4(const std::string& prefix)
{
	assert(type == ZLimbType::Skin);
	assert(skinSegmentType == ZLimbSkinType::SkinType_4);

	if (skinSegment == 0) {
		return "NULL";
	}

	// Hacky way. Change when the struct has a proper name.
	#define SKINTYPE_4_STRUCT_TYPE "Struct_800A5E28"
	#define SKINTYPE_4_STRUCT_SIZE 0x0C

	#define SKINTYPE_4_STRUCT_A598C_TYPE "Struct_800A598C"
	#define SKINTYPE_4_STRUCT_A598C_SIZE 0x10

	#define SKINTYPE_4_STRUCT_A57C0_TYPE "Struct_800A57C0"
	#define SKINTYPE_4_STRUCT_A57C0_SIZE 0x0A
	#define SKINTYPE_4_STRUCT_A598C_2_TYPE "Struct_800A598C_2"
	#define SKINTYPE_4_STRUCT_A598C_2_SIZE 0x0A

	uint32_t skinSegmentOffset = Seg2Filespace(skinSegment, parent->baseAddress);

	string struct_800A5E28_Str;
	Declaration* decl = parent->GetDeclaration(skinSegmentOffset);
	if (decl == nullptr) {
		string entryStr;
		struct_800A5E28_Str = StringHelper::Sprintf("%sSkinLimb" SKINTYPE_4_STRUCT_TYPE "_%06X", prefix.c_str(), skinSegmentOffset);

		/*int dlistLength = ZDisplayList::GetDListLength(rawData, skinSegmentOffset, Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX);
		// Does this need to be a pointer?
		ZDisplayList* dList = new ZDisplayList(rawData, skinSegmentOffset, dlistLength);
		dList->parent = parent;
		dList->SetName(struct_800A5E28_Str);
		dList->GetSourceOutputCode(prefix);*/

		uint16_t Struct_800A5E28_unk_0 = BitConverter::ToUInt16BE(rawData, skinSegmentOffset + 0);
		uint16_t Struct_800A5E28_unk_2 = BitConverter::ToUInt16BE(rawData, skinSegmentOffset + 2); // Length of Struct_800A5E28_unk_4
		segptr_t Struct_800A5E28_unk_4 = BitConverter::ToUInt32BE(rawData, skinSegmentOffset + 4); // Struct_800A598C*
		segptr_t Struct_800A5E28_unk_8 = BitConverter::ToUInt32BE(rawData, skinSegmentOffset + 8); // Gfx*

		uint32_t Struct_800A5E28_unk_4_Offset = Seg2Filespace(Struct_800A5E28_unk_4, parent->baseAddress);
		string Struct_800A5E28_unk_4_Str = "NULL";
		// TODO: if (decl == nullptr) {
		if (Struct_800A5E28_unk_4 != 0) {
			Struct_800A5E28_unk_4_Str = StringHelper::Sprintf("%sSkinLimb" SKINTYPE_4_STRUCT_A598C_TYPE "_%06X", prefix.c_str(), Struct_800A5E28_unk_4_Offset);

			int arrayItemCnt = Struct_800A5E28_unk_2;
			entryStr = "";
			for (uint16_t i = 0; i < arrayItemCnt; i++) {
				entryStr += StringHelper::Sprintf("    { %s },%s", 
					GetSourceOutputCodeSkin_Type_4_StructA5E28_Entry(prefix, Struct_800A5E28_unk_4_Offset, i).c_str(), 
					(i + 1 < arrayItemCnt) ? "\n" : "");
			}

			// TODO: Prevent adding the declaration to the header. 
			parent->AddDeclarationArray(
				Struct_800A5E28_unk_4_Offset, DeclarationAlignment::None, 
				SKINTYPE_4_STRUCT_A598C_SIZE * arrayItemCnt, SKINTYPE_4_STRUCT_A598C_TYPE, 
				Struct_800A5E28_unk_4_Str, arrayItemCnt, entryStr);
			
			Struct_800A5E28_unk_4_Str = "&" + Struct_800A5E28_unk_4_Str;
		}

		string Struct_800A5E28_unk_8_Str = "NULL";
		if (Struct_800A5E28_unk_8 != 0) {
			// TODO: Fix
			// Struct_800A5E28_unk_8_Str = ZLimb::MakeLimbDListSourceOutputCode(prefix, SKINTYPE_4_STRUCT_TYPE, Struct_800A5E28_unk_8, rawData, parent);
			Struct_800A5E28_unk_8_Str = StringHelper::Sprintf("0x%08X", Struct_800A5E28_unk_8);
		}


		entryStr = StringHelper::Sprintf("0x%04X, 0x%04X, %s, %s", 
			Struct_800A5E28_unk_0, Struct_800A5E28_unk_2, 
			Struct_800A5E28_unk_4_Str.c_str(), Struct_800A5E28_unk_8_Str.c_str());

		// TODO: Prevent adding the declaration to the header. 
		parent->AddDeclaration(
			skinSegmentOffset, DeclarationAlignment::None, 
			SKINTYPE_4_STRUCT_SIZE, SKINTYPE_4_STRUCT_TYPE, 
			struct_800A5E28_Str, entryStr);
	}
	else {
		struct_800A5E28_Str = decl->varName;
	}

	return struct_800A5E28_Str;
}

std::string ZLimb::GetSourceOutputCodeSkin_Type_4_StructA5E28_Entry(const std::string& prefix, uint32_t fileOffset, uint16_t index)
{
	uint16_t Struct_800A598C_unk_0 = BitConverter::ToUInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_SIZE) + 0x00); // length of Struct_800A598C_unk_8
	uint16_t Struct_800A598C_unk_2 = BitConverter::ToUInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_SIZE) + 0x02);
	uint16_t Struct_800A598C_unk_4 = BitConverter::ToUInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_SIZE) + 0x04);
	segptr_t Struct_800A598C_unk_8 = BitConverter::ToUInt32BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_SIZE) + 0x08); // Struct_800A57C0*
	segptr_t Struct_800A598C_unk_C = BitConverter::ToUInt32BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_SIZE) + 0x0C); // Struct_800A598C_2*

	string entryStr;

	uint32_t Struct_800A598C_unk_8_Offset = Seg2Filespace(Struct_800A598C_unk_8, parent->baseAddress);
	string Struct_800A598C_unk_8_Str = "NULL";
	// TODO: if (decl == nullptr) {
	if (Struct_800A598C_unk_8 != 0) {
		Struct_800A598C_unk_8_Str = StringHelper::Sprintf("%sSkinLimb" SKINTYPE_4_STRUCT_A57C0_TYPE "_%06X", prefix.c_str(), Struct_800A598C_unk_8_Offset);

		int arrayItemCnt = Struct_800A598C_unk_0;
		entryStr = "";
		for (uint16_t i = 0; i < arrayItemCnt; i++) {
			entryStr += StringHelper::Sprintf("    { %s },%s", 
				GetSourceOutputCodeSkin_Type_4_StructA57C0_Entry(prefix, Struct_800A598C_unk_8_Offset, i).c_str(), 
				(i + 1 < arrayItemCnt) ? "\n" : "");
		}

		// TODO: Prevent adding the declaration to the header. 
		parent->AddDeclarationArray(
			Struct_800A598C_unk_8_Offset, DeclarationAlignment::None, 
			SKINTYPE_4_STRUCT_A57C0_SIZE * arrayItemCnt, SKINTYPE_4_STRUCT_A57C0_TYPE, 
			Struct_800A598C_unk_8_Str, arrayItemCnt, entryStr);

		Struct_800A598C_unk_8_Str = "&" + Struct_800A598C_unk_8_Str;
	}

	uint32_t Struct_800A598C_unk_C_Offset = Seg2Filespace(Struct_800A598C_unk_C, parent->baseAddress);;
	string Struct_800A598C_unk_C_Str = "NULL";
	// TODO: if (decl == nullptr) {
	if (Struct_800A598C_unk_C != 0) {
		Struct_800A598C_unk_C_Str = StringHelper::Sprintf("%sSkinLimb" SKINTYPE_4_STRUCT_A598C_2_TYPE "_%06X", prefix.c_str(), Struct_800A598C_unk_C_Offset);

		int arrayItemCnt = Struct_800A598C_unk_2; // TODO: Check.
		entryStr = "";
		for (uint16_t i = 0; i < arrayItemCnt; i++) {
			entryStr += StringHelper::Sprintf("    { %s },%s", 
				GetSourceOutputCodeSkin_Type_4_StructA598C_2_Entry(prefix, Struct_800A598C_unk_C_Offset, i).c_str(), 
				(i + 1 < arrayItemCnt) ? "\n" : "");
		}

		// TODO: Prevent adding the declaration to the header. 
		parent->AddDeclarationArray(
			Struct_800A598C_unk_C_Offset, DeclarationAlignment::None, 
			SKINTYPE_4_STRUCT_A598C_2_SIZE * arrayItemCnt, SKINTYPE_4_STRUCT_A598C_2_TYPE, 
			Struct_800A598C_unk_C_Str, arrayItemCnt, entryStr);

		Struct_800A598C_unk_C_Str = "&" + Struct_800A598C_unk_C_Str;
	}

	//string Struct_800A598C_unk_8_Str = StringHelper::Sprintf("0x%08X", Struct_800A598C_unk_8);
	//string Struct_800A598C_unk_C_Str = StringHelper::Sprintf("0x%08X", Struct_800A598C_unk_C);

	return StringHelper::Sprintf("0x%04X, 0x%04X, 0x%04X, %s, %s", 
		Struct_800A598C_unk_0, Struct_800A598C_unk_2, Struct_800A598C_unk_4,
		Struct_800A598C_unk_8_Str.c_str(), Struct_800A598C_unk_C_Str.c_str());
}

	#undef SKINTYPE_4_STRUCT_TYPE
	#undef SKINTYPE_4_STRUCT_TYPE_SIZE

	#undef SKINTYPE_4_STRUCT_A598C_TYPE
	#undef SKINTYPE_4_STRUCT_A598C_SIZE

std::string ZLimb::GetSourceOutputCodeSkin_Type_4_StructA57C0_Entry(const std::string& prefix, uint32_t fileOffset, uint16_t index)
{
	uint16_t Struct_800A57C0_unk_0 = BitConverter::ToUInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A57C0_SIZE) + 0x00);
	int16_t Struct_800A57C0_unk_2 = BitConverter::ToInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A57C0_SIZE) + 0x02);
	int16_t Struct_800A57C0_unk_4 = BitConverter::ToInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A57C0_SIZE) + 0x04);
	int8_t Struct_800A57C0_unk_6 = BitConverter::ToInt8BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A57C0_SIZE) + 0x06);
	int8_t Struct_800A57C0_unk_7 = BitConverter::ToInt8BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A57C0_SIZE) + 0x07);
	int8_t Struct_800A57C0_unk_8 = BitConverter::ToInt8BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A57C0_SIZE) + 0x08);
	uint8_t Struct_800A57C0_unk_9 = BitConverter::ToUInt8BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A57C0_SIZE) + 0x09);

	return StringHelper::Sprintf("0x%02X, %i, %i, %i, %i, %i, 0x%02X", 
		Struct_800A57C0_unk_0, 
		Struct_800A57C0_unk_2, Struct_800A57C0_unk_4,
		Struct_800A57C0_unk_6, Struct_800A57C0_unk_7, Struct_800A57C0_unk_8,
		Struct_800A57C0_unk_9);
}

std::string ZLimb::GetSourceOutputCodeSkin_Type_4_StructA598C_2_Entry(const std::string& prefix, uint32_t fileOffset, uint16_t index)
{
	uint8_t Struct_800A598C_2_unk_0 = BitConverter::ToUInt8BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_2_SIZE) + 0x00);
	int16_t Struct_800A598C_2_x = BitConverter::ToInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_2_SIZE) + 0x02);
	int16_t Struct_800A598C_2_y = BitConverter::ToInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_2_SIZE) + 0x04);
	int16_t Struct_800A598C_2_z = BitConverter::ToInt16BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_2_SIZE) + 0x06);
	uint8_t Struct_800A598C_2_unk_8 = BitConverter::ToUInt8BE(rawData, fileOffset + (index * SKINTYPE_4_STRUCT_A598C_2_SIZE) + 0x08);

	return StringHelper::Sprintf("0x%02X, %i, %i, %i, 0x%02X", 
		Struct_800A598C_2_unk_0, 
		Struct_800A598C_2_x, Struct_800A598C_2_y, Struct_800A598C_2_z,
		Struct_800A598C_2_unk_8);
}

#undef SKINTYPE_4_STRUCT_A57C0_TYPE
#undef SKINTYPE_4_STRUCT_A57C0_SIZE
#undef SKINTYPE_4_STRUCT_A598C_2_TYPE
#undef SKINTYPE_4_STRUCT_A598C_2_SIZE


ZSkeleton::ZSkeleton() : ZResource()
{
	type = ZSkeletonType::Normal;
	limbs = vector<ZLimb*>();
	rootLimb = nullptr;
	dListCount = 0;
}

ZSkeleton* ZSkeleton::FromXML(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath, ZFile* nParent)
{
	ZSkeleton* skeleton = new ZSkeleton();
	skeleton->name = reader->Attribute("Name");
	skeleton->parent = nParent;
	ZSkeletonType skeletonType = ZSkeletonType::Normal;
	int limbCount = 0;

	skeleton->rawData = nRawData;
	skeleton->rawDataIndex = rawDataIndex;

	const char* skelType = reader->Attribute("Type");
	if (skelType != nullptr)
	{
		if (string(skelType) == "Flex")
			skeletonType = ZSkeletonType::Flex;
		else if (string(skelType) == "Skin")
			skeletonType = ZSkeletonType::Skin;
		else if (string(skelType) != "Normal")
		{
			fprintf(stderr, "ZSkeleton::FromXML: Error in '%s'.\n\t Invalid Type found: '%s'. Defaulting to 'Normal'.\n", skeleton->name.c_str(), skelType);
		}
	}

	skeleton->type = skeletonType;

	if (reader->Attribute("LimbType") != nullptr)
	{
		if (string(reader->Attribute("LimbType")) == "Standard")
			skeleton->limbType = ZLimbType::Standard;
		else if (string(reader->Attribute("LimbType")) == "LOD")
			skeleton->limbType = ZLimbType::LOD;
		else if (string(reader->Attribute("LimbType")) == "Skin")
			skeleton->limbType = ZLimbType::Skin;
	}

	limbCount = nRawData.at(rawDataIndex + 4);
	skeleton->dListCount = nRawData.at(rawDataIndex + 8);

	string defaultPrefix = skeleton->name.c_str();
	defaultPrefix.replace(0, 1, "s"); // replace g prefix with s for local variables
	// TODO: Fix?
	uint32_t ptr = (uint32_t)BitConverter::ToInt32BE(nRawData, rawDataIndex) & 0x00FFFFFF;

	for (int i = 0; i < limbCount; i++)
	{
		uint32_t ptr2 = (uint32_t)BitConverter::ToInt32BE(nRawData, ptr) & 0x00FFFFFF;

		ZLimb* limb = new ZLimb(reader, nRawData, ptr2, nParent);
		limb->SetName(StringHelper::Sprintf("%sLimb_%06X", defaultPrefix.c_str(), limb->GetFileAddress()));
		skeleton->limbs.push_back(limb);

		ptr += 4;
	}

	return skeleton;
}

void ZSkeleton::Save(const std::string& outFolder)
{

}

void ZSkeleton::GenerateHLIntermediette(HLFileIntermediette& hlFile)
{
	HLModelIntermediette* mdl = (HLModelIntermediette*)&hlFile;
	HLModelIntermediette::FromZSkeleton(mdl, this);
	//mdl->blocks.push_back(new HLTerminator());
}

int ZSkeleton::GetRawDataSize()
{
	switch (type) {
	case ZSkeletonType::Normal:
		return 0x8;
	case ZSkeletonType::Flex:
		return 0xC;
	default:
		fprintf(stderr, "Error in ZSkeleton '%s': Type not implemented: %i. Defaulting to 'SkeletonHeader'\n", name.c_str(), type);
		exit(-1); // TODO: remove
		return 0x8;
	}
}

std::string ZSkeleton::GetSourceOutputCode(const std::string& prefix)
{
	if (parent == nullptr) {
		return "";
	}

	string defaultPrefix = name.c_str();
	defaultPrefix.replace(0, 1, "s"); // replace g prefix with s for local variables

	for (auto& limb: limbs) {
		limb->GetSourceOutputCode(defaultPrefix);
	}

	uint32_t ptr = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex) & 0x00FFFFFF;

	if (!parent->HasDeclaration(ptr))
	{
		// Table
		string tblStr = "";

		for (int i = 0; i < limbs.size(); i++)
		{
			ZLimb* limb = limbs.at(i);

			string decl = StringHelper::Sprintf("    &%s,", parent->GetDeclarationName(limb->GetFileAddress()).c_str());
			if (i != (limbs.size() - 1)) {
				decl += "\n";
			}

			tblStr += decl;
		}

		parent->AddDeclarationArray(ptr, DeclarationAlignment::None, 4 * limbs.size(),
			StringHelper::Sprintf("static %s*", ZLimb::GetSourceTypeName(limbType)), 
			StringHelper::Sprintf("%sLimbs", defaultPrefix.c_str()), limbs.size(), tblStr);
	}

	string headerStr;
	switch (type) {
	case ZSkeletonType::Normal:
		headerStr = StringHelper::Sprintf("%sLimbs, %i", defaultPrefix.c_str(), limbs.size());
		break;
	case ZSkeletonType::Flex:
		headerStr = StringHelper::Sprintf("%sLimbs, %i, %i", defaultPrefix.c_str(), limbs.size(), dListCount);
		break;
	case ZSkeletonType::Skin:
		// TODO
		fprintf(stderr, "Oh no!\n");
		break;
	}

	parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align16, GetRawDataSize(),
		GetSourceTypeName(), StringHelper::Sprintf("%s", name.c_str()), headerStr);

	return "";
}

std::string ZSkeleton::GetSourceTypeName()
{
	switch (type) {
	case ZSkeletonType::Normal:
		return "SkeletonHeader";
	case ZSkeletonType::Flex:
		return "FlexSkeletonHeader";
	case ZSkeletonType::Skin:
		return "SkeletonHeader"; // ?
	}
	return "SkeletonHeader";
}

ZResourceType ZSkeleton::GetResourceType()
{
	return ZResourceType::Skeleton;
}

