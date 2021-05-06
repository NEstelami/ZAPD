#pragma once

#include <stdint.h>
#include <memory>

enum class SeekOffsetType
{
	Start,
	Current,
	End
};

class Stream
{
public:
	virtual uint64_t GetLength() = 0;
	uint64_t GetBaseAddress() { return baseAddress; }

	//Stream();

	virtual void Seek(int32_t offset, SeekOffsetType seekType) = 0;

	virtual std::unique_ptr<char[]> Read(int32_t length) = 0;
	virtual int8_t ReadByte() = 0;

	virtual void Write(char* destBuffer, int32_t length) = 0;
	virtual void WriteByte(int8_t value) = 0;

	virtual void Flush() = 0;
	virtual void Close() = 0;

protected:
	int64_t baseAddress;
};