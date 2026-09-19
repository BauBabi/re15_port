/* Sonde r16 / pri-masken-audit (2026-09-19): was der PORT fuer jeden STAGE1-Cut an
 * Vordergrundmasken WIRKLICH laedt — ueber die ECHTEN Engine-Leser, nicht ueber einen
 * Python-Nachbau (der hat schon einmal vier Fehler nicht gesehen, Memory
 * reai-v2-live-statt-nachbildung).
 *
 * Ausgabe (stdout, zeilenorientiert, von analysis/…/pri_audit.py gelesen):
 *   ROOM <hex> cuts=<n> sca=<n> bands=<lo>..<hi>
 *   B <band>                       Bodenpunkte dieses Bands folgen (re15_collision_on_floor
 *   F <x> <z>                      mit GESETZTEM Band, Raster 200 ueber die SCA-Huelle)
 *   C <cut> orig=<n> msk=<n> n=<n> draw=<n> H=<fov_screen_dist> rot=<9> trans=<3>
 *   R <i> <srcX> <srcY> <dstX> <dstY> <w> <h> <depth>       (i < n; gezeichnet wird i < draw)
 *   T <cut> ok=<0|1> w=<w> h=<h> opak=<n> clut0000=<n>      Atlas MASKS/ROOM…_PRI##.TIM
 *   D <240 Zeilen a 80 Hex>        Deckung 320x240 = Rechteck-Blit atlas[src]->screen[dst],
 *                                  Palettenindex != 0 (genau wie bg_pc.c pri_publish_tim +
 *                                  render_pc.c SDL_RenderCopy), nur die draw_count Rechtecke.
 *   P <cut> <240 Zeilen>           je Bildpunkt die NAECHSTE Maskentiefe (u16 hex, 0 = keine)
 *
 * Kein add_test: reine Messsonde. Aufruf: probe_r16_pri_masken_audit > dump.txt */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_collision.h"
#include "re15_pri.h"
#include "re15_tim.h"

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb");
    uint8_t *p; long len;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    p = (uint8_t *) malloc((size_t) len);
    if (!p) { fclose(f); return NULL; }
    if (fread(p, 1, (size_t) len, f) != (size_t) len) { free(p); fclose(f); return NULL; }
    fclose(f);
    if (n) *n = (size_t) len;
    return p;
}

static uint8_t  g_deck[240][320];
static uint16_t g_tief[240][320];

