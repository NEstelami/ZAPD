#pragma once

#include <map>
#include <string>
#include <vector>

class OutputFormatter
{
private:
	const int32_t tabSize;
	const int32_t defaultIndent;
	const int32_t lineLimit;

	int32_t col;
	int32_t nest;
	int32_t nestIndent[8];
	int32_t currentIndent;

	char word[128];
	char space[128];
	char* wordP;
	char* spaceP;

	std::string str;

	int32_t Write(const char* buf, int32_t count);

	static OutputFormatter* Instance;
	static int32_t WriteStatic(const char* buf, int32_t count);

public:
	OutputFormatter(int32_t tabSize = 4, int32_t defaultIndent = 4, int32_t lineLimit = 120);

	int32_t (*StaticWriter())(const char* buf, int32_t count);

	std::string GetOutput();
};