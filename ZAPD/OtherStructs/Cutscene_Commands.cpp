#include "Cutscene_Commands.h"

#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"

/* CutsceneSubCommandEntry */

CutsceneSubCommandEntry::CutsceneSubCommandEntry(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
{
	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	pad = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
}

std::string CutsceneSubCommandEntry::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CMD_HH(0x%04X, 0x%04X), CMD_HH(0x%04X, 0x%04X),", base,
	                             startFrame, endFrame, pad);
}

size_t CutsceneSubCommandEntry::GetRawSize() const
{
	return 0x08;
}

/* CutsceneCommand */

CutsceneCommand::CutsceneCommand(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
{
	numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);
}

CutsceneCommand::~CutsceneCommand()
{
	for (auto& entry : entries)
	{
		delete entry;
	}
}

std::string CutsceneCommand::GetCommandMacro() const
{
	return StringHelper::Sprintf("CMD_W(0x%08X), CMD_W(0x%08X)", commandID, numEntries);
}

std::string CutsceneCommand::GenerateSourceCode() const
{
	std::string result;

	result += GetCommandMacro();
	result += ",\n";

	for (auto& entry : entries)
	{
		result += "        ";
		result += entry->GetBodySourceCode();
		result += "\n";
	}

	return result;
}

size_t CutsceneCommand::GetCommandSize() const
{
	size_t size = 0;
	if (entries.size() > 0)
	{
		size = entries.at(0)->GetRawSize() * entries.size();
	}
	else
	{
		size = 0x08 * numEntries;
	}
	return 0x08 + size;
}

void CutsceneCommand::SetCommandID(uint32_t nCommandID)
{
    commandID = nCommandID;

	for (auto& entry : entries)
	{
        entry->commandID = commandID;
    }
}



CutsceneCameraPoint::CutsceneCameraPoint(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	continueFlag = BitConverter::ToInt8BE(rawData, rawDataIndex + 0);
	cameraRoll = BitConverter::ToInt8BE(rawData, rawDataIndex + 1);
	nextPointFrame = BitConverter::ToInt16BE(rawData, rawDataIndex + 2);
	viewAngle = BitConverter::ToFloatBE(rawData, rawDataIndex + 4);

	posX = BitConverter::ToInt16BE(rawData, rawDataIndex + 8);
	posY = BitConverter::ToInt16BE(rawData, rawDataIndex + 10);
	posZ = BitConverter::ToInt16BE(rawData, rawDataIndex + 12);

	unused = BitConverter::ToInt16BE(rawData, rawDataIndex + 14);
}

std::string CutsceneCameraPoint::GetBodySourceCode() const
{
    std::string result = "";

	if (commandID == (int32_t)CutsceneCommands::SetCameraFocus)
	{
		result += "CS_CAM_FOCUS_POINT";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraFocusLink)
	{
		result += "CS_CAM_FOCUS_POINT_PLAYER";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraPosLink)
	{
		result += "CS_CAM_POS_PLAYER";
	}
	else
	{
		result += "CS_CAM_POS";
	}

    std::string continueMacro = "CS_CMD_CONTINUE";
    if (continueFlag != 0)
        continueMacro = "CS_CMD_STOP";

    result += StringHelper::Sprintf("(%s, 0x%02X, %i, %ff, %i, %i, %i, 0x%04X),",
		                                continueMacro.c_str(),
		                                cameraRoll, nextPointFrame,
		                                viewAngle, posX, posY,
		                                posZ, unused);
    return result;
}

size_t CutsceneCameraPoint::GetRawSize() const
{
    return 0x10;
}

CutsceneCommandSetCameraPos::CutsceneCommandSetCameraPos(const std::vector<uint8_t>& rawData,
                                                         uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	unused = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);

	bool shouldContinue = true;

	uint32_t currentPtr = rawDataIndex + 8;

	while (shouldContinue)
	{
		CutsceneCameraPoint* camPoint = new CutsceneCameraPoint(rawData, currentPtr);
		entries.push_back(camPoint);

		if (camPoint->continueFlag == -1)
			shouldContinue = false;

		currentPtr += camPoint->GetRawSize();
	}
}

std::string CutsceneCommandSetCameraPos::GetCommandMacro() const
{
	std::string result;

	std::string listStr;

	if (commandID == (int32_t)CutsceneCommands::SetCameraFocus)
	{
		listStr = "CS_CAM_FOCUS_POINT_LIST";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraFocusLink)
	{
		listStr = "CS_CAM_FOCUS_POINT_PLAYER_LIST";
	}
	else if (commandID == (int32_t)CutsceneCommands::SetCameraPosLink)
	{
		listStr = "CS_CAM_POS_PLAYER_LIST";
	}
	else
	{
		listStr = "CS_CAM_POS_LIST";
	}

	result += StringHelper::Sprintf("%s(%i, %i)", listStr.c_str(), startFrame, endFrame);

    return result;
}

