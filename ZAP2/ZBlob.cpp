#include "ZBlob.h"
#include "File.h"

using namespace tinyxml2;
using namespace std;

ZBlob::ZBlob()
{

}

ZBlob::ZBlob(std::vector<uint8_t> nRawData, int rawDataIndex, int size, std::string nName)
{
	rawData = vector<uint8_t>(nRawData.data() + rawDataIndex, nRawData.data() + rawDataIndex + size);
	name = nName;
}

ZBlob* ZBlob::ExtractFromXML(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath)
{
	ZBlob* blob = new ZBlob();

	blob->name = reader->Attribute("Name");
	int size = strtol(reader->Attribute("Size"), NULL, 16);
	blob->rawData = vector<uint8_t>(nRawData.data() + rawDataIndex, nRawData.data() + rawDataIndex + size);

	blob->relativePath = nRelPath;

	return blob;
}

ZBlob* ZBlob::BuildFromXML(XMLElement* reader, string inFolder)
{
	ZBlob* blob = new ZBlob();

	blob->name = reader->Attribute("Name");
	blob->rawData = File::ReadAllBytes(inFolder + "/" + blob->name + ".bin");

	return blob;
}

string ZBlob::GetSourceOutputCode(std::string prefix)
{
	sourceOutput = "";

	//sprintf(line, "u64 _%s[] = \n{\n", name.c_str());
	sourceOutput += StringHelper::Sprintf("u8 _%s[] = \n{\n", name.c_str());

	for (int i = 0; i < rawData.size(); i += 1)
	{
		//uint64_t data;

		//memcpy(&data, &rawData[i], 8);

		if (i % 16 == 0)
			sourceOutput += "\t";

		//sprintf(line, "0x%016llX, ", data);
		sourceOutput += StringHelper::Sprintf("0x%02X, ", rawData[i]);

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
