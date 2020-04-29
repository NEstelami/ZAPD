#include "ZResource.h"

using namespace std;

void ZResource::Save(string outFolder)
{

}

string ZResource::GetName()
{
	return name;
}

void ZResource::SetName(string nName)
{
	name = nName;
}

string ZResource::GetRelativePath()
{
	return relativePath;
}

vector<uint8_t> ZResource::GetRawData()
{
	return rawData;
}

int ZResource::GetRawDataSize()
{
	return rawData.size();
}

string ZResource::GetSourceOutputCode(std::string prefix)
{
	return "";
}

string ZResource::GetSourceOutputHeader(std::string prefix)
{
	return "";
}