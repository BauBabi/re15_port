/*
 * RE1.5 Rebuilt — TIM parser API (Phase 4.3, 2026-05-18).
 *
 * TIM = PSX texture format. 4/8/16/24 BPP, optional CLUT (color lookup table).
 * Reference: Sony PSX SDK docs + de.re15.extractors.tim Java parser.
 *
 * File layout:
 *   [0x00] u32 magic        = 0x00000010
 *   [0x04] u32 flag         = bits 0..2 = bpp (0=4,1=8,2=16,3=24), bit 3 = has CLUT
 *
 *   If has CLUT:
 *     [0x08] u32 clut_size   (total bytes including this 12-byte header)
 *     [0x0C] u16 clut_x      (VRAM coords)
 *     [0x0E] u16 clut_y
 *     [0x10] u16 clut_width  (entries)
 *     [0x12] u16 clut_height (rows)
 *     [0x14] u16[N] CLUT entries (RGB555)
 *
 *   Then image data section:
 *     [N+0x00] u32 data_size  (total bytes including this 12-byte header)
 *     [N+0x04] u16 data_x     (VRAM coords)
 *     [N+0x06] u16 data_y
 *     [N+0x08] u16 data_width (in 16-bit units! actual pixel width depends on bpp)
 *     [N+0x0A] u16 data_height
 *     [N+0x0C] u16[...] pixel data
 *
 * Pixel width by BPP (data_width is in 16-bit units):
 *   4bpp:  pixel_w = data_width * 4
 *   8bpp:  pixel_w = data_width * 2
 *   16bpp: pixel_w = data_width
 *   24bpp: pixel_w = data_width * 16 / 24
 */
#ifndef RE15_TIM_H
#define RE15_TIM_H

#include <stdint.h>

#define TIM_MAGIC      0x00000010
#define TIM_BPP_4       0
#define TIM_BPP_8       1
#define TIM_BPP_16      2
#define TIM_BPP_24      3
#define TIM_FLAG_HAS_CLUT 0x08

typedef struct {
    int bpp;               /* 4/8/16/24 */
    int has_clut;          /* 1 if CLUT present */
    int clut_x, clut_y;    /* VRAM coords */
    int clut_entries;      /* total CLUT colors */
    const uint16_t *clut;  /* pointer into source data, RGB555 */

    int data_x, data_y;    /* VRAM coords */
    int width;             /* in pixels */
    int height;            /* in pixels */
    const uint16_t *pixels; /* pointer into source data */
} re15_tim_t;

/* Parse a TIM blob in memory. Returns 0 on success, -1 on parse error.
 * Does not copy data — fields point into `data`. */
int re15_tim_parse(const uint8_t *data, int size, re15_tim_t *out);

/* PC-side helper: blit a TIM into the software framebuffer at (x, y).
 * Only available on PC target. Supports 4/8/16 BPP. */
#ifdef RE15_PLATFORM_PC
void re15_tim_blit_pc(const re15_tim_t *tim, int x, int y);
#endif

#endif
