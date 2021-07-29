#pragma once

#include <map>
#include <string>
#include <vector>

class OutputFormatter
{
private:
	std::string str;

	char word[128];
	char space[128];
	char* wordP;
	char* spaceP;
	const uint32_t tabSize;
	const uint32_t defaultIndent;
	const uint32_t lineLimit;

	uint32_t col;
	uint32_t nest;
	uint32_t nestIndent[8];
	uint32_t currentIndent;
	uint32_t wordNests;


	void Flush();

	static OutputFormatter* Instance;
	static int WriteStatic(const char* buf, int count);

public:
	OutputFormatter(uint32_t tabSize = 4, uint32_t defaultIndent = 4, uint32_t lineLimit = 120);

	int (*StaticWriter())(const char* buf, int count);  // Must be `int` due to libgfxd

	int Write(const char* buf, int count);
	int Write(const std::string& buf);

	std::string GetOutput();
};