size_t CutsceneCommandSetCameraPos::GetCommandSize() const
{
	return 0x0C + entries.at(0)->GetRawSize() * entries.size();
}

MusicFadeEntry::MusicFadeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unknown1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	unknown2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
	unknown3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
	unknown4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
	unknown5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
	unknown6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
	unknown7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
    // Macro hardcodes it as zero
	unknown8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 36);
    // Macro hardcodes it as zero
	unknown9 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 40);
    // Macro hardcodes it as zero
	unknown10 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 44);
}

std::string MusicFadeEntry::GetBodySourceCode() const
{
    return StringHelper::Sprintf(
			"CS_FADE_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),", base,
			startFrame, endFrame, pad,
			unknown1, unknown2, unknown3, unknown4,
			unknown5, unknown6, unknown7);
}

size_t MusicFadeEntry::GetRawSize() const
{
    return 0x30;
}

CutsceneCommandFadeBGM::CutsceneCommandFadeBGM(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (uint32_t i = 0; i < numEntries; i++)
	{
		auto* entry = new MusicFadeEntry(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommandFadeBGM::GetCommandMacro() const 
{
    return StringHelper::Sprintf("CS_FADE_BGM_LIST(%i)", entries.size());
}

MusicChangeEntry::MusicChangeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unknown1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	unknown2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
	unknown3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
	unknown4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
	unknown5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
	unknown6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
	unknown7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
}

CutsceneCommandPlayBGM::CutsceneCommandPlayBGM(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	uint32_t numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (uint32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new MusicChangeEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandPlayBGM::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_PLAY_BGM_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"        CS_PLAY_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n",
			entries[i]->base, entries[i]->startFrame, entries[i]->endFrame,
			entries[i]->pad, entries[i]->unknown1, entries[i]->unknown2, entries[i]->unknown3,
			entries[i]->unknown4, entries[i]->unknown5, entries[i]->unknown6, entries[i]->unknown7);
	}

	return result;
}

size_t CutsceneCommandPlayBGM::GetCommandSize() const
{
	return 8 + 0x30;
}

CutsceneCommandStopBGM::CutsceneCommandStopBGM(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	uint32_t numEntries = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (uint32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new MusicChangeEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandStopBGM::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_STOP_BGM_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"\t\tCS_STOP_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n", entries[i]->base,
			entries[i]->startFrame, entries[i]->endFrame, entries[i]->pad,
			entries[i]->unknown1, entries[i]->unknown2, entries[i]->unknown3, entries[i]->unknown4,
			entries[i]->unknown5, entries[i]->unknown6, entries[i]->unknown7);
	}

	return result;
}

size_t CutsceneCommandStopBGM::GetCommandSize() const
{
	return 8 + 0x30;
}

EnvLightingEntry::EnvLightingEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unused1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	unused2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
	unused3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
	unused4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
	unused5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
	unused6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
	unused7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
}

CutsceneCommandEnvLighting::CutsceneCommandEnvLighting(const std::vector<uint8_t>& rawData,
                                                       uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new EnvLightingEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandEnvLighting::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_LIGHTING_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"\t\tCS_LIGHTING(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n", entries[i]->base,
			entries[i]->startFrame, entries[i]->endFrame, entries[i]->pad, entries[i]->unused1,
			entries[i]->unused2, entries[i]->unused3, entries[i]->unused4, entries[i]->unused5,
			entries[i]->unused6, entries[i]->unused7);
	}

	return result;
}

size_t CutsceneCommandEnvLighting::GetCommandSize() const
{
	return 8 + (0x30 * entries.size());
}

Unknown9Entry::Unknown9Entry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk2 = rawData[rawDataIndex + 6];
	unk3 = rawData[rawDataIndex + 7];
	unk4 = rawData[rawDataIndex + 8];
	unused0 = rawData[rawDataIndex + 10];
	unused1 = rawData[rawDataIndex + 11];
	;
}

CutsceneCommandUnknown9::CutsceneCommandUnknown9(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new Unknown9Entry(rawData, rawDataIndex));
		rawDataIndex += 0x0C;
	}
}

