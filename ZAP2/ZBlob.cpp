#include "ZBlob.h"
#include "File.h"

using namespace tinyxml2;
using namespace std;

// EXTRACT MODE
ZBlob::ZBlob(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath)
{
	name = reader->Attribute("Name");
	int size = strtol(reader->Attribute("Size"), NULL, 16);
	rawData = vector<uint8_t>(nRawData.data() + rawDataIndex, nRawData.data() + rawDataIndex + size);

	relativePath = nRelPath;

	//Array.Copy(nRawData, rawDataIndex, rawData, 0, rawData.Length);
}

// BUILD MODE
ZBlob::ZBlob(XMLElement* reader, string inFolder)
{
	name = reader->Attribute("Name");

	rawData = File::ReadAllBytes(inFolder + "/" + name + ".bin");
}

string ZBlob::GetSourceOutputCode()
{
	char line[2048];
	sourceOutput = "";

	sprintf(line, "_%s:\n", name.c_str());
	sourceOutput += line;

	// TODO: TEMP
	relativePath = "assets/" + relativePath;

	sprintf(line, ".incbin \"%s\"\n", (relativePath + "/" + name + ".bin").c_str());

	sourceOutput += line;

	return sourceOutput;
}

void ZBlob::Save(string outFolder)
{
	File::WriteAllBytes(outFolder + "/" + name + ".bin", rawData);
}