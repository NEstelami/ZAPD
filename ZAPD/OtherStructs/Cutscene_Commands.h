#pragma once

#include <cstdint>
#include <string>
#include <vector>

class CutsceneSubCommandEntry
{
public:
	uint16_t base;
	uint16_t startFrame;
	uint16_t endFrame;
	uint16_t pad;

	CutsceneSubCommandEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	virtual ~CutsceneSubCommandEntry() = default;

	virtual std::string GetBodySourceCode() const;

	virtual size_t GetRawSize() const;
};

class CutsceneCommand
{
public:
	uint32_t commandID;
	uint32_t commandIndex;

	uint32_t numEntries;
	std::vector<CutsceneSubCommandEntry*> entries;

	CutsceneCommand(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex);
	virtual ~CutsceneCommand();

	virtual std::string GetCommandMacro() const;
	virtual std::string GenerateSourceCode() const;
	virtual size_t GetCommandSize() const;
};
