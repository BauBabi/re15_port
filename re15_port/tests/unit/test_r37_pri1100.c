/*
 * Runde 37 (2026-10-12), Spur A "pri1100" — Riegel ueber die ECHTEN Engine-Leser.
 * Dossier: analysis/befunde_runde37/A_pri1100.md.
 *
 * Nutzer (AUFTRAG.md #1): "Room 1100 das Problem ist immer noch da, siehe Zombie auf den F9 Marker."
 * Marke befund_1100_F892_marke1.bmp: Cut 1, Zombie 1 liegt an der Suedecke (-16157,0,-23876), Arm und Kopf
 * werden ueber die rechte Wand gemalt, obwohl die Wand an ihren Punkten 468..2329 Kamera-z DAVOR liegt (der Kopf
 * liegt in der Kollisionszelle Suedblock, z -22747..-23113). Ursache: die Gitterkacheln der alten Kette trugen das MAXIMUM der Wandtiefe
 * (Kachel (166,129) 24x24 t=161, Wandtiefe darin 7118..10275) -> alles bis 3253 hinter der Wand leckte durch.
 *
 *   test_r37_pri1100 leck    Cut 1 und Cut 2, ein Gitter von Weltpunkten (Schritt 100, Hoehen 0/-500/-1000/-1500):
 *                            liegt ein Punkt HINTER der Westwand-Ostflaeche (Sehstrahl Kamera -> Punkt schneidet die
 *                            Flaeche x=X_FLAECHE innerhalb z Z0..Z1, y -4000..0) um mehr als STUFE+1 Buckets, MUSS die
 *                            Maske ihn verdecken (Original-Urteil re15_pri_mask_occludes, depth < (1023*vz)>>16,
 *                            @0x8002565c / ZSF3=341 @0x80066c70; Masken-OT = depth @0x80039650-60). Liegt er VOR der
 *                            Flaeche, darf sie ihn nie verdecken. Punkte am Silhouettenrand (Nachbarpunkt ohne Maske)
 *                            zaehlen nicht (halbe Bildpunkte entscheidet die Rasterung, nicht die Tiefe).
 *   test_r37_pri1100 marke   die Kopf-/Armpunkte der Marken (Rueckprojektion der gemessenen Dreiecke, Dossier 1.3/4.2)
 *                            sind verdeckt; dieselben Punkte waren mit den v0.8.24-Masken frei (Schluessel 10378).
 *
 *   test_r37_pri1100 elza    Nachbesserung 1 (Abnahme A#0 M1): dieselben Riegel (leck + marke) fuer Elzas ROOM1101 -
 *                            Kamera aus ROOM1101.RDT, Masken ueber re15_pri_masken_raum(0x1101) (= der Lader,
 *                            entladen_pc.c/bg_pc.c). Vorher hatte 1101 gar keine Masken (MASKS/ROOM1101.* fehlt).
 *   test_r37_pri1100 zwilling  Zensus aus den Dateien (alle Paare STAGE1..6): Rueckfall Elza -> Leon genau dann, wenn
 *                            die Leon-Variante eine MSK hat, die Elza-Variante keine eigene, die Kamerasaetze gleich
 *                            sind (RDT[1] Cuts, RDT+0x24 Satz+0x00..+0x1B je Cut; @0x80021e68 fov, @0x80021e90 ->
 *                            FUN_80053ca4 liest pos +4..+12 / Ziel +16..+24 aus s3 = a0 = Satz, @0x80053cac,
 *                            @0x80053d1c..d48) und der Hintergrund je Cut gleich ist (so, wie
 *                            bg_pc.c ihn laedt; Original @0x80021c88..cd8: Raum OHNE Varianten-Ziffer). Ergebnis ==
 *                            re15_pri_masken_raum() fuer jede ungerade Id; gerade Ids bleiben unveraendert.
 *
 * Die Flaechenlage X_FLAECHE (-15690 Cut 1 / -15710 Cut 2) und die Wandzelle (z -23200..-12100) sind die Quader der
 * Auswahl (analysis/esp_masken_2026-09-03/auswahl.json, ROOM1100 "Westwand"; Wandbasis von Runde 36 Spur O am
 * Hintergrund gemessen, SCA-Zelle 2 @ROOM1100.RDT x-17600 z-23200 w2000 d11100). STUFE = die Tiefenstufe, mit der
 * bau_p2 den Cut gebaut hat (raum.py-Bericht: Cut 1 Stufe 4, Cut 2 Stufe 3).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_pri.h"
#include "re15_tim.h"
#include "re15_pri1100_r37.h"   /* re15_pri_masken_raum (Nachbesserung 1) */

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

