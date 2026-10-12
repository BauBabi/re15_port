/*
 * Runde 36 (2026-10-11), Spur O "pri" — Riegel ueber die ECHTEN Engine-Leser.
 * Dossier: analysis/befunde_runde36/O_pri.md, Regeln: include/re15_pri_schatten_r36.h.
 *
 *   test_r36_pri absperrung   Punkt 1 (ROOM10F0 Cut 2, Absperrung des Nutzers pri/STAGE1/10F0/02_02.png)
 *   test_r36_pri schatten     Punkt 2 (ROOM1100: Figurenschatten gegen die Wandmasken, OT-Regel)
 *
 * Alles laeuft ueber re15_rdt_parse -> re15_pri_msk_section_offset -> re15_pri_parse_section ->
 * re15_tim_parse und die Rasterung atlas[src] -> screen[dst] mit Palettenindex != 0 — dieselbe Kette
 * wie test_pri_silhouette / test_pri_kopfschnitt (gegen die Engine mit 0 Abweichungen in 84 Cuts).
 * Das Verdeckungsurteil ist das Original-Urteil re15_pri_mask_occludes (depth < (1023*vz)>>16,
 * @0x8002565c / ZSF3=341 @0x80066c70), die Koerpermasse die Messgroessen der Standplatz-Schiene
 * (Fuss..Kopf 1500, halbe Schulterbreite 450).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_pri.h"
#include "re15_tim.h"
#include "re15_cut10f0.h"
#include "re15_pri_schatten_r36.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

#define KOPF_HOCH   1500
#define HALB_BREIT   450

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

/* Ein Cut: RDT-Kamera, MSK-Sektion, Atlas, Rasterung (Deckung + naechste Tiefe je Bildpunkt). */
typedef struct {
    uint8_t *roh, *msk, *tb;
    re15_rdt_t rdt; re15_camera_view_t view; re15_pri_cut_t pri; re15_tim_t tim;
    uint8_t deck[240][320];
    uint16_t tief[240][320];
} cut_t;

static int cut_laden(cut_t *c, unsigned raum, int cut)
{
    char pfad[700]; size_t sz = 0, msz = 0, tsz = 0; uint32_t off; int n, i, x, y;
    memset(c, 0, sizeof *c);
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/STAGE%u/ROOM%04X.RDT", RE15_PORT_SRC_DIR, raum >> 12, raum);
    c->roh = slurp(pfad, &sz);
    if (!c->roh || re15_rdt_parse(c->roh, sz, &c->rdt) < 0 || cut >= c->rdt.cut_count
        || re15_camera_build_view(&c->rdt.cuts[cut], &c->view) != 0) {
        printf("  FEHLER: ROOM%04X Cut %d: RDT/Kamera (%s)\n", raum, cut, pfad); return 0;
    }
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X.MSK", RE15_PORT_SRC_DIR, raum);
    c->msk = slurp(pfad, &msz);
    off = c->msk ? re15_pri_msk_section_offset(c->msk, msz, cut) : 0;
    n = off ? re15_pri_parse_section(c->msk, msz, off, &c->pri) : 0;
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/MASKS/ROOM%04X_PRI%02d.TIM", RE15_PORT_SRC_DIR, raum, cut);
    c->tb = slurp(pfad, &tsz);
    if (n <= 0 || !c->tb || re15_tim_parse(c->tb, (int) tsz, &c->tim) != 0 || c->tim.bpp != 8 || !c->tim.has_clut) {
        printf("  FEHLER: ROOM%04X Cut %d: Sektion (%d) oder Atlas fehlt\n", raum, cut, n); return 0;
    }
    {   const uint8_t *idx = (const uint8_t *) c->tim.pixels;
        for (i = 0; i < c->pri.draw_count; i++) {
            const re15_pri_mask_t *m = &c->pri.masks[i];
            int dx = (int16_t) m->dstX, dy = (int16_t) m->dstY;
            for (y = 0; y < m->height; y++) {
                int sy = m->srcY + y, ty = dy + y;
                if (sy < 0 || sy >= c->tim.height || ty < 0 || ty >= 240) continue;
                for (x = 0; x < m->width; x++) {
                    int sx = m->srcX + x, tx = dx + x;
                    if (sx < 0 || sx >= c->tim.width || tx < 0 || tx >= 320) continue;
                    if (!idx[sy * c->tim.width + sx]) continue;
                    c->deck[ty][tx] = 1;
                    if (!c->tief[ty][tx] || m->depth < c->tief[ty][tx]) c->tief[ty][tx] = m->depth;
                }
            }
        }
    }
    return 1;
}

