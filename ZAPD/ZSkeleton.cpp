#include "ZSkeleton.h"

#include <cassert>

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

REGISTER_ZFILENODE(Skeleton, ZSkeleton);
REGISTER_ZFILENODE(LimbTable, ZLimbTable);

ZSkeleton::ZSkeleton(ZFile* nParent) : ZResource(nParent), limbsTable(nParent)
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
		fprintf(stderr,
		        "ZSkeleton::ParseXML: Warning in '%s'.\n"
		        "\t Invalid Type found: '%s'.\n"
		        "\t Defaulting to 'Normal'.\n",
		        name.c_str(), skelTypeXml.c_str());
		type = ZSkeletonType::Normal;
	}

	std::string limbTypeXml = registeredAttributes.at("LimbType").value;
	limbType = ZLimb::GetTypeByAttributeName(limbTypeXml);
	if (limbType == ZLimbType::Invalid)
	{
		throw std::runtime_error(StringHelper::Sprintf("ZSkeleton::ParseXML: Error in '%s'.\n"
		                                               "\t Invalid LimbType found: '%s'.\n",
		                                               name.c_str(), limbTypeXml.c_str()));
	}
}

void ZSkeleton::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();
	limbsArrayAddress = BitConverter::ToUInt32BE(rawData, rawDataIndex);
	limbCount = BitConverter::ToUInt8BE(rawData, rawDataIndex + 4);
	dListCount = BitConverter::ToUInt8BE(rawData, rawDataIndex + 8);

	if (limbsArrayAddress != 0 && GETSEGNUM(limbsArrayAddress) == parent->segment)
	{
		uint32_t ptr = Seg2Filespace(limbsArrayAddress, parent->baseAddress);
		limbsTable.ExtractFromBinary(ptr, limbType, limbCount);
	}
}

void ZSkeleton::DeclareReferences(const std::string& prefix)
{
	std::string defaultPrefix = name;
	if (defaultPrefix == "")
		defaultPrefix = prefix;

	ZResource::DeclareReferences(defaultPrefix);

	if (limbsArrayAddress != 0 && GETSEGNUM(limbsArrayAddress) == parent->segment)
	{
		uint32_t ptr = Seg2Filespace(limbsArrayAddress, parent->baseAddress);
		if (!parent->HasDeclaration(ptr))
		{
			limbsTable.SetName(StringHelper::Sprintf("%sLimbs", defaultPrefix.c_str()));
			limbsTable.DeclareReferences(prefix);
			limbsTable.GetSourceOutputCode(prefix);
		}
	}
}

std::string ZSkeleton::GetBodySourceCode() const
{
	std::string limbTableName = parent->GetDeclarationPtrName(limbsArrayAddress);

	std::string headerStr;
	switch (type)
	{
	case ZSkeletonType::Normal:
	case ZSkeletonType::Curve:
		headerStr = StringHelper::Sprintf("\n\t%s, %i\n", limbTableName.c_str(), limbCount);
		break;
	case ZSkeletonType::Flex:
		headerStr = StringHelper::Sprintf("\n\t{ %s, %i }, %i\n", limbTableName.c_str(), limbCount,
		                                  dListCount);
		break;
	}

	return headerStr;
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

/* ZLimbTable */

ZLimbTable::ZLimbTable(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("LimbType");
	RegisterRequiredAttribute("Count");
}

void ZLimbTable::ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex)
{
	ZResource::ExtractFromXML(reader, nRawDataIndex);

	parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                            GetSourceTypeName(), name, limbsAddresses.size(), "");
}

void ZLimbTable::ExtractFromBinary(uint32_t nRawDataIndex, ZLimbType nLimbType, size_t nCount)
{
	rawDataIndex = nRawDataIndex;
	limbType = nLimbType;
	count = nCount;

	ParseRawData();
}

void ZLimbTable::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string limbTypeXml = registeredAttributes.at("LimbType").value;
	limbType = ZLimb::GetTypeByAttributeName(limbTypeXml);
	if (limbType == ZLimbType::Invalid)
	{
		fprintf(stderr,
		        "ZLimbTable::ParseXML: Warning in '%s'.\n"
		        "\t Invalid LimbType found: '%s'.\n"
		        "\t Defaulting to 'Standard'.\n",
		        name.c_str(), limbTypeXml.c_str());
		limbType = ZLimbType::Standard;
	}

	count = StringHelper::StrToL(registeredAttributes.at("Count").value);
}

void ZLimbTable::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();
	uint32_t ptr = rawDataIndex;
	for (size_t i = 0; i < count; i++)
	{
		limbsAddresses.push_back(BitConverter::ToUInt32BE(rawData, ptr));
		ptr += 4;
	}
}

void ZLimbTable::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = name;
	if (varPrefix == "")
		varPrefix = prefix;

	ZResource::DeclareReferences(varPrefix);

	for (size_t i = 0; i < count; i++)
	{
		segptr_t limbAddress = limbsAddresses[i];

		if (limbAddress != 0 && GETSEGNUM(limbAddress) == parent->segment)
		{
			uint32_t limbOffset = Seg2Filespace(limbAddress, parent->baseAddress);
			if (!parent->HasDeclaration(limbOffset))
			{
				ZLimb* limb = new ZLimb(parent);
				limb->ExtractFromBinary(limbOffset, limbType);
				limb->SetName(limb->GetDefaultName(varPrefix));
				limb->DeclareVar(varPrefix, "");
				limb->DeclareReferences(varPrefix);
				parent->AddResource(limb);
			}
		}
	}
}

Declaration* ZLimbTable::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	return parent->AddDeclarationArray(
		rawDataIndex, GetDeclarationAlignment(), GetDeclarationPadding(), GetRawDataSize(),
		GetSourceTypeName(), auxName, limbsAddresses.size(), bodyStr);
}

std::string ZLimbTable::GetBodySourceCode() const
{
	std::string body = "";

	for (size_t i = 0; i < count; i++)
	{
		std::string limbName = parent->GetDeclarationPtrName(limbsAddresses[i]);
		body += StringHelper::Sprintf("\t%s,", limbName.c_str());

		if (i + 1 < count)
			body += "\n";
	}

	return body;
}

std::string ZLimbTable::GetSourceTypeName() const
{
	switch (limbType)
	{
	case ZLimbType::Standard:
	case ZLimbType::LOD:
	case ZLimbType::Skin:
		return "void*";

	case ZLimbType::Curve:
	case ZLimbType::Legacy:
		return StringHelper::Sprintf("%s*", ZLimb::GetSourceTypeName(limbType));

	case ZLimbType::Invalid:
		assert("Invalid limb type.\n");
	}

	return "ERROR";
}

ZResourceType ZLimbTable::GetResourceType() const
{
	return ZResourceType::LimbTable;
}

size_t ZLimbTable::GetRawDataSize() const
{
	return 4 * limbsAddresses.size();
}
