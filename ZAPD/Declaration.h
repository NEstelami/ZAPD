#pragma once

#include <string>
#include <vector>

enum class DeclarationAlignment
{
	Align4,
	Align8,
	Align16
};

class Declaration
{
public:
	DeclarationAlignment alignment;
	size_t size;
	std::string preText = "";
	std::string text = "";
	std::string rightText = "";
	std::string postText = "";
	std::string preComment = "";
	std::string postComment = "";
	std::string varType = "";
	std::string varName = "";
	std::string includePath = "";
	bool isExternal = false;
	bool isArray = false;
	size_t arrayItemCnt = 0;
	std::string arrayItemCntStr = "";
	std::vector<uint32_t> references;
	bool isUnaccounted = false;
	bool isPlaceholder = false;

	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
	            std::string nVarName, bool nIsArray, std::string nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
	            std::string nVarName, bool nIsArray, size_t nArrayItemCnt, std::string nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
	            std::string nVarName, bool nIsArray, std::string nArrayItemCntStr,
	            std::string nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
	            std::string nVarName, bool nIsArray, size_t nArrayItemCnt, std::string nText,
	            bool nIsExternal);
	Declaration(std::string nIncludePath, size_t nSize, std::string nVarType, std::string nVarName);

protected:
	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nText);
};