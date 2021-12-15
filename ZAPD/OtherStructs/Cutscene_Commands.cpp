#include "Cutscene_Commands.h"

#include "CutsceneMM_Commands.h"
#include "Globals.h"
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

	result +=
		StringHelper::Sprintf("(%s, 0x%02X, %i, %ff, %i, %i, %i, 0x%04X),", continueMacro.c_str(),
	                          cameraRoll, nextPointFrame, viewAngle, posX, posY, posZ, unused);
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
	return StringHelper::Sprintf("CS_FADE_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),", base,
	                             startFrame, endFrame, pad, unknown1, unknown2, unknown3, unknown4,
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

CutsceneSubCommandEntry_PlaySeq::CutsceneSubCommandEntry_PlaySeq(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	if (Globals::Instance->game != ZGame::MM_RETAIL)
	{
		unknown1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
		unknown2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
		unknown3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
		unknown4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
		unknown5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
		unknown6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
		unknown7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
	}
}

std::string CutsceneSubCommandEntry_PlaySeq::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_PLAYSEQ(0x%04X, %i, %i, %i),", base, startFrame, endFrame,
		                             pad);
	}

	return StringHelper::Sprintf("CS_PLAY_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),", base,
	                             startFrame, endFrame, pad, unknown1, unknown2, unknown3, unknown4,
	                             unknown5, unknown6, unknown7);
}

size_t CutsceneSubCommandEntry_PlaySeq::GetRawSize() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return 0x8;
	}
	return 0x30;
}

CutsceneCommand_PlaySeq::CutsceneCommand_PlaySeq(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_PlaySeq(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_PlaySeq::GetCommandMacro() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_PLAYSEQ_LIST(%i)", numEntries);
	}

	return StringHelper::Sprintf("CS_PLAY_BGM_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_StopSeq::CutsceneSubCommandEntry_StopSeq(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	if (Globals::Instance->game != ZGame::MM_RETAIL)
	{
		unknown1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
		unknown2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 12);
		unknown3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 16);
		unknown4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 20);
		unknown5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 24);
		unknown6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 28);
		unknown7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 32);
	}
}

std::string CutsceneSubCommandEntry_StopSeq::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_STOPSEQ(0x%04X, %i, %i, %i),", base, startFrame, endFrame,
		                             pad);
	}

	return StringHelper::Sprintf("CS_STOP_BGM(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),", base,
	                             startFrame, endFrame, pad, unknown1, unknown2, unknown3, unknown4,
	                             unknown5, unknown6, unknown7);
}

size_t CutsceneSubCommandEntry_StopSeq::GetRawSize() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return 0x8;
	}
	return 0x30;
}

CutsceneCommand_StopSeq::CutsceneCommand_StopSeq(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_StopSeq(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_StopSeq::GetCommandMacro() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_STOPSEQ_LIST(%i)", numEntries);
	}

	return StringHelper::Sprintf("CS_STOP_BGM_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Lighting::CutsceneSubCommandEntry_Lighting(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	if (Globals::Instance->game != ZGame::MM_RETAIL)
	{
		unused1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x8);
		unused2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0xC);
		unused3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x10);
		unused4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x14);
		unused5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x18);
		unused6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x1C);
		unused7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x20);
	}
}

std::string CutsceneSubCommandEntry_Lighting::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_LIGHTING(0x%02X, %i, %i),", base, startFrame, endFrame);
	}

	return StringHelper::Sprintf("CS_LIGHTING(0x%02X, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),",
	                             base, startFrame, endFrame, pad, unused1, unused2, unused3,
	                             unused4, unused5, unused6, unused7);
}

size_t CutsceneSubCommandEntry_Lighting::GetRawSize() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return 0x8;
	}
	return 0x30;
}

CutsceneCommand_Lighting::CutsceneCommand_Lighting(const std::vector<uint8_t>& rawData,
                                                   uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Lighting(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_Lighting::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_LIGHTING_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_Rumble::CutsceneSubCommandEntry_Rumble(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unk_06 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x06);
	unk_07 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x07);
	unk_08 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
	unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
	unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
	unk_0B = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0B);
}

std::string CutsceneSubCommandEntry_Rumble::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_RUMBLE(%i, %i, %i, 0x%02X, 0x%02X, 0x%02X),", base,
		                             startFrame, endFrame, unk_06, unk_07, unk_08);
	}

	return StringHelper::Sprintf("CS_CMD_09(%i, %i, %i, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X),",
	                             base, startFrame, endFrame, unk_06, unk_07, unk_08, unk_09, unk_0A,
	                             unk_0B);
}

