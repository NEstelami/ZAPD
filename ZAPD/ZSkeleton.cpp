#include "ZSkeleton.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "Globals.h"
#include "HighLevel/HLModelIntermediette.h"
#include <typeinfo>

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
		skinSegmentType = BitConverter::ToInt32BE(rawData, rawDataIndex + 8);
		skinSegment = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
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
	string skinSegmentStr = "NULL";

	if (dListPtr != 0) {
		dListStr = "&" + ZLimb::MakeLimbDListSourceOutputCode(prefix, "", dListPtr, rawData, parent);
	}
	if (farDListPtr != 0) {
		dListStr2 = "&" + ZLimb::MakeLimbDListSourceOutputCode(prefix, "Far", farDListPtr, rawData, parent);
	}
	if (skinSegment != 0) {
		//skinSegmentStr = "&" + ZLimb::MakeLimbDListSourceOutputCode(prefix, "Skin", skinSegment, rawData, parent);
		skinSegmentStr = StringHelper::Sprintf("0x%08X", skinSegment);
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
		entryStr += StringHelper::Sprintf("    0x%02X, %s\n",
			skinSegmentType, skinSegmentStr.c_str());
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

std::string ZLimb::MakeLimbDListSourceOutputCode(const std::string& prefix, const std::string& limbPrefix, uint32_t dListPtr, const std::vector<uint8_t> &rawData, ZFile* parent)
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

