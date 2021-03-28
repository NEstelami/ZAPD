#include "ZResource.h"

#include <regex>
#include "StringHelper.h"

using namespace std;

ZResource::ZResource()
{
	parent = nullptr;
	name = "";
	outName = "";
	relativePath = "";
	sourceOutput = "";
	rawData = vector<uint8_t>();
	rawDataIndex = 0;
	outputDeclaration = true;
}

void ZResource::ParseXML(tinyxml2::XMLElement* reader)
{
	if (reader->Attribute("Name") != nullptr)
	{
		name = reader->Attribute("Name");
		static std::regex r("[a-zA-Z_]+[a-zA-Z0-9_]*", std::regex::icase | std::regex::optimize);

		if (!std::regex_match(name, r))
		{
			throw std::domain_error(StringHelper::Sprintf("ZResource::ParseXML: Fatal error in '%s'.\n\t Resource with invalid 'Name' attribute.\n", name.c_str()));
		}
	}
	else
		name = "";

	if (reader->Attribute("OutName") != nullptr)
		outName = reader->Attribute("OutName");
	else
		outName = name;

	if (!canHaveInner && !reader->NoChildren())
	{
		throw std::runtime_error(StringHelper::Sprintf("ZResource::ParseXML: Fatal error in '%s'.\n\t Resource with inner element/child detected.\n", name.c_str()));
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

void ZResource::SetName(string nName)
{
	name = std::move(nName);
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

int ZResource::GetRawDataIndex()
{
	return rawDataIndex;
}

int ZResource::GetRawDataSize()
{
	return rawData.size();
}

void ZResource::SetRawDataIndex(int value)
{
	rawDataIndex = value;
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
	return "";
}

ZResourceType ZResource::GetResourceType()
{
	return ZResourceType::Error;
}

void ZResource::CalcHash()
{
	hash = 0;
}

uint32_t Seg2Filespace(segptr_t segmentedAddress, uint32_t parentBaseAddress)
{
	uint32_t currentPtr = GETSEGOFFSET(segmentedAddress);

	if (GETSEGNUM(segmentedAddress) == 0x80)  // Is defined in code?
		currentPtr -= GETSEGOFFSET(parentBaseAddress);

	return currentPtr;
}
