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
    return StringHelper::Sprintf("CMD_W(0x%08X), CMD_W(0x%08X)", commandID, numEntries);
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






CutsceneSubCommandEntry_TextBox::CutsceneSubCommandEntry_TextBox(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
    type = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x06);
    textId1 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x08);
    textId2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x0A);
}
CutsceneSubCommandEntry_TextBox::~CutsceneSubCommandEntry_TextBox()
{

}

std::string CutsceneSubCommandEntry_TextBox::GetBodySourceCode() const
{
    return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X),", base, startFrame, endFrame, type, textId1, textId2);
}

size_t CutsceneSubCommandEntry_TextBox::GetRawSize()
{
    return 0x0C;
}


CutsceneMMCommand_TextBox::CutsceneMMCommand_TextBox(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry_FadeSeq(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_TextBox::~CutsceneMMCommand_TextBox()
{

}
/*
std::string CutsceneMMCommand_TextBox::GetCommandMacro() const
{
    return StringHelper::Sprintf("(%i)", numEntries);
}*/





CutsceneSubCommandEntry_Unk5A::CutsceneSubCommandEntry_Unk5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}
CutsceneSubCommandEntry_Unk5A::~CutsceneSubCommandEntry_Unk5A()
{

}

std::string CutsceneSubCommandEntry_Unk5A::GetBodySourceCode() const
{
    return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X),", base, startFrame);
}

size_t CutsceneSubCommandEntry_Unk5A::GetRawSize()
{
    return 0x04;
}

CutsceneMMCommand_5A::CutsceneMMCommand_5A(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    // TODO
    for(size_t i = 0; i < numEntries/4; i++) {
        auto* entry = new CutsceneSubCommandEntry_Unk5A(rawData, rawDataIndex);
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
    //return 0x04 + numEntries /*+ 0x04*/;

    size_t size = 0;
    if (entries.size() > 0) {
        size = entries.at(0)->GetRawSize() * entries.size();
    } else {
        // fallback during development
        size = 0x08 * numEntries;
    }
	return 0x04 + size;
}




CutsceneMMCommand_Misc::CutsceneMMCommand_Misc(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry(rawData, rawDataIndex);
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





CutsceneSubCommandEntry_Lighting::CutsceneSubCommandEntry_Lighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
    type = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x06);
    textId1 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x08);
    textId2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x0A);
}
CutsceneSubCommandEntry_Lighting::~CutsceneSubCommandEntry_Lighting()
{

}

//std::string CutsceneSubCommandEntry_Lighting::GetBodySourceCode() const
//{
//    return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X),", base, startFrame, endFrame, type, textId1, textId2);
//}