static void cut_frei(cut_t *c) { free(c->roh); free(c->msk); free(c->tb); }

/* Projektion mit Gleitkomma wie tools/maske/abnahme.proj (dieselbe Zaehlung wie der Bau). */
static int proj(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy, double *vz)
{
    double vx = (x * v->rot[0] + y * v->rot[1] + z * v->rot[2]) / 4096.0 + v->trans[0];
    double vy = (x * v->rot[3] + y * v->rot[4] + z * v->rot[5]) / 4096.0 + v->trans[1];
    *vz = (x * v->rot[6] + y * v->rot[7] + z * v->rot[8]) / 4096.0 + v->trans[2];
    if (*vz <= 64) return 0;
    *sx = 160 + vx * v->fov_screen_dist / *vz; *sy = 120 + vy * v->fov_screen_dist / *vz;
    return 1;
}

/* Kamera-z der Senkrechten durch (wx,wz) in Bildzeile y (abnahme/geometrie.profil_spalte). */
static double vz_senkrechte(const re15_camera_view_t *v, double wx, double wz, int y, double fallback)
{
    double H = v->fov_screen_dist;
    double a = v->rot[4] / 4096.0, b = (v->rot[3] * wx + v->rot[5] * wz) / 4096.0 + v->trans[1];
    double c = v->rot[7] / 4096.0, d = (v->rot[6] * wx + v->rot[8] * wz) / 4096.0 + v->trans[2];
    double sy = y + 0.5 - 120.0, n = sy * c - H * a, Y, vz;
    if (fabs(n) < 1e-9) return fallback;
    Y = (H * b - sy * d) / n; vz = c * Y + d;
    return (vz > 1) ? vz : fallback;
}

/* Koerperkasten eines Standplatzes: Maskenpunkte darin und davon die VERDECKENDEN (Original-Urteil). */
static int koerper(const cut_t *c, int wx, int wz, int *n_masken, int *x0o, int *x1o, int *y0o, int *y1o)
{
    double fsx, fsy, fvz, ksx, ksy, kvz, hw; int x0, x1, y0, y1, x, y, verd = 0, nm = 0;
    if (!proj(&c->view, wx, 0, wz, &fsx, &fsy, &fvz) || !proj(&c->view, wx, -KOPF_HOCH, wz, &ksx, &ksy, &kvz)) return -1;
    hw = HALB_BREIT * c->view.fov_screen_dist / fvz;
    x0 = (int) (fsx - hw); if (x0 < 0) x0 = 0;  x1 = (int) (fsx + hw); if (x1 > 320) x1 = 320;
    y0 = (int) (ksy < fsy ? ksy : fsy); if (y0 < 0) y0 = 0;  y1 = (int) (ksy > fsy ? ksy : fsy); if (y1 > 240) y1 = 240;
    for (y = y0; y < y1; y++) {
        double vz = vz_senkrechte(&c->view, wx, wz, y, fvz);
        for (x = x0; x < x1; x++) {
            if (!c->deck[y][x]) continue;
            nm++;
            if (re15_pri_mask_occludes((int) c->tief[y][x], (long) vz)) verd++;
        }
    }
    if (n_masken) *n_masken = nm;
    if (x0o) { *x0o = x0; *x1o = x1; *y0o = y0; *y1o = y1; }
    return verd;
}

