/*
 * codepanel_r37_pc.c — Runde 37 Spur E: die Lampe der Ziffern-Code-Panels zeichnen (PC).
 *
 * WANN und WO entscheidet engine/src/codepanel_r37.c (re15_codepanel_r37_lampe); hier wird nur gezeichnet.
 * Belege: include/re15_codepanel_r37.h, analysis/befunde_runde37/E_codepanel.md.
 *
 * KUNST: RE2 ESP 0x16 = shared_assets/RE2/LAMPE2130.TIM (bytegleich info/re2leon/PL0/RDT/room2110/esp16.tim
 *   und room2130/esp16.tim; kein neues Asset). 4 bpp, CLUT 16 x 4 bei (0,480), Pixel 64 x 32 Halbworte.
 *   CLUT-Zeile 1 = BLAU, 2 = GRUEN (Unterindex >> 3, Spawner @0x8001c9e0..0x8001c9fc); alle Nicht-Null-Eintraege
 *   tragen Bit 15. Zellen (effect.esp ESP-0x16-Block @+0x70): 3 `60 00 f0 f0`, 4 `80 00 f0 f0`, 5 `a0 00 f4 f4`
 *   (S 0x18 = 24), 6 `b8 00 f0 f0`, 7 `d8 00 f0 f0` (S 0x20 = 32), v 0.
 * ZEICHENART = RE2 wie die Generator-Lampe (platform/pc/src/panel_lampen_pc.c, C_generator.md §3.7): Texel 0
 *   durchsichtig; Bit 15 -> ABR 1 = B + F gesaettigt (Flags 0xBA03 & 0x1000 -> Prim 0x2E @0x80077a44..0x80077a50,
 *   TPAGE |= 0x20 @0x8001dc48..0x8001dc60); naechstes Texel ueber die Kante.
 * EBENE: Software-Framebuffer direkt nach Hintergrund (+ Generator-Lampe), vor 3D (Cursor) und Text (Skill
 *   re15-pc-render-order, Ebene 1).
 * Generation: die RE2-ESP-TIM ist Raumdatenbestand (RE2 laedt sie je Raum neu, @0x8001bc88 jal 0x8001bd38) ->
 *   neu dekodiert, sobald die Entlade-Generation weiterzaehlt (re15_entladen.h, Runde 35 Spur I).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_engine.h"          /* SCREEN_XRES / SCREEN_YRES, g_engine */
#include "re15_codepanel_r37.h"
#include "asset_root_pc.h"        /* re15_pc_read_re2 */
#include "re15_entladen.h"        /* g_re15_entladen_gen */

extern uint32_t *re15_pc_framebuffer(void);   /* render_pc.c: RGBA8888, R in Bit 24..31 */

#define CP_ZS 32
/* Zellen 3..7 -> Index 0..4; u-Anfang und Groesse aus effect.esp @+0x70 (s. Kopf). */
static const int k_u0[5] = { 96, 128, 160, 184, 216 };
static const int k_s[5]  = { 32,  32,  24,  32,  32 };

static int      s_zustand = 0;       /* 0 ungeprueft, 1 geladen, -1 fehlt */
static unsigned s_gen = 0;
static uint16_t s_zelle[2][5][CP_ZS * CP_ZS];   /* [Zeile blau/gruen][Zelle 3..7][v*32+u] */

