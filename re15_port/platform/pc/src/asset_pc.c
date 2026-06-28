/*
 * RE1.5 Rebuilt — PC asset loader (Phase 4.3, 2026-05-18).
 *
 * Reads files from disk (extracted RE1.5 assets in ../assets/).
 * TIM blit into the software framebuffer.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "re15_engine.h"
#include "re15_tim.h"

/* Defined in render_pc.c — exposed via extern for now (Phase 4.4 will refactor) */
extern uint32_t *re15_pc_framebuffer(void);
extern void      re15_pc_put_pixel(int x, int y, uint32_t rgba);

/* Read entire file into a malloc'd buffer. Returns NULL on failure. */
uint8_t *re15_asset_read_file(const char *path, int *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "[asset] cannot open %s\n", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) {
        fclose(f);
        return NULL;
    }
    uint8_t *buf = (uint8_t *) malloc((size_t)sz);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (out_size) *out_size = (int)sz;
    return buf;
}

/* Convert RGB555 (PSX color format) → RGBA8888 */
static uint32_t rgb555_to_rgba8888(uint16_t c)
{
    if (c == 0) return 0x00000000u; /* transparent */
    int r = ((c >>  0) & 0x1F) << 3;
    int g = ((c >>  5) & 0x1F) << 3;
    int b = ((c >> 10) & 0x1F) << 3;
    return ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFFu;
}

void re15_tim_blit_pc(const re15_tim_t *tim, int dst_x, int dst_y)
{
    if (!tim) return;

    if (tim->bpp == 8 && tim->has_clut) {
        /* 8bpp: 1 byte per pixel, indexed into CLUT */
        const uint8_t *src = (const uint8_t *) tim->pixels;
        for (int y = 0; y < tim->height; y++) {
            for (int x = 0; x < tim->width; x++) {
                uint8_t idx = src[y * tim->width + x];
                if (idx == 0) continue; /* transparent */
                uint16_t c = tim->clut[idx];
                re15_pc_put_pixel(dst_x + x, dst_y + y, rgb555_to_rgba8888(c));
            }
        }
    } else if (tim->bpp == 4 && tim->has_clut) {
        /* 4bpp: 2 pixels per byte (low nibble first) */
        const uint8_t *src = (const uint8_t *) tim->pixels;
        for (int y = 0; y < tim->height; y++) {
            for (int x = 0; x < tim->width; x++) {
                int byte_idx = (y * tim->width + x) / 2;
                int nibble = (x & 1) ? (src[byte_idx] >> 4) : (src[byte_idx] & 0xF);
                if (nibble == 0) continue;
                uint16_t c = tim->clut[nibble];
                re15_pc_put_pixel(dst_x + x, dst_y + y, rgb555_to_rgba8888(c));
            }
        }
    } else if (tim->bpp == 16) {
        /* 16bpp direct color RGB555 */
        const uint16_t *src = tim->pixels;
        for (int y = 0; y < tim->height; y++) {
            for (int x = 0; x < tim->width; x++) {
                uint16_t c = src[y * tim->width + x];
                if (c == 0) continue;
                re15_pc_put_pixel(dst_x + x, dst_y + y, rgb555_to_rgba8888(c));
            }
        }
    } else {
        fprintf(stderr, "[tim] unsupported bpp=%d clut=%d\n", tim->bpp, tim->has_clut);
    }
}
