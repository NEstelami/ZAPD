#include "ZSkeleton.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "Globals.h"
#include "HighLevel/HLModelIntermediette.h"
#include <typeinfo>

using namespace std;
using namespace tinyxml2;

ZLimbStandard::ZLimbStandard()
{
	name = "";
	transX = 0;
	transY = 0;
	transZ = 0;
	childIndex = 0;
	siblingIndex = 0;
	dListPtr = 0;
	children = vector<ZLimbStandard*>();
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
	limb->fileAddress = SEG2FILESPACE(limbAddress) - ( (GETSEGNUM(limbAddress) == 0x80) ? SEG2FILESPACE(parent->baseAddress) : 0 );

	string entryType = limbType == ZLimbType::LOD ? "LodLimb" : "StandardLimb";

	limb->parent->AddDeclaration(
		limb->fileAddress, DeclarationAlignment::None, limb->GetRawDataSize(), entryType, 
		StringHelper::Sprintf("%s", limbName.c_str(), limb->fileAddress), ""); // ?

	return limb;
}

ZLimbStandard* ZLimbStandard::FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex)
{
	ZLimbStandard* limb = new ZLimbStandard();

	// ?
	limb->segAddress = rawDataIndex;
	limb->fileAddress = rawDataIndex;

	limb->transX = BitConverter::ToInt16BE(nRawData, rawDataIndex + 0);
	limb->transY = BitConverter::ToInt16BE(nRawData, rawDataIndex + 2);
	limb->transZ = BitConverter::ToInt16BE(nRawData, rawDataIndex + 4);

	limb->childIndex = nRawData[rawDataIndex + 6];
	limb->siblingIndex = nRawData[rawDataIndex + 7];

	limb->dListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 8);

	return limb;
}

int ZLimbStandard::GetRawDataSize()
{
	return 12;
}

string ZLimbStandard::GetSourceOutputCode(const std::string& prefix)
{
	uint32_t dListOffset = SEG2FILESPACE(dListPtr);
	if (GETSEGNUM(dListPtr) == 0x80) {
		dListOffset -= SEG2FILESPACE(parent->baseAddress);
	}
	string dListStr = dListPtr == 0 ? "NULL" : StringHelper::Sprintf("%s", parent->GetVarName(dListOffset).c_str());

	string entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, %s",
		transX, transY, transZ, childIndex, siblingIndex, dListStr.c_str());

	Declaration* decl = parent->GetDeclaration(fileAddress);
	decl->text = entryStr;

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

