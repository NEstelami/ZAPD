#include "ZRoom.h"
#include "Commands/SetEchoSettings.h"
#include "Commands/SetSoundSettings.h"
#include "Commands/SetWind.h"
#include "Commands/SetTimeSettings.h"
#include "Commands/SetSpecialObjects.h"
#include "Commands/SetSkyboxSettings.h"
#include "Commands/SetSkyboxModifier.h"
#include "Commands/SetRoomBehavior.h"
#include "Commands/SetCameraSettings.h"

using namespace std;
using namespace tinyxml2;

ZRoom::ZRoom(XMLElement* reader, vector<uint8_t> nRawData, int rawDataIndex, string nRelPath)
{
	commands = vector<ZRoomCommand*>();

	rawData = nRawData;

	bool shouldContinue = true;

	while (shouldContinue)
	{
		RoomCommand opcode = (RoomCommand)rawData[rawDataIndex];

		if (opcode == RoomCommand::EndMarker)
			shouldContinue = false;
		else
		{
			switch (opcode)
			{
			case RoomCommand::SetEchoSettings: commands.push_back(new SetEchoSettings(rawData, rawDataIndex)); break;
			case RoomCommand::SetSoundSettings: commands.push_back(new SetSoundSettings(rawData, rawDataIndex)); break;
			case RoomCommand::SetWind: commands.push_back(new SetWind(rawData, rawDataIndex)); break;
			case RoomCommand::SetTimeSettings: commands.push_back(new SetTimeSettings(rawData, rawDataIndex)); break;
			case RoomCommand::SetSpecialObjects: commands.push_back(new SetSpecialObjects(rawData, rawDataIndex)); break;
			case RoomCommand::SetSkyboxSettings: commands.push_back(new SetSkyboxSettings(rawData, rawDataIndex)); break;
			case RoomCommand::SetSkyboxModifier: commands.push_back(new SetSkyboxModifier(rawData, rawDataIndex)); break;
			case RoomCommand::SetRoomBehavior: commands.push_back(new SetRoomBehavior(rawData, rawDataIndex)); break;
			case RoomCommand::SetCameraSettings: commands.push_back(new SetCameraSettings(rawData, rawDataIndex)); break;
			}
		}

		rawDataIndex += 8;
	}
}

vector<uint8_t> ZRoom::GetRawData()
{
	return rawData;
}

int ZRoom::GetRawDataSize()
{
	return 0;
}
