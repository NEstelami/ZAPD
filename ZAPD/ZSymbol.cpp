#include "ZSymbol.h"
#include "StringHelper.h"


ZSymbol::ZSymbol(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZFile* nParent)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;
	parent = nParent;

    ParseXML(reader);
}

ZSymbol* ZSymbol::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData, int nRawDataIndex, ZFile* parent)
{
    ZSymbol* symbol = new ZSymbol(reader, nRawData, nRawDataIndex, parent);

    return symbol;
}

void ZSymbol::ParseXML(tinyxml2::XMLElement* reader)
{
    ZResource::ParseXML(reader);

    const char* typeXml = reader->Attribute("Type");
    if (typeXml == nullptr) {
		fprintf(stderr, "ZLimb::ParseXML: Warning in '%s'.\n\t Missing 'Type' attribute in xml. Defaulting to 'void*'.\n", name.c_str());
        type = "void*";
    }
    else {
        type = std::string(typeXml);
    }

    const char* typeSizeXml = reader->Attribute("TypeSize");
    if (typeSizeXml == nullptr) {
		fprintf(stderr, "ZLimb::ParseXML: Warning in '%s'.\n\t Missing 'TypeSize' attribute in xml. Defaulting to '4'.\n", name.c_str());
        typeSize = 4; // Size of a word.
    }
    else {
        typeSize = std::strtoul(typeSizeXml, nullptr, 0);
    }
}

//void ZSymbol::PreGenSourceFiles();

bool ZSymbol::DoesSupportArray()
{
    return true;
}

int ZSymbol::GetRawDataSize()
{
    return typeSize;
}

/*std::string ZSymbol::GetSourceOutputCode(const std::string& prefix)
{
    return std::string();
}
*/
std::string ZSymbol::GetSourceOutputHeader(const std::string& prefix)
{
	return StringHelper::Sprintf("extern %s %s;\n", type.c_str(), name.c_str());
}

std::string ZSymbol::GetSourceTypeName()
{
    return type;
}

ZResourceType ZSymbol::GetResourceType()
{
    return ZResourceType::Symbol;
}

