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

	//sprintf(line, "_%s:\n", name.c_str());
	//sourceOutput += line;

	//// TODO: TEMP
	//relativePath = "assets/" + relativePath;

	//sprintf(line, ".incbin \"%s\"\n", (relativePath + "/" + name + ".bin").c_str());

	//sourceOutput += line;

		//sprintf(line, "const u64 _%s[] = \n{\n", name.c_str());
	sprintf(line, "const u8 _%s[] = \n{\n", name.c_str());
	sourceOutput += line;

	for (int i = 0; i < rawData.size(); i += 1)
	{
		//uint64_t data;

		//memcpy(&data, &rawData[i], 8);

		//sprintf(line, "0x%016llX, ", data);
		sprintf(line, "0x%02X, ", rawData[i]);
		sourceOutput += line;

		//if ((i / 8) % 8 == 7)
			//sourceOutput += "\n";

		if (i % 16 == 15)
			sourceOutput += "\n";
	}

	sourceOutput += "};\n";

	return sourceOutput;
}

void ZBlob::Save(string outFolder)
{
	File::WriteAllBytes(outFolder + "/" + name + ".bin", rawData);
}