/*
 * panel_lampen_pc.c — die ZWEI GRUENEN LAMPEN des Generator-Bedienfelds (ROOM11F0/11F1,
 * Cut 10), PC-Zeichner. Spur C, Runde 34 Nacht.
 *
 * Nutzer 2026-09-30 (Wortlaut und Lesart im Kopf von include/re15_panel_zeiger.h): die
 * obere Lampe leuchtet gruen, wenn die linke Schalterspalte in Loesungsstellung steht, die
 * untere fuer die rechte Spalte. WANN eine Lampe brennt, entscheidet der Engine-Teil
 * (engine/src/panel_zeiger_common.c, re15_panel_lampe_sicht); hier wird nur gezeichnet.
 *
 * KUNST = PORT-WAHL (Kombination), keine gefundene RE2-Kunst:
 *   Form    = RE2-Schalterlampe ESP 0x16, Strom 2 / Anim-Satz 4 / Zellen 3,4 (ROOM2130.RDT
 *             sub04 @0x01294 `64 01 16 02 ..`, dort ROT).
 *   Palette = CLUT-Zeile 2 (gruen) aus dem Strom-0-Ereignis @0x017A8 `64 0d 16 10` /
 *             @0x017B8 `64 0e 16 10` (dort die Rechteck-Lampe Zellen 0..2). CLUT-Zeile =
 *             Unterindex >> 3 (@0x8001c9e0 `srl v0,t5,3` / @0x8001c9e4 `sll v0,v0,6` /
 *             @0x8001c9f8 `addu` / @0x8001c9fc `sh v1,50(t0)`).
 *   RE2 zeigt die gruene Quadrat-Lampe nirgends (Zensus 250 RDTs,
 *   analysis/befunde_runde34_nacht/C_belege/gp_re2_esp16_zensus.txt).
 * Datei: shared_assets/RE2/LAMPE2130.TIM = byte-gleicher Schnitt ROOM2130.RDT[0x0E398, +4256)
 *   (RDT-Kopfwort [20]; Werkzeug tools/r34n_c/lampe2130_schnitt.py; = room2130/esp16.tim).
 *
 * ZEICHENART = RE2 (selbst disassembliert, C_generator.md §3.7):
 *   * Texel 0x0000 = durchsichtig (GPU).
 *   * Flags 0xBA03 (Zeile+18, Routine 1 @0x8001dc3c `lhu v1,18(v0)` / @0x8001dc44 `sh v1,24(v0)`)
 *     tragen 0x1000 -> Prim-Code 0x2E = POLY_FT4 halbtransparent (@0x80077a44 `andi v0,s2,0x1000`
 *     / @0x80077a4c `addiu s5,zero,44` / @0x80077a50 `addiu s5,zero,46`).
 *   * TPAGE |= 0x0020 (Zeile+20, @0x8001dc48..0x8001dc60) = ABR 1 = B+F (additiv), wirksam
 *     fuer Texel mit Bit 15 (psx-spx graphicsprocessingunitgpu.md, Semi-Transparency); Kanal
 *     saettigt. Palette 2 traegt Bit 15 in allen Nicht-Null-Eintraegen.
 *   * Modulation neutral 0x80 (FUN_800783b4 @0x800783cc/d0 `lui a0,0x2c80` / `ori a0,a0,0x8080`;
 *     der Sprite-Bauer schreibt nur das Code-Byte @0x8007808c `sb s2,7(t2)`).
 *   * Abtastung: POLY_FT4 von u0..u0+S ueber die Kante, naechstes Texel, ohne Kantenabzug, weil
 *     der Schritt < 0x1ffff ist (Kantenabzug nur darueber, FUN_80077ed0 @0x80078070..0x80078088).
 * EBENE: Software-Framebuffer (Skill re15-pc-render-order, Ebene 1), direkt nach Hintergrund und
 *   Zeiger (platform/pc/main.c). Die 3D-Hebel liegen bei x 64..172, die Lampen bei x 212..233 —
 *   keine Ueberdeckung; Meldungen (Text-Overlay, Ebene 5) liegen richtig darueber.
 * 8 statt 5 Bit je Kanal: Summe hier = Hintergrund(8 Bit) + Texel(5 Bit) << 3, bei 255 gekappt —
 *   Unterschied zur PSX hoechstens die unteren 3 Bit.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_engine.h"          /* SCREEN_XRES / SCREEN_YRES */
#include "re15_panel_zeiger.h"
#include "asset_root_pc.h"        /* re15_pc_read_re2 */

extern uint32_t *re15_pc_framebuffer(void);   /* render_pc.c: RGBA8888, R in Bit 24..31 */

#define ZS RE15_PANEL_LAMPE_ZELLE_S           /* 32 */

