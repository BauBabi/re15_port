#ifndef UTILS_H
#define UTILS_H

#include "BioModels.h"

inline bool StrIgnoreCaseEquals(const char* str1, const char* str2)
{
	while (*str1 && *str2)
	{
		if (toupper(*str1++) != toupper(*str2++))
		{
			return false;
		}
	}
	return *str1 == *str2;
}

struct BitReader
{
	u8* stream;
	u8 bitOffset;

	BitReader(u8* data) : stream(data), bitOffset(0) {}

	inline u8 Read()
	{
		u8 bit = (*stream >> bitOffset) & 1;
		if (++bitOffset == 8)
		{
			bitOffset = 0;
			stream++;
		}

		return bit;
	}

	inline u32 Read(u32 count)
	{
		ASSERT(count <= 32);
		u32 result = 0;

		for (u32 bitIndex = 0; bitIndex < count; bitIndex++)
		{
			result |= (Read() << bitIndex);
		}

		return result;
	}
};

struct StreamReader
{
	virtual u32 GetPos() = 0;
	virtual void SetPos(u32 offset, int origin) = 0;
	virtual void Read(void* dst, size_t size, size_t count) = 0;
};

struct FileStreamReader : StreamReader
{
	FILE* file;

	FileStreamReader(): file(nullptr) {}

	FileStreamReader(const char* filepath)
	{
		Load(filepath);
	}

	~FileStreamReader()
	{
		fclose(file);
	}

	inline void Load(const char* filepath)
	{
		file = fopen(filepath, "rb");
		ASSERT(file);
	}

	inline u32 GetPos() override
	{
		return ftell(file);
	}

	inline void SetPos(u32 offset, int origin) override
	{
		ASSERT(origin == SEEK_SET || origin == SEEK_CUR);
		fseek(file, offset, origin);
	}

	inline void Read(void* dst, size_t size, size_t count) override
	{
		fread(dst, size, count, file);
	}
};

struct MemoryStreamReader : StreamReader
{
	u8* memory;
	u8* beginBlock;

	MemoryStreamReader(u8* memory) : memory(memory), beginBlock(memory)
	{
		ASSERT(memory);
	}

	inline u32 GetPos() override
	{
		return (u32)(memory - beginBlock);
	}

	inline void SetPos(u32 offset, int origin) override
	{
		ASSERT(origin == SEEK_SET || origin == SEEK_CUR);
		if (origin == SEEK_SET)
		{
			memory = beginBlock + offset;
		}
		else
		{
			memory += offset;
		}
	}

	inline void Read(void* dst, size_t size, size_t count) override
	{
		memcpy(dst, memory, size * count);
		memory += size * count;
	}
};

#endif