/* ---------------------------------------------------------------------------------------------- */
static int teil_absperrung(void)
{
    cut_t c; int verd, nm, x0, x1, y0, y1, k, frei_ok = 1;
    /* Marvins Wegpunkte VOR der Scheibe, gemessen (RE15_STATE_LOG, Dossier §1.4): er laeuft an der
     * Zellenflaeche x=4100 entlang (4100 + Radius 450 = 4550 -> geklemmt 4568). */
    static const int weg[][2] = { {4568, 6941}, {4568, 7665}, {4568, 8365}, {5528, 2510} };
    printf("== r36_pri absperrung: ROOM10F0 Cut 2 ==\n");
    if (!cut_laden(&c, 0x10F0, 2)) { g_fail = 1; return 1; }
    printf("  Sektion: %d Rechtecke (gezeichnet), Atlas %dx%d\n", c.pri.draw_count, c.tim.width, c.tim.height);
    CHECK("Sektion traegt beide Objekte: > 27 Rechtecke (Stuhl allein hatte 27)", c.pri.draw_count > 27 && c.pri.draw_count <= RE15_PRI_MAX_MASKS_PER_CUT);
    /* Die Scheibenkante (Lasso 02_02.png, x=33..98 y=56..237): Bildpunkt (93,140) ist Scheibe, (176..210,115..173) der Stuhl. */
    CHECK("Scheibenpunkt (93,140) ist gedeckt", c.deck[140][93]);
    CHECK("Scheibenpunkt (93,140) liegt vor Marvins Standplatz (Tiefe < 134)", c.deck[140][93] && c.tief[140][93] < 134);
    CHECK("Stuhlpunkt (190,140) weiter gedeckt", c.deck[140][190]);
    CHECK("Boden rechts der Scheibe (120,200) bleibt frei", !c.deck[200][120]);
    verd = koerper(&c, RE15_CUT10F0_MARVIN_X, RE15_CUT10F0_MARVIN_Z, &nm, &x0, &x1, &y0, &y1);
    printf("  Marvin (%d,%d): Kasten x%d..%d y%d..%d, Maskenpunkte %d, verdeckend %d\n",
           RE15_CUT10F0_MARVIN_X, RE15_CUT10F0_MARVIN_Z, x0, x1, y0, y1, nm, verd);
    CHECK("Marvins Standplatz: linke Seite von der Scheibe verdeckt (>= 60 Punkte)", verd >= 60);
    CHECK("Marvins Standplatz: nicht der ganze Kasten (< 60 %)", verd * 100 < (x1 - x0) * (y1 - y0) * 60);
    for (k = 0; k < (int) (sizeof weg / sizeof weg[0]); k++) {
        verd = koerper(&c, weg[k][0], weg[k][1], &nm, &x0, &x1, &y0, &y1);
        printf("  Wegpunkt (%d,%d): Kasten x%d..%d y%d..%d, Maskenpunkte %d, verdeckend %d\n",
               weg[k][0], weg[k][1], x0, x1, y0, y1, nm, verd);
        if (verd != 0) frei_ok = 0;
    }
    CHECK("Wegpunkte vor der Scheibe (x=4568, Zelle 3 @0x6B4): 0 verdeckende Punkte", frei_ok);
    cut_frei(&c);
    return g_fail;
}

/* ---------------------------------------------------------------------------------------------- */
/* Schattenquad des Spielers wie platform/pc/main.c: Ecken +-500/+-600 (entity+0xbc/+0xbe), Gierung =
 * Spieler rot_y (RotMatrixY @0x8001b0e4), Basis y=0; Summe der Ecken-Kamera-z = AVSZ4-Eingang. */
static int schatten_quad(const cut_t *c, int px, int pz, int16_t rot, double sx[4], double sy[4], int32_t *vz_summe)
{
    static const int32_t ecke[4][3] = { {-500, 0, 600}, {-500, 0, -600}, {500, 0, 600}, {500, 0, -600} };
    int32_t yaw[9], rotm[9], trans[3], welt[3] = { px, 0, pz }; int v;
    re15_camera_yaw_matrix_angle(rot, yaw);
    re15_camera_compose_view_bone(&c->view, yaw, welt, rotm, trans);
    *vz_summe = 0;
    for (v = 0; v < 4; v++) {
        int32_t x = ecke[v][0], y = ecke[v][1], z = ecke[v][2];
        int32_t vx = (int32_t) (((int64_t) x * rotm[0] + (int64_t) y * rotm[1] + (int64_t) z * rotm[2]) >> 12) + trans[0];
        int32_t vy = (int32_t) (((int64_t) x * rotm[3] + (int64_t) y * rotm[4] + (int64_t) z * rotm[5]) >> 12) + trans[1];
        int32_t vz = (int32_t) (((int64_t) x * rotm[6] + (int64_t) y * rotm[7] + (int64_t) z * rotm[8]) >> 12) + trans[2];
        if (vz < 64) return 0;
        *vz_summe += (vz > 0xFFFF) ? 0xFFFF : vz;
        sx[v] = 160.0 + (double) vx * c->view.fov_screen_dist / vz;
        sy[v] = 120.0 + (double) vy * c->view.fov_screen_dist / vz;
    }
    return 1;
}

