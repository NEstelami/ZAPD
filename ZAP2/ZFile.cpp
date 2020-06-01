#include "ZFile.h"
#include "ZBlob.h"
#include "ZDisplayList.h"
#include "ZRoom/ZRoom.h"
#include "ZTexture.h"
#include "ZAnimation.h"
#include "Path.h"
#include "File.h"
#include "Directory.h"
#include "Globals.h"
#include <algorithm>

using namespace tinyxml2;
using namespace std;

ZFile::ZFile()
{
	resources = vector<ZResource*>();
	basePath = "";
	outputPath = Directory::GetCurrentDirectory();
	declarations = map<int32_t, Declaration*>();
	externs = map<int32_t, string>();
}

ZFile::ZFile(string nOutPath, string nName) : ZFile()
{
	outputPath = nOutPath;
	name = nName;
}

ZFile::ZFile(ZFileMode mode, XMLElement* reader, string nBasePath, string nOutPath) : ZFile()
{
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
	
	if (mode == ZFileMode::Extract)
		rawData = File::ReadAllBytes(basePath + "/" + name);

	int rawDataIndex = 0;

	for (XMLElement* child = reader->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
	{
		printf("%s: %08X\n", child->Attribute("Name"), rawDataIndex);

		if (child->Attribute("Address") != NULL)
		{
			rawDataIndex = strtol(StringHelper::Split(child->Attribute("Address"), "0x")[1].c_str(), NULL, 16);
		}

		if (string(child->Name()) == "Texture")
		{
			ZTexture* tex = nullptr;

			if (mode == ZFileMode::Extract)
				tex = ZTexture::ExtractFromXML(child, rawData, rawDataIndex, folderName);
			else
				tex = ZTexture::BuildFromXML(child, folderName);

			tex->parent = this;

			resources.push_back(tex);
			rawDataIndex += tex->GetRawDataSize();
		}
		else if (string(child->Name()) == "Blob")
		{
			ZBlob* blob = nullptr;

			if (mode == ZFileMode::Extract)
				blob = ZBlob::ExtractFromXML(child, rawData, rawDataIndex, folderName);
			else
				blob = ZBlob::BuildFromXML(child, folderName);

			blob->parent = this;

			resources.push_back(blob);

			rawDataIndex += blob->GetRawDataSize();
		}
		else if (string(child->Name()) == "DList")
		{
			ZDisplayList* dList = nullptr;

			if (mode == ZFileMode::Extract)
				dList = ZDisplayList::ExtractFromXML(child, rawData, rawDataIndex, ZDisplayList::GetDListLength(rawData, rawDataIndex), folderName);
			//else
				//dList = new ZBlob(child, folderName);

			dList->parent = this;

			resources.push_back(dList);

			rawDataIndex += dList->GetRawDataSize();
		}
		else if (string(child->Name()) == "Scene" || string(child->Name()) == "Room")
		{
			ZRoom* room = nullptr;

			if (mode == ZFileMode::Extract)
				room = ZRoom::ExtractFromXML(child, rawData, rawDataIndex, folderName, this, Globals::Instance->lastScene);
			//else
				//blob = new ZBlob(child, folderName);

			if (string(child->Name()) == "Scene")
				Globals::Instance->lastScene = room;

			resources.push_back(room);

			rawDataIndex += room->GetRawDataSize();
		}
		else if (string(child->Name()) == "Animation")
		{
			ZAnimation* anim = nullptr;

			if (mode == ZFileMode::Extract)
				anim = ZAnimation::ExtractFromXML(child, rawData, rawDataIndex, folderName);

			anim->parent = this;
			resources.push_back(anim);

			rawDataIndex += anim->GetRawDataSize();
		}
	}
}

void ZFile::BuildResources()
{
	cout << "Building resources " << name << "\n";

	int size = 0;

	for (ZResource* res : resources)
		size += res->GetRawDataSize();

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
		printf("Saving resource %s\n", res->GetName().c_str());
		res->Save(outputPath);
	}

	if (Globals::Instance->genSourceFile)
		GenerateSourceFiles(outputDir);
}

void ZFile::AddResource(ZResource* res)
{
	resources.push_back(res);
}

void ZFile::GenerateSourceFiles(string outputDir)
{
	sourceOutput = "";

	sourceOutput += "#include <ultra64.h>\n";
	sourceOutput += "#include <z64.h>\n";
	sourceOutput += StringHelper::Sprintf("#include \"%s\"\n", (Path::GetFileNameWithoutExtension(name) + ".h").c_str());

	// Generate Code
	for (ZResource* res : resources)
	{
		string resSrc = res->GetSourceOutputCode(name);
		sourceOutput += resSrc;

		if (resSrc != "")
			sourceOutput += "\n";
	}

	sourceOutput += ProcessDeclarations();

	File::WriteAllText(outputDir + "/" + Path::GetFileNameWithoutExtension(name) + ".c", sourceOutput);

	// Generate Header
	sourceOutput = "";

	for (ZResource* res : resources)
	{
		string resSrc = res->GetSourceOutputHeader("");
		sourceOutput += resSrc + "\n";
	}

	sourceOutput += ProcessExterns();

	File::WriteAllText(outputDir + "/" + Path::GetFileNameWithoutExtension(name) + ".h", sourceOutput);
}

