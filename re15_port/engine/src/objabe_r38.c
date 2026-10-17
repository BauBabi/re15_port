/*
 * RE1.5 Rebuilt — Runde 38 Spur W "rolltor20a0", Nachbesserung 1: halbtransparente Raum-Objekte (Attribut-Bit 0x2).
 * Regeln, Adressen und Original-Messung: include/re15_objabe_r38.h; Dossier analysis/befunde_runde38/W_rolltor20a0.md §10.
 */
#include <stddef.h>

#include "re15_objabe_r38.h"

int re15_objabe_r38_abe(uint16_t attr)
{
    return (attr & RE15_OBJABE_ATTR_ABE) != 0;          /* @0x8002c2b4 `andi v0,v0,0x2` / @0x8002c2b8 `sltu s2,zero,v0` */
}

int re15_objabe_r38_abr(uint16_t attr, uint16_t page)
{
    /* FUN_80025940 @0x800259c4/@0x800259cc/@0x80025a14, FUN_80025a98 @0x80025b24/@0x80025b2c/@0x80025b70:
     * Wort 6 |= (Attribut & 0xC) << 19  ==  Texpage |= (Attribut & 0xC) << 3; ABR = Texpage-Bits 5-6 (psx-spx). */
    const uint16_t tp = (uint16_t)(page | ((attr & RE15_OBJABE_ATTR_ABR) << 3));
    return (tp >> 5) & 3;
}

int re15_objabe_r38_flach(uint16_t attr, int obj_typ)
{
    /* @0x8002c2f4 `andi v0,v0,0x10` / @0x8002c2f8 `beq v0,zero` (0 -> Licht-Zeichner); Typ 3 laeuft vorher ueber
     * FUN_8003e64c (@0x8002c2c0 `bne v1,v0` / @0x8002c2c8 `jal`), das nur die Flach-Zeichner ruft (@0x8003e6ac/@0x8003e6dc). */
    return (attr & RE15_OBJABE_ATTR_FLACH) != 0 || obj_typ == RE15_OBJABE_TYP_KACHEL;
}

uint8_t re15_objabe_r38_mische(int abr, uint8_t b, uint8_t f)
{
    int v;
    switch (abr & 3) {
    case 0:  v = (b >> 1) + (f >> 1); break;            /* 0.5 x B + 0.5 x F */
    case 1:  v = b + f;               break;            /* 1.0 x B + 1.0 x F */
    case 2:  v = b - f;               break;            /* 1.0 x B - 1.0 x F */
    default: v = b + (f >> 2);        break;            /* 1.0 x B + 0.25 x F */
    }
    return (uint8_t)(v < 0 ? 0 : (v > 255 ? 255 : v)); /* psx-spx: additiv auf 255, subtraktiv auf 0 geklemmt */
}

int re15_objabe_r38_texel_klasse(uint16_t raw)
{
    if (raw == 0x0000) return 0;                        /* psx-spx CLUT: "Color 0000h = Fully-transparent" */
    return (raw & 0x8000) ? 2 : 1;                      /* "8000h..FFFFh = Semi-transparent (*) or Non-transparent
                                                         *  for opaque commands" */
}

int re15_objabe_r38_textur_mass(const re15_tim_t *tim, int *w, int *h)
{
    if (!tim) return -1;
    /* dieselbe Rechnung wie re15_render_pc_upload_tim_slot (render_pc.c): je CLUT-Zeile eine Kopie untereinander. */
    const int row_width = (tim->bpp == 4) ? 16 : 256;
    int n_cluts = (tim->has_clut && row_width > 0) ? (tim->clut_entries / row_width) : 1;
    if (n_cluts < 1) n_cluts = 1;
    *w = tim->width;
    *h = tim->height * n_cluts;
    return n_cluts;
}

static void eintragen(uint16_t raw, uint32_t *deckend, uint32_t *halb, long *n_halb)
{
    const int k = re15_objabe_r38_texel_klasse(raw);
    *deckend = (k == 1) ? re15_tim_texel_argb(raw) : 0u;
    *halb    = (k == 2) ? re15_tim_texel_argb(raw) : 0u;
    if (k == 2) (*n_halb)++;
}

long re15_objabe_r38_teilen(const re15_tim_t *tim, uint32_t *deckend, uint32_t *halb)
{
    int w, h;
    const int n_cluts = re15_objabe_r38_textur_mass(tim, &w, &h);
    if (n_cluts < 1 || !deckend || !halb) return -1;
    const int pro_clut = tim->width * tim->height;
    long n_halb = 0;
    if (tim->bpp == 8 && tim->has_clut) {
        const uint8_t *src = (const uint8_t *)tim->pixels;
        for (int c = 0; c < n_cluts; c++)
            for (int i = 0; i < pro_clut; i++)
                eintragen(tim->clut[c * 256 + src[i]], &deckend[c * pro_clut + i], &halb[c * pro_clut + i], &n_halb);
    } else if (tim->bpp == 4 && tim->has_clut) {
        const uint8_t *src = (const uint8_t *)tim->pixels;
        for (int c = 0; c < n_cluts; c++)
            for (int i = 0; i < pro_clut; i++) {
                const int nib = (i & 1) ? (src[i / 2] >> 4) : (src[i / 2] & 0xF);
                eintragen(tim->clut[c * 16 + nib], &deckend[c * pro_clut + i], &halb[c * pro_clut + i], &n_halb);
            }
    } else if (tim->bpp == 16) {
        for (int i = 0; i < w * h; i++)
            eintragen(tim->pixels[i], &deckend[i], &halb[i], &n_halb);   /* 15 Bit direkt: Bit 15 = STP (psx-spx) */
    } else {
        return -1;
    }
    return n_halb;
}