std::string ZLimbStandard::MakeLimbDListSourceOutputCode(const std::string& prefix, const std::string& limbPrefix, uint32_t dListOffset, const std::vector<uint8_t> &rawData, ZFile* parent)
{
	string defaultDLName = StringHelper::Sprintf("%s%sLimbDL_%06X", prefix.c_str(), limbPrefix.c_str(), dListOffset);
	string dListStr = dListOffset == 0 ? "NULL" : StringHelper::Sprintf("%s", parent->GetDeclarationName(dListOffset, defaultDLName).c_str());

	if (dListOffset != 0 && parent->GetDeclaration(dListOffset) == nullptr)
	{
		ZDisplayList* dList = new ZDisplayList(rawData, dListOffset, ZDisplayList::GetDListLength(rawData, dListOffset, Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX));
		dList->parent = parent;
		dList->SetName(dListStr);
		dList->GetSourceOutputCode(prefix);
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

	if (reader->Attribute("Type") != nullptr)
	{
		if (string(reader->Attribute("Type")) == "Flex")
			skeletonType = ZSkeletonType::Flex;
		else if (string(reader->Attribute("Type")) == "Skin")
			skeletonType = ZSkeletonType::Skin;
		else if (string(reader->Attribute("Type")) != "Normal")
		{
			// TODO: Print some error here...
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
			skeleton->limbs.push_back(limb);
		}
		else
		{
			ZLimbLOD* limb = ZLimbLOD::FromRawData(nRawData, ptr2);
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
	if (parent != nullptr)
	{
		string defaultPrefix = name.c_str();
		defaultPrefix.replace(0, 1, "s"); // replace g prefix with s for local variables

		for (int i = 0; i < limbs.size(); i++)
		{
			ZLimbStandard* limb = limbs[i];
			uint32_t dListOffset = SEG2FILESPACE(limb->dListPtr);
			if (GETSEGNUM(limb->dListPtr) == 0x80) {
				dListOffset -= SEG2FILESPACE(parent->baseAddress);
			}

			string dListStr = ZLimbStandard::MakeLimbDListSourceOutputCode(defaultPrefix, "", dListOffset, rawData, parent);

			string entryStr = "";

			if (typeid(*limb) == typeid(ZLimbLOD))
			{
				ZLimbLOD* limbLOD = (ZLimbLOD*)limbs[i];
				uint32_t farDListOffset = SEG2FILESPACE(limbLOD->farDListPtr);
				if (GETSEGNUM(limbLOD->farDListPtr) == 0x80) {
					farDListOffset -= SEG2FILESPACE(parent->baseAddress);
				}

				string dListStr2 = ZLimbStandard::MakeLimbDListSourceOutputCode(defaultPrefix, "Far", farDListOffset, rawData, parent);

				entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, { %s, %s }",
					limbLOD->transX, limbLOD->transY, limbLOD->transZ, limbLOD->childIndex, limbLOD->siblingIndex, dListStr.c_str(), dListStr2.c_str());
			}
			else
			{
				entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, %s",
					limb->transX, limb->transY, limb->transZ, limb->childIndex, limb->siblingIndex, dListStr.c_str());
			}

			string limbName = StringHelper::Sprintf("%sLimb_%06X", defaultPrefix.c_str(), limb->fileAddress);

			if (parent->HasDeclaration(limb->fileAddress))
				limbName = parent->GetDeclarationName(limb->fileAddress);

			parent->AddDeclaration(limb->fileAddress, DeclarationAlignment::None, limb->GetRawDataSize(), limb->GetSourceTypeName(), limbName, entryStr);
		}

		// Table
		string tblStr = "";

		for (int i = 0; i < limbs.size(); i++)
		{
			ZLimbStandard* limb = limbs[i];

			//string decl = StringHelper::Sprintf("    &_%sLimb_%04X,\n", prefix.c_str(), limb->fileAddress);
			string decl = "";

			if (parent->HasDeclaration(limb->fileAddress)) {
				decl = StringHelper::Sprintf("    &%s,", parent->GetDeclarationName(limb->fileAddress).c_str());
				if (i != (limbs.size() - 1)) {
				    decl += "\n";
				}
			}

			tblStr += decl;
		}

		uint32_t ptr = (uint32_t)BitConverter::ToInt32BE(rawData, rawDataIndex) & 0x00FFFFFF;

		if (!parent->HasDeclaration(ptr))
		{
			parent->AddDeclarationArray(ptr, DeclarationAlignment::None, 4 * limbs.size(),
				"static void*", StringHelper::Sprintf("%sLimbs", defaultPrefix.c_str()), limbs.size(), tblStr);
		}

		string headerStr;
		if (type == ZSkeletonType::Normal)
		{
			headerStr = StringHelper::Sprintf("%sLimbs, %i", defaultPrefix.c_str(), limbs.size());
		}
		else
		{
			headerStr = StringHelper::Sprintf("%sLimbs, %i, %i", defaultPrefix.c_str(), limbs.size(), dListCount);
		}

		parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align16, GetRawDataSize(),
			GetSourceTypeName(), StringHelper::Sprintf("%s", name.c_str()), headerStr);
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


ZLimbLOD::ZLimbLOD() : ZLimbStandard()
{
	farDListPtr = 0;
}

ZLimbLOD* ZLimbLOD::FromRawData(vector<uint8_t> nRawData, int rawDataIndex)
{
	ZLimbLOD* limb = new ZLimbLOD();

	// ?
	limb->segAddress = rawDataIndex;
	limb->fileAddress = rawDataIndex;

	limb->transX = BitConverter::ToInt16BE(nRawData, rawDataIndex + 0);
	limb->transY = BitConverter::ToInt16BE(nRawData, rawDataIndex + 2);
	limb->transZ = BitConverter::ToInt16BE(nRawData, rawDataIndex + 4);

	limb->childIndex = nRawData[rawDataIndex + 6];
	limb->siblingIndex = nRawData[rawDataIndex + 7];

	limb->dListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 8);
	limb->farDListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 12);

	return limb;
}

int ZLimbLOD::GetRawDataSize()
{
	return 16;
}

string ZLimbLOD::GetSourceOutputCode(const std::string& prefix)
{
	return std::string();
}

std::string ZLimbLOD::GetSourceTypeName()
{
	return "LodLimb";
}
