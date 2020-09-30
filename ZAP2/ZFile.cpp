#include "ZFile.h"
#include "ZBlob.h"
#include "ZDisplayList.h"
#include "ZRoom/ZRoom.h"
#include "ZTexture.h"
#include "ZAnimation.h"
#include "ZHierarchy.h"
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
	defines = "";
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
			rawDataIndex = strtol(StringHelper::Split(child->Attribute("Address"), "0x")[1].c_str(), NULL, 16);

		if (string(child->Name()) == "Texture")
		{
			ZTexture* tex = nullptr;

			if (mode == ZFileMode::Extract)
				tex = ZTexture::ExtractFromXML(child, rawData, rawDataIndex, folderName);
			else
				tex = ZTexture::BuildFromXML(child, folderName, mode == ZFileMode::Build);

			tex->SetRawDataIndex(rawDataIndex);

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
				blob = ZBlob::BuildFromXML(child, folderName, mode == ZFileMode::Build);

			blob->parent = this;

			resources.push_back(blob);

			rawDataIndex += blob->GetRawDataSize();
		}
		else if (string(child->Name()) == "DList")
		{
			ZResource* dList = nullptr;

			if (mode == ZFileMode::Extract)
				dList = ZDisplayList::ExtractFromXML(child, rawData, rawDataIndex, ZDisplayList::GetDListLength(rawData, rawDataIndex), folderName);
			//else
				//dList = ZDisplayList::BuildFromXML(child, folderName, mode == ZFileMode::Build);
			else
				dList = ZBlob::BuildFromXML(child, folderName, mode == ZFileMode::Build);

			dList->parent = this;

			resources.push_back(dList);

			rawDataIndex += dList->GetRawDataSize();
		}
		else if (string(child->Name()) == "Scene" || string(child->Name()) == "Room")
		{
			ZRoom* room = nullptr;

			if (mode == ZFileMode::Extract)
				room = ZRoom::ExtractFromXML(child, rawData, rawDataIndex, folderName, this, Globals::Instance->lastScene);

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
		else if (string(child->Name()) == "Hierarchy")
		{
			ZHierarchy* hierarchy = nullptr;

			if (mode == ZFileMode::Extract)
				hierarchy = ZHierarchy::FromXML(child, rawData, rawDataIndex, folderName, this);

			resources.push_back(hierarchy);
			rawDataIndex += hierarchy->GetRawDataSize();
		}
		else if (string(child->Name()) == "Limb")
		{
			ZLimbStandard* limb = nullptr;

			if (mode == ZFileMode::Extract)
				limb = ZLimbStandard::FromXML(child, rawData, rawDataIndex, folderName, this);

			resources.push_back(limb);

			rawDataIndex += limb->GetRawDataSize();
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

void ZFile::BuildSourceFile(string outputDir)
{
	string folderName = Path::GetFileNameWithoutExtension(outputPath);

	if (!Directory::Exists(outputPath))
		Directory::CreateDirectory(outputPath);

	GenerateSourceFiles(outputDir);
}

std::string ZFile::GetVarName(int address)
{
	for (pair<int32_t, Declaration*> pair : declarations)
	{
		if (pair.first == address)
			return pair.second->varName;
	}

	return "";
}

void ZFile::ExtractResources(string outputDir)
{
	string folderName = Path::GetFileNameWithoutExtension(outputPath);

	//printf("DIR CHECK: %s\n", folderName.c_str());
	//printf("OUT CHECK: %s\n", outputDir.c_str());

	if (!Directory::Exists(outputPath))
		Directory::CreateDirectory(outputPath);

	if (Globals::Instance->genSourceFile)
		GenerateSourceFiles(outputDir);

	for (ZResource* res : resources)
	{
		printf("Saving resource %s\n", res->GetName().c_str());
		res->Save(outputPath);
	}
}

void ZFile::AddResource(ZResource* res)
{
	resources.push_back(res);
}

Declaration* ZFile::AddDeclaration(uint32_t address, DeclarationAlignment alignment, uint32_t size, std::string varType, std::string varName, std::string body)
{
	Declaration* decl = new Declaration(alignment, size, varType, varName, false, body);
	declarations[address] = decl;
	return decl;
}

void ZFile::AddDeclaration(uint32_t address, DeclarationAlignment alignment, DeclarationPadding padding, uint32_t size, string varType, string varName, std::string body)
{
	declarations[address] = new Declaration(alignment, padding, size, varType, varName, false, body);
}

void ZFile::AddDeclarationArray(uint32_t address, DeclarationAlignment alignment, uint32_t size, std::string varType, std::string varName, int arrayItemCnt, std::string body)
{
	declarations[address] = new Declaration(alignment, size, varType, varName, true, arrayItemCnt, body);
}


void ZFile::AddDeclarationArray(uint32_t address, DeclarationAlignment alignment, DeclarationPadding padding, uint32_t size, string varType, string varName, int arrayItemCnt, std::string body)
{
	declarations[address] = new Declaration(alignment, padding, size, varType, varName, true, arrayItemCnt, body);
}


void ZFile::AddDeclarationPlaceholder(uint32_t address)
{
	declarations[address] = new Declaration(DeclarationAlignment::None, 0, "", "", false, "");
}

void ZFile::AddDeclarationInclude(uint32_t address, string includePath, uint32_t size, string varType, string varName)
{
	declarations[address] = new Declaration(includePath, size, varType, varName);
}

void ZFile::AddDeclarationIncludeArray(uint32_t address, std::string includePath, uint32_t size, std::string varType, std::string varName, int arrayItemCnt)
{
	Declaration* decl = new Declaration(includePath, size, varType, varName);

	decl->isArray = true;
	decl->arrayItemCnt = arrayItemCnt;

	declarations[address] = decl;
}

std::string ZFile::GetDeclarationName(uint32_t address)
{
	if (declarations.find(address) != declarations.end())
		return declarations[address]->varName;

	return "ERROR_COULD_NOT_FIND_DECLARATION";
}

Declaration* ZFile::GetDeclaration(uint32_t address)
{
	if (declarations.find(address) != declarations.end())
		return declarations[address];

	return nullptr;
}

bool ZFile::HasDeclaration(uint32_t address)
{
	return (declarations.find(address) != declarations.end());
}

void ZFile::GenerateSourceFiles(string outputDir)
{
	sourceOutput = "";

	sourceOutput += "#include <ultra64.h>\n";
	sourceOutput += "#include <z64.h>\n";
	sourceOutput += StringHelper::Sprintf("#include \"%s\"\n\n", (Path::GetFileNameWithoutExtension(name) + ".h").c_str());

	// Generate Code
	for (ZResource* res : resources)
	{
		string resSrc = res->GetSourceOutputCode(name);

		if (res->IsExternalResource())
		{
			//cout << "EXTERN\n";
			string path = Path::GetFileNameWithoutExtension(res->GetName()).c_str();

			while (StringHelper::EndsWith(outputDir, "/"))
				outputDir = outputDir.substr(0, outputDir.length() - 1);

			//sourceOutput += StringHelper::Sprintf("#include <../build/%s/%s.%s.c.inc>", outputDir.c_str(), Path::GetFileNameWithoutExtension(res->GetName()).c_str(), res->GetExternalExtension().c_str());
			AddDeclarationIncludeArray(res->GetRawDataIndex(), StringHelper::Sprintf("../build/%s/%s.%s.c.inc",
				outputDir.c_str(), Path::GetFileNameWithoutExtension(res->GetName()).c_str(), res->GetExternalExtension().c_str()), res->GetRawDataSize(),
				"", "", 0);
			
			
			//File::WriteAllText("build/" + outputDir + "/" + Path::GetFileNameWithoutExtension(res->GetName()) + ".inc.c", resSrc);
		}
		else
		{
			//cout << "NOT EXTERN\n";
			sourceOutput += resSrc;
		}

		if (resSrc != "")
			sourceOutput += "\n";
	}

	sourceOutput += ProcessDeclarations();

	while (StringHelper::EndsWith(outputDir, "/"))
		outputDir = outputDir.substr(0, outputDir.length() - 1);

	string buildPath = outputDir + "/" + "basefile.txt";
	string outPath = outputDir + "/" + Path::GetFileNameWithoutExtension(name) + ".c";
	//printf("WRITING %s\n", buildPath.c_str());

	//if (!Directory::Exists(Path::GetPath(outPath)))
		//Directory::CreateDirectory(Path::GetPath(outPath));

	//if (!Directory::Exists(Path::GetPath(buildPath)))
		//Directory::CreateDirectory(Path::GetPath(buildPath));

	File::WriteAllText(outPath, sourceOutput);
	//File::WriteAllText(buildPath, outPath);

	// Generate Header
	sourceOutput = "";

	for (ZResource* res : resources)
	{
		string resSrc = res->GetSourceOutputHeader("");
		sourceOutput += resSrc;

		if (resSrc != "")
			sourceOutput += "\n";
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
					declarations[item.first]->preText = buffer + declarations[item.first]->preText;

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
				declarations[item.first]->postText += StringHelper::Sprintf("static u32 pad%02X = 0;\n", curPtr);

				item.second->size += 4;
				curPtr += 4;
			}
		}

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

				string src = "\t";

				for (int i = 0; i < diff; i++)
				{
					src += StringHelper::Sprintf("0x%02X, ", rawDataArr[lastAddr + declarations[lastAddr]->size + i]);

					if (i % 16 == 15)
						src += "\n\t";
				}

				if (declarations.find(lastAddr + declarations[lastAddr]->size) == declarations.end())
				{
					AddDeclarationArray(lastAddr + declarations[lastAddr]->size, DeclarationAlignment::None, diff, "static u8", StringHelper::Sprintf("unaccounted%04X", lastAddr + declarations[lastAddr]->size),
						diff, src);
				}
			}
		}

		lastAddr = item.first;
	}

	// TODO: THIS CONTAINS REDUNDANCIES. CLEAN THIS UP!
	if (lastAddr + declarations[lastAddr]->size < rawData.size())
	{
		int diff = (int)(rawData.size() - (lastAddr + declarations[lastAddr]->size));

		string src = "\t";

		for (int i = 0; i < diff; i++)
		{
			src += StringHelper::Sprintf("0x%02X, ", rawData[lastAddr + declarations[lastAddr]->size + i]);

			if (i % 16 == 15)
				src += "\n\t";
		}

		if (declarations.find(lastAddr + declarations[lastAddr]->size) == declarations.end())
		{
			AddDeclarationArray(lastAddr + declarations[lastAddr]->size, DeclarationAlignment::None, diff, "static u8", StringHelper::Sprintf("unaccounted%04X", lastAddr + declarations[lastAddr]->size),
				diff, src);
		}
	}

	// Go through include declarations
	declarationKeysSorted = vector<pair<int32_t, Declaration*>>(declarations.begin(), declarations.end());
	sort(declarationKeysSorted.begin(), declarationKeysSorted.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs.first < rhs.first;
	});

	for (pair<int32_t, Declaration*> item : declarationKeysSorted)
	{
		if (item.second->includePath != "")
		{
			output += StringHelper::Sprintf("#include <%s>\n", item.second->includePath.c_str());
		}
		else
		{
			if (item.second->preText != "")
				output += item.second->preText + "\n";

			if (item.second->isArray)
			{
				if (item.second->arrayItemCnt == 0)
					output += StringHelper::Sprintf("%s %s[] = \n{\n", item.second->varType.c_str(), item.second->varName.c_str());
				else
					output += StringHelper::Sprintf("%s %s[%i] = \n{\n", item.second->varType.c_str(), item.second->varName.c_str(), item.second->arrayItemCnt);

				output += item.second->text + "\n";
			}
			else
			{
				output += StringHelper::Sprintf("%s %s = {", item.second->varType.c_str(), item.second->varName.c_str());
				output += item.second->text;
			}


			output += "};";
			output += " " + item.second->rightText + "\n\n";
			
			if (item.second->postText != "")
				output += item.second->postText + "\n";
		}
	}

	output += "\n";

	return output;
}


string ZFile::ProcessExterns()
{
	string output = "";

	auto declarationKeysSorted = vector<pair<int32_t, Declaration*>>(declarations.begin(), declarations.end());
	sort(declarationKeysSorted.begin(), declarationKeysSorted.end(), [](const auto& lhs, const auto& rhs)
	{
		return lhs.first < rhs.first;
	});

	for (pair<int32_t, Declaration*> item : declarationKeysSorted)
	{
		if (!StringHelper::StartsWith(item.second->varType, "static ") && item.second->varType != "")// && item.second->includePath == "")
		{
			if (item.second->isArray)
			{
				if (item.second->arrayItemCnt == 0)
					output += StringHelper::Sprintf("extern %s %s[];\n", item.second->varType.c_str(), item.second->varName.c_str());
				else
					output += StringHelper::Sprintf("extern %s %s[%i];\n", item.second->varType.c_str(), item.second->varName.c_str(), item.second->arrayItemCnt);
			}
			else
				output += StringHelper::Sprintf("extern %s %s;\n", item.second->varType.c_str(), item.second->varName.c_str());
		}
	}

	output += "\n";

	output += defines;

	return output;
}