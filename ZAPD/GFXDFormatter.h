#pragma once

#include <vector>
#include <map>
#include <string>

class GFXDFormatter
{
private:
	const int tabSize;
	const int defaultIndent;
	const int lineLimit;

	int col;
	int nest;
	int nestIndent[8];
	int currentIndent;

	char word[128];
	char space[128];
	char *wordP;
	char *spaceP;

	std::string str;

	int Write(const char *buf, int count);

	static GFXDFormatter *Instance;
	static int WriteStatic(const char *buf, int count);

public:
	GFXDFormatter(int tabSize = 4, int defaultIndent = 4, int lineLimit = 120);

	int (*StaticWriter())(const char *buf, int count);

	std::string GetOutput();
};