#include "ZCutscene.h"

#include "Globals.h"
#include "Utils/BitConverter.h"
#include "Utils/StringHelper.h"
#include "WarningHandler.h"
#include "ZResource.h"

REGISTER_ZFILENODE(Cutscene, ZCutscene);

ZCutscene::ZCutscene(ZFile* nParent) : ZCutsceneBase(nParent)
{
}

ZCutscene::~ZCutscene()
{
	for (CutsceneCommand* cmd : commands)
		delete cmd;
}

std::string ZCutscene::GetBodySourceCode() const
{
	std::string output = "";
	uint32_t curPtr = 0;

	output += StringHelper::Sprintf("    CS_BEGIN_CUTSCENE(%i, %i),\n", commands.size(), endFrame);

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];
		output += "    " + cmd->GenerateSourceCode();
		curPtr += cmd->GetCommandSize();
	}

	output += StringHelper::Sprintf("    CS_END(),\n", commands.size(), endFrame);

	return output;
}

size_t ZCutscene::GetRawDataSize() const
{
	size_t size = 0;

	// Beginning
	size += 8;

	for (size_t i = 0; i < commands.size(); i++)
	{
		CutsceneCommand* cmd = commands[i];
		size += cmd->GetCommandSize();
	}

	// End
	size += 8;

	return size;
}

void ZCutscene::ParseRawData()
{
	ZResource::ParseRawData();

	const auto& rawData = parent->GetRawData();

	numCommands = BitConverter::ToInt32BE(rawData, rawDataIndex + 0);
	commands = std::vector<CutsceneCommand*>();

	endFrame = BitConverter::ToInt32BE(rawData, rawDataIndex + 4);
	uint32_t currentPtr = rawDataIndex + 8;

	for (int32_t i = 0; i < numCommands; i++)
	{
		int32_t id = BitConverter::ToInt32BE(rawData, currentPtr);

		if (id == -1)
			break;

		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		{
			printf("Cutscene Command: 0x%X (%i)\n", id, id);
		}

		CutsceneCommands cmdID = (CutsceneCommands)GetCommandFromID(id);
		currentPtr += 4;

		CutsceneCommand* cmd = nullptr;

		switch (cmdID)
		{
		case CutsceneCommands::Cmd00:
			break;
		case CutsceneCommands::SetCameraPos:
			cmd = new CutsceneCommandSetCameraPos(rawData, currentPtr);
			break;
		case CutsceneCommands::SetCameraFocus:
			cmd = new CutsceneCommandSetCameraPos(rawData, currentPtr);
			break;
		case CutsceneCommands::SpecialAction:
			cmd = new CutsceneCommandSpecialAction(rawData, currentPtr);
			break;
		case CutsceneCommands::SetLighting:
			cmd = new CutsceneCommandEnvLighting(rawData, currentPtr);
			break;
		case CutsceneCommands::SetCameraPosLink:
			cmd = new CutsceneCommandSetCameraPos(rawData, currentPtr);
			break;
		case CutsceneCommands::SetCameraFocusLink:
			cmd = new CutsceneCommandSetCameraPos(rawData, currentPtr);
			break;
		case CutsceneCommands::Cmd07:
			break;
		case CutsceneCommands::Cmd08:
			break;
		case CutsceneCommands::Cmd09:
			cmd = new CutsceneCommandUnknown9(rawData, currentPtr);
			break;
		case CutsceneCommands::Textbox:
			cmd = new CutsceneCommandTextbox(rawData, currentPtr);
			break;
		case CutsceneCommands::Unknown:
			cmd = new CutsceneCommandUnknown(rawData, currentPtr);
			break;
		case CutsceneCommands::SetActorAction0:
		case CutsceneCommands::SetActorAction1:
		case CutsceneCommands::SetActorAction2:
		case CutsceneCommands::SetActorAction3:
		case CutsceneCommands::SetActorAction4:
		case CutsceneCommands::SetActorAction5:
		case CutsceneCommands::SetActorAction6:
		case CutsceneCommands::SetActorAction7:
		case CutsceneCommands::SetActorAction8:
		case CutsceneCommands::SetActorAction9:
		case CutsceneCommands::SetActorAction10:
			cmd = new CutsceneCommandActorAction(rawData, currentPtr);
			break;
		case CutsceneCommands::SetSceneTransFX:
			cmd = new CutsceneCommandSceneTransFX(rawData, currentPtr);
			break;
		case CutsceneCommands::PlayBGM:
			cmd = new CutsceneCommandPlayBGM(rawData, currentPtr);
			break;
		case CutsceneCommands::StopBGM:
			cmd = new CutsceneCommandStopBGM(rawData, currentPtr);
			break;
		case CutsceneCommands::FadeBGM:
			cmd = new CutsceneCommandFadeBGM(rawData, currentPtr);
			break;
		case CutsceneCommands::SetTime:
			cmd = new CutsceneCommand_SetTime(rawData, currentPtr);
			break;
		case CutsceneCommands::Terminator:
			cmd = new CutsceneCommandTerminator(rawData, currentPtr);
			break;
		case CutsceneCommands::End:
			cmd = new CutsceneCommandEnd(rawData, currentPtr);
			break;
		case CutsceneCommands::Error:
			HANDLE_ERROR_RESOURCE(
				WarningType::NotImplemented, parent, this, rawDataIndex,
				StringHelper::Sprintf("Cutscene command (0x%X) not implemented", cmdID),
				StringHelper::Sprintf(
					"Command ID: 0x%X\nIndex: %d\ncurrentPtr-rawDataIndex: 0x%X", id, i,
					currentPtr - rawDataIndex));
			break;
		}

		cmd->commandIndex = i;
		cmd->SetCommandID(id);
		size_t commmandSize = cmd->GetCommandSize();
		if (Globals::Instance->verbosity >= VerbosityLevel::VERBOSITY_DEBUG)
		{
			printf("\t Command size: 0x%zX (%zu)\n", commmandSize, commmandSize);
		}
		currentPtr += commmandSize - 4;

		commands.push_back(cmd);
	}
	
}

