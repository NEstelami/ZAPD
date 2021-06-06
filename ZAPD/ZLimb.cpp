#include "ZLimb.h"

#include <cassert>
#include "BitConverter.h"
#include "Globals.h"
#include "StringHelper.h"

REGISTER_ZFILENODE(Limb, ZLimb);

ZLimb::ZLimb(ZFile* nParent) : ZResource(nParent), segmentStruct(nParent)
{
	RegisterOptionalAttribute("LimbType");
	RegisterOptionalAttribute("Type");
}

void ZLimb::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	// Reading from a <Skeleton/>
	std::string limbType = registeredAttributes.at("LimbType").value;
	if (limbType == "")  // Reading from a <Limb/>
		limbType = registeredAttributes.at("Type").value;

	if (limbType == "")
	{
		fprintf(stderr,
		        "ZLimb::ParseXML: Warning in '%s'.\n"
		        "\t Missing 'LimbType' attribute in xml.\n"
		        "\t Defaulting to 'Standard'.\n",
		        name.c_str());
		type = ZLimbType::Standard;
	}
	else
	{
		if (limbType == "Standard")
		{
			type = ZLimbType::Standard;
		}
		else if (limbType == "LOD")
		{
			type = ZLimbType::LOD;
		}
		else if (limbType == "Skin")
		{
			type = ZLimbType::Skin;
		}
		else if (limbType == "Curve")
		{
			type = ZLimbType::Curve;
		}
		else
		{
			fprintf(stderr,
			        "ZLimb::ParseXML: Warning in '%s'.\n"
			        "\t Invalid LimbType found: '%s'.\n"
			        "\t Defaulting to 'Standard'.\n",
			        name.c_str(), limbType.c_str());
			type = ZLimbType::Standard;
		}
	}
}

void ZLimb::ParseRawData()
{
	ZResource::ParseRawData();
	const auto& rawData = parent->GetRawData();

	if (type == ZLimbType::Curve)
	{
		childIndex = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0);
		siblingIndex = BitConverter::ToUInt8BE(rawData, rawDataIndex + 1);

		dListPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
		dList2Ptr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
		return;
	}

	transX = BitConverter::ToInt16BE(rawData, rawDataIndex + 0);
	transY = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	transZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 4);

	childIndex = rawData.at(rawDataIndex + 6);
	siblingIndex = rawData.at(rawDataIndex + 7);

	switch (type)
	{
	case ZLimbType::LOD:
		dList2Ptr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
		// Intended fallthrough
	case ZLimbType::Standard:
		dListPtr = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
		break;

	case ZLimbType::Skin:
		skinSegmentType =
			static_cast<ZLimbSkinType>(BitConverter::ToInt32BE(rawData, rawDataIndex + 8));
		skinSegment = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
		if (skinSegmentType == ZLimbSkinType::SkinType_4)
		{
			if (skinSegment != 0 && GETSEGNUM(skinSegment) == parent->segment)
			{
				uint32_t skinSegmentOffset = Seg2Filespace(skinSegment, parent->baseAddress);
				segmentStruct.ExtractFromFile(skinSegmentOffset);
			}
		}
		break;

	default:
		throw std::runtime_error("Invalid ZLimb type");
		break;
	}
}