static uint32_t rd32(const uint8_t *p)
{ return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

/* Die TIM pruefen und die Zellen 3..7 mit CLUT-Zeile 1 und 2 aufloesen. 0 = ok. (Riegel: probe_r37_codepanel) */
int re15_codepanel_r37_pc_dekodieren(const uint8_t *tim, int n, uint16_t out[2][5][CP_ZS * CP_ZS])
{
    if (!tim || n < 20 || !out) return -1;
    if (rd32(tim) != 0x10u || rd32(tim + 4) != 0x08u) return -1;
    uint32_t bl_c = rd32(tim + 8);
    uint16_t cw = rd16(tim + 16), ch = rd16(tim + 18);
    if (cw != 16 || ch < 3) return -1;
    if ((size_t)8 + bl_c + 12 > (size_t)n) return -1;
    const uint8_t *pb = tim + 8 + bl_c;
    uint32_t bl_p = rd32(pb);
    uint16_t pw = rd16(pb + 8), ph = rd16(pb + 10);
    if (pw != 64 || ph != 32) return -1;
    if ((size_t)8 + bl_c + bl_p > (size_t)n) return -1;
    const uint8_t *pix = pb + 12;
    for (int z = 0; z < 2; z++) {
        const uint8_t *clut = tim + 20 + (size_t)(z ? RE15_CP_ZEILE_GRUEN : RE15_CP_ZEILE_BLAU) * 16u * 2u;
        for (int c = 0; c < 5; c++) {
            memset(out[z][c], 0, sizeof out[z][c]);
            for (int v = 0; v < k_s[c]; v++)
                for (int u = 0; u < k_s[c]; u++) {
                    int uu = k_u0[c] + u;
                    int idx = (pix[(size_t)v * pw * 2u + (size_t)(uu >> 1)] >> (4 * (uu & 1))) & 15;
                    out[z][c][v * CP_ZS + u] = rd16(clut + idx * 2);
                }
        }
    }
    return 0;
}

static int laden(void)
{
    if (s_zustand && s_gen == g_re15_entladen_gen) return s_zustand;
    s_zustand = -1;
    s_gen = g_re15_entladen_gen;
    int n = 0;
    uint8_t *tim = re15_pc_read_re2("LAMPE2130.TIM", &n);
    if (!tim || re15_codepanel_r37_pc_dekodieren(tim, n, s_zelle) != 0) {
        fprintf(stderr, "[codepanel] shared_assets/RE2/LAMPE2130.TIM fehlt/defekt -> Panel-Lampe bleibt dunkel\n");
        free(tim);
        return s_zustand;
    }
    free(tim);
    s_zustand = 1;
    return s_zustand;
}

/* Eine Zelle (S x S Texel) als achsparalleles Viereck der Kante k, Ecke (x0,y0), additiv in den Framebuffer. */
void re15_codepanel_r37_pc_zelle(uint32_t *fb, const uint16_t *zelle, int s, int x0, int y0, int k)
{
    if (!fb || !zelle || k <= 0 || s <= 0) return;
    for (int j = 0; j < k; j++) {
        int y = y0 + j;
        if (y < 0 || y >= SCREEN_YRES) continue;
        int v = j * s / k;
        for (int i = 0; i < k; i++) {
            int x = x0 + i;
            if (x < 0 || x >= SCREEN_XRES) continue;
            uint16_t t = zelle[v * CP_ZS + i * s / k];
            if (t == 0) continue;                                   /* durchsichtig */
            unsigned tr = (unsigned)(t & 31) << 3, tg = (unsigned)((t >> 5) & 31) << 3,
                     tb = (unsigned)((t >> 10) & 31) << 3;
            uint32_t *px = &fb[y * SCREEN_XRES + x];
            if (t & 0x8000u) {                                       /* ABR 1: B + F, gesaettigt */
                unsigned r = ((*px >> 24) & 0xffu) + tr, g = ((*px >> 16) & 0xffu) + tg,
                         b = ((*px >> 8) & 0xffu) + tb;
                if (r > 255) r = 255;
                if (g > 255) g = 255;
                if (b > 255) b = 255;
                *px = (r << 24) | (g << 16) | (b << 8) | 0xffu;
            } else {
                *px = (tr << 24) | (tg << 16) | (tb << 8) | 0xffu;
            }
        }
    }
}

void re15_codepanel_r37_pc_zeichnen(int cut_gezeigt)
{
    re15_cp_lampe_t l;
    if (!re15_codepanel_r37_lampe(g_engine.frame_count, cut_gezeigt, &l)) return;
    if (laden() != 1) return;
    if (l.zelle < 3 || l.zelle > 7) return;
    const uint16_t *z = s_zelle[l.zeile == RE15_CP_ZEILE_GRUEN ? 1 : 0][l.zelle - 3];
    re15_codepanel_r37_pc_zelle(re15_pc_framebuffer(), z, l.s, l.x0, l.y0, l.kante);
}
