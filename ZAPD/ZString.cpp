#include "ZString.h"
#include "File.h"
#include "ZFile.h"
#include "StringHelper.h"

using namespace tinyxml2;
using namespace std;

ZString::ZString() : ZResource()
{

}

ZString* ZString::ExtractFromXML(XMLElement* reader, const vector<uint8_t>& nRawData, int nRawDataIndex, string nRelPath)
{
	ZString* str = new ZString();

	str->rawDataIndex = nRawDataIndex;

	str->ParseXML(reader);
	size_t size = 0;
    for (size_t i = str->rawDataIndex; i < nRawData.size(); ++i) {
        ++size;
        if (nRawData[i] == '\0') {
            break;
        }
    }
	str->rawData = vector<uint8_t>(nRawData.data() + str->rawDataIndex, nRawData.data() + str->rawDataIndex + size);
	str->relativePath = std::move(nRelPath);

	return str;
}

string ZString::GetSourceOutputHeader(const std::string& prefix)
{
	return StringHelper::Sprintf("#define %s \"%s\"", name.c_str(), rawData.data());
}

std::string ZString::GetSourceTypeName()
{
	return "char*";
}

ZResourceType ZString::GetResourceType()
{
	return ZResourceType::String;
}
