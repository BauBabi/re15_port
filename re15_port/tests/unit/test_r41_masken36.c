/*
 * Runde 41 (2026-10-24), Spur M "masken36" — Riegel ueber die ECHTEN Engine-Leser. Dossier analysis/befunde_runde41/M_masken36.md.
 *
 * Punkt 1 (Nutzer): "Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall Loecher
 * aufweisen." -> "danach kannst du deine Masken schliessen in der naechsten Version."
 *
 *   test_r41_masken36 loecher   MECHANIK DES SCHLIESSENS an allen 216 Generator-Sektionen (30 MSK-Dateien der Stages 3..6):
 *     - jede Sektion hat ihren eigenen Atlas MASKS/ROOM%04X_PRI%02d.TIM und der Lader nimmt ihn (re15_pri_atlas_quelle);
 *     - die ALTEN Generator-Rechtecke sind noch da: Rechtecke, deren Quelle im BSS-Atlas des Cuts (Engine-Entpacker
 *       re15_sld_*, FUN_800c47e8) voll deckend ist und zum Hintergrund passt (Blit wie R37 §2.1) - ihre Verschiebung
 *       s = Ziel - Quelle ist die Lage des Kuenstler-Atlas im Bild;
 *     - OFFENE KERBEN: ein Bildpunkt p, der NICHT gezeichnet wird, 4-Nachbar eines gezeichneten Randpunkts eines alten
 *       Rechtecks r, mit Kuenstler-Atlas an p - s_r deckend (Index != 0) und farbgleich zum Hintergrund
 *       (|dR|+|dG|+|dB| <= 48, die Grenze der R37-Blit-Pruefung) = ein Stueck des Objekts direkt an seiner Kante, durch das
 *       die Figur dahinter durchscheint (die Treppenkanten der Vollrechtecke, gen_masken.py rects_from_mask).
 *       Gezaehlt fuer VORHER = nur die alten Rechtecke durch den BSS-Atlas (so zeichnete der Lader bis Runde 40) und
 *       NACHHER = alle Rechtecke durch den eigenen Atlas, beide mit dem Ausschluss der Spur P R37 (re15_masken36_filter wie
 *       main.c) - der Vergleich misst allein die Saum-Rechtecke. Verlangt: NACHHER <= 30 % von VORHER (gemessen 15498 von
 *       61672 = 25,1 %, Leon + Elza, Dossier 1.4; nach Nachbesserung 1 - Zellen-Tiefe fuer ausgeschlossene Rechtecke,
 *       tools/maske/tiefe_zelle_r41.py - 11494 von 62432 = 18,4 %; Nachbesserung 2, Zellen-Modelle mit 3D-Urteil und Zuordnung von
 *       Hand, tools/maske/zellen3d_r41.py - 10178 von 63546 = 16,0 %; Nachbesserung 3, Teilrechtecke je Koerper, tools/maske/
 *       kacheln_r41.py - 9036 von 63116 = 14,3 %) und VORHER > 10000 (Gegenprobe: das Mass findet die Kerben);
 *     - Blit: die gezeichneten Punkte NACHHER passen zum Hintergrund (eigener Atlas = Hintergrund an der Zielstelle).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re15_pri.h"
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_tim.h"
#include "re15_sld.h"
#include "re15_bss.h"
#include "re15_masken36_r37.h"
#include "re15_pri1100_r37.h"

#define PSX RE15_PORT_SRC_DIR "/shared_assets/PSX"
#define CHUNK 0x10000
#define FARBE_MAX 48           /* R37 §2.1 Blit-Pruefung: Anteil > 48 */

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static uint8_t *slurp(const char *pfad, size_t *n)
{
    FILE *f = fopen(pfad, "rb"); long len; uint8_t *p;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len <= 0) { fclose(f); return NULL; }
    p = (uint8_t *) malloc((size_t) len);
    if (p && fread(p, 1, (size_t) len, f) != (size_t) len) { free(p); p = NULL; }
    fclose(f);
    if (n) *n = (size_t) len;
    return p;
}

static uint8_t *bg_block(unsigned raum, int cut, size_t *n)
{
    char p[700]; uint8_t *b, *sb; size_t nsb = 0;
    snprintf(p, sizeof p, "%s/BSS/ROOM%04X/BG%02d.BSS", PSX, raum, cut);
    b = slurp(p, n);
    if (b) return b;
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%03X.BSS", PSX, raum >> 12, raum >> 4);
    sb = slurp(p, &nsb);
    if (!sb || (size_t) (cut + 1) * CHUNK > nsb) { free(sb); *n = 0; return NULL; }
    b = (uint8_t *) malloc(CHUNK);
    if (b) memcpy(b, sb + (size_t) cut * CHUNK, CHUNK);
    free(sb);
    *n = b ? CHUNK : 0;
    return b;
}

