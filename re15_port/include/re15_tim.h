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

/* ============================================================================
 * DER TEXEL-FARBSCHLUESSEL — EINE REGEL, KEINE SLOT-LISTE
 * ============================================================================
 * Welche Texel gelten als "nicht zeichnen"? Auf echter Hardware entscheidet das
 * die GPU ALLEIN am aufgeloesten 16-Bit-Texel-Wert. Es gibt im Original weder
 * eine Tabelle noch ein Per-Objekt-Flag dafuer — der Port muss das beim
 * Dekodieren nach RGBA vorwegnehmen, weil SDL keinen Farbschluessel kennt.
 *
 * BELEG (nocash psx-spx, info/Resident_Evil_und_Playstation_Information/
 *        psx-spx.github.io-master/docs/graphicsprocessingunitgpu.md):
 *   Z.1128-1131 "16bit Texture (Direct Color)" UND Z.1157-1160 "Texture
 *   Palettes - CLUT" tragen WORTGLEICH dieselbe Tabelle:
 *       Color 0000h        = Fully-transparent
 *       Color 0001h..7FFFh = Non-transparent
 *       Color 8000h..FFFFh = Semi-transparent (*)
 *       Bit15 Semi-transparency Flag  /(*) or Non-transparent for opaque commands
 *   Z.1167-1176 "Texture Color Black Limitations": "On the PSX, texture color
 *   0000h is fully-transparent, that means textures cannot contain Black
 *   pixels. However, in some cases, Color 8000h (Black with semi-transparent
 *   flag) can be used, depending on the rendering command:
 *       opaque command, eg. GP0(24h)      --> 8000h = Non-Transparent Black
 *       semi-transp command, eg. GP0(26h) --> 8000h = Semi-Transparent Black"
 *
 * DARAUS FOLGT — und das ist der ganze Grund, warum hier keine Liste steht:
 *   (a) Der Schluessel haengt am WERT, nicht am Index und nicht am Slot. Er
 *       gilt fuer 4bpp und 8bpp (ueber die CLUT aufgeloest) genauso wie fuer
 *       16bpp direkt, und fuer JEDES texturierte Primitiv — Modell, Raum-Prop,
 *       Effekt-Sprite, UI. Ein Sonderfall existiert NICHT.
 *   (b) Das ABE-Bit des Primitivs (RE1.5: FUN_800254a0 `cd = param_4<<1|0x34`
 *       POLY_GT3 / FUN_800256b0 `|0x3c` POLY_GT4, Bit 0x02) aendert daran
 *       nichts. Es unterscheidet ausschliesslich, wie 8000h..FFFFh behandelt
 *       wird (semi-transparent vs. opak) — der Port bildet das getrennt ueber
 *       den Blend-Modus der Zeichen-Queue ab, nicht ueber den Farbschluessel.
 *
 * WARUM DAS FRUEHER EINE LISTE WAR (und viermal einzeln repariert wurde):
 * der Port hatte statt der Wert-Regel einen INDEX-Schalter ("Index 0
 * transparent ja/nein") mit Default "nein". Beides ist falsch:
 *   - Default "nein" malt jeden neuen Effekt-/Prop-Slot als schwarzen Klotz.
 *   - "Index 0 transparent" reisst Loecher in Modelltexturen, denn dort ist
 *     Index 0 eine echte, sichtbare Farbe (gemessen, probe_texel_key_census:
 *     EM45 3635, EM42 1787, Elliot/EM47 478 gesampelte Index-0-Texel mit
 *     CLUT-Wert != 0). Genau das waren die gemeldeten "dreieckigen Loecher".
 * Unter der WERT-Regel passiert beides nicht: gemessen ueber alle Assets
 * aendert sie an den Modelltexturen praktisch nichts (Leon PL00 und Elza PL04:
 * 0 von 117197 bzw. 133212 gesampelten Texeln; 22 von 26 Gegner-EMDs exakt 0),
 * schaltet aber die Effekt-Sheets korrekt frei (ROOM1090 Flammen-Sheet
 * id 0x10: 28103 von 36864 Texeln = 76.2%).
 * ============================================================================ */

/* PSX RGB555 -> SDL/ARGB8888 (opak). */
static inline uint32_t re15_tim_rgb555_to_argb8888(uint16_t c)
{
    uint32_t r = (uint32_t)(((c >>  0) & 0x1F) << 3);
    uint32_t g = (uint32_t)(((c >>  5) & 0x1F) << 3);
    uint32_t b = (uint32_t)(((c >> 10) & 0x1F) << 3);
    return 0xFF000000u | (r << 16) | (g << 8) | b;
}

/* DIE REGEL. EIN Texel -> ARGB8888, Rueckgabe 0x00000000 = "nicht zeichnen".
 *   raw = aufgeloester 16-Bit-Texel-Wert (CLUT[idx] bei 4/8 bpp, der Pixel
 *         selbst bei 16 bpp).
 * Jeder Dekodier-Pfad des Ports geht hier durch (render_pc.c Slot-Upload,
 * asset_pc.c re15_tim_blit_pc) — deshalb ist eine neu geladene Textur ohne
 * jeden Zusatzeintrag automatisch richtig. Tests pinnen diese Funktion direkt,
 * statt sie nachzubauen. */
static inline uint32_t re15_tim_texel_argb(uint16_t raw)
{
    if (raw == 0x0000) return 0u;                 /* GPU-Farbschluessel */
    return re15_tim_rgb555_to_argb8888(raw);
}

/* ---- HISTORISCH: die drei Modi, die der Port vor 2026-08-21 hatte -----------
 * NICHT im Zeichenpfad. Sie existieren nur noch, damit die Tests den alten
 * Zustand als NEGATIV-KONTROLLE festhalten koennen ("faellt der Fix zurueck,
 * ist DAS das Ergebnis") und damit dokumentiert bleibt, was die "Loecher"
 * verursacht hat. Neuer Code ruft re15_tim_texel_argb().
 *   RE15_TIM_KEY_NONE   Index 0 opak (CLUT[0]-Farbe)  -> schwarze Kloetze
 *   RE15_TIM_KEY_INDEX0 Index 0 transparent           -> Loecher in Modellen
 *   RE15_TIM_KEY_PSX    die Wert-Regel                -> == re15_tim_texel_argb */
#define RE15_TIM_KEY_NONE    0
#define RE15_TIM_KEY_INDEX0  1
#define RE15_TIM_KEY_PSX     2

static inline uint32_t re15_tim_texel_argb_legacy(uint16_t raw, int index_is_zero, int key_mode)
{
    if (index_is_zero && key_mode == RE15_TIM_KEY_INDEX0) return 0u;
    if (key_mode == RE15_TIM_KEY_PSX && raw == 0x0000)    return 0u;
    return re15_tim_rgb555_to_argb8888(raw);
}

/* PC-side helper: blit a TIM into the software framebuffer at (x, y).
 * Only available on PC target. Supports 4/8/16 BPP. */
#ifdef RE15_PLATFORM_PC
void re15_tim_blit_pc(const re15_tim_t *tim, int x, int y);

/* Der EINZIGE TIM-Slot-Upload. Absichtlich OHNE Schluessel-/Modus-Parameter: die
 * Transparenz-Entscheidung ist `re15_tim_texel_argb()` und gilt fuer jede Textur gleich.
 * Wer hier einen Sonderfall braucht, hat einen anderen Bug. */
void re15_render_pc_upload_tim_slot(const re15_tim_t *tim, int slot);
#endif

#endif
