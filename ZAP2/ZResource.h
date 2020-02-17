#pragma once

#include <string>
#include <vector>
#include <stdint.h>

enum class SourceType
{
	C,
	ASM
};

class ZResource
{
public:
	virtual void Save(std::string outFolder);
	std::string GetName();
	std::string GetRelativePath();
	virtual std::vector<uint8_t> GetRawData();
	virtual int GetRawDataSize();
	virtual std::string GetSourceOutputHeader(std::string prefix);
	virtual std::string GetSourceOutputCode(std::string prefix);
	virtual SourceType GetSourceType();

protected:
	std::string name;
	std::string relativePath;
	std::vector<uint8_t> rawData;
	std::string sourceOutput;
};