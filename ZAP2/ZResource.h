#pragma once

#include <map>
#include <string>
#include <vector>
#include <stdint.h>

class ZFile;

class Declaration;
struct CommandSet;

class ZResource
{
public:
	ZFile* parent;

	ZResource();
	virtual void Save(std::string outFolder);
	std::string GetName();
	void SetName(std::string nName);
	std::string GetRelativePath();
	virtual std::vector<uint8_t> GetRawData();
	virtual bool IsExternalResource();
	virtual std::string GetExternalExtension();
	virtual int GetRawDataIndex();
	virtual int GetRawDataSize();
	virtual void SetRawDataIndex(int value);
	virtual std::string GetSourceOutputCode(std::string prefix);
	virtual std::string GetSourceOutputHeader(std::string prefix);

protected:
	std::string name;
	std::string relativePath;
	std::vector<uint8_t> rawData;
	int rawDataIndex;
	std::string sourceOutput;
};

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
	uint32_t size;
	std::string preText;
	std::string text;
	std::string postText;
	std::string preComment;
	std::string postComment;
	std::string varType;
	std::string varName;
	std::string includePath;
	bool isArray;
	int arrayItemCnt;

	Declaration(DeclarationAlignment nAlignment, uint32_t nSize, std::string nVarType, std::string nVarName, bool nIsArray, std::string nText);
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, uint32_t nSize, std::string nVarType, std::string nVarName, bool nIsArray, std::string nText);
	Declaration(DeclarationAlignment nAlignment, uint32_t nSize, std::string nVarType, std::string nVarName, bool nIsArray, int nArrayItemCnt, std::string nText);
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, uint32_t nSize, std::string nVarType, std::string nVarName, bool nIsArray, int nArrayItemCnt, std::string nText);
	Declaration(std::string nIncludePath, uint32_t nSize, std::string nVarType, std::string nVarName);

protected:
	Declaration(DeclarationAlignment nAlignment, DeclarationPadding nPadding, uint32_t nSize, std::string nText);
};