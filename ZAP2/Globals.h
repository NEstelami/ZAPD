#pragma once

#include <string>
#include "ZFile.h"
#include "ZTexture.h"
#include "ZRoom/ZRoom.h"

class Globals
{
public:
	static Globals* Instance;

	bool genSourceFile; // Used for extraction
	ZFileMode fileMode;
	std::string baseRomPath, inputPath, outputPath, cfgPath;
	TextureType texType;

	ZRoom* lastScene;

	Globals();
};