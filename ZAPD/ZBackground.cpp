#include "ZBackground.h"
#include "BitConverter.h"
#include "StringHelper.h"
#include "ZFile.h"
#include <filesystem>
#include "File.h"


ZBackground::ZBackground(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, const std::string& nRelPath, ZFile* nParent)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;
	parent = nParent;
    relativePath = nRelPath;

	ParseXML(reader);
	ParseRawData();
}

ZBackground::ZBackground(const std::string& prefix, const std::vector<uint8_t>& nRawData, int nRawDataIndex, const std::string& nRelPath, ZFile* nParent)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;
	parent = nParent;
    relativePath = nRelPath;

	name = GetDefaultName(prefix.c_str(), rawDataIndex);

	ParseRawData();
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

ZBackground* ZBackground::ExtractFromXML(tinyxml2::XMLElement* reader,
            const std::vector<uint8_t>& nRawData, int nRawDataIndex, const std::string& nRelPat, ZFile* nParent)
{
	ZBackground* mtx = new ZBackground(reader, nRawData, nRawDataIndex, nRelPat, nParent);

    mtx->DeclareVar("", "");

	return mtx;
}

int ZBackground::GetRawDataSize()
{
    return data.size();
}

void ZBackground::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
    std::string auxName = name;
    if (name == "")
    {
        auxName = GetDefaultName(prefix, rawDataIndex);
    }
    /*parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::Align8, GetRawDataSize(),
                            GetSourceTypeName(), auxName, 0, bodyStr);
    parent->AddDeclarationIncludeArray(rawDataIndex,
        StringHelper::Sprintf("%s/%s.%s.inc.c", outputDir.c_str(),
                                Path::GetFileNameWithoutExtension(res->GetOutName()).c_str(),
                                res->GetExternalExtension().c_str()),
        res->GetRawDataSize(), declType, res->GetName(), 0);*/
}

bool ZBackground::IsExternalResource()
{
    return true;
}

void ZBackground::Save(const std::string& outFolder)
{
    std::filesystem::path folder(outFolder);
    std::filesystem::path filepath = folder / (outName + ".jfif");
	File::WriteAllBytes(filepath, data);
}

std::string ZBackground::GetBodySourceCode()
{
	std::string bodyStr = "\n";
    /*
    for (const auto& val: data)
    {
        // TODO
    }
    */
    return bodyStr;
}

std::string ZBackground::GetSourceOutputCode(const std::string& prefix)
{
    std::string bodyStr = GetBodySourceCode();

	Declaration* decl = parent->GetDeclaration(rawDataIndex);
	if (decl == nullptr)
	{
        DeclareVar(prefix, bodyStr);
	}
	else
	{
		decl->text = bodyStr;
	}

    return "";
}

std::string ZBackground::GetDefaultName(const std::string& prefix, uint32_t address)
{
    // TODO: change Jfif, probably
    return StringHelper::Sprintf("%sJfif_%06X", prefix.c_str(), address);
}

std::string ZBackground::GetSourceTypeName()
{
    return "u8";
}

ZResourceType ZBackground::GetResourceType()
{
    return ZResourceType::Background;
}
