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
	limb->address = limbAddress;

	string entryType = limbType == ZLimbType::LOD ? "LodLimb" : "StandardLimb";

	limb->parent->AddDeclaration(limb->address, DeclarationAlignment::None, 12, entryType, StringHelper::Sprintf("%s", limbName.c_str(), limb->address), "");

	return limb;
}

ZLimbStandard* ZLimbStandard::FromRawData(std::vector<uint8_t> nRawData, int rawDataIndex)
{
	ZLimbStandard* limb = new ZLimbStandard();

	limb->address = rawDataIndex;

	limb->transX = BitConverter::ToInt16BE(nRawData, rawDataIndex + 0);
	limb->transY = BitConverter::ToInt16BE(nRawData, rawDataIndex + 2);
	limb->transZ = BitConverter::ToInt16BE(nRawData, rawDataIndex + 4);

	limb->childIndex = nRawData[rawDataIndex + 6];
	limb->siblingIndex = nRawData[rawDataIndex + 7];

	limb->dListPtr = BitConverter::ToInt32BE(nRawData, rawDataIndex + 8) & 0x00FFFFFF;

	return limb;
}

int ZLimbStandard::GetRawDataSize()
{
	return 12;
}

string ZLimbStandard::GetSourceOutputCode(const std::string& prefix)
{
	string dListStr = dListPtr == 0 ? "NULL" : StringHelper::Sprintf("%s", parent->GetVarName(dListPtr).c_str());

	string entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, %s",
		transX, transY, transZ, childIndex, siblingIndex, dListStr.c_str());

	Declaration* decl = parent->GetDeclaration(address);
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

std::string ZLimbStandard::MakeLimbDListSourceOutputCode(const std::string& prefix, const std::string& limbPrefix, uint32_t dListPtr, const std::vector<uint8_t> &rawData, ZFile* parent)
{
	string defaultDLName = StringHelper::Sprintf("%s%sLimbDL_%06X", prefix.c_str(), limbPrefix.c_str(), dListPtr);
	string dListStr = dListPtr == 0 ? "NULL" : StringHelper::Sprintf("%s", parent->GetDeclarationName(dListPtr, defaultDLName).c_str());

	if (dListPtr != 0 && parent->GetDeclaration(dListPtr) == nullptr)
	{
		ZDisplayList* dList = new ZDisplayList(rawData, dListPtr, ZDisplayList::GetDListLength(rawData, dListPtr, Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX));
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

std::string ZSkeleton::GetSourceOutputCode(const std::string& prefix)
{
	if (parent != nullptr)
	{
		string defaultPrefix = name.c_str();
		defaultPrefix.replace(0, 1, "s"); // replace g prefix with s for local variables

		for (int i = 0; i < limbs.size(); i++)
		{
			ZLimbStandard* limb = limbs[i];

			string dListStr = ZLimbStandard::MakeLimbDListSourceOutputCode(defaultPrefix, "", limb->dListPtr, rawData, parent);

			string entryStr = "";

			if (typeid(*limb) == typeid(ZLimbLOD))
			{
				ZLimbLOD* limbLOD = (ZLimbLOD*)limbs[i];
				string dListStr2 = ZLimbStandard::MakeLimbDListSourceOutputCode(defaultPrefix, "Far", limbLOD->farDListPtr, rawData, parent);

				entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, { %s, %s }",
					limbLOD->transX, limbLOD->transY, limbLOD->transZ, limbLOD->childIndex, limbLOD->siblingIndex, dListStr.c_str(), dListStr2.c_str());
			}
			else
			{
				entryStr = StringHelper::Sprintf("{ %i, %i, %i }, %i, %i, %s",
					limb->transX, limb->transY, limb->transZ, limb->childIndex, limb->siblingIndex, dListStr.c_str());
			}

			string limbName = StringHelper::Sprintf("%sLimb_%06X", defaultPrefix.c_str(), limb->address);

			if (parent->HasDeclaration(limb->address))
				limbName = parent->GetDeclarationName(limb->address);

			parent->AddDeclaration(limb->address, DeclarationAlignment::None, limb->GetRawDataSize(), limb->GetSourceTypeName(), limbName, entryStr);
		}

		// Table
		string tblStr = "";

		for (int i = 0; i < limbs.size(); i++)
		{
			ZLimbStandard* limb = limbs[i];

			//string decl = StringHelper::Sprintf("    &_%sLimb_%04X,\n", prefix.c_str(), limb->address);
			string decl = "";

			if (parent->HasDeclaration(limb->address)) {
				decl = StringHelper::Sprintf("    &%s,", parent->GetDeclarationName(limb->address).c_str());
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

		if (type == ZSkeletonType::Normal)
		{
			string headerStr = StringHelper::Sprintf("%sLimbs, %i", defaultPrefix.c_str(), limbs.size());
			parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align16, 8,
				GetSourceTypeName(), StringHelper::Sprintf("%s", name.c_str()), headerStr);
		}
		else
		{
			string headerStr = StringHelper::Sprintf("%sLimbs, %i, %i", defaultPrefix.c_str(), limbs.size(), dListCount);
			parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align16, 12,
				GetSourceTypeName(), StringHelper::Sprintf("%s", name.c_str()), headerStr);
		}
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
