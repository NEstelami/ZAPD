#include "Declaration.h"

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         std::string nText)
{
	address = nAddress;
	alignment = nAlignment;
	size = nSize;
	text = nText;
}

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         std::string nVarType, std::string nVarName, bool nIsArray,
                         std::string nText)
	: Declaration(nAddress, nAlignment, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
}

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         std::string nVarType, std::string nVarName, bool nIsArray,
                         size_t nArrayItemCnt, std::string nText)
	: Declaration(nAddress, nAlignment, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
	arrayItemCnt = nArrayItemCnt;
}

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         std::string nVarType, std::string nVarName, bool nIsArray,
                         std::string nArrayItemCntStr, std::string nText)
	: Declaration(nAddress, nAlignment, nSize, nText)
{
	varType = nVarType;
	varName = nVarName;
	isArray = nIsArray;
	arrayItemCntStr = nArrayItemCntStr;
}

Declaration::Declaration(offset_t nAddress, DeclarationAlignment nAlignment, size_t nSize,
                         std::string nVarType, std::string nVarName, bool nIsArray,
                         size_t nArrayItemCnt, std::string nText, bool nIsExternal)
	: Declaration(nAddress, nAlignment, nSize, nVarType, nVarName, nIsArray, nArrayItemCnt, nText)
{
	isExternal = nIsExternal;
}

Declaration::Declaration(offset_t nAddress, std::string nIncludePath, size_t nSize,
                         std::string nVarType, std::string nVarName)
	: Declaration(nAddress, DeclarationAlignment::Align4, nSize, "")
{
	includePath = nIncludePath;
	varType = nVarType;
	varName = nVarName;
}
