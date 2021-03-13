#include "ZSkeleton.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "HighLevel/HLModelIntermediette.h"

using namespace std;

ZLimbStandard::ZLimbStandard(ZFile* nParent) : ZResource(nParent)
{
	name = "";
	transX = 0;
	transY = 0;
	transZ = 0;
	childIndex = 0;
	siblingIndex = 0;
	dListPtr = 0;
	dList = nullptr;
	children = vector<ZLimbStandard*>();
	skeleton = nullptr;
}

ZSkeleton::ZSkeleton(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZFile* nParent)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;
	parent = nParent;

	ParseXML(reader);
	ParseRawData();

	string defaultPrefix = name;
	defaultPrefix.replace(0, 1, "s"); // replace g prefix with s for local variables
	uint32_t ptr = Seg2Filespace(limbsArrayAddress, parent->baseAddress);

	for (size_t i = 0; i < limbCount; i++) {
		uint32_t ptr2 = Seg2Filespace(BitConverter::ToUInt32BE(rawData, ptr), parent->baseAddress);

		ZLimb* limb = new ZLimb(reader, rawData, ptr2, parent);
		limb->SetName(StringHelper::Sprintf("%sLimb_%06X", defaultPrefix.c_str(), limb->GetFileAddress()));
		limbs.push_back(limb);

		ptr += 4;
	}
}

ZSkeleton::~ZSkeleton()
{
	for (auto& limb: limbs) {
		delete limb;
	}
}

void ZSkeleton::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	const char* skelTypeXml = reader->Attribute("Type");
	if (skelTypeXml == nullptr) {
		fprintf(stderr, "ZSkeleton::ParseXML: Warning in '%s'.\n\t Type not found found. Defaulting to 'Normal'.\n", name.c_str());
		type = ZSkeletonType::Normal;
	}
	else {
		string skelTypeStr(skelTypeXml);
		if (skelTypeStr == "Flex") {
			type = ZSkeletonType::Flex;
		}
		else if (skelTypeStr != "Normal") {
			fprintf(stderr, "ZSkeleton::ParseXML: Warning in '%s'.\n\t Invalid Type found: '%s'. Defaulting to 'Normal'.\n", name.c_str(), skelTypeXml);
			type = ZSkeletonType::Normal;
		}
	}

	const char* limbTypeXml = reader->Attribute("LimbType");
	if (limbTypeXml == nullptr) {
		fprintf(stderr, "ZSkeleton::ParseXML: Warning in '%s'.\n\t LimbType not found found. Defaulting to 'Standard'.\n", name.c_str());
		limbType = ZLimbType::Standard;
	}
	else {
		string limbTypeStr(limbTypeXml);
		if (limbTypeStr == "Standard") {
			limbType = ZLimbType::Standard;
		}
		else if (limbTypeStr == "LOD") {
			limbType = ZLimbType::LOD;
		}
		else if (limbTypeStr == "Skin") {
			limbType = ZLimbType::Skin;
		}
		else {
			fprintf(stderr, "ZSkeleton::ParseXML: Warning in '%s'.\n\t Invalid LimbType found: '%s'. Defaulting to 'Standard'.\n", name.c_str(), limbTypeXml);
			limbType = ZLimbType::Standard;
		}
	}
}

void ZSkeleton::ParseRawData()
{
	ZResource::ParseRawData();

	limbsArrayAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex);
	limbCount = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4);
	dListCount = BitConverter::ToUInt8BE(rawData, rawDataIndex + 8);
}

ZSkeleton* ZSkeleton::FromXML(tinyxml2::XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath, ZFile* nParent)
{
	ZSkeleton* skeleton = new ZSkeleton(reader, nRawData, rawDataIndex, nParent);
	skeleton->relativePath = std::move(nRelPath);

	skeleton->parent->AddDeclaration(
		skeleton->rawDataIndex, DeclarationAlignment::Align16, skeleton->GetRawDataSize(), 
		skeleton->GetSourceTypeName(), skeleton->name, "");

	return skeleton;
}

void ZSkeleton::Save(const std::string& outFolder)
{
}

ZSkeleton::ZSkeleton(ZFile* nParent) : ZResource(nParent)
{
	type = ZSkeletonType::Normal;
	limbs = vector<ZLimbStandard*>();
	rootLimb = nullptr;
	dListCount = 0;
}

void ZSkeleton::GenerateHLIntermediette(HLFileIntermediette& hlFile)
{
	HLModelIntermediette* mdl = (HLModelIntermediette*)&hlFile;
	HLModelIntermediette::FromZSkeleton(mdl, this);
	mdl->blocks.push_back(new HLTerminator());
}

