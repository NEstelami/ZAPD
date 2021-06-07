#include "ZBetaLimb.h"

#include "BitConverter.h"
#include "StringHelper.h"
#include "ZFile.h"


REGISTER_ZFILENODE(BetaLimb, ZBetaLimb);


ZBetaLimb::ZBetaLimb(ZFile* nParent)
: ZResource(nParent)
{
}

void ZBetaLimb::ExtractFromXML(tinyxml2::XMLElement* reader, uint32_t nRawDataIndex)
{
    ZResource::ExtractFromXML(reader, nRawDataIndex);

	parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                            GetSourceTypeName(), name, "");
}

void ZBetaLimb::ParseRawData()
{
    ZResource::ParseRawData();

    const auto& rawData = parent->GetRawData();
    dList = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x00);
    transX = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x04);
    transY = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x08);
    transZ = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x0C);
    rotX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x10);
    rotY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x12);
    rotZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x14);
    child = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x18);
    sibling = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x1C);
}

void ZBetaLimb::DeclareReferences(const std::string& prefix)
{
    std::string varPrefix = prefix;
    if (name != "")
        varPrefix = name;

    ZResource::DeclareReferences(varPrefix);

    // TODO
}

std::string ZBetaLimb::GetBodySourceCode() const
{
    std::string body = "\n";

    std::string dListName = parent->GetDeclarationPtrName(dList);
    std::string childName = parent->GetDeclarationPtrName(child);
    std::string siblingName = parent->GetDeclarationPtrName(sibling);

    body += StringHelper::Sprintf("\t%s,\n", dListName.c_str());
    body += StringHelper::Sprintf("\t{ %ff, %ff, %ff },\n", transX, transY, transZ);
    body += StringHelper::Sprintf("\t{ 0x%04X, 0x%04X, 0x%04X },\n", rotX, rotY, rotZ);
    body += StringHelper::Sprintf("\t%s,\n", childName.c_str());
    body += StringHelper::Sprintf("\t%s\n", siblingName.c_str());

    return body;
}


std::string ZBetaLimb::GetSourceOutputCode(const std::string& prefix)
{
	std::string body = GetBodySourceCode();

	Declaration* decl = parent->GetDeclaration(rawDataIndex);
	if (decl == nullptr || decl->isPlaceholder)
	    parent->AddDeclaration(rawDataIndex, DeclarationAlignment::Align4, GetRawDataSize(),
	                            GetSourceTypeName(), name, body);
	else
		decl->text = body;

	return "";
}

std::string ZBetaLimb::GetSourceTypeName() const
{
    return "BetaLimb";
}

ZResourceType ZBetaLimb::GetResourceType() const
{
    return ZResourceType::Limb;
}

size_t ZBetaLimb::GetRawDataSize() const
{
    return 0x20;
}
