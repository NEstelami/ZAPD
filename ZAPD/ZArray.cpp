#include "ZArray.h"
#include "Globals.h"
#include "StringHelper.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Array, ZArray);

ZArray::ZArray(ZFile* nParent) : ZResource(nParent)
{
	canHaveInner = true;
	RegisterRequiredAttribute("Count");
}

ZArray::~ZArray()
{
	delete testFile;
}

void ZArray::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	arrayCnt = StringHelper::StrToL(requiredAttributes.at("Count"), 0);
	testFile = new ZFile(ZFileMode::Extract, reader, Globals::Instance->baseRomPath, "",
	                     parent->GetName(), "ZArray subfile", true);
}

// TODO: This is a bit hacky, but until we refactor how ZFile parses the XML, it'll have to do.
std::string ZArray::GetSourceOutputCode(const std::string& prefix)
{
	std::string output = "";

	if (testFile->resources.size() <= 0)
		throw std::runtime_error(
			StringHelper::Sprintf("Error! Array needs at least one sub-element.\n"));

	ZResource* res = testFile->resources[0];
	size_t resSize = res->GetRawDataSize();

	if (!res->DoesSupportArray())
	{
		throw std::runtime_error(StringHelper::Sprintf(
			"Error! Resource %s does not support being wrapped in an array!\n",
			res->GetName().c_str()));
	}

	for (size_t i = 0; i < arrayCnt; i++)
	{
		size_t childIndex = rawDataIndex + (i * resSize);
		res->SetRawDataIndex(childIndex);
		res->ParseRawData();
		std::string test = res->GetSourceOutputCode("");
		// output += "   { " + testFile->declarations[childIndex]->text + " }";
		output += testFile->declarations[childIndex]->text;

		if (i < arrayCnt - 1)
			output += ",\n";
	}

	if (parent != nullptr)
		parent->AddDeclarationArray(rawDataIndex, DeclarationAlignment::None, GetRawDataSize(),
		                            res->GetSourceTypeName(), name, arrayCnt, output);

	return "";
}

size_t ZArray::GetRawDataSize()
{
	return arrayCnt * testFile->resources[0]->GetRawDataSize();
}

ZResourceType ZArray::GetResourceType() const
{
	return ZResourceType::Array;
}