int ZSkeleton::GetRawDataSize()
{
	switch (type) {
	case ZSkeletonType::Normal:
		return 0x8;
	case ZSkeletonType::Flex:
		return 0xC;
	default:
		return 0x8;
	}

	skeleton->type = skeletonType;

	if (reader->Attribute("LimbType") != nullptr)
	{
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
			ZLimbStandard* limb = ZLimbStandard::FromRawData(nRawData, ptr2, nParent);
			limb->skeleton = skeleton;
			skeleton->limbs.push_back(limb);
		}
		else
		{
			ZLimbLOD* limb = ZLimbLOD::FromRawData(nRawData, ptr2, nParent);
			limb->skeleton = skeleton;
			skeleton->limbs.push_back(limb);
		}

		ptr += 4;
	}

	return skeleton;
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

	uint32_t ptr = Seg2Filespace(limbsArrayAddress, parent->baseAddress);
	if (!parent->HasDeclaration(ptr))
	{
			if (limb->dListPtr != 0 && parent->GetDeclaration(limb->dListPtr) == nullptr)
			{
				ZDisplayList* dList = new ZDisplayList(rawData, limb->dListPtr, ZDisplayList::GetDListLength(rawData, limb->dListPtr, Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX), parent);
				dList->SetName(StringHelper::Sprintf("%sLimbDL_%06X", defaultPrefix.c_str(), limb->dListPtr));
				dList->GetSourceOutputCode(defaultPrefix);
				limb->dList = dList;
			}

			string entryStr = "";
			string entryType = "";

			if (typeid(*limb) == typeid(ZLimbLOD))
			{
				ZLimbLOD* limbLOD = (ZLimbLOD*)limbs[i];
				string defaultFarDLName = StringHelper::Sprintf("%sFarLimbDlist0x%06X", defaultPrefix.c_str(), limbLOD->farDListPtr);
				string dListStr2 = limbLOD->farDListPtr == 0 ? "NULL" : StringHelper::Sprintf("%s", parent->GetDeclarationName(limbLOD->farDListPtr, defaultFarDLName).c_str());

				if (limbLOD->farDListPtr != 0 && parent->GetDeclaration(limbLOD->farDListPtr) == nullptr)
				{
					ZDisplayList* dList = new ZDisplayList(rawData, limbLOD->farDListPtr, ZDisplayList::GetDListLength(rawData, limbLOD->farDListPtr, Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX), parent);
					dList->SetName(StringHelper::Sprintf("%s_farLimbDlist_%06X", defaultPrefix.c_str(), limbLOD->farDListPtr));
					dList->GetSourceOutputCode(defaultPrefix);
					limb->dList = dList;
				}

				entryType = "LodLimb";

				entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, { %s, %s }",
					limbLOD->transX, limbLOD->transY, limbLOD->transZ, limbLOD->childIndex, limbLOD->siblingIndex, dListStr.c_str(), dListStr2.c_str());
			}
			else
			{
				entryType = "StandardLimb";

				entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, %s",
					limb->transX, limb->transY, limb->transZ, limb->childIndex, limb->siblingIndex, dListStr.c_str());
			}

			string limbName = StringHelper::Sprintf("%sLimb_%06X", defaultPrefix.c_str(), limb->address);

			if (parent->HasDeclaration(limb->address))
				limbName = parent->GetDeclarationName(limb->address);

			parent->AddDeclaration(limb->address, DeclarationAlignment::None, limb->GetRawDataSize(), entryType, limbName, entryStr);
		}

		// Table
		string tblStr = "";

		for (size_t i = 0; i < limbs.size(); i++)
		{
			ZLimb* limb = limbs.at(i);

			string decl = StringHelper::Sprintf("    &%s,", parent->GetDeclarationName(limb->GetFileAddress()).c_str());
			if (i != (limbs.size() - 1)) {
				decl += "\n";
			}

			tblStr += decl;
		}

		parent->AddDeclarationArray(ptr, DeclarationAlignment::None, 4 * limbCount,
			StringHelper::Sprintf("static %s*", ZLimb::GetSourceTypeName(limbType)), 
			StringHelper::Sprintf("%sLimbs", defaultPrefix.c_str()), limbCount, tblStr);
	}

	string headerStr;
	switch (type) {
	case ZSkeletonType::Normal:
		headerStr = StringHelper::Sprintf("%sLimbs, %i", defaultPrefix.c_str(), limbCount);
		break;
	case ZSkeletonType::Flex:
		headerStr = StringHelper::Sprintf("%sLimbs, %i, %i", defaultPrefix.c_str(), limbCount, dListCount);
		break;
	}

	Declaration* decl = parent->GetDeclaration(GetAddress());
	if (decl == nullptr) {
		parent->AddDeclaration(GetAddress(), DeclarationAlignment::Align16, 
			GetRawDataSize(), GetSourceTypeName(), name, headerStr);
	}
	else {
		decl->text = headerStr;
	}

	return "";
}

std::string ZSkeleton::GetSourceTypeName()
{
	switch (type) {
	case ZSkeletonType::Normal:
		return "SkeletonHeader";
	case ZSkeletonType::Flex:
		return "FlexSkeletonHeader";
	}
	return "SkeletonHeader";
}

ZResourceType ZSkeleton::GetResourceType()
{
	return ZResourceType::Skeleton;
}

segptr_t ZSkeleton::GetAddress()
ZLimbLOD::ZLimbLOD(ZFile* nParent) : ZLimbStandard(nParent)
{
	return rawDataIndex;
}

ZLimbLOD* ZLimbLOD::FromRawData(vector<uint8_t> nRawData, int rawDataIndex, ZFile* nParent)
{
	ZLimbLOD* limb = new ZLimbLOD(nParent);

	limb->address = rawDataIndex;

	limb->transX = BitConverter::ToInt16BE(nRawData, rawDataIndex + 0);
	limb->transY = BitConverter::ToInt16BE(nRawData, rawDataIndex + 2);
	limb->transZ = BitConverter::ToInt16BE(nRawData, rawDataIndex + 4);

	limb->childIndex = nRawData[rawDataIndex + 6];
	limb->siblingIndex = nRawData[rawDataIndex + 7];

	limb->dListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 8) & 0x00FFFFFF;
	limb->farDListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 12) & 0x00FFFFFF;

	return limb;
}

string ZLimbLOD::GetSourceOutputCode(const std::string& prefix)
{
	return std::string();
}

int ZLimbLOD::GetRawDataSize()
{
	return 16;
}
