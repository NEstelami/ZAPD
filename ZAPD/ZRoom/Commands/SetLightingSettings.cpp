#include "SetLightingSettings.h"
#include "../../BitConverter.h"
#include "../../StringHelper.h"
#include "../../ZFile.h"
#include "../ZRoom.h"

using namespace std;

SetLightingSettings::SetLightingSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData,
                                         int rawDataIndex)
	: ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	ParseRawData();
}

void SetLightingSettings::ParseRawData()
{
	uint8_t numLights = cmdArg1;

	for (int i = 0; i < numLights; i++)
		settings.push_back(LightingSettings(rawData, segmentOffset + (i * 22)));

	if (numLights > 0)
	{
		string declaration = "";

		for (int i = 0; i < numLights; i++)
		{
			declaration += StringHelper::Sprintf(
				"\t{ %s }, // 0x%06X \n",
				settings.at(i).GetBodySourceCode().c_str(), segmentOffset + (i * 22));
		}

		parent->AddDeclarationArray(
			segmentOffset, DeclarationAlignment::None, DeclarationPadding::None, numLights * 22,
			"LightSettings",
			StringHelper::Sprintf("%sLightSettings0x%06X", zRoom->GetName().c_str(), segmentOffset),
			numLights, declaration);
	}
}

std::string SetLightingSettings::GetBodySourceCode()
{
	std::string listName = "NULL";
	if (segmentOffset != 0)
	{
		Declaration* decl = parent->GetDeclaration(segmentOffset);
		if (decl != nullptr)
		{
			listName = "&" + decl->varName;
		}
		else
		{
			listName = StringHelper::Sprintf("0x%08X", segmentOffset);
		}
	}

	return StringHelper::Sprintf("%s, %i, (u32)%s", GetCommandHex().c_str(), settings.size(), listName.c_str());
}

string SetLightingSettings::GetCommandCName()
{
	return "SCmdLightSettingList";
}

string SetLightingSettings::GenerateExterns()
{
	return StringHelper::Sprintf("extern LightSettings %sLightSettings0x%06X[];\n",
	                             zRoom->GetName().c_str(), segmentOffset);
}

RoomCommand SetLightingSettings::GetRoomCommand()
{
	return RoomCommand::SetLightingSettings;
}

LightingSettings::LightingSettings(const vector<uint8_t>& rawData, int rawDataIndex)
{
	ambientClrR = rawData.at(rawDataIndex + 0);
	ambientClrG = rawData.at(rawDataIndex + 1);
	ambientClrB = rawData.at(rawDataIndex + 2);

	diffuseClrA_R = rawData.at(rawDataIndex + 3);
	diffuseClrA_G = rawData.at(rawDataIndex + 4);
	diffuseClrA_B = rawData.at(rawDataIndex + 5);

	diffuseDirA_X = rawData.at(rawDataIndex + 6);
	diffuseDirA_Y = rawData.at(rawDataIndex + 7);
	diffuseDirA_Z = rawData.at(rawDataIndex + 8);

	diffuseClrB_R = rawData.at(rawDataIndex + 9);
	diffuseClrB_G = rawData.at(rawDataIndex + 10);
	diffuseClrB_B = rawData.at(rawDataIndex + 11);

	diffuseDirB_X = rawData.at(rawDataIndex + 12);
	diffuseDirB_Y = rawData.at(rawDataIndex + 13);
	diffuseDirB_Z = rawData.at(rawDataIndex + 14);

	fogClrR = rawData.at(rawDataIndex + 15);
	fogClrG = rawData.at(rawDataIndex + 16);
	fogClrB = rawData.at(rawDataIndex + 17);

	unk = BitConverter::ToInt16BE(rawData, rawDataIndex + 18);
	drawDistance = BitConverter::ToInt16BE(rawData, rawDataIndex + 20);
}

std::string LightingSettings::GetBodySourceCode() const
{
	return StringHelper::Sprintf("0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%04X, 0x%04X",
				ambientClrR, ambientClrG, ambientClrB,
				diffuseClrA_R, diffuseClrA_G, diffuseClrA_B,
				diffuseDirA_X, diffuseDirA_Y, diffuseDirA_Z,
				diffuseClrB_R, diffuseClrB_G, diffuseClrB_B,
				diffuseDirB_X, diffuseDirB_Y, diffuseDirB_Z,
				fogClrR, fogClrG, fogClrB, unk,
				drawDistance);
}
