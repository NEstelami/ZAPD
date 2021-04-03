#include "ZBackground.h"
#include "BitConverter.h"
#include "File.h"
#include "Path.h"
#include "StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Background, ZBackground);

ZBackground::ZBackground(ZFile* nParent) : ZResource(nParent)
{
	// ParseXML(reader);
}

ZBackground::ZBackground(const std::string& prefix, const std::vector<uint8_t>& nRawData,
                       int nRawDataIndex, ZFile* nParent)
	: ZResource(nParent)
{
	name = GetDefaultName(prefix.c_str(), rawDataIndex);
	outName = name;

	ExtractFromXML(nullptr, nRawData, nRawDataIndex, "");
}

void ZBackground::ParseRawData()
{
	ZResource::ParseRawData();

	size_t i = 0;
	while (true)
	{
		uint8_t val = rawData.at(rawDataIndex + i);
		data.push_back(val);

		if (BitConverter::ToUInt16BE(rawData, rawDataIndex + i) == 0xFFD9)
		{
			data.push_back(rawData.at(rawDataIndex + i + 1));
			break;
		}

		i++;
	}
}

void ZBackground::ParseBinaryFile(const std::string& inFolder, bool appendOutName)
{
	fs::path filepath(inFolder);
	if (appendOutName)
	{
		filepath = filepath / (outName + "." + GetExternalExtension());
	}
	data = File::ReadAllBytes(filepath.string());

	// Add padding.
	while (data.size() < GetRawDataSize())
	{
		data.push_back(0x00);
	}
}

void ZBackground::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
                                int nRawDataIndex, const std::string& nRelPath)
{
	ZResource::ExtractFromXML(reader, nRawData, nRawDataIndex, nRelPath);
	DeclareVar("", "");
}

// ZBackground* ZBackground::BuildFromXML(tinyxml2::XMLElement* reader, std::string inFolder,
//                                     ZFile* nParent, bool readFile)
//{
//	ZBackground* back = new ZBackground(reader, nParent);
//
//	back->ParseXML(reader);
//
//	if (readFile)
//		back->ParseBinaryFile(inFolder, true);
//
//	return back;
//}

int ZBackground::GetRawDataSize()
{
	// Jpgs seems to add padding until reaching this size.
	return 0x25800;
}

void ZBackground::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (name == "")
		auxName = GetDefaultName(prefix, rawDataIndex);

	parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align8, GetRawDataSize(),
	                            GetSourceTypeName(), auxName, 0, bodyStr);
}

bool ZBackground::IsExternalResource()
{
	return true;
}

std::string ZBackground::GetExternalExtension()
{
	return "jpg";
}

void ZBackground::Save(const std::string& outFolder)
{
	fs::path folder(outFolder);
	fs::path filepath = folder / (outName + "." + GetExternalExtension());
	File::WriteAllBytes(filepath.string(), data);
}

std::string ZBackground::GetBodySourceCode()
{
	std::string bodyStr = "    ";

	for (size_t i = 0; i < data.size()/8; ++i)
	{
		bodyStr += StringHelper::Sprintf("0x%016llX, ", BitConverter::ToUInt64BE(data, i*8));

		if (i % 8 == 7)
			bodyStr += "\n    ";
	}

	bodyStr += "\n";

	return bodyStr;
}

std::string ZBackground::GetSourceOutputCode(const std::string& prefix)
{
	std::string bodyStr = GetBodySourceCode();

	Declaration* decl = parent->GetDeclaration(rawDataIndex);

	if (decl == nullptr)
		DeclareVar(prefix, bodyStr);
	else
		decl->text = bodyStr;

	return "";
}

std::string ZBackground::GetDefaultName(const std::string& prefix, uint32_t address)
{
	return StringHelper::Sprintf("%sBackground_%06X", prefix.c_str(), address);
}

std::string ZBackground::GetSourceTypeName()
{
	return "u64";
}

ZResourceType ZBackground::GetResourceType()
{
	return ZResourceType::Background;
}