size_t CutsceneSubCommandEntry_Rumble::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_Rumble::CutsceneCommand_Rumble(const std::vector<uint8_t>& rawData,
                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Rumble(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_Rumble::GetCommandMacro() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_RUMBLE_LIST(%i)", numEntries);
	}
	return StringHelper::Sprintf("CS_CMD_09_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_UnknownCommand::CutsceneSubCommandEntry_UnknownCommand(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	unused0 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x0);
	unused1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x4);
	unused2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x8);
	unused3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0xC);
	unused4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x10);
	unused5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x14);
	unused6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x18);
	unused7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x1C);
	unused8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x20);
	unused9 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x24);
	unused10 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x28);
	unused11 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x2C);
}

std::string CutsceneSubCommandEntry_UnknownCommand::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_UNK_DATA(0x%02X, %i, %i, %i),", base, startFrame, endFrame,
		                             pad);
	}

	return StringHelper::Sprintf("CS_UNK_DATA(0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, "
	                             "0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X),",
	                             unused0, unused1, unused2, unused3, unused4, unused5, unused6,
	                             unused7, unused8, unused9, unused10, unused11);
}

size_t CutsceneSubCommandEntry_UnknownCommand::GetRawSize() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return 0x08;
	}
	return 0x30;
}

CutsceneCommand_UnknownCommand::CutsceneCommand_UnknownCommand(const std::vector<uint8_t>& rawData,
                                                               uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_UnknownCommand(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_UnknownCommand::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_UNK_DATA_LIST(0x%X, %i)", commandID, numEntries);
}

CutsceneSubCommandEntry_SetTime::CutsceneSubCommandEntry_SetTime(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	hour = BitConverter::ToUInt8BE(rawData, rawDataIndex + 6);
	minute = BitConverter::ToUInt8BE(rawData, rawDataIndex + 7);
	unk_08 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 8);
}

std::string CutsceneSubCommandEntry_SetTime::GetBodySourceCode() const
{
	return StringHelper::Sprintf("CS_TIME(%i, %i, %i, %i, %i, %i),", base, startFrame, endFrame,
	                             hour, minute, unk_08);
}

size_t CutsceneSubCommandEntry_SetTime::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_SetTime::CutsceneCommand_SetTime(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_SetTime(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_SetTime::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_TIME_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_TextBox::CutsceneSubCommandEntry_TextBox(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	type = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x6);
	textId1 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x8);
	textId2 = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0xA);
}

std::string CutsceneSubCommandEntry_TextBox::GetBodySourceCode() const
{
	if (type == 0xFFFF)
	{
		return StringHelper::Sprintf("CS_TEXT_NONE(%i, %i),", startFrame, endFrame);
	}
	if (type == 2)
	{
		return StringHelper::Sprintf("CS_TEXT_LEARN_SONG(%i, %i, %i, 0x%X),", base, startFrame,
		                             endFrame, textId1);
	}

	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		if (type == 0)
		{
			return StringHelper::Sprintf("CS_TEXT_DEFAULT(0x%X, %i, %i, 0x%X, 0x%X),", base,
			                             startFrame, endFrame, textId1, textId2);
		}
		if (type == 1)
		{
			return StringHelper::Sprintf("CS_TEXT_TYPE_1(0x%X, %i, %i, 0x%X, 0x%X),", base,
			                             startFrame, endFrame, textId1, textId2);
		}

		if (type == 3)
		{
			return StringHelper::Sprintf("CS_TEXT_TYPE_3(0x%X, %i, %i, 0x%X, 0x%X),", base,
			                             startFrame, endFrame, textId1, textId2);
		}

		if (type == 4)
		{
			return StringHelper::Sprintf("CS_TEXT_BOSSES_REMAINS(0x%X, %i, %i, 0x%X),", base,
			                             startFrame, endFrame, textId1);
		}
		if (type == 5)
		{
			return StringHelper::Sprintf("CS_TEXT_ALL_NORMAL_MASKS(0x%X, %i, %i, 0x%X),", base,
			                             startFrame, endFrame, textId1);
		}
	}

	return StringHelper::Sprintf("CS_TEXT_DISPLAY_TEXTBOX(0x%X, %i, %i, %i, 0x%X, 0x%X),", base,
	                             startFrame, endFrame, type, textId1, textId2);
}

