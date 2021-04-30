#include "MemoryStream.h"

MemoryStream::MemoryStream()
{
	//buffer = std::make_shared<char[]>(new char[10](), [](char* p) { delete[] p; });
	//buffer = std::shared_ptr<char[]>(nBuffer, [](char *p) { delete[] p; });
	//bufferSize = nBufferSize;

	buffer = std::vector<char>();
	bufferSize = 0;
	BaseAddress = 0;
}

MemoryStream::MemoryStream(char* nBuffer, int32_t nBufferSize)
{
	//buffer = std::make_shared<char[]>(new char[10](), [](char* p) { delete[] p; });
	//buffer = std::shared_ptr<char[]>(nBuffer, [](char *p) { delete[] p; });
	buffer = std::vector<char>(nBuffer, nBuffer + nBufferSize);
	bufferSize = nBufferSize;
	BaseAddress = 0;
}

MemoryStream::~MemoryStream()
{
	int bp = 0;
}

uint64_t MemoryStream::GetLength()
{
	return buffer.size();
}

void MemoryStream::Seek(int32_t offset, SeekOffsetType seekType)
{
	if (seekType == SeekOffsetType::Start)
		BaseAddress = offset;
	else if (seekType == SeekOffsetType::Current)
		BaseAddress += offset;
	else if (seekType == SeekOffsetType::End)
		BaseAddress = bufferSize - 1 - offset;
}

std::unique_ptr<char[]> MemoryStream::Read(int32_t length)
{
	std::unique_ptr<char[]> result = std::make_unique<char[]>(length);

	memcpy_s(result.get(), length, &buffer[BaseAddress], length);
	BaseAddress += length;

	if (BaseAddress >= bufferSize)
		; // EXCEPTION

	return result;
}

int8_t MemoryStream::ReadByte()
{
	return buffer[BaseAddress++];
}

void MemoryStream::Write(char* srcBuffer, int32_t length)
{
	if (BaseAddress + length >= bufferSize)
	{
		buffer.resize(BaseAddress + length);
		bufferSize += length;
	}

	memcpy_s(&buffer[BaseAddress], length, srcBuffer, length);
	BaseAddress += length;

	if (BaseAddress >= bufferSize)
		; // EXCEPTION
}

void MemoryStream::WriteByte(int8_t value)
{
	if (BaseAddress >= bufferSize)
	{
		buffer.resize(BaseAddress + 1);
		bufferSize = BaseAddress;
	}

	buffer[BaseAddress++] = value;
}

std::vector<char> MemoryStream::ToVector()
{
	return buffer;
}

void MemoryStream::Flush()
{
}

void MemoryStream::Close()
{
}