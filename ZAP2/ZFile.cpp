#include "ZFile.h"
#include "ZBlob.h"
#include "ZDisplayList.h"
#include "ZTexture.h"
#include "Path.h"
#include "File.h"
#include "Directory.h"

using namespace tinyxml2;
using namespace std;

ZFile::ZFile(ZFileMode mode, XMLElement* reader, string nBasePath)
{
	resources = vector<ZResource*>();

	if (nBasePath == "")
		basePath = Directory::GetCurrentDirectory();
	else
		basePath = nBasePath;

	ParseXML(mode, reader);
}

void ZFile::ParseXML(ZFileMode mode, XMLElement* reader)
{
	name = reader->Attribute("Name");

	string folderName = basePath + "/" + Path::GetFileNameWithoutExtension(name);
	
	vector<uint8_t> rawData;

	if (mode == ZFileMode::Extract)
		rawData = File::ReadAllBytes(basePath + "/" + name);

	int rawDataIndex = 0;

	for (XMLElement* child = reader->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
	{
		printf("%s: %08X\n", child->Attribute("Name"), rawDataIndex);

		if (string(child->Name()) == "Texture")
		{
			ZTexture* tex = NULL;

			if (mode == ZFileMode::Extract)
				tex = new ZTexture(child, rawData, rawDataIndex, folderName);
			else
				tex = new ZTexture(child, folderName);

			resources.push_back(tex);
			rawDataIndex += tex->GetRawDataSize();
		}
		else if (string(child->Name()) == "Blob")
		{
			ZBlob* blob = NULL;

			if (mode == ZFileMode::Extract)
				blob = new ZBlob(child, rawData, rawDataIndex, folderName);
			else
				blob = new ZBlob(child, folderName);

			resources.push_back(blob);

			rawDataIndex += blob->GetRawDataSize();
		}
		else if (string(child->Name()) == "DisplayList")
		{
			ZDisplayList* dList = NULL;

			if (mode == ZFileMode::Extract)
				dList = new ZDisplayList(child, rawData, rawDataIndex, folderName);
			//else
				//dList = new ZBlob(child, folderName);

			resources.push_back(dList);

			rawDataIndex += dList->GetRawDataSize();
		}
	}
}

void ZFile::BuildResources()
{
	cout << "Building resources " << name << "\n";

	int size = 0;

	for (ZResource* res : resources)
	{
		size += res->GetRawDataSize();
	}

	// Make sure size is 16 byte aligned
	if (size % 16 != 0)
		size = ((size / 16) + 1) * 16;

	vector<uint8_t> file = vector<uint8_t>(size);
	int fileIndex = 0;

	for (ZResource* res : resources)
	{
		//Console.WriteLine("Building resource " + res.GetName());
		memcpy(file.data() + fileIndex, res->GetRawData().data(), res->GetRawData().size());
		//Array.Copy(res.GetRawData(), 0, file, fileIndex, res.GetRawData().Length);
		fileIndex += res->GetRawData().size();
	}

	File::WriteAllBytes(basePath + "/" + name, file);
}

void ZFile::ExtractResources()
{
	string folderName = Path::GetFileNameWithoutExtension(name);

	if (!Directory::Exists(folderName))
		Directory::CreateDirectory(folderName);

	for (ZResource* res : resources)
	{
		//Console.WriteLine("Saving resource " + res.GetName());
		res->Save(folderName);
	}

	GenerateSourceFiles();
}

void ZFile::GenerateSourceFiles()
{
	char* buffer = new char[1024 * 1024];

	// Generate Header
	//sourceOutput = "";

	//for (ZResource* res : resources)
	//{
	//	string resSrc = res->GetSourceOutputHeader();

	//	//sprintf(buffer, "const char ", 0);

	//	sourceOutput += resSrc + "\n";
	//}

	//File::WriteAllText(name + ".h", sourceOutput);

	//sourceOutput = ".section .rodata\n.include \"macros.inc\"\n\n";
	sourceOutput = ".section .rodata\n\n";

	// Generate Code
	for (ZResource* res : resources)
	{
		string resSrc = res->GetSourceOutputCode();
		sourceOutput += resSrc + "\n";
	}

	File::WriteAllText(Path::GetFileNameWithoutExtension(name) + ".s", sourceOutput);

	delete buffer;
}