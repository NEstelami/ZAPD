#pragma once

#include <string>
#include "ZFile.h"
#include "ZTexture.h"

class Globals
{
public:
	static Globals* Instance;

	bool genSourceFile; // Used for extraction
	ZFileMode fileMode;
	std::string baseRomPath, inputPath, outputPath, cfgPath;
	TextureType texType;

	Globals();
};