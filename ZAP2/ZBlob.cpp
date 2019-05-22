#include "ZBlob.h"
#include "File.h"

using namespace tinyxml2;
using namespace std;

ZBlob::ZBlob(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex)
{
	name = reader->Attribute("Name");
	int size = strtol(reader->Attribute("Size"), NULL, 16);
	rawData = vector<uint8_t>(nRawData.data(), nRawData.data() + size);

	//Array.Copy(nRawData, rawDataIndex, rawData, 0, rawData.Length);
}

ZBlob::ZBlob(XMLElement* reader, string inFolder)
{
	name = reader->Attribute("Name");

	rawData = File::ReadAllBytes(inFolder + "/" + name + ".bin");
}

void ZBlob::Save(string outFolder)
{
	File::WriteAllBytes(outFolder + "/" + name + ".bin", rawData);
}