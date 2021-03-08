#include "ZSkeleton.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "Globals.h"
#include "HighLevel/HLModelIntermediette.h"
#include <typeinfo>

using namespace std;
using namespace tinyxml2;

ZLimbStandard::ZLimbStandard(const std::vector<uint8_t>& nRawData, int rawDataIndex)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = rawDataIndex;

	segAddress = rawDataIndex;

	transX = BitConverter::ToInt16BE(nRawData, rawDataIndex + 0);
	transY = BitConverter::ToInt16BE(nRawData, rawDataIndex + 2);
	transZ = BitConverter::ToInt16BE(nRawData, rawDataIndex + 4);

	childIndex = nRawData[rawDataIndex + 6];
	siblingIndex = nRawData[rawDataIndex + 7];

	dListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 8);
}

ZLimbStandard::~ZLimbStandard()
{
	for (auto& child: children) {
		delete child;
	}
}

ZLimbStandard* ZLimbStandard::FromXML(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath, ZFile* parent)
{
	// TODO: Fix
	ZLimbType limbType = ZLimbType::Standard;
	string limbName = reader->Attribute("Name");
	int limbAddress = strtol(StringHelper::Split(reader->Attribute("Offset"), "0x")[1].c_str(), NULL, 16);

	if (string(reader->Attribute("Type")) == "LOD")
		limbType = ZLimbType::LOD;

	ZLimbStandard* limb = ZLimbStandard::FromRawData(nRawData, rawDataIndex);
	limb->ParseXML(reader);
	limb->parent = parent;
	limb->name = limbName;
	limb->segAddress = limbAddress;

	string entryType = limbType == ZLimbType::LOD ? "LodLimb" : "StandardLimb";

	limb->parent->AddDeclaration(
		limb->GetFileAddress(), DeclarationAlignment::None, limb->GetRawDataSize(), entryType, 
		StringHelper::Sprintf("%s", limbName.c_str(), limb->GetFileAddress()), ""); // ?

	return limb;
}

ZLimbStandard* ZLimbStandard::FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex)
{
	ZLimbStandard* limb = new ZLimbStandard(nRawData, rawDataIndex);
	return limb;
}

int ZLimbStandard::GetRawDataSize()
{
	return 0x0C;
}

string ZLimbStandard::GetSourceOutputCode(const std::string& prefix)
{
	string dListStr = "NULL";
	if (dListPtr != 0) {
		dListStr = "&" + ZLimbStandard::MakeLimbDListSourceOutputCode(prefix, "", dListPtr, rawData, parent);
	}

	string entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, %s",
		transX, transY, transZ, childIndex, siblingIndex, dListStr.c_str());

	Declaration* decl = parent->GetDeclaration(GetFileAddress());
	if (decl == nullptr) {
		string limbName = StringHelper::Sprintf("%sLimb_%06X", prefix.c_str(), GetFileAddress());
		parent->AddDeclaration(GetFileAddress(), DeclarationAlignment::None, GetRawDataSize(), GetSourceTypeName(), limbName, entryStr);
	}
	else {
		decl->text = entryStr;
	}

	return "";
}

std::string ZLimbStandard::GetSourceTypeName()
{
	return "StandardLimb";
}

ZResourceType ZLimbStandard::GetResourceType()
{
	return ZResourceType::Limb;
}

uint32_t ZLimbStandard::GetFileAddress()
{
	return Seg2Filespace(segAddress, parent->baseAddress);
}

