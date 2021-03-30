#pragma once

#include "../ZRoomCommand.h"

// TODO move into header and add all types
class AnimatedTexture
{
public:
	AnimatedTexture(std::vector<uint8_t> rawData, int rawDataIndex);

	int8_t segment;
	int16_t type;
	uint32_t segmentOffset;
};

class SetAnimatedTextureList : public ZRoomCommand
{
public:
	SetAnimatedTextureList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex);
	~SetAnimatedTextureList();

	std::string GetSourceOutputCode(std::string prefix);
	virtual std::string GenerateSourceCodePass1(std::string roomName, int baseAddress);
	virtual RoomCommand GetRoomCommand();
	virtual int32_t GetRawDataSize();
	virtual std::string GetCommandCName();
	virtual std::string GenerateExterns();

private:
	uint32_t segmentOffset;
	std::vector<AnimatedTexture*> textures;
	std::vector<uint8_t> _rawData;
	int _rawDataIndex;
};