string ZFile::ProcessDeclarations()
{
	string output = "";

	if (declarations.size() == 0)
		return output;

	auto declarationKeysSorted = vector<pair<int32_t, Declaration*>>(declarations.begin(), declarations.end());
	sort(declarationKeysSorted.begin(), declarationKeysSorted.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs.first < rhs.first;
	});

	// Account for padding/alignment
	int lastAddr = 0;

	for (pair<int32_t, Declaration*> item : declarationKeysSorted)
	{
		while (declarations[item.first]->size % 4 != 0)
		{
			declarations[item.first]->size++;
		}

		if (lastAddr != 0)
		{
			if (item.second->alignment == DeclarationAlignment::Align16)
			{
				int lastAddrSizeTest = declarations[lastAddr]->size;
				int curPtr = lastAddr + declarations[lastAddr]->size;

				while (curPtr % 4 != 0)
				{
					declarations[lastAddr]->size++;
					//declarations[item.first]->size++;
					curPtr++;
				}

				/*while (curPtr % 16 != 0)
				{
					char buffer[2048];

					sprintf(buffer, "static u32 align%02X = 0;\n", curPtr);
					declarations[item.first]->text = buffer + declarations[item.first]->text;

					declarations[lastAddr]->size += 4;
					curPtr += 4;
				}*/
			}
			else if (item.second->alignment == DeclarationAlignment::Align8)
			{
				int curPtr = lastAddr + declarations[lastAddr]->size;

				while (curPtr % 4 != 0)
				{
					declarations[lastAddr]->size++;
					//item.second->size++;
					//declarations[item.first]->size++;
					curPtr++;
				}

				while (curPtr % 8 != 0)
				{
					char buffer[2048];

					sprintf(buffer, "static u32 align%02X = 0;\n", curPtr);
					declarations[item.first]->text = buffer + declarations[item.first]->text;

					declarations[lastAddr]->size += 4;
					//item.second->size += 4;
					//declarations[item.first]->size += 4;
					curPtr += 4;
				}
			}
		}

		if (item.second->padding == DeclarationPadding::Pad16)
		{
			int curPtr = item.first + item.second->size;

			while (curPtr % 4 != 0)
			{
				item.second->size++;
				curPtr++;
			}

			while (curPtr % 16 != 0)
			{
				char buffer[2048];

				sprintf(buffer, "static u32 pad%02X = 0;\n", curPtr);
				declarations[item.first]->text += buffer;

				item.second->size += 4;
				curPtr += 4;
			}
		}

		//sourceOutput += declarations[item.first]->text + "\n";

		lastAddr = item.first;
	}

	// Handle for unaccounted data
	lastAddr = 0;
	for (pair<int32_t, Declaration*> item : declarationKeysSorted)
	{
		if (lastAddr != 0)
		{
			if (lastAddr + declarations[lastAddr]->size > item.first)
			{
				// UH OH!
				int bp = 0;
			}

			uint8_t* rawDataArr = rawData.data();

			if (lastAddr + declarations[lastAddr]->size != item.first)
			{
				int diff = item.first - (lastAddr + declarations[lastAddr]->size);

				string src = "";

				src += StringHelper::Sprintf("static u8 unaccounted%04X[] = \n{\n\t", lastAddr + declarations[lastAddr]->size);

				for (int i = 0; i < diff; i++)
				{
					src += StringHelper::Sprintf("0x%02X, ", rawDataArr[lastAddr + declarations[lastAddr]->size + i]);

					if (i % 16 == 15)
						src += "\n\t";
				}

				src += "\n};\n";

				declarations[lastAddr + declarations[lastAddr]->size] = new Declaration(DeclarationAlignment::None, diff, src);
			}
		}

		lastAddr = item.first;
	}

	// TODO: THIS CONTAINS REDUNDANCIES. CLEAN THIS UP!
	if (lastAddr + declarations[lastAddr]->size < rawData.size())
	{
		int diff = (int)(rawData.size() - (lastAddr + declarations[lastAddr]->size));

		string src = "";

		src += StringHelper::Sprintf("static u8 unaccounted%04X[] = \n{\n\t", lastAddr + declarations[lastAddr]->size);

		for (int i = 0; i < diff; i++)
		{
			src += StringHelper::Sprintf("0x%02X, ", rawData[lastAddr + declarations[lastAddr]->size + i]);

			if (i % 16 == 15)
				src += "\n\t";
		}

		src += "\n};\n";

		declarations[lastAddr + declarations[lastAddr]->size] = new Declaration(DeclarationAlignment::None, diff, src);
	}

	declarationKeysSorted = vector<pair<int32_t, Declaration*>>(declarations.begin(), declarations.end());
	sort(declarationKeysSorted.begin(), declarationKeysSorted.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs.first < rhs.first;
	});

	for (pair<int32_t, Declaration*> item : declarationKeysSorted)
	{
		output += item.second->text + "\n";
	}

	output += "\n";

	return output;
}


string ZFile::ProcessExterns()
{
	string output = "";

	auto externsKeysSorted = vector<pair<int32_t, string>>(externs.begin(), externs.end());
	sort(externsKeysSorted.begin(), externsKeysSorted.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs.first < rhs.first;
	});

	for (pair<int32_t, string> item : externsKeysSorted)
		output += item.second + "\n";

	output += "\n";

	return output;
}