static int bg_rgb(unsigned raum, int cut, uint8_t *rgb)
{
    re15_bss_chunk_t ch; int16_t *co; size_t cap, n = 0; int k, rv; uint8_t *blk = bg_block(raum, cut, &n);
    if (!blk || !re15_bss_parse_chunk(blk, n, &ch) || !re15_bss_chunk_has_video(&ch)) { free(blk); return 0; }
    cap = ((size_t) ch.run_length_words + 2) * 8 + 4096;
    co = (int16_t *) malloc(cap * sizeof(int16_t));
    if (!co) { free(blk); return 0; }
    k = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant, ch.version, co, cap);
    rv = (k >= 0) ? re15_bss_mdec_decode(co, (size_t) k, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, rgb) : -1;
    free(co); free(blk);
    return rv == 0;
}

typedef struct { uint8_t *roh; re15_tim_t tim; int da; } atlas_t;
static uint8_t g_sld[RE15_SLD_MAX_UNPACKED];

static int atlas_bss(unsigned raum, int cut, atlas_t *a)
{
    char p[700]; size_t nb = 0, ns = 0; uint8_t *blk, *sb; uint16_t L = 0; int len = 0, rv;
    memset(a, 0, sizeof *a);
    blk = bg_block(raum, cut, &nb);
    snprintf(p, sizeof p, "%s/BIN/STAGE%u.BIN", PSX, raum >> 12);
    sb = slurp(p, &ns);
    if (blk && sb) {
        rv = re15_sld_used_len(sb, (int) ns, (int) (raum >> 12), (int) ((raum >> 4) & 0xFFu), cut, &L);
        if (rv == RE15_SLD_OK) rv = re15_sld_atlas_from_chunk(blk, (int) nb, L, g_sld, RE15_SLD_MAX_UNPACKED, &len);
        if (rv == RE15_SLD_OK && re15_tim_parse(g_sld, len, &a->tim) == 0 && a->tim.bpp == 8 && a->tim.has_clut) a->da = 1;
    }
    free(blk); free(sb);
    return a->da;
}

static int atlas_eigen(unsigned raum, int cut, atlas_t *a)
{
    char p[700]; size_t n = 0;
    memset(a, 0, sizeof *a);
    snprintf(p, sizeof p, "%s/MASKS/ROOM%04X_PRI%02d.TIM", PSX, raum, cut);
    a->roh = slurp(p, &n);
    if (a->roh && re15_tim_parse(a->roh, (int) n, &a->tim) == 0 && a->tim.bpp == 8 && a->tim.has_clut) { a->da = 1; return 1; }
    free(a->roh); a->roh = NULL;
    return 0;
}

static int farbe(const atlas_t *a, int ax, int ay, int *r, int *g, int *b)
{
    const uint8_t *idx = (const uint8_t *) a->tim.pixels; uint16_t c; int i;
    if (ax < 0 || ay < 0 || ax >= a->tim.width || ay >= a->tim.height) return 0;
    i = idx[ay * a->tim.width + ax];
    if (!i) return 0;
    c = a->tim.clut[i];
    *r = (c & 0x1f) << 3; *g = ((c >> 5) & 0x1f) << 3; *b = ((c >> 10) & 0x1f) << 3;
    return 1;
}

