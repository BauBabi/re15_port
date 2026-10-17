/* nclip_r38.c — Runde 38 Spur A: Rueckseiten-Verwurf der Objekt-Zeichner (GTE NCLIP, MAC0 < 0).
 * Belege und Herleitung: include/re15_nclip_r38.h, analysis/befunde_runde38/A_cursor.md. */
#include "re15_nclip_r38.h"

unsigned g_re15_nclip_r38_verworfen = 0;

static int32_t saettige(int v)
{
    if (v < RE15_NCLIP_R38_SXY_MIN) return RE15_NCLIP_R38_SXY_MIN;
    if (v > RE15_NCLIP_R38_SXY_MAX) return RE15_NCLIP_R38_SXY_MAX;
    return (int32_t)v;
}

int32_t re15_nclip_r38_mac0(int x0, int y0, int x1, int y1, int x2, int y2)
{
    /* psx-spx geometrytransformationenginegte.md:439 — woertlich in dieser Reihenfolge. Mit 11-Bit-Werten
     * (-400h..+3FFh) passt jedes Produkt in 21 Bit, die Summe in 32 Bit (MAC0 der GTE ist 32 Bit breit). */
    const int64_t sx0 = saettige(x0), sy0 = saettige(y0);
    const int64_t sx1 = saettige(x1), sy1 = saettige(y1);
    const int64_t sx2 = saettige(x2), sy2 = saettige(y2);
    return (int32_t)(sx0 * sy1 + sx1 * sy2 + sx2 * sy0 - sx0 * sy2 - sx1 * sy0 - sx2 * sy1);
}

int re15_nclip_r38_verwerfen(int x0, int y0, int x1, int y1, int x2, int y2)
{
    /* FUN_800254a0 @0x800255a8 / FUN_800256b0 @0x800257c0 `bgez v0` (zeichnen bei MAC0 >= 0),
     * FUN_80022f0c @0x80023010 `bltz v0` (verwerfen bei MAC0 < 0), FUN_80023098 @0x80023190 `bgez v0`. */
    if (re15_nclip_r38_mac0(x0, y0, x1, y1, x2, y2) < 0) {
        g_re15_nclip_r38_verworfen++;
        return 1;
    }
    return 0;
}
