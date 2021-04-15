#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include "ZCutscene.h"
#include "ZFile.h"
#include "tinyxml2.h"

class ZCutsceneMM : public ZCutsceneBase
{
public:
	uint32_t segmentOffset;

	ZCutsceneMM(ZFile* nParent);
	~ZCutsceneMM();

	std::string GetBodySourceCode() override;
	void DeclareVar(const std::string& prefix, const std::string& bodyStr) override;
	std::string GetSourceOutputCode(const std::string& prefix) override;
	int GetRawDataSize() override;
	uint32_t getSegmentOffset() override { return segmentOffset; }

	void ParseRawData() override;
	ZResourceType GetResourceType() override;

	void ExtractFromXML(tinyxml2::XMLElement* reader, const std::vector<uint8_t>& nRawData,
                               const int nRawDataIndex, const std::string& nRelPath) override;

protected:
	int numCommands;
	int endFrame;
	std::vector<CutsceneCommand*> commands;

	std::vector<uint32_t> data;
};
