#include "ZSymbol.h"

#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZFile.h"

REGISTER_ZFILENODE(Symbol, ZSymbol);

ZSymbol::ZSymbol(ZFile* nParent) : ZResource(nParent)
{
	RegisterOptionalAttribute("Type");
	RegisterOptionalAttribute("TypeSize");
	RegisterOptionalAttribute("Count");
}

void ZSymbol::ParseXML(tinyxml2::XMLElement* reader)
{
	ZResource::ParseXML(reader);

	std::string typeXml = registeredAttributes.at("Type").value;

	if (typeXml == "")
	{
		HANDLE_WARNING_RESOURCE(WarningType::MissingAttribute, parent, this, rawDataIndex,
		                        "missing 'Type' attribute in <Symbol>", "Defaulting to 'void*'.");
		type = "void*";
	}
	else
	{
		type = typeXml;
	}

	std::string typeSizeXml = registeredAttributes.at("TypeSize").value;
	if (typeSizeXml == "")
	{
		HANDLE_WARNING_RESOURCE(WarningType::MissingAttribute, parent, this, rawDataIndex,
		                        "missing 'TypeSize' attribute in <Symbol>", "Defaulting to '4'.");
		typeSize = 4;  // Size of a word.
	}
	else
	{
		typeSize = StringHelper::StrToL(typeSizeXml, 0);
	}

	if (registeredAttributes.at("Count").wasSet)
	{
		isArray = true;

		std::string countXml = registeredAttributes.at("Count").value;
		if (countXml != "")
			count = StringHelper::StrToL(countXml, 0);
	}
}

size_t ZSymbol::GetRawDataSize() const
{
	if (isArray)
		return count * typeSize;

	return typeSize;
}

std::string ZSymbol::GetSourceOutputHeader(const std::string& prefix)
{
	if (isArray)
	{
		if (count == 0)
			return StringHelper::Sprintf("extern %s %s%s[];\n", type.c_str(), prefix.c_str(),
			                             name.c_str());
		else
			return StringHelper::Sprintf("extern %s %s%s[%i];\n", type.c_str(), prefix.c_str(),
			                             name.c_str(), count);
	}

	return StringHelper::Sprintf("extern %s %s%s;\n", type.c_str(), prefix.c_str(), name.c_str());
}

std::string ZSymbol::GetSourceTypeName() const
{
	return type;
}

ZResourceType ZSymbol::GetResourceType() const
{
	return ZResourceType::Symbol;
}