CutsceneCommands ZCutscene::GetCommandFromID(int32_t id)
{
	switch (id)
	{
	case 0x0003:
		return CutsceneCommands::SpecialAction;
	case 0x0004:
		return CutsceneCommands::SetLighting;
	case 0x0056:
		return CutsceneCommands::PlayBGM;
	case 0x0057:
		return CutsceneCommands::StopBGM;
	case 0x007C:
		return CutsceneCommands::FadeBGM;
	case 0x0009:
		return CutsceneCommands::Cmd09;
	case 0x0013:
		return CutsceneCommands::Textbox;
	case 0x008C:
		return CutsceneCommands::SetTime;
	case 0x0001:
		return CutsceneCommands::SetCameraPos;
	case 0x0002:
		return CutsceneCommands::SetCameraFocus;
	case 0x0005:
		return CutsceneCommands::SetCameraPosLink;
	case 0x0006:
		return CutsceneCommands::SetCameraFocusLink;
	case 0x0007:
		return CutsceneCommands::Cmd07;
	case 0x0008:
		return CutsceneCommands::Cmd08;
	case 0x03E8:
		return CutsceneCommands::Terminator;
	case 0xFFFF:
		return CutsceneCommands::End;
	case 0x002D:
		return CutsceneCommands::SetSceneTransFX;
	case 10:
		return CutsceneCommands::SetActorAction0;
	case 15:
	case 17:
	case 18:
	case 23:
	case 34:
	case 39:
	case 46:
	case 76:
	case 85:
	case 93:
	case 105:
	case 107:
	case 110:
	case 119:
	case 123:
	case 138:
	case 139:
	case 144:
		return CutsceneCommands::SetActorAction1;
	case 14:
	case 16:
	case 24:
	case 35:
	case 40:
	case 48:
	case 64:
	case 68:
	case 70:
	case 78:
	case 80:
	case 94:
	case 116:
	case 118:
	case 120:
	case 125:
	case 131:
	case 141:
		return CutsceneCommands::SetActorAction2;
	case 25:
	case 36:
	case 41:
	case 50:
	case 67:
	case 69:
	case 72:
	case 81:
	case 106:
	case 117:
	case 121:
	case 126:
	case 132:
		return CutsceneCommands::SetActorAction3;
	case 29:
	case 37:
	case 42:
	case 51:
	case 53:
	case 63:
	case 65:
	case 66:
	case 75:
	case 82:
	case 108:
	case 127:
	case 133:
		return CutsceneCommands::SetActorAction4;
	case 30:
	case 38:
	case 43:
	case 47:
	case 54:
	case 79:
	case 83:
	case 128:
	case 135:
		return CutsceneCommands::SetActorAction5;
	case 44:
	case 55:
	case 77:
	case 84:
	case 90:
	case 129:
	case 136:
		return CutsceneCommands::SetActorAction6;
	case 31:
	case 52:
	case 57:
	case 58:
	case 88:
	case 115:
	case 130:
	case 137:
		return CutsceneCommands::SetActorAction7;
	case 49:
	case 60:
	case 89:
	case 111:
	case 114:
	case 134:
	case 142:
		return CutsceneCommands::SetActorAction8;
	case 62:
		return CutsceneCommands::SetActorAction9;
	case 143:
		return CutsceneCommands::SetActorAction10;
	case 0x0B:
	case 0x0D:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x20:
	case 0x21:
	case 0x3B:
	case 0x3D:
	case 0x47:
	case 0x49:
	case 0x6D:
	case 0x15:
	case 0x16:
	case 0x70:
	case 0x71:
	case 0x4A:
		return CutsceneCommands::Unknown;
	}

	HANDLE_WARNING_RESOURCE(
		WarningType::NotImplemented, parent, this, rawDataIndex,
		StringHelper::Sprintf("could not identify cutscene command. ID 0x%04X", id), "");

	return CutsceneCommands::Error;
}

ZCutsceneBase::ZCutsceneBase(ZFile* nParent) : ZResource(nParent)
{
}

Declaration* ZCutsceneBase::DeclareVar(const std::string& prefix, const std::string& bodyStr)
{
	std::string auxName = name;

	if (auxName == "")
		auxName = GetDefaultName(prefix);

	Declaration* decl =
		parent->AddDeclarationArray(rawDataIndex, GetDeclarationAlignment(), GetRawDataSize(),
	                                GetSourceTypeName(), auxName, 0, bodyStr);
	decl->staticConf = staticConf;
	return decl;
}

std::string ZCutsceneBase::GetSourceTypeName() const
{
	return "CutsceneData";
}

ZResourceType ZCutsceneBase::GetResourceType() const
{
	return ZResourceType::Cutscene;
}
