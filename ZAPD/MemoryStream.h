#pragma once

#include "Stream.h"
#include <memory>
#include <vector>

class MemoryStream : public Stream
{
public:
	MemoryStream();
	MemoryStream(char* nBuffer, int32_t nBufferSize);
	~MemoryStream();

	uint64_t GetLength() override;

	void Seek(int32_t offset, SeekOffsetType seekType);

	std::unique_ptr<char[]> Read(int32_t length);
	int8_t ReadByte();

	void Write(char* srcBuffer, int32_t length);
	void WriteByte(int8_t value);

	std::vector<char> ToVector();

	void Flush();
	void Close();

protected:
	//std::shared_ptr<char[]> buffer;
	std::vector<char> buffer;
	int32_t bufferSize;
};