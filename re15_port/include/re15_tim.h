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

/* --- Texel-Transparenz beim TIM->RGBA-Upload (PC-Renderer) -------------------
 * Welche Texel gelten als "nicht zeichnen"? Auf echter Hardware entscheidet das
 * die GPU allein am aufgeloesten 16-Bit-Texel-Wert; der Port muss das beim
 * Dekodieren in RGBA vorwegnehmen, weil SDL keinen Farbschluessel kennt.
 *
 *  RE15_TIM_KEY_NONE   Index 0 wird OPAK gezeichnet (CLUT[0]-Farbe). Alt-Verhalten
 *      der Charakter-/Waffen-TIMs; siehe Kommentar in re15_render_pc_upload_tim_slot_keyed.
 *  RE15_TIM_KEY_INDEX0 Index 0 transparent (Effekt-Sprite-Slots 19/20).
 *  RE15_TIM_KEY_PSX    BYTE-TRUE GPU-Regel: aufgeloester Texel-Wert == 0x0000 ->
 *      gar nicht zeichnen, unabhaengig vom ABE-Bit des Primitivs.
 *      Beleg (nocash psx-spx, docs/graphicsprocessingunitgpu.md):
 *        "Texture Palettes - CLUT": `Color 0000h = Fully-transparent` (Z. 1158)
 *        "Texture Color Black Limitations": "On the PSX, texture color 0000h is
 *        fully-transparent, that means textures cannot contain Black pixels." (Z. 1168)
 *      Die Regel haengt am TEXEL-WERT, nicht am Zeichenbefehl — opaque vs.
 *      semi-transparent unterscheidet dort nur die Behandlung von 8000h. */
#define RE15_TIM_KEY_NONE    0
#define RE15_TIM_KEY_INDEX0  1
#define RE15_TIM_KEY_PSX     2

/* PSX RGB555 -> SDL/ARGB8888 (opak). */
static inline uint32_t re15_tim_rgb555_to_argb8888(uint16_t c)
{
    uint32_t r = (uint32_t)(((c >>  0) & 0x1F) << 3);
    uint32_t g = (uint32_t)(((c >>  5) & 0x1F) << 3);
    uint32_t b = (uint32_t)(((c >> 10) & 0x1F) << 3);
    return 0xFF000000u | (r << 16) | (g << 8) | b;
}

/* EIN Texel -> ARGB8888. Rueckgabe 0x00000000 = "nicht zeichnen".
 *   raw            = aufgeloester 16-Bit-Texel-Wert (CLUT[idx] bei 4/8 bpp,
 *                    der Pixel selbst bei 16 bpp)
 *   index_is_zero  = Palettenindex war 0 (bei 16 bpp immer 0 uebergeben)
 *   key_mode       = RE15_TIM_KEY_*
 * Diese Funktion IST die Regel, die der PC-Renderer beim TIM-Upload anwendet
 * (render_pc.c, re15_render_pc_upload_tim_slot_keyed) — Tests pinnen sie direkt,
 * statt sie nachzubauen. */
static inline uint32_t re15_tim_texel_argb(uint16_t raw, int index_is_zero, int key_mode)
{
    if (index_is_zero && key_mode == RE15_TIM_KEY_INDEX0) return 0u;
    if (key_mode == RE15_TIM_KEY_PSX && raw == 0x0000)    return 0u;
    return re15_tim_rgb555_to_argb8888(raw);
}

/* PC-side helper: blit a TIM into the software framebuffer at (x, y).
 * Only available on PC target. Supports 4/8/16 BPP. */
#ifdef RE15_PLATFORM_PC
void re15_tim_blit_pc(const re15_tim_t *tim, int x, int y);
void re15_render_pc_upload_tim_slot_keyed(const re15_tim_t *tim, int slot, int key_mode);
#endif

#endif
