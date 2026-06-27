/*
 * RE1.5 Rebuilt — TIM parser (Phase 4.3, 2026-05-18).
 *
 * Platform-agnostic parser. Pure bit-twiddling, no platform deps.
 * Lives in psx_dev/re15_reborn/src/ for PSX build, also copied/symlinked
 * into psx_dev/re15_reborn_pc/src/ for PC build (handled via CMake).
 */

#include "re15_tim.h"

static uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

int re15_tim_parse(const uint8_t *data, int size, re15_tim_t *out)
{
    if (size < 8) return -1;
    if (rd32(data + 0) != TIM_MAGIC) return -1;

    uint32_t flag = rd32(data + 4);
    int bpp_bits = (int)(flag & 0x07);
    int has_clut = (flag & TIM_FLAG_HAS_CLUT) ? 1 : 0;

    int bpp;
    switch (bpp_bits) {
        case TIM_BPP_4:  bpp = 4;  break;
        case TIM_BPP_8:  bpp = 8;  break;
        case TIM_BPP_16: bpp = 16; break;
        case TIM_BPP_24: bpp = 24; break;
        default: return -1;
    }

    out->bpp = bpp;
    out->has_clut = has_clut;
    out->clut = 0;
    out->clut_entries = 0;
    out->clut_x = out->clut_y = 0;

    int off = 8;
    if (has_clut) {
        if (size < off + 12) return -1;
        uint32_t clut_size = rd32(data + off + 0);
        uint16_t clut_x    = rd16(data + off + 4);
        uint16_t clut_y    = rd16(data + off + 6);
        uint16_t clut_w    = rd16(data + off + 8);
        uint16_t clut_h    = rd16(data + off + 10);

        out->clut_x = clut_x;
        out->clut_y = clut_y;
        out->clut_entries = (int)clut_w * (int)clut_h;
        out->clut = (const uint16_t *)(data + off + 12);

        if (size < (int)(off + clut_size)) return -1;
        off += (int)clut_size;
    }

    if (size < off + 12) return -1;
    /* uint32_t data_size = rd32(data + off + 0); */
    uint16_t data_x = rd16(data + off + 4);
    uint16_t data_y = rd16(data + off + 6);
    uint16_t data_w = rd16(data + off + 8);  /* in 16-bit units */
    uint16_t data_h = rd16(data + off + 10);

    out->data_x = data_x;
    out->data_y = data_y;

    /* Convert data_w (16-bit units) → pixel width by BPP */
    switch (bpp) {
        case 4:  out->width = (int)data_w * 4; break;
        case 8:  out->width = (int)data_w * 2; break;
        case 16: out->width = (int)data_w;     break;
        case 24: out->width = (int)data_w * 16 / 24; break;
    }
    out->height = data_h;
    out->pixels = (const uint16_t *)(data + off + 12);

    return 0;
}
