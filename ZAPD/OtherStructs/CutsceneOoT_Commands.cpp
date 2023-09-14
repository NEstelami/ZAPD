#include "CutsceneOoT_Commands.h"
#include "CutsceneOoT_CommandSubTypes.h"

#include <cassert>
#include <unordered_map>
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"


/*** GENERIC ****/

// Specific for command lists where each entry has size 0x30 bytes
const std::unordered_map<CutsceneCommands, CsCommandListDescriptor> csCommandsDesc = {
	{CutsceneCommands::CS_CMD_MISC,
     {"CS_MISC", "(%s, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
	{CutsceneCommands::CS_CMD_LIGHT_SETTING,
     {"CS_LIGHT_SETTING", "(0x%02X, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
	{CutsceneCommands::CS_CMD_START_SEQ,
     {"CS_START_SEQ", "(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
	{CutsceneCommands::CS_CMD_STOP_SEQ,
     {"CS_STOP_SEQ", "(%i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
	{CutsceneCommands::CS_CMD_FADE_OUT_SEQ,
     {"CS_FADE_OUT_SEQ", "(%s, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i)"}},
};

CutsceneSubCommandEntry_GenericCmd::CutsceneSubCommandEntry_GenericCmd(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex, CutsceneCommands cmdId)
	: CutsceneSubCommandEntry(rawData, rawDataIndex), commandId(cmdId)
{
	word0 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x0);
	word1 = BitConverter::ToUInt32BE(rawData, rawDataIndex + 0x4);

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

std::string CutsceneSubCommandEntry_GenericCmd::GetBodySourceCode() const
{
	const auto& element = csCommandsDesc.find(commandId);

	if (element != csCommandsDesc.end())
	{
		std::string entryFmt = element->second.cmdMacro;
		entryFmt += element->second.args;

		if (((commandId != CutsceneCommands::CS_CMD_MISC) &&
		     (commandId != CutsceneCommands::CS_CMD_FADE_OUT_SEQ)) ||
		    (base >= sizeof(csOoTMiscTypes)))
		{
			bool baseOne = (commandId == CutsceneCommands::CS_CMD_LIGHT_SETTING ||
			                commandId == CutsceneCommands::CS_CMD_START_SEQ ||
			                commandId == CutsceneCommands::CS_CMD_STOP_SEQ);
			return StringHelper::Sprintf(entryFmt.c_str(), baseOne ? base - 1 : base, startFrame,
			                             endFrame, pad, unused1, unused2, unused3, unused4, unused5,
			                             unused6, unused7, unused8, unused9, unused10);
		}
		else
		{
			std::string firstArg;

			if (commandId == CutsceneCommands::CS_CMD_MISC)
			{
				firstArg = csOoTMiscTypes[base];
			}
			else
			{
				firstArg = base == 3 ? "CS_FADE_OUT_FANFARE" : "CS_FADE_OUT_BGM_MAIN";
			}

			return StringHelper::Sprintf(entryFmt.c_str(), firstArg.c_str(), startFrame, endFrame,
			                             pad, unused1, unused2, unused3, unused4, unused5, unused6,
			                             unused7, unused8, unused9, unused10);
		}
	}

	return StringHelper::Sprintf("CS_UNK_DATA(0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, "
	                             "0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X)",
	                             word0, word1, unused1, unused2, unused3, unused4, unused5, unused6,
	                             unused7, unused8, unused9, unused10);
}

size_t CutsceneSubCommandEntry_GenericCmd::GetRawSize() const
{
	return 0x30;
}

CutsceneCommand_GenericCmd::CutsceneCommand_GenericCmd(const std::vector<uint8_t>& rawData,
                                                       offset_t rawDataIndex,
                                                       CutsceneCommands cmdId)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	commandID = static_cast<uint32_t>(cmdId);
	entries.reserve(numEntries);

	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_GenericCmd(rawData, rawDataIndex, cmdId);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_GenericCmd::GetCommandMacro() const
{
	const auto& element = csCommandsDesc.find(static_cast<CutsceneCommands>(commandID));

	if (element != csCommandsDesc.end())
	{
		return StringHelper::Sprintf("%s_LIST(%i)", element->second.cmdMacro, numEntries);
	}

	return StringHelper::Sprintf("CS_UNK_DATA_LIST(0x%X, %i)", commandID, numEntries);
}

/*** CAMERA ****/

CutsceneCameraPoint::CutsceneCameraPoint(const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
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
	std::string continueMacro = "CS_CAM_CONTINUE";
	if (continueFlag != 0)
		continueMacro = "CS_CAM_STOP";

	return StringHelper::Sprintf("CS_CAM_POINT(%s, 0x%02X, %i, %ff, %i, %i, %i, 0x%04X)",
	                             continueMacro.c_str(), cameraRoll, nextPointFrame, viewAngle, posX,
	                             posY, posZ, unused);
}

size_t CutsceneCameraPoint::GetRawSize() const
{
	return 0x10;
}

CutsceneCommand_GenericCameraCmd::CutsceneCommand_GenericCameraCmd(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
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

std::string CutsceneCommand_GenericCameraCmd::GetCommandMacro() const
{
	std::string result;
	std::string listStr;

	if (commandID == (uint32_t)CutsceneCommands::CS_CMD_CAM_AT_SPLINE)
	{
		listStr = "CS_CAM_AT_SPLINE";
	}
	else if (commandID == (uint32_t)CutsceneCommands::CS_CMD_CAM_AT_SPLINE_REL_TO_PLAYER)
	{
		listStr = "CS_CAM_AT_SPLINE_REL_TO_PLAYER";
	}
	else if (commandID == (uint32_t)CutsceneCommands::CS_CMD_CAM_EYE_SPLINE_REL_TO_PLAYER)
	{
		listStr = "CS_CAM_EYE_SPLINE_REL_TO_PLAYER";
	}
	else
	{
		listStr = "CS_CAM_EYE_SPLINE";
	}

	result += StringHelper::Sprintf("%s(%i, %i)", listStr.c_str(), startFrame, endFrame);

	return result;
}

size_t CutsceneCommand_GenericCameraCmd::GetCommandSize() const
{
	return 0x0C + entries.at(0)->GetRawSize() * entries.size();
}

/*** RUMBLE ****/

CutsceneSubCommandEntry_Rumble::CutsceneSubCommandEntry_Rumble(const std::vector<uint8_t>& rawData,
                                                               offset_t rawDataIndex)
	: CutsceneSubCommandEntry(rawData, rawDataIndex)
{
	sourceStrength = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x06);
	duration = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x07);
	decreaseRate = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x08);
	unk_09 = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x09);
	unk_0A = BitConverter::ToUInt8BE(rawData, rawDataIndex + 0x0A);
}

std::string CutsceneSubCommandEntry_Rumble::GetBodySourceCode() const
{
	// Note: the first argument is unused
	return StringHelper::Sprintf("CS_RUMBLE_CONTROLLER(%i, %i, %i, %i, %i, %i, 0x%02X, 0x%02X)",
	                             base, startFrame, endFrame, sourceStrength, duration, decreaseRate,
	                             unk_09, unk_0A);
}

size_t CutsceneSubCommandEntry_Rumble::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_Rumble::CutsceneCommand_Rumble(const std::vector<uint8_t>& rawData,
                                               offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_Rumble(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_Rumble::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_RUMBLE_CONTROLLER_LIST(%i)", numEntries);
}

/*** TEXT ****/

CutsceneSubCommandEntry_TextBox::CutsceneSubCommandEntry_TextBox(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
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
		return StringHelper::Sprintf("CS_TEXT_NONE(%i, %i)", startFrame, endFrame);
	}
	if (type == 2)
	{
		return StringHelper::Sprintf("CS_TEXT_OCARINA_ACTION(%i, %i, %i, 0x%X)", base, startFrame,
		                             endFrame, textId1);
	}

	if (type < sizeof(csOoTTextTypes))
	{
		return StringHelper::Sprintf("CS_TEXT(0x%X, %i, %i, %s, 0x%X, 0x%X)", base, startFrame,
		                             endFrame, csOoTTextTypes[type].c_str(), textId1, textId2);
	}

	return StringHelper::Sprintf("CS_TEXT(0x%X, %i, %i, %i, 0x%X, 0x%X)", base, startFrame,
	                             endFrame, type, textId1, textId2);
}

size_t CutsceneSubCommandEntry_TextBox::GetRawSize() const
{
	return 0x0C;
}

CutsceneCommand_Text::CutsceneCommand_Text(const std::vector<uint8_t>& rawData,
                                           offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_TextBox(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_Text::GetCommandMacro() const
{
	return StringHelper::Sprintf("CS_TEXT_LIST(%i)", numEntries);
}

/*** ACTOR CUE ****/

CutsceneSubCommandEntry_ActorCue::CutsceneSubCommandEntry_ActorCue(
	const std::vector<uint8_t>& rawData, offset_t rawDataIndex)
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

std::string CutsceneSubCommandEntry_ActorCue::GetBodySourceCode() const
{
	std::string result;

	if (static_cast<CutsceneCommands>(commandID) == CutsceneCommands::CS_CMD_PLAYER_CUE)
	{
		result = "CS_PLAYER_CUE";
	}
	else
	{
		result = "CS_ACTOR_CUE";
	}

	result +=
		StringHelper::Sprintf("(%i, %i, %i, 0x%04X, 0x%04X, 0x%04X, %i, %i, "
	                          "%i, %i, %i, %i, %.11ef, %.11ef, %.11ef)",
	                          base, startFrame, endFrame, rotX, rotY, rotZ, startPosX, startPosY,
	                          startPosZ, endPosX, endPosY, endPosZ, normalX, normalY, normalZ);
	return result;
}

size_t CutsceneSubCommandEntry_ActorCue::GetRawSize() const
{
	return 0x30;
}

CutsceneCommand_ActorCue::CutsceneCommand_ActorCue(const std::vector<uint8_t>& rawData,
                                                   offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	entries.reserve(numEntries);
	for (size_t i = 0; i < numEntries; i++)
	{
		auto* entry = new CutsceneSubCommandEntry_ActorCue(rawData, rawDataIndex);
		entries.push_back(entry);
		rawDataIndex += entry->GetRawSize();
	}
}

std::string CutsceneCommand_ActorCue::GetCommandMacro() const
{
	if (static_cast<CutsceneCommands>(commandID) == CutsceneCommands::CS_CMD_PLAYER_CUE)
	{
		return StringHelper::Sprintf("CS_PLAYER_CUE_LIST(%i)", entries.size());
	}

	const auto& element = csOoTEnumNameToString.find(commandID);

	if (element != csOoTEnumNameToString.end())
	{
		return StringHelper::Sprintf("CS_ACTOR_CUE_LIST(%s, %i)", element->second.c_str(),
		                             entries.size());
	}
	return StringHelper::Sprintf("CS_ACTOR_CUE_LIST(%04X, %i)", commandID, entries.size());
}

/*** DESTINATION ****/

CutsceneCommand_Destination::CutsceneCommand_Destination(const std::vector<uint8_t>& rawData,
                                                         offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
	unknown = BitConverter::ToUInt16BE(rawData, rawDataIndex + 6);  // endFrame duplicate
}

std::string CutsceneCommand_Destination::GenerateSourceCode() const
{
	if (base < sizeof(csOoTDestinationType))
	{
		return StringHelper::Sprintf("CS_DESTINATION(%s, %i, %i),\n",
		                             csOoTDestinationType[base].c_str(), startFrame, endFrame);
	}
	return StringHelper::Sprintf("CS_DESTINATION(%i, %i, %i),\n", base, startFrame, endFrame);
}

size_t CutsceneCommand_Destination::GetCommandSize() const
{
	return 0x10;
}

/*** TRANSITION ****/

CutsceneCommand_Transition::CutsceneCommand_Transition(const std::vector<uint8_t>& rawData,
                                                       offset_t rawDataIndex)
	: CutsceneCommand(rawData, rawDataIndex)
{
	rawDataIndex += 4;

	base = BitConverter::ToUInt16BE(rawData, rawDataIndex + 0);
	startFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 2);
	endFrame = BitConverter::ToUInt16BE(rawData, rawDataIndex + 4);
}

std::string CutsceneCommand_Transition::GenerateSourceCode() const
{
	size_t index = base - 1;

	if (index < sizeof(csOoTTransitionType))
	{
		return StringHelper::Sprintf("CS_TRANSITION(%s, %i, %i),\n",
		                             csOoTTransitionType[index].c_str(), startFrame, endFrame);
	}

	return StringHelper::Sprintf("CS_TRANSITION(%i, %i, %i),\n", base, startFrame, endFrame);
}

size_t CutsceneCommand_Transition::GetCommandSize() const
{
	return 0x10;
}
