#pragma once

#include <string>
#include <vector>

enum class DeclarationAlignment : uint8_t
{
	Align4,
	Align8,
	Align16
};

enum class StaticConfig
{
	Off,
	Global,
	On
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
	StaticConfig staticConf = StaticConfig::Global;

	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName, bool nIsArray, const std::string& nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName, bool nIsArray, size_t nArrayItemCnt, const std::string& nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName, bool nIsArray, const std::string& nArrayItemCntStr,
	            const std::string& nText);
	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nVarType,
	            const std::string& nVarName, bool nIsArray, size_t nArrayItemCnt, const std::string& nText,
	            bool nIsExternal);
	Declaration(const std::string& nIncludePath, size_t nSize, const std::string& nVarType, const std::string& nVarName);

	bool IsStatic() const;

	std::string GetNormalDeclarationStr() const;
	std::string GetExternalDeclarationStr() const;

	std::string GetExternStr() const;

	std::string GetStaticForwardDeclarationStr() const;

protected:
	Declaration(DeclarationAlignment nAlignment, size_t nSize, const std::string& nText);
};
