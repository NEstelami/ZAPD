#include "ZResource.h"

#include <regex>
#include "StringHelper.h"
#include "ZFile.h"

using namespace std;

ZResource::ZResource(ZFile* nParent)
{
	parent = nParent;
	name = "";
	outName = "";
	relativePath = "";
	sourceOutput = "";
	rawData = vector<uint8_t>();
	rawDataIndex = 0;
	outputDeclaration = true;

	RegisterRequiredAttribute("Name");
	RegisterOptionalAttribute("OutName");
	RegisterOptionalAttribute("Offset");
	RegisterNonValueAttribute("Custom");
}

void ZResource::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
                               const uint32_t nRawDataIndex, const std::string& nRelPath)
{
	rawData = nRawData;
	rawDataIndex = nRawDataIndex;
	relativePath = nRelPath;

	if (reader != nullptr)
		ParseXML(reader);

	ParseRawData();
}

void ZResource::ExtractFromFile(const std::vector<uint8_t>& nRawData, uint32_t nRawDataIndex,
                                const std::string& nRelPath)
{
	rawData = nRawData;
	rawDataIndex = nRawDataIndex;
	relativePath = nRelPath;

	ParseRawData();
}

void ZResource::ParseXML(tinyxml2::XMLElement* reader)
{
	if (reader != nullptr)
	{
		auto attrs = reader->FirstAttribute();
		while (attrs != nullptr)
		{
			std::string attrName = attrs->Name();
			bool attrDeclared = false;

			if (requiredAttributes.find(attrName) != requiredAttributes.end())
			{
				requiredAttributes[attrName] = attrs->Value();
				attrDeclared = true;
			}
			if (optionalAttributes.find(attrName) != optionalAttributes.end())
			{
				optionalAttributes[attrName] = attrs->Value();
				attrDeclared = true;
			}
			if (nonValueAttributes.find(attrName) != nonValueAttributes.end())
			{
				nonValueAttributes[attrName] = true;
				attrDeclared = true;
			}

			if (!attrDeclared)
				throw std::runtime_error(StringHelper::Sprintf(
					"ZResource::ParseXML: Fatal error while parsing '%s'.\n"
					"\t Unexpected '%s' attribute in resource '%s'.\n"
					"\t Aborting...", parent->GetName().c_str(), 
					attrName.c_str(), reader->Name()));
			attrs = attrs->Next();
		}

		if (!canHaveInner && !reader->NoChildren())
		{
			throw std::runtime_error(
				StringHelper::Sprintf("ZResource::ParseXML: Fatal error in '%s'.\n\t Resource '%s' "
			                          "with inner element/child detected.\n",
			                          name.c_str(), reader->Name()));
		}

		for (const auto& attr : requiredAttributes)
		{
			if (attr.second == "")
				throw std::runtime_error(StringHelper::Sprintf(
					"ZResource::ParseXML: Fatal error while parsing '%s'.\n"
					"\t Missing required attribute '%s' in resource '%s'.\n"
					"\t Aborting...", parent->GetName().c_str(), 
					attr.first.c_str(), reader->Name()));
		}

		name = requiredAttributes.at("Name");
		static std::regex r("[a-zA-Z_]+[a-zA-Z0-9_]*",
							std::regex::icase | std::regex::optimize);

		if (!std::regex_match(name, r))
		{
			throw std::domain_error(
				StringHelper::Sprintf("ZResource::ParseXML: Fatal error in '%s'.\n\t Resource "
										"with invalid 'Name' attribute.\n",
										name.c_str()));
		}

		outName = optionalAttributes.at("OutName");
		if (outName == "")
			outName = name;

		isCustomAsset = nonValueAttributes["Custom"];
	}
}

void ZResource::Save(const std::string& outFolder)
{
}

void ZResource::PreGenSourceFiles()
{
}

string ZResource::GetName()
{
	return name;
}

std::string ZResource::GetOutName()
{
	return outName;
}

void ZResource::SetOutName(const std::string& nName)
{
	outName = nName;
}

void ZResource::SetName(const std::string& nName)
{
	name = nName;
}

bool ZResource::IsExternalResource()
{
	return false;
}

bool ZResource::DoesSupportArray()
{
	return false;
}

std::string ZResource::GetExternalExtension()
{
	return "";
}

string ZResource::GetRelativePath()
{
	return relativePath;
}

vector<uint8_t> ZResource::GetRawData()
{
	return rawData;
}

void ZResource::SetRawData(std::vector<uint8_t> nData)
{
	rawData = nData;
}

uint32_t ZResource::GetRawDataIndex()
{
	return rawDataIndex;
}

void ZResource::SetRawDataIndex(uint32_t value)
{
	rawDataIndex = value;
}

std::string ZResource::GetBodySourceCode() const
{
	return "ERROR";
}

string ZResource::GetSourceOutputCode(const std::string& prefix)
{
	return "";
}

string ZResource::GetSourceOutputHeader(const std::string& prefix)
{
	return "";
}

void ZResource::ParseRawData()
{
}

void ZResource::GenerateHLIntermediette(HLFileIntermediette& hlFile)
{
}

std::string ZResource::GetSourceTypeName()
{
	return "u8";
}

ZResourceType ZResource::GetResourceType() const
{
	return ZResourceType::Error;
}

void ZResource::CalcHash()
{
	hash = 0;
}

void ZResource::RegisterRequiredAttribute(const std::string& attr)
{
	requiredAttributes[attr] = "";
}

void ZResource::RegisterOptionalAttribute(const std::string& attr, const std::string& defaultValue)
{
	optionalAttributes[attr] = defaultValue;
}

void ZResource::RegisterNonValueAttribute(const std::string& attr)
{
	nonValueAttributes[attr] = false;
}

uint32_t Seg2Filespace(segptr_t segmentedAddress, uint32_t parentBaseAddress)
{
	uint32_t currentPtr = GETSEGOFFSET(segmentedAddress);

	if (GETSEGNUM(segmentedAddress) == 0x80)  // Is defined in code?
		currentPtr -= GETSEGOFFSET(parentBaseAddress);

	return currentPtr;
}

ZResource::~ZResource()
{
}