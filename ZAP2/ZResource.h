#pragma once

#include <string>
#include <vector>
#include <stdint.h>

class ZResource
{
public:
	virtual void Save(std::string outFolder);
	std::string GetName();
	virtual std::vector<uint8_t> GetRawData();
	virtual int GetRawDataSize();

protected:
	std::string name;
	std::vector<uint8_t> rawData;
};