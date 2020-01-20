#pragma once

#include <stdint.h>
#include <vector>

class BitConverter
{
public:
	static int16_t ToInt16BE(std::vector<uint8_t> data, int offset)
	{
		//return (data[offset + 1] << 8) + data[offset + 0];
		return (data[offset + 0] << 8) + data[offset + 1];
	}

	static int32_t ToInt32BE(std::vector<uint8_t> data, int offset)
	{
		//return (data[offset + 3] << 24) + (data[offset + 2] << 16) + (data[offset + 1] << 8) + data[offset + 0];
		return (data[offset + 0] << 24) + (data[offset + 1] << 16) + (data[offset + 2] << 8) + data[offset + 3];
	}

	static int64_t ToInt64BE(std::vector<uint8_t> data, int offset)
	{
		return (data[offset + 0] << 56) + (data[offset + 1] << 48) + (data[offset + 2] << 40) + (data[offset + 3] << 32) + (data[offset + 4] << 24) + (data[offset + 5] << 16) + (data[offset + 6] << 8) + (data[offset + 7]);
	}
};