#include "ZMtx.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Mtx, ZMtx);

ZMtx::ZMtx(ZFile* nParent) : ZResource(nParent)
{
}

ZMtx::ZMtx(const std::string& prefix, const std::vector<uint8_t>& nRawData, uint32_t nRawDataIndex,
           ZFile* nParent)
	: ZResource(nParent)
{
	name = GetDefaultName(prefix.c_str());
	ExtractFromFile(nRawData, nRawDataIndex);
	DeclareVar("", "");
}

void ZMtx::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
                          uint32_t nRawDataIndex)
{
	ZResource::ExtractFromXML(reader, nRawData, nRawDataIndex);
	DeclareVar("", "");
}

void ZMtx::ParseRawData()
{
	ZResource::ParseRawData();

	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 4; ++j)
			mtx[i][j] = BitConverter::ToInt32BE(rawData, rawDataIndex + (i * 4 + j) * 4);
}

void ZMtx::DeclareVar(const std::string& prefix, const std::string& bodyStr) const
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix);

	parent->AddDeclaration(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                       GetSourceTypeName(), auxName, bodyStr);
}

std::string ZMtx::GetBodySourceCode()
{
	std::string bodyStr = "\n";

	for (const auto& row : mtx)
	{
		bodyStr += "    ";

		for (int32_t val : row)
			bodyStr += StringHelper::Sprintf("%-11i, ", val);

		bodyStr += "\n";
	}

	return bodyStr;
}

std::string ZMtx::GetSourceOutputCode(const std::string& prefix)
{
	std::string bodyStr = GetBodySourceCode();

	Declaration* decl = parent->GetDeclaration(rawDataIndex);

	if (decl == nullptr)
		DeclareVar(prefix, bodyStr);
	else
		decl->text = bodyStr;

	return "";
}

std::string ZMtx::GetSourceTypeName() const
{
	return "Mtx";
}

ZResourceType ZMtx::GetResourceType() const
{
	return ZResourceType::Mtx;
}

size_t ZMtx::GetRawDataSize() const
{
	return 64;
}

DeclarationAlignment ZMtx::GetDeclarationAlignment() const
{
	return DeclarationAlignment::Align8;
}