/* 0 = ungeprueft, 1 = geladen, -1 = fehlt/defekt (dann still aus, einmal gemeldet) */
static int      s_zustand = 0;
/* Zelle 3 und Zelle 4, schon ueber CLUT-Zeile 2 aufgeloest: PSX-15-Bit-Farbe inkl. Bit 15. */
static uint16_t s_zelle[2][ZS * ZS];

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }

/* Die RE2-ESP-TIM pruefen und die zwei Zellen dekodieren. Erwartet (lampe2130_schnitt.py):
 * Magic 0x10, Flags 0x08 (4 bpp + CLUT), CLUT-Block 16 x 4 bei (0,480), Pixelblock 64 x 32
 * Halbworte (= 256 x 32 Texel). */
int re15_panel_lampen_pc_dekodieren(const uint8_t *tim, int n, uint16_t zelle3[ZS * ZS],
                                    uint16_t zelle4[ZS * ZS])
{
    if (!tim || n < 20) return -1;
    if (rd32(tim) != 0x10u || rd32(tim + 4) != 0x08u) return -1;
    uint32_t bl_c = rd32(tim + 8);
    uint16_t cw = rd16(tim + 16), ch = rd16(tim + 18);
    if (cw != 16 || ch < RE15_PANEL_LAMPE_CLUT_ZEILE + 1) return -1;
    if ((size_t)8 + bl_c + 12 > (size_t)n) return -1;
    const uint8_t *clut = tim + 20 + (size_t)RE15_PANEL_LAMPE_CLUT_ZEILE * 16u * 2u;
    const uint8_t *pb = tim + 8 + bl_c;
    uint32_t bl_p = rd32(pb);
    uint16_t pw = rd16(pb + 8), ph = rd16(pb + 10);
    if (pw != 64 || ph != 32) return -1;
    if ((size_t)8 + bl_c + bl_p > (size_t)n) return -1;
    const uint8_t *pix = pb + 12;
    uint16_t *ziel[2] = { zelle3, zelle4 };
    for (int z = 0; z < 2; z++) {
        int u0 = (z == 0) ? 96 : 128;   /* Zelle 3 / 4: effect.esp @0x7C `60 00 f0 f0`, @0x80 `80 00 f0 f0` */
        for (int v = 0; v < ZS; v++)
            for (int u = 0; u < ZS; u++) {
                int uu = u0 + u;
                int idx = (pix[(size_t)v * pw * 2u + (size_t)(uu >> 1)] >> (4 * (uu & 1))) & 15;
                ziel[z][v * ZS + u] = rd16(clut + idx * 2);
            }
    }
    return 0;
}

static int laden(void)
{
    if (s_zustand) return s_zustand;
    s_zustand = -1;
    int n = 0;
    uint8_t *tim = re15_pc_read_re2("LAMPE2130.TIM", &n);
    if (!tim || re15_panel_lampen_pc_dekodieren(tim, n, s_zelle[0], s_zelle[1]) != 0) {
        fprintf(stderr, "[panellampe] shared_assets/RE2/LAMPE2130.TIM fehlt/defekt -> "
                        "Generator-Lampen bleiben dunkel\n");
        free(tim);
        return s_zustand;
    }
    free(tim);
    s_zustand = 1;
    return s_zustand;
}

/* Eine Zelle als achsparalleles Viereck (Ecke x0/y0, Kante k) additiv in den Framebuffer. */
void re15_panel_lampen_pc_zelle(uint32_t *fb, const uint16_t *zelle, int x0, int y0, int k)
{
    if (!fb || !zelle || k <= 0) return;
    for (int j = 0; j < k; j++) {
        int y = y0 + j;
        if (y < 0 || y >= SCREEN_YRES) continue;
        int v = j * ZS / k;
        for (int i = 0; i < k; i++) {
            int x = x0 + i;
            if (x < 0 || x >= SCREEN_XRES) continue;
            uint16_t t = zelle[v * ZS + i * ZS / k];
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
            } else {                                                 /* ohne Bit 15: deckend */
                *px = (tr << 24) | (tg << 16) | (tb << 8) | 0xffu;
            }
        }
    }
}

void re15_panel_lampen_pc_zeichnen(void)
{
    int x0, y0, k, z;
    int sicht0 = re15_panel_lampe_sicht(0, NULL, NULL, NULL, NULL);
    int sicht1 = re15_panel_lampe_sicht(1, NULL, NULL, NULL, NULL);
    if (!sicht0 && !sicht1) return;                 /* nichts zu tun: auch nichts laden */
    if (laden() != 1) return;
    uint32_t *fb = re15_pc_framebuffer();
    for (int nr = 0; nr < 2; nr++) {
        if (!re15_panel_lampe_sicht(nr, &x0, &y0, &k, &z)) continue;
        re15_panel_lampen_pc_zelle(fb, s_zelle[z == RE15_PANEL_LAMPE_ZELLE_A ? 0 : 1], x0, y0, k);
    }
}
