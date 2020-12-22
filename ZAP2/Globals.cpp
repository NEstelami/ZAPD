#include "Globals.h"
#include "File.h"
#include <algorithm>

Globals* Globals::Instance;

Globals::Globals()
{
	Instance = this;

	files = std::vector<ZFile*>();
	segments = std::vector<int>();
	symbolMap = std::map <uint32_t, std::string>();
	genSourceFile = true;
	testMode = false;
	debugMessages = false;
	profile = false;
	includeFilePrefix = false;
	useExternalResources = true;
	lastScene = nullptr;
}

void Globals::GenSymbolMap(std::string symbolMapPath)
{
	auto symbolLines = File::ReadAllLines(symbolMapPath);

	for (std::string symbolLine : symbolLines)
	{
		auto split = StringHelper::Split(symbolLine, " ");
		uint32_t addr = strtoul(split[0].c_str(), NULL, 16);
		std::string symbolName = split[1];

		symbolMap[addr] = symbolName;
	}
}

void Globals::AddSegment(int segment)
{
	if (std::find(segments.begin(), segments.end(), segment) == segments.end())
		segments.push_back(segment);
}

bool Globals::HasSegment(int segment)
{
	return std::find(segments.begin(), segments.end(), segment) != segments.end();
}
