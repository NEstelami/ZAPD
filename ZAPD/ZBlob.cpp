#include "ZBlob.h"
#include "BitConverter.h"
#include "File.h"
#include "Path.h"
#include "StringHelper.h"
#include "ZFile.h"

using namespace tinyxml2;
using namespace std;

REGISTER_ZFILENODE(Blob, ZBlob);

ZBlob::ZBlob(ZFile* nParent) : ZResource(nParent)
{
	RegisterRequiredAttribute("Count");
}

ZBlob::ZBlob(const std::vector<uint8_t>& nRawData, uint32_t nRawDataIndex, size_t size, std::string nName,
             ZFile* nParent)
	: ZBlob(nParent)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;
	blobData =
		vector<uint8_t>(nRawData.data() + rawDataIndex, nRawData.data() + rawDataIndex + size);
	name = nName;
	RegisterRequiredAttribute("Count");
}

void ZBlob::ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
                           const uint32_t nRawDataIndex, const std::string& nRelPath)
{
	rawData.assign(nRawData.begin(), nRawData.end());
	rawDataIndex = nRawDataIndex;

	ParseXML(reader);
	// TODO: move to ParseRawData?
	blobData.assign(nRawData.data() + rawDataIndex, nRawData.data() + rawDataIndex + blobSize);
	relativePath = nRelPath;
}

// Build Source File Mode
ZBlob* ZBlob::BuildFromXML(XMLElement* reader, const std::string& inFolder, bool readFile)
{
	ZBlob* blob = new ZBlob(nullptr);

	blob->ParseXML(reader);

	if (readFile)
		blob->blobData = File::ReadAllBytes(inFolder + "/" + blob->name + ".bin");

	return blob;
}

ZBlob* ZBlob::FromFile(const std::string& filePath)
{
	ZBlob* blob = new ZBlob(nullptr);
	blob->name = StringHelper::Split(Path::GetFileNameWithoutExtension(filePath), ".")[0];
	blob->blobData = File::ReadAllBytes(filePath);

	return blob;
}

void ZBlob::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	blobSize = StringHelper::StrToL(requiredAttributes.at("Size"), 16);
}

string ZBlob::GetSourceOutputCode(const std::string& prefix)
{
	sourceOutput = "";

	for (size_t i = 0; i < blobData.size(); i += 1)
	{
		if (i % 16 == 0)
			sourceOutput += "    ";

		sourceOutput += StringHelper::Sprintf("0x%02X, ", blobData[i]);

		if (i % 16 == 15)
			sourceOutput += "\n";
	}

	// Ensure there's always a trailing line feed to prevent dumb warnings.
	// Please don't remove this line, unless you somehow made a way to prevent
	// that warning when building the OoT repo.
	sourceOutput += "\n";

	return sourceOutput;
}

string ZBlob::GetSourceOutputHeader(const std::string& prefix)
{
	return StringHelper::Sprintf("extern u8 %s[];\n", name.c_str());
}

void ZBlob::Save(const std::string& outFolder)
{
	File::WriteAllBytes(outFolder + "/" + name + ".bin", blobData);
}

bool ZBlob::IsExternalResource()
{
	return true;
}

string ZBlob::GetExternalExtension()
{
	return "bin";
}

std::string ZBlob::GetSourceTypeName()
{
	return "u8";
}

ZResourceType ZBlob::GetResourceType() const
{
	return ZResourceType::Blob;
}
