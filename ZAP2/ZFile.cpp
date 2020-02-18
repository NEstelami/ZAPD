#include "ZFile.h"
#include "ZBlob.h"
#include "ZDisplayList.h"
#include "ZRoom/ZRoom.h"
#include "ZTexture.h"
#include "Path.h"
#include "File.h"
#include "Directory.h"
#include "Globals.h"

using namespace tinyxml2;
using namespace std;

ZFile::ZFile(ZFileMode mode, XMLElement* reader, string nBasePath, string nOutPath)
{
	resources = vector<ZResource*>();

	if (nBasePath == "")
		basePath = Directory::GetCurrentDirectory();
	else
		basePath = nBasePath;

	if (nOutPath == "")
		outputPath = Directory::GetCurrentDirectory();
	else
		outputPath = nOutPath;

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
		//printf("%s: %08X\n", child->Attribute("Name"), rawDataIndex);

		if (string(child->Name()) == "Texture")
		{
			ZTexture* tex = nullptr;

			if (mode == ZFileMode::Extract)
				tex = new ZTexture(child, rawData, rawDataIndex, folderName);
			else
				tex = new ZTexture(child, folderName);

			resources.push_back(tex);
			rawDataIndex += tex->GetRawDataSize();
		}
		else if (string(child->Name()) == "Blob")
		{
			ZBlob* blob = nullptr;

			if (mode == ZFileMode::Extract)
				blob = new ZBlob(child, rawData, rawDataIndex, folderName);
			else
				blob = new ZBlob(child, folderName);

			resources.push_back(blob);

			rawDataIndex += blob->GetRawDataSize();
		}
		else if (string(child->Name()) == "DisplayList")
		{
			ZDisplayList* dList = nullptr;

			if (mode == ZFileMode::Extract)
				dList = new ZDisplayList(child, rawData, rawDataIndex, folderName);
			//else
				//dList = new ZBlob(child, folderName);

			resources.push_back(dList);

			rawDataIndex += dList->GetRawDataSize();
		}
		else if (string(child->Name()) == "Scene" || string(child->Name()) == "Room")
		{
			ZRoom* room = nullptr;

			if (mode == ZFileMode::Extract)
				room = new ZRoom(child, rawData, rawDataIndex, folderName, Globals::Instance->lastScene);
			//else
				//blob = new ZBlob(child, folderName);

			if (string(child->Name()) == "Scene")
				Globals::Instance->lastScene = room;
			
			resources.push_back(room);

			rawDataIndex += room->GetRawDataSize();
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

void ZFile::ExtractResources(string outputDir)
{
	//string folderName = Path::GetFileNameWithoutExtension(name);
	string folderName = Path::GetFileNameWithoutExtension(outputPath);

	if (!Directory::Exists(folderName))
		Directory::CreateDirectory(folderName);

	for (ZResource* res : resources)
	{
		//Console.WriteLine("Saving resource " + res.GetName());
		res->Save(outputPath);
	}

	if (Globals::Instance->genSourceFile)
		GenerateSourceFiles(outputDir);
}

void ZFile::GenerateSourceFiles(string outputDir)
{
	char* buffer = new char[1024 * 1024];

	sourceOutput = "";

	sourceOutput += "#include <ultra64.h>\n";

	// Generate Code
	for (ZResource* res : resources)
	{
		string resSrc = res->GetSourceOutputCode(res->GetName());
		sourceOutput += resSrc + "\n";
	}

	File::WriteAllText(outputDir + "/" + Path::GetFileNameWithoutExtension(name) + ".c", sourceOutput);

	// Generate Header
	sourceOutput = "";

	for (ZResource* res : resources)
	{
		string resSrc = res->GetSourceOutputHeader("");

		sourceOutput += resSrc + "\n";
	}

	File::WriteAllText(outputDir + "/" + Path::GetFileNameWithoutExtension(name) + ".h", sourceOutput);

	delete buffer;
}