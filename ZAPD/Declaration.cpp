#include "Declaration.h"

#include "Globals.h"
#include "Utils/StringHelper.h"

Declaration::Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
                         std::string nText)
{
	alignment = nAlignment;
	padding = nPadding;
	size = nSize;
	text = nText;
}

Declaration::Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
                         std::string nVarName, bool nIsArray, std::string nText)
	: Declaration(nAlignment, DeclarationPadding::None, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
}

Declaration::Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
                         std::string nVarType, std::string nVarName, bool nIsArray,
                         std::string nText)
	: Declaration(nAlignment, nPadding, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
}

Declaration::Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
                         std::string nVarName, bool nIsArray, size_t nArrayItemCnt,
                         std::string nText)
	: Declaration(nAlignment, DeclarationPadding::None, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
	arrayItemCnt = nArrayItemCnt;
}

Declaration::Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
                         std::string nVarName, bool nIsArray, std::string nArrayItemCntStr,
                         std::string nText)
	: Declaration(nAlignment, DeclarationPadding::None, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
	arrayItemCntStr = nArrayItemCntStr;
}

Declaration::Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
                         std::string nVarName, bool nIsArray, size_t nArrayItemCnt,
                         std::string nText, bool nIsExternal)
	: Declaration(nAlignment, nSize, nVarType, nVarName, nIsArray, nArrayItemCnt, nText)
{
	isExternal = nIsExternal;
}

Declaration::Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
                         std::string nVarType, std::string nVarName, bool nIsArray,
                         size_t nArrayItemCnt, std::string nText)
	: Declaration(nAlignment, nPadding, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
	arrayItemCnt = nArrayItemCnt;
}

Declaration::Declaration(std::string nIncludePath, size_t nSize, std::string nVarType,
                         std::string nVarName)
	: Declaration(DeclarationAlignment::None, DeclarationPadding::None, nSize, "")
{
	includePath = nIncludePath;
	varType = nVarType;
	varName = nVarName;
}

bool Declaration::IsStatic() const
{
	//if (isArray)
	//{
	//	if (arrayItemCnt == 0 && arrayItemCntStr == "")
	//	{
	//		return false;
	//	}
	//}

	switch (staticConf)
	{
	case StaticConfig::Off:
		return false;

	case StaticConfig::Global:
		return Globals::Instance->forceStatic;

	case StaticConfig::On:
		return true;
	}

	return false;
}

std::string Declaration::GetNormalDeclarationStr() const
{
	std::string output;

	if (preText != "")
		output += preText + "\n";

	if (IsStatic())
	{
		output += "static ";
	}

	if (isArray)
	{
		if (arrayItemCntStr != "")
		{
			output += StringHelper::Sprintf(
				"%s %s[%s];\n", varType.c_str(),
				varName.c_str(), arrayItemCntStr.c_str());
		}
		else if (arrayItemCnt == 0)
		{
				output +=
					StringHelper::Sprintf("%s %s[] = {\n", varType.c_str(),
											varName.c_str());
		}
		else
		{
				output += StringHelper::Sprintf(
					"%s %s[%i] = {\n", varType.c_str(),
					varName.c_str(), arrayItemCnt);
		}

		output += text + "\n";
	}
	else
	{
		output += StringHelper::Sprintf("%s %s = { ", varType.c_str(),
										varName.c_str());
		output += text;
	}

	if (output.back() == '\n')
		output += "};";
	else
		output += " };";

	if (rightText != "")
		output += " " + rightText + "";

	output += "\n";

	if (postText != "")
		output += postText + "\n";

	output += "\n";

	return output;
}

std::string Declaration::GetExternalDeclarationStr() const
{
	std::string output;

	if (preText != "")
		output += preText + "\n";

	if (IsStatic())
	{
		output += "static ";
	}

	// Do not asm_process vertex arrays. They have no practical use being overridden.
	// if (varType == "Vtx")
	if (varType != "u64" &&  varType != "u8")
	{
		output += StringHelper::Sprintf(
			"%s %s[] = {\n#include \"%s\"\n};", varType.c_str(),
			varName.c_str(),
			StringHelper::Replace(includePath, "assets/", "../assets/")
				.c_str());
	}
	else
	{
		if (arrayItemCntStr != "")
			output += StringHelper::Sprintf(
				"%s %s[%s] = {\n#include \"%s\"\n};", varType.c_str(),
				varName.c_str(), arrayItemCntStr.c_str(),
				includePath.c_str());
		else if (arrayItemCnt != 0)
			output += StringHelper::Sprintf(
				"%s %s[%i] = {\n#include \"%s\"\n};", varType.c_str(),
				varName.c_str(), arrayItemCnt, includePath.c_str());
		else
			output += StringHelper::Sprintf(
				"%s %s[] = {\n#include \"%s\"\n};", varType.c_str(),
				varName.c_str(), includePath.c_str());
	}

	if (rightText != "")
		output += " " + rightText + "";

	output += "\n";

	if (postText != "")
		output += postText + "\n";

	output += "\n";

	return output;
}

std::string Declaration::GetExternStr() const
{
	if (IsStatic() || varType == "")
	{
		return "";
	}

	if (isArray)
	{
		if (arrayItemCntStr != "")
		{
			return StringHelper::Sprintf("extern %s %s[%s];\n", varType.c_str(),
										varName.c_str(), arrayItemCntStr.c_str());
		}
		else if (arrayItemCnt != 0)
			return StringHelper::Sprintf(
				"extern %s %s[%i];\n", varType.c_str(),
				varName.c_str(), arrayItemCnt);
		else
			return StringHelper::Sprintf("extern %s %s[];\n", varType.c_str(),
										varName.c_str());
	}

	return StringHelper::Sprintf("extern %s %s;\n", varType.c_str(),
										varName.c_str());
}

std::string Declaration::GetStaticForwardDeclarationStr() const
{
	if (!IsStatic() || isUnaccounted)
		return "";

	if (isArray)
	{
		if (arrayItemCntStr == "" && arrayItemCnt == 0)
		{
			// Forward declaring static arrays without specifying the size is not allowed.
			return "";
		}

		if (arrayItemCntStr != "")
		{
			return StringHelper::Sprintf("static %s %s[%s];\n", varType.c_str(),
											varName.c_str(),
											arrayItemCntStr.c_str());
		}
		else
		{
			return StringHelper::Sprintf("static %s %s[%i];\n", varType.c_str(),
											varName.c_str(),
											arrayItemCnt);
		}
	}

	return StringHelper::Sprintf("static %s %s;\n", varType.c_str(), varName.c_str());
}
