#include "ZResource.h"

using namespace std;

void ZResource::Save(string outFolder)
{

}

string ZResource::GetName()
{
	return name;
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

string ZResource::GetSourceOutputCode()
{
	return "";
}

string ZResource::GetSourceOutputHeader()
{
	return "";
}

SourceType ZResource::GetSourceType()
{
	return SourceType::C;
}