/* Punkt im Viereck (Ecken 0,2,3,1 = Umlauf) */
static int im_quad(const double *sx, const double *sy, double x, double y)
{
    static const int u[4] = { 0, 2, 3, 1 }; int i, inside = 0;
    for (i = 0; i < 4; i++) {
        double xa = sx[u[i]], ya = sy[u[i]], xb = sx[u[(i + 1) & 3]], yb = sy[u[(i + 1) & 3]];
        if ((ya > y) != (yb > y) && x < xa + (y - ya) * (xb - xa) / (yb - ya)) inside = !inside;
    }
    return inside;
}

static int schatten_messen(unsigned raum, int cut, int px, int pz, int16_t rot, int *in_quad, int *ueber)
{
    cut_t c; double sx[4], sy[4]; int32_t summe; int bucket, x, y, n = 0, u = 0, r = 0;
    if (!cut_laden(&c, raum, cut)) { g_fail = 1; return -1; }
    if (!schatten_quad(&c, px, pz, rot, sx, sy, &summe)) { printf("  FEHLER: Schattenquad hinter der Kamera\n"); cut_frei(&c); g_fail = 1; return -1; }
    bucket = re15_schatten_bucket(summe);
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++) {
            int rand = 0, dx, dy;
            if (!c.deck[y][x] || !im_quad(sx, sy, x + 0.5, y + 0.5)) continue;
            n++;
            if (!re15_schatten_maske_darueber((int) c.tief[y][x], bucket, 1)) continue;
            /* Runde 37 Spur A: Randpunkte der Silhouette (ein Nachbar ohne Maske) zaehlen nicht. Seit die Wandmasken
             * je Rechteck nur EINE Tiefenstufe tragen (analysis/befunde_runde37/A_pri1100.md), traegt der Wandfuss die
             * Tiefe der Wandbasis statt des Kachel-Maximums; an Cut 2 (-15100,-18500) liegen dann 3 Randpunkte
             * (53,237)/(52,238)/(51,239) mit Tiefe 63 < Bucket 65 im Quad — ihr Pixelmittelpunkt zeigt Boden 8..13
             * Einheiten vor der Maskenflaeche x=-15710, also die halbe Rasterzelle, nicht die Tiefe. Die echte exe
             * zeigt dort 0 geaenderte Punkte (integration_r36_pri_schatten). Der Befund der Runde 36 (Bodenstreifen
             * des Zellenpolsters, v0.8.23-Masken) bleibt erkannt: 31 innere Punkte UEBER dem Schatten. */
            for (dy = -1; dy <= 1; dy++)
                for (dx = -1; dx <= 1; dx++)
                    if (y + dy < 0 || y + dy >= 240 || x + dx < 0 || x + dx >= 320 || !c.deck[y + dy][x + dx]) rand = 1;
            if (rand) r++; else u++;
        }
    printf("  ROOM%04X Cut %d Spieler (%d,%d) rot %d: Schatten-Bucket %d (Summe %ld), Maskenpunkte im Quad %d, davon UEBER dem Schatten %d (+%d am Silhouettenrand)\n",
           raum, cut, px, pz, rot, bucket, (long) summe, n, u, r);
    *in_quad = n; *ueber = u;
    cut_frei(&c);
    return bucket;
}