int main(int argc, char **argv)
{
    unsigned raum;
    unsigned von = 0x1000, bis = 0x2000;
    if (argc > 2) { von = (unsigned) strtoul(argv[1], NULL, 16); bis = (unsigned) strtoul(argv[2], NULL, 16); }

    for (raum = von; raum < bis; raum++) {
        char pfad[600];
        size_t sz = 0, msz = 0;
        uint8_t *roh, *msk;
        re15_rdt_t rdt;
        int ci, lo = 0, hi = 0, b;

        snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, raum >> 12, raum);
        roh = slurp(pfad, &sz);
        if (!roh) continue;
        if (re15_rdt_parse(roh, sz, &rdt) < 0) { free(roh); continue; }
        snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM%04X.MSK", RE15_ASSET_PSX_DIR, raum);
        msk = slurp(pfad, &msz);

        re15_collision_reset_band();
        re15_collision_band_range(&rdt, &lo, &hi);
        printf("ROOM %04X cuts=%d sca=%d bands=%d..%d msk=%d\n", raum, rdt.cut_count,
               rdt.sca_count, lo, hi, msk ? 1 : 0);

        /* Bodenpunkte je Band (die Engine entscheidet, nicht ein Nachbau). */
        {
            int X0 = 1 << 30, X1 = -(1 << 30), Z0 = 1 << 30, Z1 = -(1 << 30), s;
            for (s = 0; s < rdt.sca_count; s++) {
                const re15_sca_entry_t *e = &rdt.sca[s];
                if ((int) e->x < X0) X0 = e->x;
                if ((int) e->z < Z0) Z0 = e->z;
                if ((int) e->x + (int) e->width   > X1) X1 = e->x + e->width;
                if ((int) e->z + (int) e->density > Z1) Z1 = e->z + e->density;
            }
            for (b = lo; b <= hi; b++) {
                int gx, gz, n = 0;
                re15_collision_set_band(b);
                printf("B %d\n", b);
                for (gx = X0; gx <= X1; gx += 200)
                    for (gz = Z0; gz <= Z1; gz += 200)
                        if (re15_collision_on_floor(&rdt, gx, gz)) { printf("F %d %d\n", gx, gz); n++; }
                printf("BN %d %d\n", b, n);
            }
            re15_collision_reset_band();
        }

        for (ci = 0; ci < rdt.cut_count; ci++) {
            re15_camera_view_t view;
            re15_pri_cut_t pri;
            int n_orig, n_msk = 0, n, i, ok = 0;
            uint8_t *tbuf = NULL; size_t tsz = 0;
            re15_tim_t tim;

            if (re15_camera_build_view(&rdt.cuts[ci], &view) != 0) {
                printf("C %d KEINE_SICHT\n", ci); continue;
            }
            n_orig = re15_pri_parse_section(roh, sz, rdt.cuts[ci].pri_offset, &pri);
            n = n_orig;
            if (n == 0 && msk) {
                uint32_t off = re15_pri_msk_section_offset(msk, msz, ci);
                if (off) { n_msk = re15_pri_parse_section(msk, msz, off, &pri); n = n_msk; }
            }
            printf("C %d orig=%d msk=%d n=%d draw=%d H=%d rot=%d %d %d %d %d %d %d %d %d trans=%d %d %d\n",
                   ci, n_orig, n_msk, n, n ? pri.draw_count : 0, (int) view.fov_screen_dist,
                   view.rot[0], view.rot[1], view.rot[2], view.rot[3], view.rot[4], view.rot[5],
                   view.rot[6], view.rot[7], view.rot[8], view.trans[0], view.trans[1], view.trans[2]);
            for (i = 0; i < n; i++)
                printf("R %d %d %d %d %d %d %d %d\n", i, pri.masks[i].srcX, pri.masks[i].srcY,
                       (int16_t) pri.masks[i].dstX, (int16_t) pri.masks[i].dstY,
                       pri.masks[i].width, pri.masks[i].height, pri.masks[i].depth);
            if (n_msk <= 0) continue;

            /* Atlas der nachgezeichneten Sektion — genau der Pfad von bg_pc.c
             * (re15_pri_load_cut_atlas_ex(nachgezeichnet=1) -> pri_publish_tim). */
            snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM%04X_PRI%02d.TIM", RE15_ASSET_PSX_DIR, raum, ci);
            tbuf = slurp(pfad, &tsz);
            memset(g_deck, 0, sizeof g_deck);
            memset(g_tief, 0, sizeof g_tief);
            if (tbuf && re15_tim_parse(tbuf, (int) tsz, &tim) == 0 && tim.bpp == 8 && tim.has_clut) {
                const uint8_t *idx = (const uint8_t *) tim.pixels;
                int opak = 0, c0 = 0, k, y, x;
                for (k = 0; k < tim.width * tim.height; k++)
                    if (idx[k]) { opak++; if (tim.clut[idx[k]] == 0) c0++; }
                ok = 1;
                printf("T %d ok=1 w=%d h=%d opak=%d clut0000=%d\n", ci, tim.width, tim.height, opak, c0);
                for (i = 0; i < pri.draw_count; i++) {
                    const re15_pri_mask_t *m = &pri.masks[i];
                    int dx = (int16_t) m->dstX, dy = (int16_t) m->dstY;
                    for (y = 0; y < m->height; y++) {
                        int sy = m->srcY + y, ty = dy + y;
                        if (sy < 0 || sy >= tim.height || ty < 0 || ty >= 240) continue;
                        for (x = 0; x < m->width; x++) {
                            int sx = m->srcX + x, tx = dx + x;
                            uint8_t ix;
                            if (sx < 0 || sx >= tim.width || tx < 0 || tx >= 320) continue;
                            ix = idx[sy * tim.width + sx];
                            if (!ix) continue;
                            g_deck[ty][tx] = 1;
                            if (!g_tief[ty][tx] || m->depth < g_tief[ty][tx]) g_tief[ty][tx] = m->depth;
                        }
                    }
                }
                printf("D %d\n", ci);
                for (y = 0; y < 240; y++) {
                    for (x = 0; x < 320; x += 4) {
                        int v = (g_deck[y][x] << 3) | (g_deck[y][x+1] << 2) | (g_deck[y][x+2] << 1) | g_deck[y][x+3];
                        putchar("0123456789abcdef"[v]);
                    }
                    putchar('\n');
                }
                printf("P %d\n", ci);
                for (y = 0; y < 240; y++) {
                    for (x = 0; x < 320; x++) printf("%03x", g_tief[y][x]);
                    putchar('\n');
                }
            } else {
                printf("T %d ok=0 w=0 h=0 opak=0 clut0000=0\n", ci);
            }
            (void) ok;
            free(tbuf);
        }
        free(msk);
        free(roh);
    }
    return 0;
}
