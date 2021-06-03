#include "ZSkeleton.h"

#include "BitConverter.h"
#include "Globals.h"
#include "HighLevel/HLModelIntermediette.h"
#include "StringHelper.h"

REGISTER_ZFILENODE(Skeleton, ZSkeleton);

ZSkeleton::ZSkeleton(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("Type");
	RegisterRequiredAttribute("LimbType");
}

void ZSkeleton::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string skelTypeXml = registeredAttributes.at("Type").value;

	if (skelTypeXml == "Flex")
		type = ZSkeletonType::Flex;
	else if (skelTypeXml == "Curve")
		type = ZSkeletonType::Curve;
	else if (skelTypeXml != "Normal")
	{
		throw std::runtime_error(StringHelper::Sprintf(
		        "ZSkeleton::ParseXML: Error in '%s'.\n"
		        "\t Invalid Type found: '%s'.\n"
		        "\t Defaulting to 'Normal'.\n",
		        name.c_str(), skelTypeXml.c_str()));
		type = ZSkeletonType::Normal;
	}

	std::string limbTypeXml = registeredAttributes.at("LimbType").value;

	if (limbTypeXml == "Standard")
		limbType = ZLimbType::Standard;
	else if (limbTypeXml == "LOD")
		limbType = ZLimbType::LOD;
	else if (limbTypeXml == "Skin")
		limbType = ZLimbType::Skin;
	else if (limbTypeXml == "Curve")
		limbType = ZLimbType::Curve;
	else
	{
		throw std::runtime_error(StringHelper::Sprintf(
		        "ZSkeleton::ParseXML: Error in '%s'.\n"
		        "\t Invalid LimbType found: '%s'.\n"
		        "\t Defaulting to 'Standard'.\n",
		        name.c_str(), limbTypeXml.c_str()));
		limbType = ZLimbType::Standard;
	}
}

void ZSkeleton::ParseRawData()
{
	ZResource::ParseRawData();

	limbsArrayAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex);
	limbCount = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4);
	dListCount = BitConverter::ToUInt8BE(rawData, rawDataIndex + 8);
}

void ZSkeleton::DeclareReferences(const std::string& prefix)
{
	std::string defaultPrefix = name;
	if (defaultPrefix == "")
		defaultPrefix = prefix;

	// replace g prefix with s for local variables
	if (defaultPrefix.at(0) == 'g')
		defaultPrefix.replace(0, 1, "s");

	uint32_t ptr = Seg2Filespace(limbsArrayAddress, parent->baseAddress);
	for (size_t i = 0; i < limbCount; i++)
	{
		segptr_t limbAddress = BitConverter::ToUInt32BE(rawData, ptr);
		uint32_t limbOffset = Seg2Filespace(limbAddress, parent->baseAddress);

		limbsTblAddresses.push_back(limbAddress);

		Declaration* decl = parent->GetDeclaration(limbOffset);
		if (decl == nullptr)
		{
			std::string limbName = StringHelper::Sprintf("%sLimb_%06X", defaultPrefix.c_str(), limbOffset);

			ZLimb* limb = new ZLimb(parent);
			limb->SetLimbType(limbType);
			limb->SetName(limbName);
			limb->ExtractFromFile(rawData, limbOffset);
			limb->DeclareVar(defaultPrefix, "");
			parent->AddResource(limb);
		}

		ptr += 4;
	}

	// Limbs ptr array / table
	ptr = Seg2Filespace(limbsArrayAddress, parent->baseAddress);
	if (!parent->HasDeclaration(ptr))
	{
		// Table
		std::string tblStr = "";
		std::string limbArrTypeStr = "static void*";
		if (limbType == ZLimbType::Curve)
		{
			limbArrTypeStr =
				StringHelper::Sprintf("static %s*", ZLimb::GetSourceTypeName(limbType));
		}

		for (size_t i = 0; i < limbsTblAddresses.size(); i++)
		{
			segptr_t limbAddress = limbsTblAddresses.at(i);

			std::string decl;
			Globals::Instance->GetSegmentedPtrName(limbAddress, parent, decl);

			tblStr += StringHelper::Sprintf("\t%s,", decl.c_str());

			if (i + 1 < limbCount)
				tblStr += "\n";
		}

		parent->AddDeclarationArray(ptr, DeclarationAlignment::Align4, 4 * limbCount, limbArrTypeStr,
		                            StringHelper::Sprintf("%sLimbs", defaultPrefix.c_str()),
		                            limbCount, tblStr);
	}
}

std::string ZSkeleton::GetBodySourceCode() const
{
	std::string limbArrayName;
	Globals::Instance->GetSegmentedPtrName(limbsArrayAddress, parent, limbArrayName);

	switch (type)
	{
	case ZSkeletonType::Normal:
	case ZSkeletonType::Curve:
		return StringHelper::Sprintf("\n\t%s, %i\n", limbArrayName.c_str(), limbCount);

	case ZSkeletonType::Flex:
		return StringHelper::Sprintf("\n\t{ %s, %i }, %i\n", limbArrayName.c_str(),
		                                  limbCount, dListCount);
	}

	// TODO: Throw exception?
	return "ERROR";
}

void ZSkeleton::GenerateHLIntermediette(HLFileIntermediette& hlFile)
{
	HLModelIntermediette* mdl = (HLModelIntermediette*)&hlFile;
	HLModelIntermediette::FromZSkeleton(mdl, this);
	mdl->blocks.push_back(new HLTerminator());
}

std::string ZSkeleton::GetSourceTypeName() const
{
	switch (type)
	{
	case ZSkeletonType::Normal:
		return "SkeletonHeader";
	case ZSkeletonType::Flex:
		return "FlexSkeletonHeader";
	case ZSkeletonType::Curve:
		return "SkelCurveLimbList";
	}

	return "SkeletonHeader";
}

ZResourceType ZSkeleton::GetResourceType() const
{
	return ZResourceType::Skeleton;
}

size_t ZSkeleton::GetRawDataSize() const
{
	switch (type)
	{
	case ZSkeletonType::Flex:
		return 0xC;
	case ZSkeletonType::Normal:
	case ZSkeletonType::Curve:
	default:
		return 0x8;
	}
}

DeclarationAlignment ZSkeleton::GetDeclarationAlignment() const 
{
	return DeclarationAlignment::Align16;
}

segptr_t ZSkeleton::GetAddress()
{
	return rawDataIndex;
}

uint8_t ZSkeleton::GetLimbCount()
{
	return limbCount;
}