std::string CutsceneCommandUnknown9::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_CMD_09_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf("\t\tCS_CMD_09(%i, %i, %i, %i, %i, %i, %i, %i),\n",
		                                entries[i]->base, entries[i]->startFrame,
		                                entries[i]->endFrame, entries[i]->unk2, entries[i]->unk3,
		                                entries[i]->unk4, entries[i]->unused0, entries[i]->unused1);
	}

	return result;
}

size_t CutsceneCommandUnknown9::GetCommandSize() const
{
	return 8 + (entries.size() * 12);
}

UnkEntry::UnkEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unused0 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0);
	unused1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 4);
	unused2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	unused3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
	unused4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
	unused5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
	unused6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
	unused7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
	unused8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
	unused9 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 36);
	unused10 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 40);
	unused11 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 44);
}

CutsceneCommandUnknown::CutsceneCommandUnknown(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new UnkEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandUnknown::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_UNK_DATA_LIST(0x%02X, %i),\n", commandID, entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"    CS_UNK_DATA(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),\n",
			entries[i]->unused0, entries[i]->unused1, entries[i]->unused2, entries[i]->unused3,
			entries[i]->unused4, entries[i]->unused5, entries[i]->unused6, entries[i]->unused7,
			entries[i]->unused8, entries[i]->unused9, entries[i]->unused10, entries[i]->unused11);
	}

	return result;
}

size_t CutsceneCommandUnknown::GetCommandSize() const
{
	return 8 + (entries.size() * 0x30);
}

DayTimeEntry::DayTimeEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	hour = rawData[rawDataIndex + 6];
	minute = rawData[rawDataIndex + 7];
	unused = rawData[rawDataIndex + 8];
}

CutsceneCommandDayTime::CutsceneCommandDayTime(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new DayTimeEntry(rawData, rawDataIndex));
		rawDataIndex += 12;
	}
}

std::string CutsceneCommandDayTime::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_TIME_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"        CS_TIME(%i, %i, %i, %i, %i, %i),\n", entries[i]->base, entries[i]->startFrame,
			entries[i]->endFrame, entries[i]->hour, entries[i]->minute, entries[i]->unused);
	}

	return result;
}

size_t CutsceneCommandDayTime::GetCommandSize() const
{
	return 8 + (entries.size() * 12);
}

TextboxEntry::TextboxEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	type = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
	textID1 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 8);
	textID2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 10);
}

CutsceneCommandTextbox::CutsceneCommandTextbox(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new TextboxEntry(rawData, rawDataIndex));
		rawDataIndex += 12;
	}
}

std::string CutsceneCommandTextbox::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_TEXT_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		if (entries[i]->base == 0xFFFF)
		{
			result += StringHelper::Sprintf("        CS_TEXT_NONE(%i, %i),\n",
			                                entries[i]->startFrame, entries[i]->endFrame);
		}
		else
		{
			result += StringHelper::Sprintf(
				"        CS_TEXT_DISPLAY_TEXTBOX(%i, %i, %i, %i, %i, %i),\n", entries[i]->base,
				entries[i]->startFrame, entries[i]->endFrame, entries[i]->type, entries[i]->textID1,
				entries[i]->textID2);
		}
	}

	return result;
}

size_t CutsceneCommandTextbox::GetCommandSize() const
{
	return 8 + (entries.size() * 12);
}

ActorAction::ActorAction(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	rotX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);
	rotY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 8);
	rotZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 10);
	startPosX = BitConverter::ToInt32BE(rawData, rawDataIndex + 12);
	startPosY = BitConverter::ToInt32BE(rawData, rawDataIndex + 16);
	startPosZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 20);
	endPosX = BitConverter::ToInt32BE(rawData, rawDataIndex + 24);
	endPosY = BitConverter::ToInt32BE(rawData, rawDataIndex + 28);
	endPosZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 32);
	normalX = BitConverter::ToFloatBE(rawData, rawDataIndex + 36);
	normalY = BitConverter::ToFloatBE(rawData, rawDataIndex + 40);
	normalZ = BitConverter::ToFloatBE(rawData, rawDataIndex + 44);
}