void ZLimb::DeclareReferences(const std::string& prefix)
{
	std::string varPrefix = prefix;
	if (name != "")
		varPrefix = name;

	ZResource::DeclareReferences(varPrefix);

	std::string suffix;
	switch (type)
	{
	case ZLimbType::Curve:
	case ZLimbType::LOD:
		suffix = "Far";
		if (type == ZLimbType::Curve)
			suffix = "Curve2";
		DeclareDList(dList2Ptr, varPrefix, suffix);
		// Intended fallthrough
	case ZLimbType::Standard:
		suffix = "";
		if (type == ZLimbType::Curve)
			suffix = "Curve";
		DeclareDList(dListPtr, varPrefix, suffix);
		break;

	case ZLimbType::Skin:
		switch (skinSegmentType)
		{
		case ZLimbSkinType::SkinType_4:
			if (skinSegment != 0 && GETSEGNUM(skinSegment) == parent->segment)
			{
				segmentStruct.DeclareReferences(varPrefix);
				segmentStruct.GetSourceOutputCode(varPrefix);
			}
			break;

		case ZLimbSkinType::SkinType_DList:
			DeclareDList(skinSegment, varPrefix, "");
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
}

size_t ZLimb::GetRawDataSize() const
{
	switch (type)
	{
	case ZLimbType::Standard:
	case ZLimbType::Curve:
		return 0x0C;
	case ZLimbType::LOD:
	case ZLimbType::Skin:
		return 0x10;
	}

	return 0x0C;
}

std::string ZLimb::GetSourceOutputCode(const std::string& prefix)
{
	std::string dListStr = GetLimbDListSourceOutputCode(dListPtr);
	std::string dListStr2 = GetLimbDListSourceOutputCode(dList2Ptr);

	std::string entryStr = "\n\t";
	if (type != ZLimbType::Curve)
	{
		entryStr += StringHelper::Sprintf("{ %i, %i, %i }, ", transX, transY, transZ);
	}

	entryStr += StringHelper::Sprintf("0x%02X, 0x%02X,\n", childIndex, siblingIndex);

	switch (type)
	{
	case ZLimbType::Standard:
		entryStr += StringHelper::Sprintf("\t%s\n", dListStr.c_str());
		break;
	case ZLimbType::LOD:
	case ZLimbType::Curve:
		entryStr += StringHelper::Sprintf("\t{ %s, %s }\n", dListStr.c_str(), dListStr2.c_str());
		break;
	case ZLimbType::Skin:
	{
		std::string skinSegmentStr;
		Globals::Instance->GetSegmentedPtrName(skinSegment, parent, skinSegmentStr);
		entryStr +=
			StringHelper::Sprintf("\t0x%02X, %s\n", skinSegmentType, skinSegmentStr.c_str());
	}
	break;
	}

	Declaration* decl = parent->GetDeclaration(GetFileAddress());

	if (decl == nullptr)
		parent->AddDeclaration(GetFileAddress(), GetDeclarationAlignment(), GetRawDataSize(),
		                       GetSourceTypeName(), name, entryStr);
	else
		decl->text = entryStr;

	return "";
}

std::string ZLimb::GetSourceTypeName() const
{
	return GetSourceTypeName(type);
}

ZResourceType ZLimb::GetResourceType() const
{
	return ZResourceType::Limb;
}

ZLimbType ZLimb::GetLimbType()
{
	return type;
}

void ZLimb::SetLimbType(ZLimbType value)
{
	type = value;
}

const char* ZLimb::GetSourceTypeName(ZLimbType limbType)
{
	switch (limbType)
	{
	case ZLimbType::Standard:
		return "StandardLimb";
	case ZLimbType::LOD:
		return "LodLimb";
	case ZLimbType::Skin:
		return "SkinLimb";
	case ZLimbType::Curve:
		return "SkelCurveLimb";
	default:
		return "StandardLimb";
	}
}

uint32_t ZLimb::GetFileAddress()
{
	return Seg2Filespace(rawDataIndex, parent->baseAddress);
}

void ZLimb::DeclareDList(segptr_t dListSegmentedPtr, const std::string& prefix,
                         const std::string& limbSuffix)
{
	if (dListSegmentedPtr == 0 || GETSEGNUM(dListSegmentedPtr) != parent->segment)
		return;

	uint32_t dlistOffset = Seg2Filespace(dListSegmentedPtr, parent->baseAddress);
	if (parent->HasDeclaration(dlistOffset))
		return;

	if (dlistOffset >= parent->rangeEnd || dlistOffset >= parent->GetRawData().size())
		return;

	int32_t dlistLength = ZDisplayList::GetDListLength(
		parent->GetRawData(), dlistOffset,
		Globals::Instance->game == ZGame::OOT_SW97 ? DListType::F3DEX : DListType::F3DZEX);
	ZDisplayList* dlist = new ZDisplayList(parent);
	dlist->ExtractFromBinary(dlistOffset, dlistLength);

	std::string dListStr = StringHelper::Sprintf("%s%sDL", prefix.c_str(), limbSuffix.c_str());
	dlist->SetName(dListStr);
	dlist->DeclareVar(prefix, "");
	parent->AddResource(dlist);
}

// Returns the ptrname of a dlist.
std::string ZLimb::GetLimbDListSourceOutputCode(segptr_t dListPtr)
{
	if (dListPtr == 0)
		return "NULL";

	uint32_t dListOffset = Seg2Filespace(dListPtr, parent->baseAddress);

	// Check if it is already declared
	Declaration* decl = parent->GetDeclaration(dListOffset);
	if (decl != nullptr)
		return decl->varName;

	// Check if it points to the middle of a DList
	decl = parent->GetDeclarationRanged(dListOffset);
	if (decl != nullptr)
	{
		// TODO: Figure out a way to not hardcode the "Gfx" type.
		if (decl->varType == "Gfx")
		{
			uint32_t declAddress = parent->GetDeclarationRangedAddress(dListOffset);
			if (dListOffset < declAddress + decl->size)
			{
				uint32_t index = (dListOffset - declAddress) / 8;
				return StringHelper::Sprintf("&%s[%u]", decl->varName.c_str(), index);
			}
		}
	}

	return StringHelper::Sprintf("0x%08X", dListPtr);
}
