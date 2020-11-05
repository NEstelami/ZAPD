#include "Globals.h"
#include "File.h"

Globals* Globals::Instance;

Globals::Globals()
{
	Instance = this;

	files = std::vector<ZFile*>();
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