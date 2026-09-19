/* Silhouetten-Riegel: die Deckung, die die ENGINE aus Sektion + Atlas blittet, muss BITGENAU
 * die Ziel-Silhouette des Baus sein (MASKS/ROOM####_PRI##.PBM = die Freistellung des Nutzers
 * bei Alpha > 110, punktgenau — kein Halo, keine Aufweitung).
 *
 * ⛔ WOZU (Audit 2026-09-19, pri-masken-audit.md §1.4a): bei 40 von 84 STAGE1-Cuts war die
 * gelieferte Deckung das PNG des Nutzers um 4 Zeilen nach oben und 1 px rundum aufgeweitet
 * (raum.py STD oben=4/grow=1) — 1..62 % Hintergrundpunkte mit Objekttiefe UEBER der Figur.
 * Kein Test hat das gesehen, weil keiner die DECKUNG gegen die Vorlage hielt.
 *
 * Geprueft wird auf dem ECHTEN Ladeweg: re15_rdt_parse -> re15_pri_msk_section_offset ->
 * re15_pri_parse_section (draw_count Rechtecke) -> re15_tim_parse (8 bpp + CLUT) -> Blit
 * atlas[src] -> screen[dst] mit Palettenindex != 0 (bg_pc.c pri_publish_tim, render_pc.c
 * SDL_RenderCopy). Das ist dieselbe Rasterung wie in der Sonde probe_r16_pri_masken_audit,
 * die gegen den Python-Leser in 84 Cuts 0 Abweichungen hatte.
 *
 * Der Vorrang-Riegel bleibt: Cuts mit Original-Sektion tragen keine PBM (der Bau schreibt
 * dort nichts); traegt einer doch eine, faellt der Test. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include "re15_rdt.h"
#include "re15_pri.h"
#include "re15_tim.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

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

static uint8_t g_deck[240][320];
static uint8_t g_soll[240][320];

/* P4-PBM 320x240 (abnahme.pbm_schreiben): "P4\n320 240\n" + 40 Byte je Zeile, MSB zuerst. */
static int pbm_lesen(const uint8_t *b, size_t n)
{
    const char *kopf = "P4\n320 240\n";
    size_t k = strlen(kopf), y, x;
    if (n != k + 240 * 40 || memcmp(b, kopf, k) != 0) return 0;
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++)
            g_soll[y][x] = (b[k + y * 40 + x / 8] >> (7 - (x & 7))) & 1;
    return 1;
}

/* Rasterung wie bg_pc.c/render_pc.c (und die Sonde): nur draw_count Rechtecke, Index != 0. */
static int rastern(const re15_pri_cut_t *pri, const re15_tim_t *tim)
{
    const uint8_t *idx = (const uint8_t *) tim->pixels;
    int i, x, y, n = 0;
    memset(g_deck, 0, sizeof g_deck);
    for (i = 0; i < pri->draw_count; i++) {
        const re15_pri_mask_t *m = &pri->masks[i];
        int dx = (int16_t) m->dstX, dy = (int16_t) m->dstY;
        for (y = 0; y < m->height; y++) {
            int sy = m->srcY + y, ty = dy + y;
            if (sy < 0 || sy >= tim->height || ty < 0 || ty >= 240) continue;
            for (x = 0; x < m->width; x++) {
                int sx = m->srcX + x, tx = dx + x;
                if (sx < 0 || sx >= tim->width || tx < 0 || tx >= 320) continue;
                if (idx[sy * tim->width + sx]) { g_deck[ty][tx] = 1; n++; }
            }
        }
    }
    return n;
}