size_t CutsceneSubCommandEntry_TextBox::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_TextBox::CutsceneCommand_TextBox(const std::vector<uint8_t>& rawData,
                                                 uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_TextBox(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_TextBox::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_TEXT_LIST(%i)", numEntries);
}

CutsceneSubCommandEntry_ActorAction::CutsceneSubCommandEntry_ActorAction(
	const std::vector<uint8_t>& rawData, uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	rotX = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x6);
	rotY = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0x8);
	rotZ = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0xA);
	startPosX = BitConverter::ToInt32BE(rawData, rawDataIndex + 0xC);
	startPosY = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x10);
	startPosZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x14);
	endPosX = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x18);
	endPosY = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x1C);
	endPosZ = BitConverter::ToInt32BE(rawData, rawDataIndex + 0x20);
	normalX = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x24);
	normalY = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x28);
	normalZ = BitConverter::ToFloatBE(rawData, rawDataIndex + 0x2C);
}

std::string CutsceneSubCommandEntry_ActorAction::GetBodySourceCode() const
{
	std::string result;

	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		if (static_cast<CutsceneMMCommands>(commandID) ==
		    CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION)
		{
			result = "CS_PLAYER_ACTION";
		}
		else
		{
			result = "CS_ACTOR_ACTION";
		}
	}
	else
	{
		if (static_cast<CutsceneCommands>(commandID) == CutsceneCommands::SetActorAction0)
		{
			result = "CS_PLAYER_ACTION";
		}
		else
		{
			result = "CS_NPC_ACTION";
		}
	}

	result +=
		StringHelper::Sprintf("(0x%04X, %i, %i, 0x%04X, 0x%04X, 0x%04X, %i, %i, "
	                          "%i, %i, %i, %i, %.11ef, %.11ef, %.11ef),",
	                          base, startFrame, endFrame, rotX, rotY, rotZ, startPosX, startPosY,
	                          startPosZ, endPosX, endPosY, endPosZ, normalX, normalY, normalZ);
	return result;
}

size_t CutsceneSubCommandEntry_ActorAction::GetRawSize() const
{
	return 0x30;
}

CutsceneCommand_ActorAction::CutsceneCommand_ActorAction(const std::vector<uint8_t>& rawData,
                                                         uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_ActorAction(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_ActorAction::GetCommandMacro() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		if (static_cast<CutsceneMMCommands>(commandID) ==
		    CutsceneMMCommands::CS_CMD_SET_PLAYER_ACTION)
		{
			return StringHelper::Sprintf("CS_PLAYER_ACTION_LIST(%i)", numEntries);
		}
		return StringHelper::Sprintf("CS_ACTOR_ACTION_LIST(0x%03X, %i)", commandID, numEntries);
	}

	if (static_cast<CutsceneCommands>(commandID) == CutsceneCommands::SetActorAction0)
	{
		return StringHelper::Sprintf("CS_PLAYER_ACTION_LIST(%i)", entries.size());
	}
	return StringHelper::Sprintf("CS_NPC_ACTION_LIST(0x%03X, %i)", commandID, entries.size());
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

CutsceneSubCommandEntry_Misc::CutsceneSubCommandEntry_Misc(const std::vector<uint8_t>& rawData,
                                                           uint32_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	if (Globals::Instance->game != ZGame::MM_RETAIL)
	{
		unused1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x8);
		unused2 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0xC);
		unused3 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x10);
		unused4 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x14);
		unused5 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x18);
		unused6 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x1C);
		unused7 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x20);
		unused8 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x24);
		unused9 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x28);
		unused10 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x2C);
	}
}

std::string CutsceneSubCommandEntry_Misc::GetBodySourceCode() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return StringHelper::Sprintf("CS_MISC(0x%02X, %i, %i, %i),", base, startFrame, endFrame,
		                             pad);
	}

	return StringHelper::Sprintf(
		"CS_MISC(0x%04X, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i),", base, startFrame,
		endFrame, pad, unused1, unused2, unused3, unused4, unused5, unused6, unused7, unused8,
		unused9, unused10);
}

size_t CutsceneSubCommandEntry_Misc::GetRawSize() const
{
	if (Globals::Instance->game == ZGame::MM_RETAIL)
	{
		return 0x08;
	}
	return 0x30;
}

CutsceneCommand_Misc::CutsceneCommand_Misc(const std::vector<uint8_t>& rawData,
                                           uint32_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Misc(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_Misc::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_MISC_LIST(%i)", numEntries);
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
