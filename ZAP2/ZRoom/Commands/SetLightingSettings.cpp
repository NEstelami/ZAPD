#include "SetLightingSettings.h"
#include "../ZRoom.h"
#include "../../BitConverter.h"

using namespace std;

SetLightingSettings::SetLightingSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	uint8_t numLights = rawData[rawDataIndex + 1];
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;

	for (int i = 0; i < numLights; i++)
	{
		settings.push_back(new LightingSettings(rawData, segmentOffset + (i * 22)));
	}

	if (numLights > 0)
	{
		char line[2048];
		string declaration = "";

		sprintf(line, "LightSettings _%s_lightSettings_%08X[] = \n{\n", zRoom->GetName().c_str(), segmentOffset);
		declaration += line;

		for (int i = 0; i < numLights; i++)
		{
			sprintf(line, "\t{ 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%04X, 0x%04X }, // 0x%08X \n",
				settings[i]->ambientClrR, settings[i]->ambientClrG, settings[i]->ambientClrB,
				settings[i]->diffuseClrA_R, settings[i]->diffuseClrA_G, settings[i]->diffuseClrA_B,
				settings[i]->diffuseDirA_X, settings[i]->diffuseDirA_Y, settings[i]->diffuseDirA_Z,
				settings[i]->diffuseClrB_R, settings[i]->diffuseClrB_G, settings[i]->diffuseClrB_B,
				settings[i]->diffuseDirB_X, settings[i]->diffuseDirB_Y, settings[i]->diffuseDirB_Z,
				settings[i]->fogClrR, settings[i]->fogClrG, settings[i]->fogClrB,
				settings[i]->unk, settings[i]->drawDistance, segmentOffset + (i * 22));

			declaration += line;
		}

		declaration += "};\n";

		zRoom->declarations[segmentOffset] = new Declaration(DeclarationAlignment::None, DeclarationPadding::Pad16, numLights * 22, declaration);

		/*int curPtr = segmentOffset + (numLights * 22);

		while (curPtr % 16 != 0)
		{
			declaration = "";

			sprintf(line, "static u32 padding%02X = 0;\n", curPtr);
			declaration += line;

			zRoom->declarations[curPtr] = new Declaration(DeclarationAlignment::None, 4, declaration);

			curPtr += 4;
		}*/
	}
}

string SetLightingSettings::GenerateSourceCodePass1(string roomName, int baseAddress)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s %i, (u32)&_%s_lightSettings_%08X};", ZRoomCommand::GenerateSourceCodePass1(roomName, baseAddress).c_str(), settings.size(), zRoom->GetName().c_str(), segmentOffset);
	sourceOutput = line;

	return sourceOutput;
}

string SetLightingSettings::GenerateSourceCodePass2(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	return sourceOutput;
}

string SetLightingSettings::GetCommandCName()
{
	return "SCmdLightSettingList";
}

string SetLightingSettings::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "extern LightSettings _%s_lightSettings_%08X[];\n", zRoom->GetName().c_str(), segmentOffset);
	sourceOutput = line;

	return sourceOutput;
}

RoomCommand SetLightingSettings::GetRoomCommand()
{
	return RoomCommand::SetLightingSettings;
}

LightingSettings::LightingSettings(vector<uint8_t> rawData, int rawDataIndex)
{
	ambientClrR = rawData[rawDataIndex + 0];
	ambientClrG = rawData[rawDataIndex + 1];
	ambientClrB = rawData[rawDataIndex + 2];

	diffuseClrA_R = rawData[rawDataIndex + 3];
	diffuseClrA_G = rawData[rawDataIndex + 4];
	diffuseClrA_B = rawData[rawDataIndex + 5];

	diffuseDirA_X = rawData[rawDataIndex + 6];
	diffuseDirA_Y = rawData[rawDataIndex + 7];
	diffuseDirA_Z = rawData[rawDataIndex + 8];

	diffuseClrB_R = rawData[rawDataIndex + 9];
	diffuseClrB_G = rawData[rawDataIndex + 10];
	diffuseClrB_B = rawData[rawDataIndex + 11];

	diffuseDirB_X = rawData[rawDataIndex + 12];
	diffuseDirB_Y = rawData[rawDataIndex + 13];
	diffuseDirB_Z = rawData[rawDataIndex + 14];

	fogClrR = rawData[rawDataIndex + 15];
	fogClrG = rawData[rawDataIndex + 16];
	fogClrB = rawData[rawDataIndex + 17];

	unk = BitConverter::ToInt16BE(rawData, rawDataIndex + 18);
	drawDistance = BitConverter::ToInt16BE(rawData, rawDataIndex + 20);

}