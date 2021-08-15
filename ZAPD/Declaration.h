#pragma once

#include <string>
#include <vector>

enum class DeclarationAlignment : uint8_t
{
	None,
	Align4,
	Align8,
	Align16
};

enum class DeclarationPadding : uint8_t
{
	None,
	Pad4,
	Pad8,
	Pad16
};

class Declaration
{
public:
	std::string preText;
	std::string text;
	std::string rightText;
	std::string postText;
	std::string preComment;
	std::string postComment;
	std::string varType;
	std::string varName;
	std::string includePath;
	std::string arrayItemCntStr;
	std::vector<uint32_t> references;
	size_t size = 0;
	size_t arrayItemCnt = 0;
	bool isExternal = false;
	bool isArray = false;
	bool isUnaccounted = false;
	bool isPlaceholder = false;
	DeclarationAlignment alignment;
	DeclarationPadding padding;

	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName, bool nIsArray, const std::string& nText);
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
	            const std::string& nVarType, const std::string& nVarName, bool nIsArray,
	            const std::string& nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName, bool nIsArray, size_t nArrayItemCnt,
	            const std::string& nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName, bool nIsArray, const std::string& nArrayItemCntStr,
	            const std::string& nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName, bool nIsArray, size_t nArrayItemCnt,
	            const std::string& nText, bool nIsExternal);
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
	            const std::string& nVarType, const std::string& nVarName, bool nIsArray,
	            size_t nArrayItemCnt, const std::string& nText);
	Declaration(const std::string& nIncludePath, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName);

protected:
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, size_t nSize,
	            const std::string& nText);
};
