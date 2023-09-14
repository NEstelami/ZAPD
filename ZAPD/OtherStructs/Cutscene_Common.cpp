#include "Cutscene_Common.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

/*** TIME ****/

CutsceneSubCommandEntry_SetTime::CutsceneSubCommandEntry_SetTime(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	hour = BitConverter::ToUInt8BE(rawData, rawDataIndex + 6);
	minute = BitConverter::ToUInt8BE(rawData, rawDataIndex + 7);
}

std::string CutsceneSubCommandEntry_SetTime::GetBodySourceCode() const
{
	// Note: Both OoT and MM have the first argument unused
	return StringHelper::Sprintf("CS_TIME(%i, %i, %i, %i, %i)", base, startFrame, endFrame, hour,
	                             minute);
}

size_t CutsceneSubCommandEntry_SetTime::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_Time::CutsceneCommand_Time(const std::vector<uint8_t>& rawData,
                                           offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_SetTime(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_Time::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_TIME_LIST(%i)", numEntries);
}
