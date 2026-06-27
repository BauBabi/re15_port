#include "Tim.h"

u32* LoadTIMAsRGBA(StreamReader* streamReader, u32& width, u32& height, u32& clutCount)
{
	TimHeader timHeader;
	TimClutHeader clutHeader;

	streamReader->Read(&timHeader, sizeof(TimHeader), 1);
	ASSERT(timHeader.magic == 0x10);
	ASSERT(timHeader.type == 0x09);

	clutCount = timHeader.clutCount;
	u32 clutSize = clutCount * timHeader.clutColors;
	u16* cluts = new u16[clutSize];
	streamReader->Read(cluts, sizeof(u16), clutSize);

	streamReader->Read(&clutHeader, sizeof(TimClutHeader), 1);

	switch (timHeader.type)
	{
	case 0x08: width = clutHeader.width * 4; break;
	case 0x09: width = clutHeader.width * 2; break;
	case 0x02: width = clutHeader.width; break;
	default: ASSERT(0); break;
	}

	height = clutHeader.height;
	u32* pixels = new u32[width * height];
	u32 clutWidth = width / clutCount;

	u32* pixelPtr = (u32*)pixels;
	for (u32 y = 0; y < height; y++)
	{
		for (u32 x = 0; x < width; x++)
		{
			u8 clutIndexByte;
			streamReader->Read(&clutIndexByte, sizeof(u8), 1);

			int clutIndex = x / clutWidth;
			u16 color = cluts[(clutIndex * timHeader.clutColors) + clutIndexByte];

			// RGBA encoding
			u8 r = (color & 0x1F) * 255 / 31;
			u8 g = ((color >> 5) & 0x1F) * 255 / 31;
			u8 b = ((color >> 10) & 0x1F) * 255 / 31;
			u8 a = 255;
			// Set black as transparent
			if (r == 0 && g == 0 && b == 0)
			{
				a = 0;
			}
			*pixelPtr++ = (a << 24) | (b << 16) | (g << 8) | r;
		}
	}

	delete[] cluts;

	return pixels;
}