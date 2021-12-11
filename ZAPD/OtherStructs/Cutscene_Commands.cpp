#include "Cutscene_Commands.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"


/* CutsceneSubCommandEntry */

CutsceneSubCommandEntry::CutsceneSubCommandEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
    base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
    startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
    endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
    pad = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
}

std::string CutsceneSubCommandEntry::GetBodySourceCode() const {
    return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X),", base, startFrame, endFrame, pad);
}

size_t CutsceneSubCommandEntry::GetRawSize() const
{
    return 0x08;
}


/* CutsceneCommand */

CutsceneCommand::CutsceneCommand([[maybe_unused]] const std::vector<uint8_t>& rawData,
                                 [[maybe_unused]] uint32_t rawDataIndex)
{
    numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);
}

CutsceneCommand::~CutsceneCommand()
{
    for(auto& entry : entries) {
        delete entry;
    }
}


std::string CutsceneCommand::GetCommandMacro() const
{
    return StringHelper::Sprintf("CMD_W(0x%08X), CMD_W(0x%08X)", commandID, numEntries);
}

std::string CutsceneCommand::GenerateSourceCode(uint32_t baseAddress)
{
	return StringHelper::Sprintf("PLACEHOLDER CutsceneData%04XCmd%02X = { 0x%02X,",
	                             baseAddress, commandIndex, commandID);
}

size_t CutsceneCommand::GetCommandSize() const
{
	return 0x04;
}