CutsceneCommandActorAction::CutsceneCommandActorAction(const std::vector<uint8_t>& rawData,
                                                       uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new ActorAction(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandActorAction::GenerateSourceCode() const
{
	std::string result;
	std::string subCommand;

	if (commandID == 10)
	{
		result += StringHelper::Sprintf("CS_PLAYER_ACTION_LIST(%i),\n", entries.size());
		subCommand = "CS_PLAYER_ACTION";
	}
	else
	{
		result += StringHelper::Sprintf("CS_NPC_ACTION_LIST(%i, %i),\n", commandID, entries.size());
		subCommand = "CS_NPC_ACTION";
	}

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"\t\t%s(0x%04X, %i, %i, 0x%04X, 0x%04X, 0x%04X, %i, %i, %i, %i, %i, %i, %.11ef, "
			"%.11ef, %.11ef),\n",
			subCommand.c_str(), entries[i]->base, entries[i]->startFrame, entries[i]->endFrame,
			entries[i]->rotX, entries[i]->rotY, entries[i]->rotZ, entries[i]->startPosX,
			entries[i]->startPosY, entries[i]->startPosZ, entries[i]->endPosX, entries[i]->endPosY,
			entries[i]->endPosZ, entries[i]->normalX, entries[i]->normalY, entries[i]->normalZ);
	}

	return result;
}

size_t CutsceneCommandActorAction::GetCommandSize() const
{
	return 8 + (entries.size() * 0x30);
}

CutsceneCommandTerminator::CutsceneCommandTerminator(const std::vector<uint8_t>& rawData,
                                                     uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	unknown = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);  // endFrame duplicate
}

std::string CutsceneCommandTerminator::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_TERMINATOR(0x%04X, %i, %i),\n", base, startFrame, endFrame);

	return result;
}

size_t CutsceneCommandTerminator::GetCommandSize() const
{
	return 8 + 8;
}

CutsceneCommandEnd::CutsceneCommandEnd(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
}

std::string CutsceneCommandEnd::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_END(),\n");

	return result;
}

size_t CutsceneCommandEnd::GetCommandSize() const
{
	return 8 + 6;
}

SpecialActionEntry::SpecialActionEntry(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unused1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
	unused2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
	unused3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
	unused4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
	unused5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
	unused6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
	unused7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
	unused8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 36);
	unused9 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 40);
	unused10 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 44);
}

CutsceneCommandSpecialAction::CutsceneCommandSpecialAction(const std::vector<uint8_t>& rawData,
                                                           uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	int32_t numEntries = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);

	rawDataIndex += 4;

	for (int32_t i = 0; i < numEntries; i++)
	{
		entries.push_back(new SpecialActionEntry(rawData, rawDataIndex));
		rawDataIndex += 0x30;
	}
}

std::string CutsceneCommandSpecialAction::GenerateSourceCode() const
{
	std::string result;

	result += StringHelper::Sprintf("CS_MISC_LIST(%i),\n", entries.size());

	for (size_t i = 0; i < entries.size(); i++)
	{
		result += StringHelper::Sprintf(
			"\t\tCS_MISC(0x%04X, %i, %i, 0x%04X, 0x%04X, 0x%04X, %i, %i, %i, %i, %i, %i, %i, "
			"%i),\n",
			entries[i]->base, entries[i]->startFrame, entries[i]->endFrame, entries[i]->pad,
			entries[i]->unused1, entries[i]->unused2, entries[i]->unused3, entries[i]->unused4,
			entries[i]->unused5, entries[i]->unused6, entries[i]->unused7, entries[i]->unused8,
			entries[i]->unused9, entries[i]->unused10);
	}

	return result;
}

size_t CutsceneCommandSpecialAction::GetCommandSize() const
{
	return 8 + (0x30 * entries.size());
}

CutsceneCommandNop::CutsceneCommandNop(const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
}

size_t CutsceneCommandNop::GetCommandSize() const
{
	return 8 + 6;
}

CutsceneCommandSceneTransFX::CutsceneCommandSceneTransFX(const std::vector<uint8_t>& rawData,
                                                         uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
}

std::string CutsceneCommandSceneTransFX::GenerateSourceCode() const
{
	return StringHelper::Sprintf("CS_SCENE_TRANS_FX(%i, %i, %i),\n", base, startFrame, endFrame);
}

size_t CutsceneCommandSceneTransFX::GetCommandSize() const
{
	return 8 + 8;
}



CutsceneCommandSpecialAction::~CutsceneCommandSpecialAction()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandPlayBGM::~CutsceneCommandPlayBGM()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandStopBGM::~CutsceneCommandStopBGM()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandEnvLighting::~CutsceneCommandEnvLighting()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandUnknown9::~CutsceneCommandUnknown9()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandUnknown::~CutsceneCommandUnknown()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandDayTime::~CutsceneCommandDayTime()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandTextbox::~CutsceneCommandTextbox()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandActorAction::~CutsceneCommandActorAction()
{
	for (auto e : entries)
		delete e;
}

CutsceneCommandSceneTransFX::~CutsceneCommandSceneTransFX()
{
}