/* Ein Cut: RDT-Kamera, MSK-Sektion, Atlas, Rasterung (Deckung + naechste Tiefe je Bildpunkt) — dieselbe Kette wie
 * test_r36_pri.c / test_pri_silhouette (re15_pri_msk_section_offset -> re15_pri_parse_section -> re15_tim_parse,
 * atlas[src] -> screen[dst], Palettenindex != 0). */
typedef struct {
    uint8_t *roh, *msk, *tb;
    re15_rdt_t rdt; re15_camera_view_t view; re15_pri_cut_t pri; re15_tim_t tim;
    uint8_t deck[240][320];
    uint16_t tief[240][320];
} cut_t;

static int cut_laden(cut_t *c, unsigned raum, int cut, const char *mskdir)
{
    char pfad[700]; size_t sz = 0, msz = 0, tsz = 0; uint32_t off; int n, i, x, y;
    /* Nachbesserung 1: die Masken-Dateien kommen wie im Lader (entladen_pc.c / bg_pc.c) von re15_pri_masken_raum -
     * fuer ROOM1100 unveraendert ROOM1100, fuer Elzas ROOM1101 die Leon-Variante. Die KAMERA kommt aus der RDT von raum. */
    const unsigned mraum = re15_pri_masken_raum(raum);
    memset(c, 0, sizeof *c);
    snprintf(pfad, sizeof pfad, "%s/shared_assets/PSX/STAGE%u/ROOM%04X.RDT", RE15_PORT_SRC_DIR, raum >> 12, raum);
    c->roh = slurp(pfad, &sz);
    if (!c->roh || re15_rdt_parse(c->roh, sz, &c->rdt) < 0 || cut >= c->rdt.cut_count
        || re15_camera_build_view(&c->rdt.cuts[cut], &c->view) != 0) {
        printf("  FEHLER: ROOM%04X Cut %d: RDT/Kamera (%s)\n", raum, cut, pfad); return 0;
    }
    snprintf(pfad, sizeof pfad, "%s/ROOM%04X.MSK", mskdir, mraum);
    c->msk = slurp(pfad, &msz);
    off = c->msk ? re15_pri_msk_section_offset(c->msk, msz, cut) : 0;
    n = off ? re15_pri_parse_section(c->msk, msz, off, &c->pri) : 0;
    snprintf(pfad, sizeof pfad, "%s/ROOM%04X_PRI%02d.TIM", mskdir, mraum, cut);
    c->tb = slurp(pfad, &tsz);
    if (n <= 0 || !c->tb || re15_tim_parse(c->tb, (int) tsz, &c->tim) != 0 || c->tim.bpp != 8 || !c->tim.has_clut) {
        printf("  FEHLER: ROOM%04X Cut %d: Sektion (%d) oder Atlas fehlt (%s)\n", raum, cut, n, mskdir); return 0;
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

/* Raum der Riegel leck/marke: ROOM1100 (Leon); "elza" setzt ROOM1101 (Nachbesserung 1). */
static unsigned g_raum = 0x1100;

/* Projektion mit Gleitkomma wie tools/maske/abnahme.proj und test_r36_pri.c. */
static int proj(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy, double *vz)
{
    double vx = (x * v->rot[0] + y * v->rot[1] + z * v->rot[2]) / 4096.0 + v->trans[0];
    double vy = (x * v->rot[3] + y * v->rot[4] + z * v->rot[5]) / 4096.0 + v->trans[1];
    *vz = (x * v->rot[6] + y * v->rot[7] + z * v->rot[8]) / 4096.0 + v->trans[2];
    if (*vz <= 64) return 0;
    *sx = 160 + vx * v->fov_screen_dist / *vz; *sy = 120 + vy * v->fov_screen_dist / *vz;
    return 1;
}

/* Kameraort in Weltkoordinaten: v = R w / 4096 + t  ->  w(v=0) = -4096 R^-1 t. R ist die Q12-Matrix der Ansicht; ihr
 * EXAKTES Inverses (Cramer) statt R^T, weil die Q12-Rundung R nicht orthonormal laesst — R^T verschob den Kameraort
 * um rund 50..120 Welteinheiten (gemessen an den Schattenquad-Ecken), genug, um Punkte an der Wand falsch zuzuordnen. */
static void kamera_ort(const re15_camera_view_t *v, double c[3])
{
    const double a = v->rot[0], b = v->rot[1], cc = v->rot[2], d = v->rot[3], e = v->rot[4], f = v->rot[5],
                 g = v->rot[6], h = v->rot[7], k = v->rot[8];
    double det = a * (e * k - f * h) - b * (d * k - f * g) + cc * (d * h - e * g);
    double inv[9], t0 = -4096.0 * v->trans[0], t1 = -4096.0 * v->trans[1], t2 = -4096.0 * v->trans[2];
    inv[0] = (e * k - f * h) / det; inv[1] = (cc * h - b * k) / det; inv[2] = (b * f - cc * e) / det;
    inv[3] = (f * g - d * k) / det; inv[4] = (a * k - cc * g) / det; inv[5] = (cc * d - a * f) / det;
    inv[6] = (d * h - e * g) / det; inv[7] = (b * g - a * h) / det; inv[8] = (a * e - b * d) / det;
    c[0] = inv[0] * t0 + inv[1] * t1 + inv[2] * t2;
    c[1] = inv[3] * t0 + inv[4] * t1 + inv[5] * t2;
    c[2] = inv[6] * t0 + inv[7] * t1 + inv[8] * t2;
}

/* Sehstrahl durch den Bildpunkt (bx, by) als Weltrichtung (exaktes Inverses wie kamera_ort). */
static void strahl(const re15_camera_view_t *v, double bx, double by, double dir[3])
{
    const double a = v->rot[0], b = v->rot[1], cc = v->rot[2], d = v->rot[3], e = v->rot[4], f = v->rot[5],
                 g = v->rot[6], h = v->rot[7], k = v->rot[8];
    double det = a * (e * k - f * h) - b * (d * k - f * g) + cc * (d * h - e * g);
    double vx = (bx - 160.0) / v->fov_screen_dist, vy = (by - 120.0) / v->fov_screen_dist, vz = 1.0;
    dir[0] = ((e * k - f * h) * vx + (cc * h - b * k) * vy + (b * f - cc * e) * vz) / det;
    dir[1] = ((f * g - d * k) * vx + (a * k - cc * g) * vy + (cc * d - a * f) * vz) / det;
    dir[2] = ((d * h - e * g) * vx + (b * g - a * h) * vy + (a * e - b * d) * vz) / det;
}

/* Westwand-Ostflaeche des Cuts (auswahl.json ROOM1100 "Westwand": Quader x-17600..X_FLAECHE z-23200..-12100,
 * Hoehe -4000). Rueckgabe: Abstand HINTER der Flaeche in Kamera-z (> 0), 0 = Sehstrahl trifft die Flaeche nicht. */
#define ZELLE_X (-15600.0)       /* SCA-Zelle 2 ROOM1100.RDT: x -17600 + Breite 2000 = Ostflaeche der Kollision */
#define WAND_Z0 (-23200.0)
#define WAND_Z1 (-12100.0)
#define WAND_OBEN (-4000.0)
static double hinter_flaeche(const double cam[3], double flaeche_x, const double p[3], double vz_p)
{
    double s, qy, qz;
    if (fabs(p[0] - cam[0]) < 1e-9) return 0;
    s = (flaeche_x - cam[0]) / (p[0] - cam[0]);
    if (s <= 0 || s >= 1) return 0;               /* Flaeche liegt nicht zwischen Kamera und Punkt */
    qz = cam[2] + s * (p[2] - cam[2]);
    qy = cam[1] + s * (p[1] - cam[1]);
    if (qz < WAND_Z0 || qz > WAND_Z1 || qy < WAND_OBEN || qy > 0) return 0;
    return (1.0 - s) * vz_p;                      /* vz waechst linear entlang des Strahls, vz(Kamera) = 0 */
}

/* Rand der Silhouette: ein Nachbarpunkt (8er-Umgebung) ohne Maske. */
static int am_rand(const cut_t *c, int x, int y)
{
    int dx, dy;
    for (dy = -1; dy <= 1; dy++)
        for (dx = -1; dx <= 1; dx++) {
            int xx = x + dx, yy = y + dy;
            if (xx < 0 || xx >= 320 || yy < 0 || yy >= 240 || !c->deck[yy][xx]) return 1;
        }
    return 0;
}

typedef struct { int hinter_n, hinter_frei, vor_n, vor_verdeckt; double groesstes_leck; } gitter_t;

/* Gitter der Weltpunkte gegen die Maske. schwelle = Kamera-z-Abstand hinter der Flaeche, ab dem verdeckt sein MUSS. */
static void gitter(const cut_t *c, double flaeche_x, double schwelle, gitter_t *g, int beispiele)
{
    static const double hoehen[] = { 0.0, -500.0, -1000.0, -1500.0 };
    double cam[3]; int ix, iz, ih, gezeigt = 0;
    memset(g, 0, sizeof *g);
    kamera_ort(&c->view, cam);
    for (ix = -18000; ix <= -12700; ix += 100)
        for (iz = -26400; iz <= -9800; iz += 100)
            for (ih = 0; ih < 4; ih++) {
                double p[3] = { ix, hoehen[ih], iz }, sx, sy, vz, d;
                int px, py, verd;
                /* Auch Punkte IN den Wandzellen zaehlen: genau dort steckt der Kopf von Zombie 1 (Rueckprojektion
                 * z -22750..-23120 in der Zelle Suedblock, Dossier 1.3) — liegende Figuren ragen in die Wand. */
                if (!proj(&c->view, p[0], p[1], p[2], &sx, &sy, &vz)) continue;
                px = (int) floor(sx); py = (int) floor(sy);
                if (px < 1 || px > 318 || py < 1 || py > 238) continue;
                if (!c->deck[py][px] || am_rand(c, px, py)) continue;
                verd = re15_pri_mask_occludes((int) c->tief[py][px], (long) vz);
                d = hinter_flaeche(cam, flaeche_x, p, vz);
                if (d > 0) {
                    if (d < schwelle) continue;            /* innerhalb der Stufenbreite: Rasterung entscheidet */
                    g->hinter_n++;
                    if (!verd) {
                        g->hinter_frei++;
                        if (d > g->groesstes_leck) g->groesstes_leck = d;
                        if (gezeigt < beispiele) {
                            printf("    LECK (%d,%d,%d) Bild (%d,%d) vz %.0f, %.0f hinter der Flaeche, Maskentiefe %u (Schluessel %.0f)\n",
                                   ix, (int) hoehen[ih], iz, px, py, vz, d, c->tief[py][px],
                                   (c->tief[py][px] + 1) * 65536.0 / 1023.0);
                            gezeigt++;
                        }
                    }
                } else if (p[0] >= ZELLE_X) {
                    /* VOR der Wand, im Flur: oestlich der Kollisionsflaeche der Zelle (x >= -15600), also dort, wo
                     * eine Figur stehen kann. Zwischen Zellen- und Maskenflaeche (-15600..-15690/-15710) liegt das
                     * Zellenpolster; ein Punkt 10 Einheiten vor der Maskenflaeche hat dieselbe Kamera-z-Stufe wie die
                     * Wand (gemessen: (-15700,-500,-16800) Cut 2 vz 5830 = Schluessel 5830) — Gleichstand, kein Befund. */
                    g->vor_n++;
                    if (verd) {
                        g->vor_verdeckt++;
                        if (gezeigt < beispiele) {
                            printf("    VOR VERDECKT (%d,%d,%d) Bild (%d,%d) vz %.0f, Maskentiefe %u (Schluessel %.0f)\n",
                                   ix, (int) hoehen[ih], iz, px, py, vz, c->tief[py][px],
                                   (c->tief[py][px] + 1) * 65536.0 / 1023.0);
                            gezeigt++;
                        }
                    }
                }
            }
}

static void teil_leck_cut(int cut, double flaeche_x, int stufe, const char *mskdir, int erwartung_gruen)
{
    cut_t c; gitter_t g; char t[256];
    /* Schwelle = (Stufe + 1) Buckets: die Stufe selbst (Tiefenspanne eines Rechtecks) plus die Bucket-Rundung. */
    double schwelle = (stufe + 1) * 65536.0 / 1023.0;
    if (!cut_laden(&c, g_raum, cut, mskdir)) { g_fail = 1; return; }
    gitter(&c, flaeche_x, schwelle, &g, 4);
    printf("  ROOM%04X (Masken ROOM%04X) Cut %d (%s): %d Rechtecke; Punkte hinter der Flaeche (> %.0f) %d, davon FREI %d (groesstes Leck %.0f); "
           "vor der Flaeche %d, davon verdeckt %d\n", g_raum, re15_pri_masken_raum(g_raum), cut, mskdir, c.pri.draw_count, schwelle, g.hinter_n,
           g.hinter_frei, g.groesstes_leck, g.vor_n, g.vor_verdeckt);
    if (erwartung_gruen) {
        snprintf(t, sizeof t, "Cut %d: jeder Punkt > %.0f hinter der Westwand ist verdeckt (%d Punkte)", cut, schwelle, g.hinter_n);
        CHECK(t, g.hinter_n > 1000 && g.hinter_frei == 0);
        snprintf(t, sizeof t, "Cut %d: kein Punkt VOR der Westwand verdeckt (%d Punkte)", cut, g.vor_n);
        CHECK(t, g.vor_n > 1000 && g.vor_verdeckt == 0);
    }
    cut_frei(&c);
}

static const char *ref_dir(void)
{
    static char ref[700];
    snprintf(ref, sizeof ref, "%s/shared_assets/PSX/MASKS", RE15_PORT_SRC_DIR);
    return ref;
}

static void teil_leck(void)
{
    printf("== r37_pri1100 leck: Weltpunkte hinter / vor der Westwand, ROOM%04X gegen ROOM%04X.MSK ==\n",
           g_raum, re15_pri_masken_raum(g_raum));
    teil_leck_cut(1, -15690.0, 4, ref_dir(), 1);
    teil_leck_cut(2, -15710.0, 3, ref_dir(), 1);
}

/* Die Kopf-/Armpunkte der Marken: Dreiecksschwerpunkt (Bild) + Dreiecks-z aus RE15_TRILOG (Messlaeufe m2_trilog =
 * Spawnlage (-16453,-23817) wie Marke F240, m4_f892 = Lage der Marke F892 (-16157,-23876) per RE15_GEGNER_POS; Dossier
 * 1.2/4.2). Jeder liegt HINTER der Wandflaeche (Wandtiefe am Punkt 7772..9890) und wurde mit den v0.8.24-Masken
 * gezeichnet (Schluessel 10378 > z). Diese Punkte MUESSEN verdeckt sein. */
static void teil_marke(const char *mskdir, int erwartung_gruen)
{
    /* x, y (Bild), z (Kamera-z des Dreiecks) */
    static const int punkte[][3] = {
        { 182, 144,  9991 }, { 183, 141, 10101 }, { 183, 142, 10220 }, { 180, 145, 10375 },   /* Kopf, Spawnlage (F240) */
        { 173, 143, 10353 }, { 174, 144, 10368 }, { 174, 143, 10375 },                        /* Arm,  Spawnlage */
        { 176, 145,  9980 }, { 177, 141, 10090 }, { 177, 142, 10209 }, { 174, 141, 10376 },   /* Kopf, Lage F892 */
        { 168, 144, 10357 }, { 168, 143, 10364 }, { 175, 143, 10367 },                        /* Arm,  Lage F892 */
    };
    cut_t c; int i, verd = 0, n = 0;
    printf("== r37_pri1100 marke: Kopf und Arm von Zombie 1 (Marken F240/F892), ROOM%04X, Masken %s/ROOM%04X ==\n",
           g_raum, mskdir, re15_pri_masken_raum(g_raum));
    if (!cut_laden(&c, g_raum, 1, mskdir)) { g_fail = 1; return; }
    for (i = 0; i < (int) (sizeof punkte / sizeof punkte[0]); i++) {
        int x = punkte[i][0], y = punkte[i][1], z = punkte[i][2];
        int d = c.deck[y][x] ? c.tief[y][x] : -1;
        int v = c.deck[y][x] && re15_pri_mask_occludes(d, z);
        printf("  (%d,%d) z %d: Maskentiefe %d (Schluessel %.0f) -> %s\n", x, y, z, d,
               d >= 0 ? (d + 1) * 65536.0 / 1023.0 : -1.0, v ? "verdeckt" : "FREI");
        n++; verd += v;
    }
    printf("  %d von %d Punkten verdeckt\n", verd, n);
    if (erwartung_gruen)
        CHECK("alle Kopf-/Armpunkte der Marken liegen unter der Maske", n > 0 && verd == n);
    cut_frei(&c);
}

/* MESSUNG (kein Riegel): die Maskenpunkte im Schattenquad des Spielers wie test_r36_pri.c schatten_messen (Ecken
 * +-500/+-600, re15_camera_yaw_matrix_angle + compose_view_bone, Pixelmitte) — mit Maskentiefe, Schatten-Bucket und
 * dem Bodenpunkt der Pixelmitte (vor/hinter der Maskenflaeche). */
static void teil_schatten_messung(int cut, int px, int pz, int16_t rot, double flaeche_x, const char *mskdir)
{
    static const int32_t ecke[4][3] = { {-500, 0, 600}, {-500, 0, -600}, {500, 0, 600}, {500, 0, -600} };
    static const int u[4] = { 0, 2, 3, 1 };
    cut_t c; int32_t yaw[9], rotm[9], trans[3], welt[3]; double sx[4], sy[4], cam[3]; int32_t summe = 0; int v, x, y, bucket;
    if (!cut_laden(&c, 0x1100, cut, mskdir)) { g_fail = 1; return; }
    welt[0] = px; welt[1] = 0; welt[2] = pz;
    re15_camera_yaw_matrix_angle(rot, yaw);
    re15_camera_compose_view_bone(&c.view, yaw, welt, rotm, trans);
    for (v = 0; v < 4; v++) {
        int32_t ex = ecke[v][0], ey = ecke[v][1], ez = ecke[v][2];
        int32_t vx = (int32_t) (((int64_t) ex * rotm[0] + (int64_t) ey * rotm[1] + (int64_t) ez * rotm[2]) >> 12) + trans[0];
        int32_t vy = (int32_t) (((int64_t) ex * rotm[3] + (int64_t) ey * rotm[4] + (int64_t) ez * rotm[5]) >> 12) + trans[1];
        int32_t vz = (int32_t) (((int64_t) ex * rotm[6] + (int64_t) ey * rotm[7] + (int64_t) ez * rotm[8]) >> 12) + trans[2];
        summe += (vz > 0xFFFF) ? 0xFFFF : vz;
        sx[v] = 160.0 + (double) vx * c.view.fov_screen_dist / vz;
        sy[v] = 120.0 + (double) vy * c.view.fov_screen_dist / vz;
    }
    bucket = (int) (summe >> 8);
    kamera_ort(&c.view, cam);
    for (v = 0; v < 4; v++) {   /* Gegenprobe der Rueckrechnung: Ecke im Bild -> Bodenpunkt */
        double d[3], s;
        strahl(&c.view, sx[v], sy[v], d);
        s = (0.0 - cam[1]) / d[1];
        printf("  Ecke %d (%.1f,%.1f) -> Bodenpunkt (%.0f,%.0f)\n", v, sx[v], sy[v], cam[0] + s * d[0], cam[2] + s * d[2]);
    }
    printf("== Schattenquad Cut %d Spieler (%d,%d) rot %d: Ecken (%.1f,%.1f) (%.1f,%.1f) (%.1f,%.1f) (%.1f,%.1f), Bucket %d ==\n",
           cut, px, pz, rot, sx[0], sy[0], sx[1], sy[1], sx[2], sy[2], sx[3], sy[3], bucket);
    for (y = 0; y < 240; y++)
        for (x = 0; x < 320; x++) {
            int i, inside = 0; double X = x + 0.5, Y = y + 0.5;
            if (!c.deck[y][x]) continue;
            for (i = 0; i < 4; i++) {
                double xa = sx[u[i]], ya = sy[u[i]], xb = sx[u[(i + 1) & 3]], yb = sy[u[(i + 1) & 3]];
                if ((ya > Y) != (yb > Y) && X < xa + (Y - ya) * (xb - xa) / (yb - ya)) inside = !inside;
            }
            if (!inside) continue;
            {   /* Bodenpunkt der Pixelmitte: Sehstrahl Kamera -> Bildpunkt gegen y = 0 */
                double d[3], s, bx;
                strahl(&c.view, X, Y, d);
                s = (0.0 - cam[1]) / d[1]; bx = cam[0] + s * d[0];
                printf("  (%d,%d) Maskentiefe %u %s Bucket %d | Bodenpunkt der Pixelmitte x=%.0f (%s Maskenflaeche %.0f)\n",
                       x, y, c.tief[y][x], c.tief[y][x] < bucket ? "UEBER dem Schatten," : "unter dem Schatten,", bucket,
                       bx, bx > flaeche_x ? "VOR der" : "HINTER der", flaeche_x);
            }
        }
    cut_frei(&c);
}

/* ---- Nachbesserung 1: Zensus Masken-Zwilling (VERTRAG.md 1.7) ------------------------------------------------- */
static int datei_da(const char *pfad)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}

static uint32_t le32(const uint8_t *p) { return (uint32_t) p[0] | ((uint32_t) p[1] << 8) | ((uint32_t) p[2] << 16) | ((uint32_t) p[3] << 24); }

/* Hintergrund-Block (Raum, Cut) wie bg_pc.c re15_bg_load_room_cut: BSS/ROOM%04X/BG%02d.BSS, sonst die Scheibe
 * Cut*0x10000 der Stage-Datei STAGE%u/ROOM%03X.BSS (Original @0x80021cd8 sll a3,a3,5 = Cut*32 Sektoren; der Name
 * traegt den Raum OHNE Varianten-Ziffer @0x80021c90 lh a1,0x0fe2). *sb wird erst bei Bedarf gelesen. */
static uint8_t *bg_block(unsigned raum, int cut, uint8_t **sb, size_t *nsb, int *sb_gelesen, size_t *n)
{
    char p[700]; uint8_t *b;
    snprintf(p, sizeof p, "%s/shared_assets/PSX/BSS/ROOM%04X/BG%02d.BSS", RE15_PORT_SRC_DIR, raum, cut);
    b = slurp(p, n);
    if (b) return b;
    if (!*sb_gelesen) {
        *sb_gelesen = 1;
        snprintf(p, sizeof p, "%s/shared_assets/PSX/STAGE%u/ROOM%03X.BSS", RE15_PORT_SRC_DIR, raum >> 12, raum >> 4);
        *sb = slurp(p, nsb);
    }
    if (*sb && (size_t) (cut + 1) * 0x10000u <= *nsb) {
        b = (uint8_t *) malloc(0x10000u);
        if (b) { memcpy(b, *sb + (size_t) cut * 0x10000u, 0x10000u); *n = 0x10000u; return b; }
    }
    *n = 0;
    return NULL;
}

static void teil_zwilling(void)
{
    int stage, raum, n_paare = 0, n_rueck = 0, fehler = 0;
    int kam_1030 = -1, kam_11c0 = -1;   /* Gegenprobe: diese Paare unterscheiden sich (Cuts 13/14, Satz 12 flag) */
    printf("== r37_pri1100 zwilling: Zensus Elza -> Leon (Kamerasaetze + Hintergrund je Cut) gegen re15_pri_masken_raum ==\n");
    for (stage = 1; stage <= 6; stage++)
        for (raum = 0; raum < 0x40; raum++) {
            unsigned ge = ((unsigned) stage << 12) | ((unsigned) raum << 4), un = ge | 1u, soll, ist;
            char p[700]; uint8_t *rg, *ru, *sb = NULL; size_t ng = 0, nu = 0, nsb = 0;
            int msk_g, msk_u, kam = 1, bg = 1, c, sb_gelesen = 0;
            snprintf(p, sizeof p, "%s/shared_assets/PSX/STAGE%d/ROOM%04X.RDT", RE15_PORT_SRC_DIR, stage, ge);
            rg = slurp(p, &ng);
            snprintf(p, sizeof p, "%s/shared_assets/PSX/STAGE%d/ROOM%04X.RDT", RE15_PORT_SRC_DIR, stage, un);
            ru = slurp(p, &nu);
            if (!rg || !ru || ng < 0x60 || nu < 0x60) { free(rg); free(ru); continue; }   /* kein Paar / 4-Byte-Platzhalter */
            n_paare++;
            snprintf(p, sizeof p, "%s/shared_assets/PSX/MASKS/ROOM%04X.MSK", RE15_PORT_SRC_DIR, ge);
            msk_g = datei_da(p);
            snprintf(p, sizeof p, "%s/shared_assets/PSX/MASKS/ROOM%04X.MSK", RE15_PORT_SRC_DIR, un);
            msk_u = datei_da(p);
            if (rg[1] != ru[1]) kam = 0;              /* RDT[1] = Zahl der Kamerasaetze (Cuts) */
            else {
                uint32_t og = le32(rg + 0x24), ou = le32(ru + 0x24);
                for (c = 0; c < rg[1] && kam; c++) {
                    if (og + (uint32_t) c * 0x20u + 0x1Cu > ng || ou + (uint32_t) c * 0x20u + 0x1Cu > nu) kam = 0;
                    else if (memcmp(rg + og + c * 0x20, ru + ou + c * 0x20, 0x1C)) kam = 0;
                }
            }
            if (ge == 0x1030) kam_1030 = kam;
            if (ge == 0x11C0) kam_11c0 = kam;
            /* Hintergrund nur fuer Kandidaten (fuer alle anderen entscheidet er nichts). */
            if (msk_g && !msk_u && kam)
                for (c = 0; c < rg[1] && bg; c++) {
                    size_t a = 0, b = 0;
                    uint8_t *ba = bg_block(ge, c, &sb, &nsb, &sb_gelesen, &a);
                    uint8_t *bb = bg_block(un, c, &sb, &nsb, &sb_gelesen, &b);
                    if (a != b || (a && memcmp(ba, bb, a))) {
                        bg = 0;
                        printf("  ROOM%04X/%04X Cut %d: Hintergrund verschieden\n", ge, un, c);
                    }
                    free(ba); free(bb);
                }
            free(sb);
            soll = (msk_g && !msk_u && kam && bg) ? ge : un;
            ist = re15_pri_masken_raum(un);
            if (soll == ge) {
                n_rueck++;
                printf("  ROOM%04X -> ROOM%04X (%d Cuts: Kamerasaetze + Hintergrund gleich, keine eigene MSK)\n", un, ge, rg[1]);
            }
            if (ist != soll) {
                fehler++;
                printf("  ABWEICHUNG ROOM%04X: Zensus -> ROOM%04X, Tabelle -> ROOM%04X (Leon-MSK %d, Elza-MSK %d, Kamera %d, "
                       "Hintergrund %d)\n", un, soll, ist, msk_g, msk_u, kam, bg);
            }
            if (re15_pri_masken_raum(ge) != ge) {
                fehler++;
                printf("  ABWEICHUNG: die gerade Variante ROOM%04X wird auf ROOM%04X umgelenkt\n", ge, re15_pri_masken_raum(ge));
            }
            free(rg); free(ru);
        }
    printf("  %d Paare, %d Rueckfaelle, %d Abweichungen\n", n_paare, n_rueck, fehler);
    /* Gegenproben: der Vergleich ist nicht leer. ROOM1030/1031 (13/14 Cuts) und ROOM11C0/11C1 (14/13, Satz 12 flag 0/1)
     * haben verschiedene Kamerasaetze; zwei Cuts desselben Raums haben verschiedene Hintergruende. */
    CHECK("Gegenprobe Kamera: ROOM1030/1031 und ROOM11C0/11C1 werden als verschieden erkannt", kam_1030 == 0 && kam_11c0 == 0);
    {
        uint8_t *sb = NULL; size_t nsb = 0, a = 0, b = 0; int gel = 0;
        uint8_t *b0 = bg_block(0x1100, 0, &sb, &nsb, &gel, &a), *b1 = bg_block(0x1100, 1, &sb, &nsb, &gel, &b);
        CHECK("Gegenprobe Hintergrund: ROOM1100 Cut 0 und Cut 1 werden als verschieden erkannt",
              b0 && b1 && (a != b || memcmp(b0, b1, a) != 0));
        free(b0); free(b1); free(sb);
    }
    CHECK("Zensus aus den Dateien == Tabelle re15_pri_masken_raum (alle Paare STAGE1..6)", n_paare >= 100 && fehler == 0);
    CHECK("ROOM1101 (Elza, Pflichtweg 10D1 -> 1101) nutzt die Masken von ROOM1100", re15_pri_masken_raum(0x1101) == 0x1100);
    CHECK("eigene Elza-Masken gewinnen (ROOM1221, ROOM2091 bleiben bei sich)",
          re15_pri_masken_raum(0x1221) == 0x1221 && re15_pri_masken_raum(0x2091) == 0x2091);
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    if (!strcmp(teil, "leck")) teil_leck();
    else if (!strcmp(teil, "marke")) teil_marke(ref_dir(), 1);
    else if (!strcmp(teil, "zwilling")) teil_zwilling();
    else if (!strcmp(teil, "elza")) {      /* Nachbesserung 1: Elzas ROOM1101 (Kamera ROOM1101.RDT, Masken per Zwilling) */
        CHECK("ROOM1101 hat keine eigenen Masken, der Lader nimmt ROOM1100", re15_pri_masken_raum(0x1101) == 0x1100);
        g_raum = 0x1101;
        teil_leck();
        teil_marke(ref_dir(), 1);
    }
    else if (!strcmp(teil, "schatten") && argc > 2) {      /* Messung: schatten <maskenverzeichnis> */
        teil_schatten_messung(2, -15100, -18500, -1024, -15710.0, argv[2]);
        teil_schatten_messung(1, -15100, -16900, -1024, -15690.0, argv[2]);
    }
    else if (!strcmp(teil, "vergleich") && argc > 2) {
        /* Messung (kein Riegel): dieselbe Zaehlung gegen ein anderes Maskenverzeichnis (z.B. die v0.8.24-Masken). */
        printf("== r37_pri1100 vergleich: %s ==\n", argv[2]);
        teil_leck_cut(1, -15690.0, 4, argv[2], 0);
        teil_leck_cut(2, -15710.0, 3, argv[2], 0);
        teil_marke(argv[2], 0);
    } else { teil_leck(); teil_marke(ref_dir(), 1); }
    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