static int teil_schatten(void)
{
    int in_quad, ueber, b, verd, nm, x0, x1, y0, y1;
    cut_t c;
    printf("== r36_pri schatten: OT-Regel (re15_pri_schatten_r36.h) ==\n");
    /* Bucket = Summe >> 8  (AVSZ4: otz = (256*Summe)>>12 = Summe>>4 @0x80066c7c/80; OT-Index otz>>4 @0x8001b2d8) */
    CHECK("bucket(4*6400) == 100 == ((4*6400)>>4)>>4", re15_schatten_bucket(4 * 6400) == 100 && re15_schatten_bucket(4 * 6400) == ((4 * 6400) >> 4) >> 4);
    CHECK("bucket(4*6400+255) == 100, bucket(4*6400+256) == 101", re15_schatten_bucket(4 * 6400 + 255) == 100 && re15_schatten_bucket(4 * 6400 + 256) == 101);
    CHECK("bucket(negativ) == -1 (kein Schatten)", re15_schatten_bucket(-1) == -1);
    /* Maske ueber Schatten: depth < bucket; Gleichstand: Spieler-Schatten (vor den Masken @0x8001ce54 eingehaengt) oben */
    CHECK("Maske 99 ueber Schatten 100 (Spieler)", re15_schatten_maske_darueber(99, 100, 1) == 1);
    CHECK("Maske 100 NICHT ueber Schatten 100 (Spieler, Gleichstand)", re15_schatten_maske_darueber(100, 100, 1) == 0);
    CHECK("Maske 100 ueber Schatten 100 (NPC, Gleichstand unten)", re15_schatten_maske_darueber(100, 100, 0) == 1);
    CHECK("Maske 101 NICHT ueber Schatten 100", re15_schatten_maske_darueber(101, 100, 0) == 0 && re15_schatten_maske_darueber(101, 100, 1) == 0);
    /* Dreieck ueber Schatten: bucket(vz) = (1023*vz)>>16 (@0x8002565c) < Schatten-Bucket; Gleichstand wie oben */
    CHECK("Dreieck vz 6400 (Bucket 99) ueber Schatten 100", re15_schatten_dreieck_darueber(6400, 100, 1) == 1 && re15_pri_bucket_of_vz(6400) == 99);
    CHECK("Dreieck vz 6410 (Bucket 100): Spieler-Schatten oben, NPC-Schatten unten", re15_pri_bucket_of_vz(6410) == 100 && re15_schatten_dreieck_darueber(6410, 100, 1) == 0 && re15_schatten_dreieck_darueber(6410, 100, 0) == 1);
    CHECK("Dreieck vz 6471 (Bucket 101) NICHT ueber Schatten 100", re15_pri_bucket_of_vz(6471) == 101 && re15_schatten_dreieck_darueber(6471, 100, 1) == 0);
    /* Dieselbe Grenze wie das Masken-Urteil der Figur: Maske 100 verdeckt ab vz 6471 (re15_pri.h) */
    CHECK("re15_pri_mask_occludes(100, 6470) == 0 und (100, 6471) == 1", !re15_pri_mask_occludes(100, 6470) && re15_pri_mask_occludes(100, 6471));

    printf("== r36_pri schatten: ROOM1100 Wandmasken gegen den Spielerschatten ==\n");
    b = schatten_messen(0x1100, 2, -15100, -18500, -1024, &in_quad, &ueber);
    CHECK("Cut 2 (-15100,-18500): Masken im Schattenquad vorhanden (Wandfuss), aber keine UEBER dem Schatten", b > 0 && in_quad > 0 && ueber == 0);
    b = schatten_messen(0x1100, 1, -15100, -16900, -1024, &in_quad, &ueber);
    CHECK("Cut 1 (-15100,-16900): Masken im Schattenquad vorhanden, keine UEBER dem Schatten", b > 0 && in_quad > 0 && ueber == 0);
    /* Gegenprobe F595: der Zombie hinter der Ecke bleibt in Cut 2 verdeckt; ein Standplatz im offenen Flur bleibt frei. */
    if (cut_laden(&c, 0x1100, 2)) {
        verd = koerper(&c, -20275, -10713, &nm, &x0, &x1, &y0, &y1);
        printf("  Zombie 5 (-20275,-10713): Kasten %d Punkte, verdeckend %d\n", (x1 - x0) * (y1 - y0), verd);
        CHECK("Cut 2: Zombie 5 hinter der Westwand voll verdeckt (F595)", verd > 0 && verd == (x1 - x0) * (y1 - y0));
        verd = koerper(&c, -15100, -18500, &nm, &x0, &x1, &y0, &y1);
        CHECK("Cut 2: Spieler an der Wand (-15100,-18500) selbst unverdeckt", verd == 0);
        cut_frei(&c);
    } else g_fail = 1;
    if (cut_laden(&c, 0x1100, 1)) {
        verd = koerper(&c, -15100, -16900, &nm, &x0, &x1, &y0, &y1);
        CHECK("Cut 1: Spieler an der Wand (-15100,-16900) selbst unverdeckt", verd == 0);
        cut_frei(&c);
    } else g_fail = 1;
    return g_fail;
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    if (!strcmp(teil, "absperrung")) teil_absperrung();
    else if (!strcmp(teil, "schatten")) teil_schatten();
    else { teil_absperrung(); teil_schatten(); }
    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail ? 1 : 0;
}
