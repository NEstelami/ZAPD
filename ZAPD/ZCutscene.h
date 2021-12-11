#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "tinyxml2.h"

#include "OtherStructs/Cutscene_Commands.h"
#include "ZFile.h"
#include "ZResource.h"

class ZCutsceneBase : public ZResource
{
public:
	ZCutsceneBase(ZFile* nParent);

	Declaration* DeclareVar(const std::string& prefix, const std::string& bodyStr) override;

	std::string GetSourceTypeName() const override;
	ZResourceType GetResourceType() const override;
};

class ZCutscene : public ZCutsceneBase
{
public:
	ZCutscene(ZFile* nParent);
	~ZCutscene();

	void ParseRawData() override;

	std::string GetBodySourceCode() const override;

	size_t GetRawDataSize() const override;

	CutsceneCommands GetCommandFromID(int32_t id);

	int32_t numCommands;
	int32_t endFrame;
	std::vector<CutsceneCommand*> commands;
};