/* deck[y][x] = 1, wenn eines der Rechtecke 0..n-1 (aus != NULL: ausgelassene) dort einen deckenden Atlaspunkt zeichnet */
static void zeichnen(const re15_pri_cut_t *pri, int n, const uint8_t *aus, const atlas_t *a, uint8_t deck[240][320])
{
    const uint8_t *idx = (const uint8_t *) a->tim.pixels; int m, x, y;
    memset(deck, 0, 240 * 320);
    for (m = 0; m < n; m++) {
        const re15_pri_mask_t *q = &pri->masks[m];
        int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY;
        if (aus && aus[m]) continue;
        for (y = 0; y < q->height; y++)
            for (x = 0; x < q->width; x++) {
                int ax = q->srcX + x, ay = q->srcY + y, tx = dx + x, ty = dy + y;
                if (ax >= a->tim.width || ay >= a->tim.height || tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
                if (idx[ay * a->tim.width + ax]) deck[ty][tx] = 1;
            }
    }
}

/* Alte Generator-Rechtecke: Quelle im BSS-Atlas voll deckend + Blit passt (Mittel <= 40, Anteil > 48 <= 20 % wie R37). */
static int alt_rechteck(const re15_pri_mask_t *q, const atlas_t *bss, const uint8_t *rgb)
{
    long summe = 0; int n = 0, gross = 0, x, y, r, g, b;
    int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY;
    for (y = 0; y < q->height; y++)
        for (x = 0; x < q->width; x++) {
            int tx = dx + x, ty = dy + y, d;
            if (!farbe(bss, q->srcX + x, q->srcY + y, &r, &g, &b)) return 0;
            if (tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
            d = abs(r - rgb[(ty * 320 + tx) * 3]) + abs(g - rgb[(ty * 320 + tx) * 3 + 1]) + abs(b - rgb[(ty * 320 + tx) * 3 + 2]);
            summe += d; n++; if (d > FARBE_MAX) gross++;
        }
    return n > 0 && summe <= 40L * n && gross * 5 <= n;
}

/* offene Kerben: s. Kopf */
static long kerben(const re15_pri_cut_t *pri, const uint8_t *alt, uint8_t deck[240][320], const atlas_t *bss, const uint8_t *rgb)
{
    static uint8_t gezaehlt[240][320]; long k = 0; int m;
    memset(gezaehlt, 0, sizeof gezaehlt);
    for (m = 0; m < pri->mask_count; m++) {
        const re15_pri_mask_t *q = &pri->masks[m];
        int X = (int16_t) q->dstX, Y = (int16_t) q->dstY, sx = X - q->srcX, sy = Y - q->srcY, i, seite;
        if (!alt[m]) continue;
        for (seite = 0; seite < 4; seite++) {
            int n = (seite < 2) ? q->width : q->height;
            for (i = 0; i < n; i++) {
                int qx, qy, px, py, r, g, b, d;
                if (seite == 0) { qx = X + i; qy = Y; px = qx; py = Y - 1; }
                else if (seite == 1) { qx = X + i; qy = Y + q->height - 1; px = qx; py = Y + q->height; }
                else if (seite == 2) { qx = X; qy = Y + i; px = X - 1; py = qy; }
                else { qx = X + q->width - 1; qy = Y + i; px = X + q->width; py = qy; }
                if (qx < 0 || qy < 0 || qx >= 320 || qy >= 240 || px < 0 || py < 0 || px >= 320 || py >= 240) continue;
                if (!deck[qy][qx] || deck[py][px] || gezaehlt[py][px]) continue;
                if (!farbe(bss, px - sx, py - sy, &r, &g, &b)) continue;
                d = abs(r - rgb[(py * 320 + px) * 3]) + abs(g - rgb[(py * 320 + px) * 3 + 1]) + abs(b - rgb[(py * 320 + px) * 3 + 2]);
                if (d > FARBE_MAX) continue;
                gezaehlt[py][px] = 1; k++;
            }
        }
    }
    return k;
}

static void teil_loecher(void)
{
    static uint8_t rgb[320 * 240 * 3];
    static uint8_t deck_alt[240][320], deck_neu[240][320];
    int stage, rr, var, sekt = 0, eigen = 0, quelle_eigen = 0, alt_n = 0, masken = 0;
    long k_alt = 0, k_neu = 0, gemalt = 0, gross = 0;
    double mittel_max = 0;
    printf("== r41_masken36 loecher: Generator-Sektionen geschlossen (offene Kerben vorher/nachher, eigener Atlas) ==\n");
    for (stage = 3; stage <= 6; stage++)
        for (rr = 0; rr < 0x40; rr++)
            for (var = 0; var < 2; var++) {
                unsigned raum = ((unsigned) stage << 12) | ((unsigned) rr << 4) | (unsigned) var;
                char p[700]; uint8_t *msk; size_t mn = 0; int c, ncut;
                long k_alt_d = 0, k_neu_d = 0;
                snprintf(p, sizeof p, "%s/MASKS/ROOM%04X.MSK", PSX, raum);
                msk = slurp(p, &mn);
                if (!msk) continue;
                ncut = (mn >= 12) ? (int) (msk[8] | (msk[9] << 8)) : 0;
                for (c = 0; c < ncut; c++) {
                    re15_pri_cut_t pri, lader; atlas_t eig, bss; uint32_t off; uint8_t alt[RE15_PRI_MAX_MASKS_PER_CUT];
                    int m, x, y, n_alt = 0; long sd = 0, sn = 0;
                    off = re15_pri_msk_section_offset(msk, mn, c);
                    if (!off || re15_pri_parse_section(msk, mn, off, &pri) <= 0) continue;
                    sekt++; masken += pri.mask_count;
                    if (!atlas_eigen(re15_pri_masken_raum(raum), c, &eig)) { printf("  ROOM%04X Cut %d: KEIN eigener Atlas\n", raum, c); continue; }
                    eigen++;
                    if (re15_pri_atlas_quelle(1, eig.da) == RE15_PRI_ATLAS_EIGEN) quelle_eigen++;
                    if (!atlas_bss(raum, c, &bss) || !bg_rgb(raum, c, rgb)) {
                        printf("  ROOM%04X Cut %d: BSS-Atlas/Hintergrund fehlt\n", raum, c); g_fail = 1; free(eig.roh); continue;
                    }
                    for (m = 0; m < pri.mask_count; m++) { alt[m] = (uint8_t) alt_rechteck(&pri.masks[m], &bss, rgb); n_alt += alt[m]; }
                    alt_n += n_alt;
                    /* VORHER: nur die alten Rechtecke durch den BSS-Atlas, mit demselben Ausschluss wie NACHHER (der Vergleich
                     * misst damit allein die Saum-Rechtecke, nicht den Ausschluss) */
                    {   uint8_t nicht_alt[RE15_PRI_MAX_MASKS_PER_CUT];
                        for (m = 0; m < pri.mask_count; m++)
                            nicht_alt[m] = (uint8_t) (!alt[m] || re15_masken36_ausgeschlossen(re15_pri_masken_raum(raum), c, m));
                        zeichnen(&pri, pri.mask_count, nicht_alt, &bss, deck_alt); }
                    /* NACHHER: wie der Lader (main.c: re15_masken36_filter = Ausschluss der Spur P R37), eigener Atlas */
                    lader = pri;
                    re15_masken36_filter(re15_pri_masken_raum(raum), c, &lader);
                    zeichnen(&lader, lader.draw_count, NULL, &eig, deck_neu);
                    k_alt_d += kerben(&pri, alt, deck_alt, &bss, rgb);
                    k_neu_d += kerben(&pri, alt, deck_neu, &bss, rgb);
                    for (y = 0; y < 240; y++)
                        for (x = 0; x < 320; x++) gemalt += deck_neu[y][x];
                    {   /* Blit NACHHER: Farbe des gezeichneten Punkts gegen den Hintergrund (vorderstes Rechteck egal: alle
                         * Texel sind Hintergrund an der Zielstelle) */
                        int mm;
                        for (mm = 0; mm < lader.draw_count; mm++) {
                            const re15_pri_mask_t *q = &lader.masks[mm];
                            int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY, xx, yy, r, g, b, d;
                            for (yy = 0; yy < q->height; yy++)
                                for (xx = 0; xx < q->width; xx++) {
                                    int tx = dx + xx, ty = dy + yy;
                                    if (tx < 0 || ty < 0 || tx >= 320 || ty >= 240) continue;
                                    if (!farbe(&eig, q->srcX + xx, q->srcY + yy, &r, &g, &b)) continue;
                                    d = abs(r - rgb[(ty * 320 + tx) * 3]) + abs(g - rgb[(ty * 320 + tx) * 3 + 1])
                                      + abs(b - rgb[(ty * 320 + tx) * 3 + 2]);
                                    sd += d; sn++; if (d > FARBE_MAX) gross++;
                                }
                        }
                        if (sn && (double) sd / sn > mittel_max) mittel_max = (double) sd / sn;
                    }
                    free(eig.roh); free(bss.roh);
                }
                if (ncut) printf("  ROOM%04X: offene Kerben vorher %ld, nachher %ld\n", raum, k_alt_d, k_neu_d);
                k_alt += k_alt_d; k_neu += k_neu_d;
                free(msk);
            }
    printf("  Sektionen %d (Rechtecke %d), eigener Atlas %d, Lader nimmt ihn %d | alte Rechtecke erkannt %d\n",
           sekt, masken, eigen, quelle_eigen, alt_n);
    printf("  offene Kerben: VORHER (alte Rechtecke, BSS-Atlas, gleicher Ausschluss) %ld | NACHHER (wie der Lader) %ld (%.1f %%)\n",
           k_alt, k_neu, k_alt ? 100.0 * k_neu / k_alt : 0.0);
    printf("  Blit NACHHER: %ld gezeichnete Punkte, Abweichung je Sektion max Mittel %.2f / 765, Anteil > 48 gesamt %.3f %%\n",
           gemalt, mittel_max, gemalt ? 100.0 * gross / (double) gemalt : 0.0);
    CHECK("216 Generator-Sektionen der Stages 3..6, jede mit eigenem Atlas, den der Lader nimmt",
          sekt == 216 && eigen == 216 && quelle_eigen == 216);
    CHECK("die alten Generator-Rechtecke sind erkennbar (Quelle im BSS-Atlas voll deckend + Blit)", alt_n > 6000);
    CHECK("Gegenprobe: VORHER gibt es offene Kerben an den Objektkanten (> 10000)", k_alt > 10000);
    CHECK("NACHHER hoechstens 30 % der Kerben von VORHER (gemessen nach Nachbesserung 3 9036 von 63116 = 14,3 %, nach NB2 16,0 %, nach NB1 18,4 %, vor NB1 25,1 %; der Rest sind Saum-Rechtecke, die der "
          "Ausschluss nimmt, weil sie eine Figur VOR dem Objekt verdecken wuerden)", k_neu * 10 <= k_alt * 3);
    CHECK("Blit NACHHER: eigener Atlas = Hintergrund an der Zielstelle (Mittel je Sektion <= 12)", gemalt > 0 && mittel_max <= 12.0);
}


/* ---- Punkt 2: ROOM2020 Cut 3 an der Kurbel (Orchestrator-Hinweis nach Abnahme E 0) ---------------------------------- */
/* Abnahme E Hinweis H2: "ROOM2020 Cut 3 - MASKS/ROOM2020.MSK zeichnet die Reling ueber Leons Beine an der Kurbel
 * (Spielerlage nach Pos_set @0x0A8C = (3650,-1800,-2925)), obwohl Leon dort VOR der Reling steht; das Original hat in
 * Cut 3 keine PRI (RDT @0x3FC = ff ff ff ff)." Gemessen (Dossier 2.1): die Strecke Kamera -> Leon kreuzt im Grundriss die
 * solide Zelle der Suedreling (Band 1) - Leon steht HINTER der Reling, das Original zeichnet ihn nur deshalb obenauf, weil
 * es fuer Cut 3 gar keine Masken hat (NULL-Sektion). Die Nutzer-Freistellung 03_01 tut, wofuer sie gemacht ist. */
static void kamera_ort(const re15_camera_view_t *v, double c[3])
{
    const double a = v->rot[0], b = v->rot[1], cc = v->rot[2], d = v->rot[3], e = v->rot[4], f = v->rot[5],
                 g = v->rot[6], h = v->rot[7], k = v->rot[8];
    double det = a * (e * k - f * h) - b * (d * k - f * g) + cc * (d * h - e * g);
    double t0 = -4096.0 * v->trans[0], t1 = -4096.0 * v->trans[1], t2 = -4096.0 * v->trans[2];
    c[0] = ((e * k - f * h) * t0 + (cc * h - b * k) * t1 + (b * f - cc * e) * t2) / det;
    c[1] = ((f * g - d * k) * t0 + (a * k - cc * g) * t1 + (cc * d - a * f) * t2) / det;
    c[2] = ((d * h - e * g) * t0 + (b * g - a * h) * t1 + (a * e - b * d) * t2) / det;
}

static int proj(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy, double *vz)
{
    double vx = (x * v->rot[0] + y * v->rot[1] + z * v->rot[2]) / 4096.0 + v->trans[0];
    double vy = (x * v->rot[3] + y * v->rot[4] + z * v->rot[5]) / 4096.0 + v->trans[1];
    *vz = (x * v->rot[6] + y * v->rot[7] + z * v->rot[8]) / 4096.0 + v->trans[2];
    if (*vz <= 64) return 0;
    *sx = 160 + vx * v->fov_screen_dist / *vz; *sy = 120 + vy * v->fov_screen_dist / *vz;
    return 1;
}

/* Grundriss-Strecke (ax,az)->(bx,bz) schneidet den Kasten [x0,x1) x [z0,z1) (Endpunkte ausserhalb)? Abtastung 25 wie
 * test_r37_masken36.c zelle_dazwischen (RASTER). */
static int strecke_kasten(double ax, double az, double bx, double bz, double x0, double z0, double x1, double z1)
{
    double l = sqrt((bx - ax) * (bx - ax) + (bz - az) * (bz - az)); int n = (int) (l / 25.0) + 1, i;
    for (i = 1; i < n; i++) {
        double t = (double) i / n, x = ax + t * (bx - ax), z = az + t * (bz - az);
        if (x >= x0 && x < x1 && z >= z0 && z < z1) return 1;
    }
    return 0;
}

static void teil_kurbel(void)
{
    static re15_rdt_t rdt;
    static uint8_t rgb[320 * 240 * 3];
    const int LX = 3650, LY = -1800, LZ = -2925, BAND = 1;   /* Pos_set ROOM2020.RDT @0x0A8C, Band 1 = y -1800 */
    re15_camera_view_t v; char p[700]; uint8_t *roh, *msk; size_t rn = 0, mn = 0; re15_pri_cut_t pri; atlas_t eig;
    uint32_t off; double cam[3], sx, sy, vz; int i, zelle = -1, verdeckt = 0, punkte = 0, null_sektion = 0, h;
    printf("== r41_masken36 kurbel: ROOM2020 Cut 3, Leon an der Kurbel (3650,-1800,-2925) ==\n");
    snprintf(p, sizeof p, "%s/STAGE2/ROOM2020.RDT", PSX);
    roh = slurp(p, &rn);
    if (!roh || re15_rdt_parse(roh, rn, &rdt) < 0 || rdt.cut_count <= 3 || re15_camera_build_view(&rdt.cuts[3], &v) != 0) {
        printf("  FAIL: ROOM2020.RDT / Kamera Cut 3\n"); g_fail = 1; free(roh); return;
    }
    kamera_ort(&v, cam);
    for (i = 0; i < rdt.sca_count; i++) {
        const re15_sca_entry_t *e = &rdt.sca[i];
        if (e->type != 1 || !(e->u0 & 1) || (e->floor >> 4) != BAND) continue;
        if (strecke_kasten(cam[0], cam[2], LX, LZ, e->x, e->z, e->x + e->width, e->z + e->density)) {
            zelle = i;
            printf("  Kamera (%.0f,%.0f,%.0f) -> Leon (%d,%d): kreuzt SCA-Zelle %d x %d..%d z %d..%d (Band %d, u0 0x%02X)\n",
                   cam[0], cam[1], cam[2], LX, LZ, i, e->x, e->x + e->width, e->z, e->z + e->density, e->floor >> 4, e->u0);
            break;
        }
    }
    if (rdt.cuts[3].pri_offset + 4 <= rn) {
        const uint8_t *q = roh + rdt.cuts[3].pri_offset;
        null_sektion = q[0] == 0xFF && q[1] == 0xFF && q[2] == 0xFF && q[3] == 0xFF;
        printf("  Original Cut 3: Sektion +0x%X = %02X %02X %02X %02X\n", (unsigned) rdt.cuts[3].pri_offset, q[0], q[1], q[2], q[3]);
    }
    snprintf(p, sizeof p, "%s/MASKS/ROOM2020.MSK", PSX);
    msk = slurp(p, &mn);
    off = msk ? re15_pri_msk_section_offset(msk, mn, 3) : 0;
    if (!off || re15_pri_parse_section(msk, mn, off, &pri) <= 0 || !atlas_eigen(0x2020, 3, &eig) || !bg_rgb(0x2020, 3, rgb)) {
        printf("  FAIL: MSK/Atlas Cut 3\n"); g_fail = 1; free(roh); free(msk); return;
    }
    /* Beine: Mittelachse der Figur vom Fuss bis 1000 ueber dem Boden (Reling-Hoehe), je Bildzeile */
    for (h = 0; h <= 1000; h += 10) {
        int px, py, m, best = 0xFFFF;
        if (!proj(&v, LX, LY - h, LZ, &sx, &sy, &vz)) continue;
        px = (int) sx; py = (int) sy;
        if (px < 0 || py < 0 || px >= 320 || py >= 240) continue;
        punkte++;
        for (m = 0; m < pri.draw_count; m++) {
            const re15_pri_mask_t *q = &pri.masks[m];
            int dx = (int16_t) q->dstX, dy = (int16_t) q->dstY, r, g, b;
            if (px < dx || py < dy || px >= dx + q->width || py >= dy + q->height) continue;
            if (!farbe(&eig, q->srcX + px - dx, q->srcY + py - dy, &r, &g, &b)) continue;
            if (q->depth < best) best = q->depth;
        }
        if (best != 0xFFFF && re15_pri_mask_occludes(best, (long) vz)) verdeckt++;
    }
    printf("  Beinachse: %d Punkte im Bild, davon von der Reling-Maske verdeckt %d (Urteil depth < bucket)\n", punkte, verdeckt);
    CHECK("Original Cut 3 hat keine Masken (NULL-Sektion) - die Nutzer-Freistellung ist die einzige Verdeckung", null_sektion);
    CHECK("an der Kurbel kreuzt die Sichtlinie Kamera -> Leon eine solide Zelle (Reling) -> Leon steht HINTER der Reling",
          zelle >= 0);
    CHECK("die Reling-Maske verdeckt Leons Beine dort (geometrisch richtig, kein Fehler der Maske)", verdeckt > 0);
    free(roh); free(msk); free(eig.roh);
}

/* ---- Nachbesserung 4 (Abnahme 3, Mangel 1): Ost-Stirn der Buero-Trennwand ROOM30A0/30A1 Cut 0 ------------------------
 * Abnahme 3: "die Ost-Stirn der Buero-Trennwand (Zelle 6 @RDT+0x0045C, Flaeche x=-19975, z 14125..15375, Bildspalten 282..325)
 * hat keine Maske". Gemessen (Dossier NB4): die Stirnseite IM BILD (die graue Stirnplatte rechts vom Rahmenpfosten) ist der
 * Kuenstler-Ausschnitt des SLD-Atlas (Komponenten an den Verschiebungen (232,-24)/(144,24)/(57,86)) - und der ist ganz gezeichnet.
 * Die Flaeche der KOLLISIONSZELLE reicht 1250 tief und 3025 hoch; die Stirnplatte im Bild nur bis z ~14870 (Zeilen 118..194)
 * und bis y -2335 (Zeile 118). Hinter der Kante zeigt das Bild Boden und Rueckwand-Schraenke HINTER der Figur.
 *   test_r41_masken36 stirn   (a) Zelle 6 @RDT+0x0045C = 96 19 e2 04 63 98 2d 37 01 ff 00 03; (b) offene Kerben im Fenster
 *     x 282..319 y 80..215 (wie loecher: Kuenstler-Atlas deckend + farbgleich direkt an der Kante eines alten Rechtecks, nicht
 *     gezeichnet) == 0; (c) die Kante der Zeichnung liegt in den Zeilen 118..194 in der Ebene x = -19975 bei z <= 14900 (Zelle bis
 *     15375); (d) KEIN gezeichneter Punkt im Fenster, dessen Sehstrahl die Zellenflaeche x = -19975 bei z > 14900 kreuzt - eine
 *     Maske dort wuerde die Figur ueber dem Boden hinter der Zelle abschneiden; (e) Leon- und Elza-Datei gleich. */
#define STIRN_X   (-19975)                 /* Ostkante der Zelle 6: x + breite = -26525 + 6550 */
#define STIRN_Z0  14125
#define STIRN_Z1  15375                    /* z + tiefe = 14125 + 1250 */
#define STIRN_KANTE_Z 14900                /* gemessene Kante des Kuenstler-Ausschnitts z 14802..14867 (Dossier NB4) */

static int stirn_ebene(const re15_camera_view_t *v, const double cam[3], double px, double py, double *ey, double *ez)
{
    double cx = px - 160.0, cy = py - 120.0, cz = v->fov_screen_dist, dx, dy, dz, s;
    dx = v->rot[0] * cx + v->rot[3] * cy + v->rot[6] * cz;      /* Richtung = R^T * (sx, sy, H) */
    dy = v->rot[1] * cx + v->rot[4] * cy + v->rot[7] * cz;
    dz = v->rot[2] * cx + v->rot[5] * cy + v->rot[8] * cz;
    if (fabs(dx) < 1e-9) return 0;
    s = (STIRN_X - cam[0]) / dx;
    if (s <= 0) return 0;
    *ey = cam[1] + s * dy; *ez = cam[2] + s * dz;
    return 1;
}

static void teil_stirn(void)
{
    static const uint8_t soll6[12] = { 0x96, 0x19, 0xe2, 0x04, 0x63, 0x98, 0x2d, 0x37, 0x01, 0xff, 0x00, 0x03 };
    static re15_rdt_t rdt;
    static uint8_t rgb[320 * 240 * 3];
    static uint8_t deck[240][320], d2[240][320];
    re15_camera_view_t v; char p[700]; uint8_t *roh, *msk, *msk1; size_t rn = 0, mn = 0, mn1 = 0; re15_pri_cut_t pri, lader;
    atlas_t eig, bss; uint32_t off; uint8_t alt[RE15_PRI_MAX_MASKS_PER_CUT]; double cam[3], ey, ez;
    int m, x, y, zelle_ok, kante_ok = 1, kante_n = 0, jenseits = 0, im_fenster = 0;
    long kerben_fenster;
    double kante_min = 1e9, kante_max = -1e9;
    printf("== r41_masken36 stirn: ROOM30A0/30A1 Cut 0, Ost-Stirn der Trennwand (Zelle 6 @RDT+0x0045C) ==\n");
    snprintf(p, sizeof p, "%s/STAGE3/ROOM30A0.RDT", PSX);
    roh = slurp(p, &rn);
    if (!roh || rn < 0x468 || re15_rdt_parse(roh, rn, &rdt) < 0 || rdt.cut_count < 1 || re15_camera_build_view(&rdt.cuts[0], &v) != 0) {
        printf("  FAIL: ROOM30A0.RDT / Kamera Cut 0\n"); g_fail = 1; free(roh); return;
    }
    zelle_ok = !memcmp(roh + 0x45C, soll6, 12);
    kamera_ort(&v, cam);
    snprintf(p, sizeof p, "%s/MASKS/ROOM30A0.MSK", PSX); msk = slurp(p, &mn);
    snprintf(p, sizeof p, "%s/MASKS/ROOM30A1.MSK", PSX); msk1 = slurp(p, &mn1);
    off = msk ? re15_pri_msk_section_offset(msk, mn, 0) : 0;
    if (!off || re15_pri_parse_section(msk, mn, off, &pri) <= 0 || !atlas_eigen(0x30A0, 0, &eig) || !atlas_bss(0x30A0, 0, &bss)
        || !bg_rgb(0x30A0, 0, rgb)) {
        printf("  FAIL: MSK/Atlas/Hintergrund Cut 0\n"); g_fail = 1; free(roh); free(msk); free(msk1); return;
    }
    for (m = 0; m < pri.mask_count; m++) alt[m] = (uint8_t) alt_rechteck(&pri.masks[m], &bss, rgb);
    lader = pri;
    re15_masken36_filter(0x30A0, 0, &lader);                       /* wie der Lader (main.c) */
    zeichnen(&lader, lader.draw_count, NULL, &eig, deck);
    memcpy(d2, deck, sizeof d2);                                    /* (b) Kerben nur im Fenster zaehlen */
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++)
            if (x < 282 || y < 80 || y > 215) d2[y][x] = 1;
    kerben_fenster = kerben(&pri, alt, d2, &bss, rgb);
    for (y = 80; y <= 215; y++) {
        int rechts = -1;
        for (x = 282; x < 320; x++) {
            if (!stirn_ebene(&v, cam, x + 0.5, y + 0.5, &ey, &ez)) continue;
            if (ez >= STIRN_Z0 && ez <= STIRN_Z1 && ey <= 0 && ey >= -3025) im_fenster++;
            if (!deck[y][x]) continue;
            rechts = x;
            if (ez > STIRN_KANTE_Z && ez <= STIRN_Z1 && ey <= 0 && ey >= -3025) jenseits++;
        }
        if (y >= 118 && y <= 194 && rechts >= 282 && stirn_ebene(&v, cam, rechts + 0.5, y + 0.5, &ey, &ez)) {
            kante_n++;
            if (ez < kante_min) kante_min = ez;
            if (ez > kante_max) kante_max = ez;
            if (ez > STIRN_KANTE_Z) kante_ok = 0;
        }
    }
    printf("  Zelle 6 @RDT+0x0045C: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x (%s)\n", roh[0x45C], roh[0x45D], roh[0x45E],
           roh[0x45F], roh[0x460], roh[0x461], roh[0x462], roh[0x463], roh[0x464], roh[0x465], roh[0x466], roh[0x467],
           zelle_ok ? "wie belegt" : "ANDERS");
    printf("  Kamera Cut 0 (%.0f,%.0f,%.0f); Zellenflaeche x = %d im Fenster x 282..319 y 80..215: %d Bildpunkte\n", cam[0], cam[1],
           cam[2], STIRN_X, im_fenster);
    printf("  offene Kerben im Fenster: %ld | Kante der Zeichnung Zeilen 118..194 (%d Zeilen): z %.0f..%.0f (Zelle bis %d)\n",
           kerben_fenster, kante_n, kante_min, kante_max, STIRN_Z1);
    printf("  gezeichnete Punkte mit Sehstrahl durch die Zellenflaeche bei z > %d: %d\n", STIRN_KANTE_Z, jenseits);
    CHECK("Zelle 6 @RDT+0x0045C = 96 19 e2 04 63 98 2d 37 01 ff 00 03 (x -26525..-19975, z 14125..15375)", zelle_ok);
    CHECK("die Stirnseite IM BILD (Kuenstler-Ausschnitt) ist gezeichnet: 0 offene Kerben im Fenster x 282..319", kerben_fenster == 0);
    CHECK("die Kante der Zeichnung liegt bei z <= 14900 (gemessen 14802..14867): Stirnplatte im Bild ~705 tief, die Zelle 1250",
          kante_n >= 60 && kante_ok);
    CHECK("keine Maske ueber der Zellenflaeche jenseits der Kante (dort zeigt das Bild den Boden/die Rueckwand hinter der Figur)",
          jenseits == 0 && im_fenster > 1000);
    CHECK("Leon- und Elza-Datei bytegleich", msk1 && mn1 == mn && !memcmp(msk, msk1, mn));
    free(roh); free(msk); free(msk1); free(eig.roh); free(bss.roh);
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "loecher";
    if (!strcmp(teil, "loecher")) teil_loecher();
    else if (!strcmp(teil, "kurbel")) teil_kurbel();
    else if (!strcmp(teil, "stirn")) teil_stirn();
    else { printf("unbekannter Teil %s\n", teil); return 2; }
    return g_fail;
}
