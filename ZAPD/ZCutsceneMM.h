#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "tinyxml2.h"

#include "ZFile.h"
#include "ZCutscene.h"
#include "OtherStructs/CutsceneMM_Commands.h"

class ZCutsceneMM : public ZCutsceneBase
{
public:
	ZCutsceneMM(ZFile* nParent);
	virtual ~ZCutsceneMM();

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;

protected:
	int32_t numCommands;
	int32_t endFrame;
	std::vector<CutsceneCommand*> commands;
};
