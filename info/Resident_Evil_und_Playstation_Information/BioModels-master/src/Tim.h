#ifndef TIM_H
#define TIM_H

#include "BioModels.h"
#include "Utils.h"

struct TimHeader
{
	u32 magic;
	u32 type;
	u32	offset;
	u16	imgOrgX;
	u16	imgOrgY;
	u16	clutColors;
	u16	clutCount;
};

struct TimClutHeader
{
	u32 size;
	u16 originX;
	u16 originY;
	u16 width;
	u16 height;
};

u32* LoadTIMAsRGBA(StreamReader* streamReader, u32& width, u32& height, u32& clutCount);

#endif