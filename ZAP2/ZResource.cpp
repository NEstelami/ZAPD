#include "ZResource.h"

using namespace std;

void ZResource::Save(string outFolder)
{

}

string ZResource::GetName()
{
	return name;
}

vector<uint8_t> ZResource::GetRawData()
{
	return rawData;
}

int ZResource::GetRawDataSize()
{
	return rawData.size();
}