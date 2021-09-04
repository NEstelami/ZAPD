#pragma once

#include <string>
#include <vector>

// TODO: should we drop the `_t` suffix because of UNIX compliance?
typedef uint32_t segptr_t;
typedef uint32_t offset_t;

enum class DeclarationAlignment
{
	None,
	Align4,
	Align8,
	Align16
};

enum class DeclarationPadding
{
	None,
	Pad4,
	Pad8,
	Pad16
};

class Declaration
{
public:
	DeclarationAlignment alignment;
	DeclarationPadding padding;
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
	std::vector<segptr_t> references;
	bool isUnaccounted = false;
	bool isPlaceholder = false;
	bool declaredInXml = false;

	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
	            std::string nVarName, bool nIsArray, std::string nText);
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
	            std::string nVarType, std::string nVarName, bool nIsArray, std::string nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
	            std::string nVarName, bool nIsArray, size_t nArrayItemCnt, std::string nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
	            std::string nVarName, bool nIsArray, std::string nArrayItemCntStr,
	            std::string nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, std::string nVarType,
	            std::string nVarName, bool nIsArray, size_t nArrayItemCnt, std::string nText,
	            bool nIsExternal);
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
	            std::string nVarType, std::string nVarName, bool nIsArray, size_t nArrayItemCnt,
	            std::string nText);
	Declaration(std::string nIncludePath, size_t nSize, std::string nVarType, std::string nVarName);

protected:
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
	            std::string nText);
};
