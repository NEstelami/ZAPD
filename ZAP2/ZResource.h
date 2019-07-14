#pragma once

#include <string>
#include <vector>
#include <stdint.h>

class ZResource
{
public:
	virtual void Save(std::string outFolder);
	std::string GetName();
	std::string GetRelativePath();
	virtual std::vector<uint8_t> GetRawData();
	virtual int GetRawDataSize();
	virtual std::string GetSourceOutputHeader();
	virtual std::string GetSourceOutputCode();

protected:
	std::string name;
	std::string relativePath;
	std::vector<uint8_t> rawData;
	std::string sourceOutput;
};