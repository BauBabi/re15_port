// FONT Library for PSX
//
// Simple font printing thing.
// 
// Need: 8x8 pix 16-colour font printer           * done
// Nice: NxN pix 16-colour font printer           * done
// Nice: Scale and intensity control              * done
// Nice: Rotation                                 * ugh!

#include <psxtypes.h>
#include <gpu.h>

static TEXQUAD      fontQuad;
static TEXCOORDS    tc[256];
static PsxUInt8     fontwidth, fontheight;
static PsxUInt8     charwidth, charheight;

// Init
void InitFont(TIMHEADER *fontTIM, int dx, int dy, int n)
{
    PsxUInt32   i;
    TIMCLUT	*clut;
    TIMIMAGE	*image;

    // Initialise all as spaces
    for (i = 0; i < 256; i++)
    {
        tc[i].u = tc[i].v = 0;
    }

    // Setup chars from 32 on ...
    for (i = 32; i < 256; i++)
    {
        PsxUInt32 offset = i - ' ';		// start at space char
        tc[i].u = (offset % n) * dx;
        tc[i].v = (offset / n) * dy;
    }

    // Setup TIM for font
    fontQuad.color.r = fontQuad.color.g = fontQuad.color.b = 255;
    // upload TIME image for the font
    UpLoadImage(fontTIM);
    // get font TIM details for the fontQuad
    clut = GPU_TIMGetClut(fontTIM);
    image = GPU_TIMGetImage(fontTIM);

    fontQuad.clutId = GPU_CalcClutID(clut->vRamPos.x,
                                     clut->vRamPos.y);
    fontQuad.texPage = GPU_CalcTexturePage(image->vRamPos.x,
                                           image->vRamPos.y, 0);		// last val = ??? (clut mode)
	// get from TIMHEADER flags

    fontwidth = dx; fontheight = dy;
    charwidth = dx; charheight = dy;		// adjustable !!!
}

// Set character size (for scaling chars)
void SetCharSize(PsxUInt8 width, PsxUInt8 height)
{
    charwidth = width;
    charheight = height;
}

// Change "lighting" of characters
void SetCharLight(PsxUInt8 r, PsxUInt8 g, PsxUInt8 b)
{
    fontQuad.color.r = r;
    fontQuad.color.g = g;
    fontQuad.color.b = b;
}

// print char to ot
void PrintChar(GsOT *ot, PsxUInt16 pos, PsxUInt16 xPos, PsxUInt16 yPos,
          PsxUInt16 chr)
{
    fontQuad.texCoords0.u = fontQuad.texCoords1.u = tc[chr].u;
    fontQuad.texCoords0.v = fontQuad.texCoords2.v = tc[chr].v;
    fontQuad.texCoords2.u = fontQuad.texCoords3.u = tc[chr].u + fontwidth;
    fontQuad.texCoords1.v = fontQuad.texCoords3.v = tc[chr].v + fontheight;

    fontQuad.vert0.x = fontQuad.vert1.x = xPos;
    fontQuad.vert0.y = fontQuad.vert2.y = yPos;
    fontQuad.vert2.x = fontQuad.vert3.x = xPos + charwidth;
    fontQuad.vert1.y = fontQuad.vert3.y = yPos + charheight;

    GPU_SortTexQuad(ot, pos, &fontQuad);
}


// print a string of chars 
void PrintString(GsOT *ot, PsxUInt16 pos, PsxUInt16 xPos, PsxUInt16 yPos, char *string)
{
    while ((xPos < 320 ) && *string)
    {
        PrintChar(ot, pos, xPos, yPos, *string);

        string++;
        xPos += charwidth;
    }
}


// upload TIM object to VRAM
void UpLoadImage(TIMHEADER *timhdr)
{
    TIMCLUT     *clut;
    TIMIMAGE    *image;

    /* Get the components of the image */
    clut = GPU_TIMGetClut(timhdr);
    image = GPU_TIMGetImage(timhdr);

    /* Load the image up to the playstation */
    if (clut)
    {
        GPU_LoadImageData(&clut->vRamPos, clut->clutData);
    }
    if (image)
    {
        GPU_LoadImageData(&image->vRamPos, image->imageData);
    }
}
