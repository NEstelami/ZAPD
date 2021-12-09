#pragma once

#include <cstdint>
#include <string>
#include <vector>

class CutsceneCommand
{
public:
	uint32_t commandID;
	uint32_t commandIndex;

	CutsceneCommand(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	virtual ~CutsceneCommand();

	virtual std::string GetCName();
	virtual std::string GenerateSourceCode(uint32_t baseAddress);
	virtual size_t GetCommandSize();
};
