#include "Cutscene_Commands.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

CutsceneCommand::CutsceneCommand([[maybe_unused]] const std::vector<uint8_t>& rawData,
                                 [[maybe_unused]] uint32_t rawDataIndex)
{
}

CutsceneCommand::~CutsceneCommand()
{
}

std::string CutsceneCommand::GetCName()
{
	return "SCmdCutsceneData";
}

std::string CutsceneCommand::GenerateSourceCode(uint32_t baseAddress)
{
	return StringHelper::Sprintf("%s CutsceneData%04XCmd%02X = { 0x%02X,", GetCName().c_str(),
	                             baseAddress, commandIndex, commandID);
}

size_t CutsceneCommand::GetCommandSize()
{
	return 4;
}