int main(void)
{
    char pfad[700];
    DIR *d; struct dirent *e;
    int n_pbm = 0, n_ok = 0, n_falsch = 0, n_fehler = 0, n_vorrang = 0;
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS", RE15_PORT_SRC_DIR);
    d = opendir(pfad);
    printf("== Silhouetten-Riegel: Engine-Deckung == PBM ==\n");
    if (!d) { printf("  FAIL: %s nicht lesbar\n", pfad); return 1; }
    while ((e = readdir(d)) != NULL) {
        unsigned raum; int cut;
        size_t sz = 0, msz = 0, psz = 0, tsz = 0;
        uint8_t *roh, *msk, *pbm, *tb;
        re15_rdt_t rdt; re15_pri_cut_t pri; re15_tim_t tim;
        uint32_t off; int n, y, x, fehlt = 0, zuviel = 0, soll = 0;
        /* ⛔ sscanf zaehlt nur die UMWANDLUNGEN: "%4x_PRI%2d.PBM" liefert auch fuer
         * ROOM1000_PRI00.TIM die 2 (der Suffix wird nicht geprueft, und .TIM/.PBM sind
         * gleich lang). Die Endung deshalb selbst pruefen — sonst meldet der Test 180
         * "PBM-Cuts" statt 78 und 102 davon als Fehler (gemessen 2026-09-19). */
        {   size_t L = strlen(e->d_name);
            if (L != strlen("ROOM0000_PRI00.PBM") || strcmp(e->d_name + L - 4, ".PBM") != 0) continue; }
        if (sscanf(e->d_name, "ROOM%4x_PRI%2d.PBM", &raum, &cut) != 2) continue;
        n_pbm++;
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/%s", RE15_PORT_SRC_DIR, e->d_name);
        pbm = slurp(pfad, &psz);
        if (!pbm || !pbm_lesen(pbm, psz)) { printf("  FEHLER: %s unlesbar\n", e->d_name); n_fehler++; free(pbm); continue; }
        free(pbm);
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/STAGE%u/ROOM%04X.RDT", RE15_PORT_SRC_DIR, raum >> 12, raum);
        roh = slurp(pfad, &sz);
        if (!roh || re15_rdt_parse(roh, sz, &rdt) < 0 || cut >= rdt.cut_count) {
            printf("  FEHLER: ROOM%04X Cut %d: RDT\n", raum, cut); n_fehler++; free(roh); continue;
        }
        memset(&pri, 0, sizeof pri);
        if (re15_pri_parse_section(roh, sz, rdt.cuts[cut].pri_offset, &pri) > 0) {
            printf("  VORRANG VERLETZT: ROOM%04X Cut %d hat Original-Masken UND eine PBM\n", raum, cut);
            n_vorrang++; free(roh); continue;
        }
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X.MSK", RE15_PORT_SRC_DIR, raum);
        msk = slurp(pfad, &msz);
        off = msk ? re15_pri_msk_section_offset(msk, msz, cut) : 0;
        n = off ? re15_pri_parse_section(msk, msz, off, &pri) : 0;
        snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X_PRI%02d.TIM", RE15_PORT_SRC_DIR, raum, cut);
        tb = slurp(pfad, &tsz);
        if (n <= 0 || !tb || re15_tim_parse(tb, (int) tsz, &tim) != 0 || tim.bpp != 8 || !tim.has_clut) {
            printf("  FEHLER: ROOM%04X Cut %d: Sektion (%d) oder Atlas fehlt\n", raum, cut, n);
            n_fehler++; free(roh); free(msk); free(tb); continue;
        }
        rastern(&pri, &tim);
        for (y = 0; y < 240; y++)
            for (x = 0; x < 320; x++) {
                soll += g_soll[y][x];
                if (g_soll[y][x] && !g_deck[y][x]) fehlt++;
                if (!g_soll[y][x] && g_deck[y][x]) zuviel++;
            }
        if (fehlt || zuviel) {
            printf("  ABWEICHUNG: ROOM%04X Cut %d: Soll %d px, fehlt %d, zuviel %d (%d Rechtecke)\n",
                   raum, cut, soll, fehlt, zuviel, pri.draw_count);
            n_falsch++;
        } else n_ok++;
        free(roh); free(msk); free(tb);
    }
    closedir(d);
    printf("  %d PBM-Cuts: %d bitgenau, %d abweichend, %d Fehler, %d Vorrang-Verletzungen\n",
           n_pbm, n_ok, n_falsch, n_fehler, n_vorrang);
    CHECK("es gibt Ziel-Silhouetten (PBM) fuer nachgezeichnete Cuts", n_pbm > 0);
    CHECK("jede PBM traegt Sektion und Atlas", n_fehler == 0);
    CHECK("keine PBM auf einem Cut mit Original-Masken", n_vorrang == 0);
    CHECK("die Engine-Deckung ist in JEDEM Cut bitgenau die Ziel-Silhouette", n_falsch == 0);
    return g_fail;
}
