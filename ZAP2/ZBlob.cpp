#include "ZBlob.h"
#include "File.h"
#include "ZFile.h"

using namespace tinyxml2;
using namespace std;

ZBlob::ZBlob()
{

}

ZBlob::ZBlob(std::vector<uint8_t> nRawData, int nRawDataIndex, int size, std::string nName)
{
	rawDataIndex = nRawDataIndex;
	rawData = vector<uint8_t>(nRawData.data() + rawDataIndex, nRawData.data() + rawDataIndex + size);
	name = nName;
}

ZBlob* ZBlob::ExtractFromXML(XMLElement* reader, vector<uint8_t> nRawData, int nRawDataIndex, string nRelPath)
{
	ZBlob* blob = new ZBlob();

	blob->rawDataIndex = nRawDataIndex;

	blob->name = reader->Attribute("Name");
	int size = strtol(reader->Attribute("Size"), NULL, 16);
	blob->rawData = vector<uint8_t>(nRawData.data() + blob->rawDataIndex, nRawData.data() + blob->rawDataIndex + size);

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
	sourceOutput += StringHelper::Sprintf("u8 _%s_%s[] = \n{\n", prefix.c_str(), name.c_str());

	for (int i = 0; i < rawData.size(); i += 1)
	{
		if (i % 16 == 0)
			sourceOutput += "\t";

		sourceOutput += StringHelper::Sprintf("0x%02X, ", rawData[i]);

		if (i % 16 == 15)
			sourceOutput += "\n";
	}

	sourceOutput += "};\n";

	if (parent != nullptr)
	{
		parent->declarations[rawDataIndex] = new Declaration(DeclarationAlignment::None, GetRawDataSize(), sourceOutput);
		return "";
	}

	return sourceOutput;
}

void ZBlob::Save(string outFolder)
{
	File::WriteAllBytes(outFolder + "/" + name + ".bin", rawData);
}
