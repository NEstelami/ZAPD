#include "CutsceneMM_Commands.h"

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

CutsceneSubCommandEntry::~CutsceneSubCommandEntry()
{

}

std::string CutsceneSubCommandEntry::GetBodySourceCode() const {
    return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X),", base, startFrame, endFrame, pad);
}

size_t CutsceneSubCommandEntry::GetRawSize()
{
    return 0x08;
}



/* CutsceneMMCommand */

CutsceneMMCommand::CutsceneMMCommand([[maybe_unused]] const std::vector<uint8_t>& rawData,
                                 [[maybe_unused]] uint32_t rawDataIndex)
: CutsceneCommand(rawData, rawDataIndex)
{
    numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);
}

CutsceneMMCommand::~CutsceneMMCommand()
{
    for(auto& entry : entries) {
        delete entry;
    }
}

std::string CutsceneMMCommand::GetCName()
{
	return "SCmdCutsceneData";
}

std::string CutsceneMMCommand::GetCommandMacro() const
{
    return StringHelper::Sprintf("0x%08X, 0x%08X", commandID, numEntries);
}

std::string CutsceneMMCommand::GenerateSourceCode(uint32_t baseAddress)
{
	std::string result;

    result += GetCommandMacro();
    result += ",\n";

    for (auto& entry : entries) {
        result += "        ";
        result += entry->GetBodySourceCode();
        result += "\n";
    }

	return result;
}

size_t CutsceneMMCommand::GetCommandSize()
{
    size_t size = 0;
    if (entries.size() > 0) {
        size = entries.at(0)->GetRawSize() * numEntries;
    } else {
        // fallback during development
        size = 0x08 * numEntries;
    }
	return 0x04 + size;
}



CutsceneMMCommand_5A::CutsceneMMCommand_5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    // TODO
    for(size_t i = 0; i < numEntries/8; i++) {
        CutsceneSubCommandEntry* entry = new CutsceneSubCommandEntry(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}
CutsceneMMCommand_5A::~CutsceneMMCommand_5A()
{

}

//std::string CutsceneMMCommand_5A::GetCommandMacro() const override;

size_t CutsceneMMCommand_5A::GetCommandSize()
{
    return CutsceneMMCommand::GetCommandSize() + numEntries + 4;
}




CutsceneMMCommand_Misc::CutsceneMMCommand_Misc(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        CutsceneSubCommandEntry* entry = new CutsceneSubCommandEntry(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_Misc::~CutsceneMMCommand_Misc()
{

}

std::string CutsceneMMCommand_Misc::GetCommandMacro() const
{
    return StringHelper::Sprintf("CS_MISC_LIST(%i)", numEntries);
}

