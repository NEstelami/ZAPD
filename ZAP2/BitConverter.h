#pragma once

#include <stdint.h>
#include <vector>

class BitConverter
{
public:
	static int16_t ToInt16BE(std::vector<uint8_t> data, int offset)
	{
		return (data[offset + 1] << 8) + data[offset + 0];
	}

	static int16_t ToInt32BE(std::vector<uint8_t> data, int offset)
	{
		return (data[offset + 3] << 24) + (data[offset + 2] << 16) + (data[offset + 1] << 8) + data[offset + 0];
	}
};