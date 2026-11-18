/*
 * RE1.5 Rebuilt — RE2-Buehne (VSync + Schuetteln), Kopf und Belege: include/re2_buehne.h.
 * Herausgeloest aus gator_re2_r41.c (Runde 41 Spur B), Inhalt unveraendert: Tabellen und Takt sind dieselben Bytes.
 */
#include "re2_buehne.h"

#include <stddef.h>

/* Bildschirm-Schuetteln FUN_8002aa28 / FUN_8002aa6c (bis Runde 41 in gator_re2_r41.c). */
static const uint8_t k_schuettel1[32] = {   /* @0x8009daf4 */
    0x00,0x01, 0x00,0x00, 0x00,0x00, 0x00,0x01, 0x01,0x01, 0x00,0x00, 0x00,0x01, 0x00,0x00,
    0x00,0x00, 0x00,0x00, 0x00,0x01, 0x00,0x01, 0x00,0x00, 0x00,0x00, 0x01,0x01, 0x00,0x00 };
static const uint8_t k_schuettel2[32] = {   /* @0x8009db14 */
    0x00,0x02, 0x00,0x00, 0x00,0x02, 0x00,0x03, 0x01,0x00, 0x00,0x01, 0x00,0x02, 0x00,0x00,
    0x00,0x02, 0x00,0x00, 0x02,0x01, 0x00,0x02, 0x00,0x02, 0x00,0x00, 0x01,0x01, 0x00,0x02 };

static uint8_t s_vsync = RE2B_VSYNC_SPIEL;   /* DAT_800dfc1a */
static uint8_t s_idx;                        /* 0x800dfd6b */
static int     s_dx, s_dy, s_modus;

void re2b_reset(void)
{
    s_vsync = RE2B_VSYNC_SPIEL;              /* `li v0,0x2 / sb v0,0x800dfc1a` @0x8003b9a8-b0 */
    s_dx = s_dy = 0; s_modus = 0;
}

int  re2b_vsync(void) { return s_vsync; }
void re2b_vsync_setzen(int v) { s_vsync = (uint8_t)v; }

void re2b_schuettel_takt(uint32_t bits)
{
    const int modus = (bits & RE2B_SCHUETTEL_STARK) ? 1 : (bits & RE2B_SCHUETTEL_LEICHT) ? 2 : 0;   /* @0x8002aa34-50 */
    s_modus = modus;
    if (!modus) { s_dx = s_dy = 0; return; }
    s_idx = (uint8_t)((s_idx + 1) & 0xf);                                                          /* @0x8002aa90-a4 */
    const uint8_t *t = (modus == 1) ? k_schuettel1 : k_schuettel2;
    s_dx = t[s_idx * 2];
    s_dy = t[s_idx * 2 + 1];
}

void re2b_schuettel_offset(int *dx, int *dy) { if (dx) *dx = s_dx; if (dy) *dy = s_dy; }
int  re2b_schuettel_modus(void) { return s_modus; }

const uint8_t *re2b_schuettel_tabelle(int modus)
{
    return modus == 1 ? k_schuettel1 : modus == 2 ? k_schuettel2 : NULL;
}