CutsceneMMCommand_Lighting::CutsceneMMCommand_Lighting(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry_Lighting(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_Lighting::~CutsceneMMCommand_Lighting()
{

}
/*
std::string CutsceneMMCommand_Lighting::GetCommandMacro() const
{
    return StringHelper::Sprintf("(%i)", numEntries);
}*/






CutsceneSubCommandEntry_GiveTatl::CutsceneSubCommandEntry_GiveTatl(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}
CutsceneSubCommandEntry_GiveTatl::~CutsceneSubCommandEntry_GiveTatl()
{

}

std::string CutsceneSubCommandEntry_GiveTatl::GetBodySourceCode() const
{
    return StringHelper::Sprintf("CS_GIVETATL(%i, %i, %i),", base, startFrame, endFrame);
}


CutsceneMMCommand_GiveTatl::CutsceneMMCommand_GiveTatl(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry_GiveTatl(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_GiveTatl::~CutsceneMMCommand_GiveTatl()
{

}

std::string CutsceneMMCommand_GiveTatl::GetCommandMacro() const
{
    return StringHelper::Sprintf("CS_GIVETATL_LIST(%i)", numEntries);
}






CutsceneSubCommandEntry_FadeSeq::CutsceneSubCommandEntry_FadeSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
    unk_08 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
}
CutsceneSubCommandEntry_FadeSeq::~CutsceneSubCommandEntry_FadeSeq()
{

}

std::string CutsceneSubCommandEntry_FadeSeq::GetBodySourceCode() const
{
    return CutsceneSubCommandEntry::GetBodySourceCode() + StringHelper::Sprintf("CMD_W(0x%08X),", unk_08);
}

size_t CutsceneSubCommandEntry_FadeSeq::GetRawSize()
{
    return 0x0C;
}


CutsceneMMCommand_FadeSeq::CutsceneMMCommand_FadeSeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry_FadeSeq(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_FadeSeq::~CutsceneMMCommand_FadeSeq()
{

}
/*
std::string CutsceneMMCommand_FadeSeq::GetCommandMacro() const
{
    return StringHelper::Sprintf("(%i)", numEntries);
}*/



CutsceneSubCommandEntry_PlaySeq::CutsceneSubCommandEntry_PlaySeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
}
CutsceneSubCommandEntry_PlaySeq::~CutsceneSubCommandEntry_PlaySeq()
{
}

CutsceneMMCommand_PlaySeq::CutsceneMMCommand_PlaySeq(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry_PlaySeq(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_PlaySeq::~CutsceneMMCommand_PlaySeq()
{

}
/*
std::string CutsceneMMCommand_PlaySeq::GetCommandMacro() const
{
    return StringHelper::Sprintf("(%i)", numEntries);
}*/



CutsceneSubCommandEntry_Unk9B::CutsceneSubCommandEntry_Unk9B(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
    unk_08 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
    unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
    unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
    unk_0B = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0B);
}
CutsceneSubCommandEntry_Unk9B::~CutsceneSubCommandEntry_Unk9B()
{

}

std::string CutsceneSubCommandEntry_Unk9B::GetBodySourceCode() const
{
    return CutsceneSubCommandEntry::GetBodySourceCode() + StringHelper::Sprintf("CMD_BBBB(0x%02X, 0x%02X, 0x%02X, 0x%02X),", unk_08, unk_09, unk_0A, unk_0B);
}

size_t CutsceneSubCommandEntry_Unk9B::GetRawSize()
{
    return 0x0C;
}

CutsceneMMCommand_Unk9B::CutsceneMMCommand_Unk9B(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry_Unk9B(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_Unk9B::~CutsceneMMCommand_Unk9B()
{

}
/*
std::string CutsceneMMCommand_Unk9B::GetCommandMacro() const
{
    return StringHelper::Sprintf("(%i)", numEntries);
}*/




CutsceneSubCommandEntry_SetTime::CutsceneSubCommandEntry_SetTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
    hour = BitConverter::ToUInt8BE(rawData, rawDataIndex + 6);
    minute = BitConverter::ToUInt8BE(rawData, rawDataIndex + 7);
    unk_08 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
}
CutsceneSubCommandEntry_SetTime::~CutsceneSubCommandEntry_SetTime()
{

}

std::string CutsceneSubCommandEntry_SetTime::GetBodySourceCode() const
{
    return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X), CMD_HBB(0x%04X, 0x%02X, 0x%02X), CMD_W(0x%08X),", base, startFrame, endFrame, hour, minute, unk_08);
}

size_t CutsceneSubCommandEntry_SetTime::GetRawSize()
{
    return 0x0C;
}




CutsceneMMCommand_SetTime::CutsceneMMCommand_SetTime(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry_SetTime(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_SetTime::~CutsceneMMCommand_SetTime()
{

}
/*
std::string CutsceneMMCommand_SetTime::GetCommandMacro() const
{
    return StringHelper::Sprintf("(%i)", numEntries);
}*/






CutsceneSubCommandEntry_Unk190::CutsceneSubCommandEntry_Unk190(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
    unk_08 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
    unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
    unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
    unk_0B = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0B);
}
CutsceneSubCommandEntry_Unk190::~CutsceneSubCommandEntry_Unk190()
{

}

std::string CutsceneSubCommandEntry_Unk190::GetBodySourceCode() const
{
    return CutsceneSubCommandEntry::GetBodySourceCode() + StringHelper::Sprintf("CMD_BBBB(0x%02X, 0x%02X, 0x%02X, 0x%02X),", unk_08, unk_09, unk_0A, unk_0B);
}

size_t CutsceneSubCommandEntry_Unk190::GetRawSize()
{
    return 0x0C;
}


CutsceneMMCommand_Unk190::CutsceneMMCommand_Unk190(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
:CutsceneMMCommand(rawData, rawDataIndex)
{
    rawDataIndex += 4;

    for(size_t i = 0; i < numEntries; i++) {
        auto* entry = new CutsceneSubCommandEntry_Unk190(rawData, rawDataIndex);
        entries.push_back(entry);
        rawDataIndex += entry->GetRawSize();
    }
}

CutsceneMMCommand_Unk190::~CutsceneMMCommand_Unk190()
{

}
/*
std::string CutsceneMMCommand_Unk190::GetCommandMacro() const
{
    return StringHelper::Sprintf("(%i)", numEntries);
}*/