std::string ZLimbStandard::MakeLimbDListSourceOutputCode(const std::string& prefix, const std::string& limbPrefix, uint32_t dListPtr, const std::vector<uint8_t> &rawData, ZFile* parent)
{
	if (dListPtr == 0) {
		return "NULL";
	}

	uint32_t dListOffset = Seg2Filespace(dListPtr, parent->baseAddress);

	string dListStr;
	Declaration* decl = parent->GetDeclaration(dListOffset);
	if (decl == nullptr) {
		dListStr = StringHelper::Sprintf("%s%sLimbDL_%06X", prefix.c_str(), limbPrefix.c_str(), dListOffset);

		// Does this need to be a pointer?
		ZDisplayList* dList = new ZDisplayList(rawData, dListOffset, ZDisplayList::GetDListLength(rawData, dListOffset, Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX));
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
	limbs = vector<ZLimbStandard*>();
	rootLimb = nullptr;
	dListCount = 0;
}

ZSkeleton* ZSkeleton::FromXML(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath, ZFile* nParent)
{
	ZSkeleton* skeleton = new ZSkeleton();
	skeleton->name = reader->Attribute("Name");
	skeleton->parent = nParent;
	ZLimbType limbType = ZLimbType::Standard;
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
			// TODO: Print some error here...
			//fprintf(stderr, "Error in ZSkeleton '%s': Unexpected type '%s'. Defaulting to 'Normal'.\n", skeleton->name.c_str(), skelType);
		}
	}

	skeleton->type = skeletonType;

	if (reader->Attribute("LimbType") != nullptr)
	{
		//printf("C3\n");

		if (string(reader->Attribute("LimbType")) == "LOD")
			limbType = ZLimbType::LOD;
	}

	limbCount = nRawData[rawDataIndex + 4];
	skeleton->dListCount = nRawData[rawDataIndex + 8];

	ZLimbStandard* currentLimb = nullptr;

	uint32_t ptr = (uint32_t)BitConverter::ToInt32BE(nRawData, rawDataIndex) & 0x00FFFFFF;

	for (int i = 0; i < limbCount; i++)
	{
		uint32_t ptr2 = (uint32_t)BitConverter::ToInt32BE(nRawData, ptr) & 0x00FFFFFF;

		if (limbType == ZLimbType::Standard)
		{
			ZLimbStandard* limb = ZLimbStandard::FromRawData(nRawData, ptr2);
			limb->parent = nParent;
			skeleton->limbs.push_back(limb);
		}
		else
		{
			ZLimbLOD* limb = ZLimbLOD::FromRawData(nRawData, ptr2);
			limb->parent = nParent;
			skeleton->limbs.push_back(limb);
		}

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
		exit(-1);
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
		string limbsType = "StandardLimb";

		for (int i = 0; i < limbs.size(); i++)
		{
			ZLimbStandard* limb = limbs[i];

			limbsType = limb->GetSourceTypeName();

			string decl = StringHelper::Sprintf("    &%s,", parent->GetDeclarationName(limb->GetFileAddress()).c_str());
			if (i != (limbs.size() - 1)) {
				decl += "\n";
			}

			tblStr += decl;
		}

		parent->AddDeclarationArray(ptr, DeclarationAlignment::None, 4 * limbs.size(),
			StringHelper::Sprintf("static %s*", limbsType.c_str()), 
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
	default:
		fprintf(stderr, "Error in ZSkeleton '%s': Type not implemented: %i. Defaulting to 'SkeletonHeader'\n", name.c_str(), type);
		exit(-1);
		return "SkeletonHeader";
	}
}

ZResourceType ZSkeleton::GetResourceType()
{
	return ZResourceType::Skeleton;
}


ZLimbLOD::ZLimbLOD(const std::vector<uint8_t>& nRawData, int rawDataIndex) : ZLimbStandard(nRawData, rawDataIndex)
{
	dListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 8);
	farDListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 12);
}

ZLimbLOD* ZLimbLOD::FromRawData(vector<uint8_t> nRawData, int rawDataIndex)
{
	ZLimbLOD* limb = new ZLimbLOD();
	return limb;
}

int ZLimbLOD::GetRawDataSize()
{
	return 0x10;
}

string ZLimbLOD::GetSourceOutputCode(const std::string& prefix)
{
	string dListStr = "NULL";
	string dListStr2 = "NULL";

	if (dListPtr != 0) {
		dListStr = "&" + ZLimbStandard::MakeLimbDListSourceOutputCode(prefix, "", dListPtr, rawData, parent);
	}
	if (farDListPtr != 0) {
		dListStr2 = "&" + ZLimbStandard::MakeLimbDListSourceOutputCode(prefix, "Far", farDListPtr, rawData, parent);
	}

	string entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, { %s, %s }",
		transX, transY, transZ, childIndex, siblingIndex, dListStr.c_str(), dListStr2.c_str());

	Declaration* decl = parent->GetDeclaration(GetFileAddress());
	if (decl == nullptr) {
		string limbName = StringHelper::Sprintf("%sFarLimb_%06X", prefix.c_str(), GetFileAddress());
		parent->AddDeclaration(GetFileAddress(), DeclarationAlignment::None, GetRawDataSize(), GetSourceTypeName(), limbName, entryStr);
	}
	else {
		decl->text = entryStr;
	}

	return "";
}

std::string ZLimbLOD::GetSourceTypeName()
{
	return "LodLimb";
}
