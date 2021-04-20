#pragma once

#include "../ZRoomCommand.h"

class LightingSettings
{
public:
	LightingSettings(const std::vector<uint8_t>& rawData, int rawDataIndex);

	std::string GetBodySourceCode() const;

protected:
	uint8_t ambientClrR, ambientClrG, ambientClrB;
	uint8_t diffuseClrA_R, diffuseClrA_G, diffuseClrA_B;
	uint8_t diffuseDirA_X, diffuseDirA_Y, diffuseDirA_Z;
	uint8_t diffuseClrB_R, diffuseClrB_G, diffuseClrB_B;
	uint8_t diffuseDirB_X, diffuseDirB_Y, diffuseDirB_Z;
	uint8_t fogClrR, fogClrG, fogClrB;
	uint16_t unk;
	uint16_t drawDistance;
};

class SetLightingSettings : public ZRoomCommand
{
public:
	SetLightingSettings(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);

	void ParseRawData() override;

	std::string GetBodySourceCode() override;
	std::string GenerateExterns() override;

	RoomCommand GetRoomCommand() override;
	std::string GetCommandCName() override;

private:
	std::vector<LightingSettings> settings;
};
