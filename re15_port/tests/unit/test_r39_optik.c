/*
 * Runde 39 Spur L (optik) — Riegel Punkt 1: Westwand-Masken ROOM1100/1101 enden an der GEMALTEN Ecke.
 * Dossier analysis/befunde_runde39/L_optik.md, Code engine/src/optik_r39.c (Kopf: include/re15_optik_r39.h).
 *
 * Nutzer (Marke befund_1100_F741_marke1.bmp): "Leon ist immer noch vom Boden ueberdeckt an den einen Streifen."
 *
 *   test_r39_optik kappen  Mechanik des Kappens an einer kuenstlichen Sektion: Teilung an der Zeilengrenze, Atlas-
 *                          Versatz (srcX/srcY) mit, Rechtecke ganz jenseits entfallen, draw_count folgt.
 *   test_r39_optik ecke    Cut 1 und Cut 2 ueber die ECHTEN Dateien: die Fenster in optik_r39.c stimmen mit der am
 *                          Hintergrund (Engine-Dekoder, BSS/ROOM1100/BG0n.BSS) NEU gemessenen Eckkante ueberein;
 *                          gekappte Deckung == rohe Deckung geschnitten mit der gemalten Wand (keine Wandpunkte verloren,
 *                          nichts dazu); Gegenprobe: die rohen Dateien decken jenseits der Ecke (Cut 1 474, Cut 2 372).
 *   test_r39_optik leon    Leon an der Lage der Marke F741 (-16007,0,-23658), Koerperpunkte ueber die Kamera von Cut 1
 *                          gegen die Masken (Original-Urteil re15_pri_mask_occludes, @0x8002565c / ZSF3 @0x80066c70 /
 *                          Masken-OT @0x80039650-60): jenseits der gemalten Ecke deckt NICHTS mehr (roh: >0), hinter
 *                          der Wand bleibt er verdeckt.
 *   test_r39_optik elza    ROOM1101 zeichnet die Masken von ROOM1100 (re15_pri_masken_raum) -> dieselben Fenster.
 *
 * Punkt 2 (Nutzer): "Der Schluessel im Raum 10F0 und 10F1 sollte schon kleiner/subtiler sein, wie er dran haengt, nicht
 * so riesig."
 *   test_r39_optik schluessel  FORM der Groesse (Beta -> Retail): Bildlaenge der gezeichneten Huelle = Median der
 *                          RE2-Bildlaenge L_nat*H/vz desselben Modells an seinen RE2-Platzierungen (aus den RE2-Dateien
 *                          mit der Engine-Kamera nachgerechnet == gen/itemorte_r38_lage.inc); Massstab kleiner als
 *                          Runde 38, Huelle 3..9 px; MD1-Spanne/s = RE2-Modell 152; Lage/Drehung/Zone unveraendert.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_pri.h"
#include "re15_tim.h"
#include "re15_bss.h"
#include "re15_pri1100_r37.h"
#include "re15_optik_r39.h"
#include "re15_md1.h"
#include "re15_itemorte_r38.h"
#ifndef RE15_RE2_DIR
#error RE15_RE2_DIR fehlt (info/re2leon)
#endif

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

/* ---------------------------------------------------------------------------------------------------------------- */
/* kappen: kuenstliche Sektion                                                                                       */
static void teil_kappen(void)
{
    re15_pri_cut_t p; char t[200];
    printf("== r39_optik kappen: Mechanik an einer kuenstlichen Sektion (Fenster ROOM1100 Cut 1) ==\n");
    memset(&p, 0, sizeof p);
    /* 0: ueber beide Baender, links jenseits (167..171, Zeilen 63..152) -> oben ganz weg, unten nur Spalte 171 */
    p.masks[0].srcX = 142; p.masks[0].srcY = 166; p.masks[0].dstX = 167; p.masks[0].dstY = 63;
    p.masks[0].width = 5;  p.masks[0].height = 90; p.masks[0].depth = 148;
    /* 1: ganz in der Wand (180..186) -> unveraendert */
    p.masks[1].srcX = 10;  p.masks[1].srcY = 20;  p.masks[1].dstX = 180; p.masks[1].dstY = 58;
    p.masks[1].width = 7;  p.masks[1].height = 112; p.masks[1].depth = 120;
    /* 2: ganz jenseits (166..167, Zeilen 120..147) -> entfaellt */
    p.masks[2].srcX = 164; p.masks[2].srcY = 151; p.masks[2].dstX = 166; p.masks[2].dstY = 120;
    p.masks[2].width = 2;  p.masks[2].height = 28; p.masks[2].depth = 160;
    /* 3: ueber beide Baender, Spalten 170..175 -> oben 173..175, unten 171..175 */
    p.masks[3].srcX = 150; p.masks[3].srcY = 158; p.masks[3].dstX = 170; p.masks[3].dstY = 62;
    p.masks[3].width = 6;  p.masks[3].height = 95; p.masks[3].depth = 140;
    p.mask_count = 4; p.draw_count = 4;
    {
        int n = re15_optik_r39_kappen(0x1100, 1, &p);
        snprintf(t, sizeof t, "5 Teilrechtecke (0 unten, 1 ganz, 3 oben+unten; 2 entfaellt), draw_count folgt: n=%d draw=%d",
                 n, p.draw_count);
        CHECK(t, n == 4 && p.mask_count == 4 && p.draw_count == 4);
        /* Reihenfolge: 0-unten, 1, 3-oben, 3-unten */
        CHECK("Teil 0 unten: dst (171,111) 1x42, src (142+4, 166+48), Tiefe 148",
              p.masks[0].dstX == 171 && p.masks[0].dstY == 111 && p.masks[0].width == 1 && p.masks[0].height == 42 &&
              p.masks[0].srcX == 146 && p.masks[0].srcY == 214 && p.masks[0].depth == 148);
        CHECK("Rechteck in der Wand unveraendert (180,58) 7x112 src (10,20)",
              p.masks[1].dstX == 180 && p.masks[1].dstY == 58 && p.masks[1].width == 7 && p.masks[1].height == 112 &&
              p.masks[1].srcX == 10 && p.masks[1].srcY == 20 && p.masks[1].depth == 120);
        CHECK("Teil 3 oben: dst (173,62) 3x49, src (153,158)",
              p.masks[2].dstX == 173 && p.masks[2].dstY == 62 && p.masks[2].width == 3 && p.masks[2].height == 49 &&
              p.masks[2].srcX == 153 && p.masks[2].srcY == 158 && p.masks[2].depth == 140);
        CHECK("Teil 3 unten: dst (171,111) 5x46, src (151,207)",
              p.masks[3].dstX == 171 && p.masks[3].dstY == 111 && p.masks[3].width == 5 && p.masks[3].height == 46 &&
              p.masks[3].srcX == 151 && p.masks[3].srcY == 207 && p.masks[3].depth == 140);
    }
    /* Andere Raeume / Cuts: unveraendert */
    memset(&p, 0, sizeof p);
    p.masks[0].dstX = 100; p.masks[0].dstY = 100; p.masks[0].width = 50; p.masks[0].height = 50; p.mask_count = 1; p.draw_count = 1;
    CHECK("ROOM1100 Cut 0 / ROOM1110 Cut 1: keine Fenster -> unveraendert",
          re15_optik_r39_kappen(0x1100, 0, &p) == 1 && re15_optik_r39_kappen(0x1110, 1, &p) == 1 &&
          p.masks[0].dstX == 100 && p.masks[0].width == 50 && re15_optik_r39_in_wand(0x1110, 1, 0, 0) == -1);
    /* draw_count < mask_count: Teile gebauter, aber nicht gezeichneter Rechtecke zaehlen nicht (re15_pri.h BUILD != DRAW) */
    memset(&p, 0, sizeof p);
    p.masks[0].dstX = 170; p.masks[0].dstY = 62; p.masks[0].width = 6; p.masks[0].height = 95;
    p.masks[1] = p.masks[0];
    p.mask_count = 2; p.draw_count = 1;
    CHECK("BUILD != DRAW: 2 gebaut / 1 gezeichnet -> 4 Teile / 2 gezeichnet",
          re15_optik_r39_kappen(0x1100, 1, &p) == 4 && p.draw_count == 2);
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Hintergrund dekodieren (wie probe_bg_dump / bg_pc.c: BSS/ROOM%04X/BG%02d.BSS == STAGE1/ROOM110.BSS Scheibe cut) */
static int bg_laden(unsigned raum, int cut, uint8_t rgb[240][320][3])
{
    char pfad[700]; size_t n = 0; uint8_t *b; re15_bss_chunk_t ch; int16_t *co; size_t cap; int k, rv;
    snprintf(pfad, sizeof pfad, "%s/BSS/ROOM%04X/BG%02d.BSS", RE15_ASSET_PSX_DIR, raum, cut);
    b = slurp(pfad, &n);
    if (!b) { printf("  FEHLER: %s fehlt\n", pfad); return 0; }
    if (!re15_bss_parse_chunk(b, n, &ch) || !re15_bss_chunk_has_video(&ch)) { free(b); printf("  FEHLER: BSS %s\n", pfad); return 0; }
    cap = ((size_t) ch.run_length_words + 2) * 8 + 4096;
    co = (int16_t *) malloc(cap * sizeof(int16_t));
    k = co ? re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant, ch.version, co, cap) : -1;
    rv = (k >= 0) ? re15_bss_mdec_decode(co, (size_t) k, RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT, &rgb[0][0][0]) : -1;
    free(co); free(b);
    if (rv != 0) { printf("  FEHLER: MDEC %s\n", pfad); return 0; }
    return 1;
}

static double hell(uint8_t rgb[240][320][3], int x, int y)
{
    return 0.299 * rgb[y][x][0] + 0.587 * rgb[y][x][1] + 0.114 * rgb[y][x][2];
}

/* Halbwert-Kante zwischen Mittel(links) und Mittel(rechts) in Zeile y, gesucht zwischen Spalte xa und xb.
 * Pixelmitte bei x+0,5 (wie im Dossier). -1 = keine Kreuzung. */
static double kante(uint8_t rgb[240][320][3], int y, int la, int lb, int ra, int rb, int xa, int xb)
{
    double l = 0, r = 0, mid; int x;
    for (x = la; x <= lb; x++) l += hell(rgb, x, y);
    for (x = ra; x <= rb; x++) r += hell(rgb, x, y);
    l /= (lb - la + 1); r /= (rb - ra + 1); mid = (l + r) / 2;
    for (x = xa; x < xb; x++) {
        double a = hell(rgb, x, y), b = hell(rgb, x + 1, y);
        if ((a - mid) * (b - mid) <= 0 && a != b) return x + 0.5 + (mid - a) / (b - a);
    }
    return -1;
}

/* Masken eines Cuts: Deckung je Bildpunkt (+ naechste Tiefe), roh und gekappt. */
typedef struct {
    uint8_t *roh, *msk, *tb;
    re15_rdt_t rdt; re15_camera_view_t view; re15_pri_cut_t pri, kap; re15_tim_t tim;
    uint8_t  deck_roh[240][320], deck_neu[240][320];
    uint16_t tief_roh[240][320], tief_neu[240][320];
} cut_t;

static void decken(const re15_pri_cut_t *p, const re15_tim_t *tim, uint8_t deck[240][320], uint16_t tief[240][320])
{
    const uint8_t *idx = (const uint8_t *) tim->pixels; int i, x, y;
    memset(deck, 0, 240 * 320); memset(tief, 0, 240 * 320 * sizeof(uint16_t));
    for (i = 0; i < p->draw_count; i++) {
        const re15_pri_mask_t *m = &p->masks[i];
        int dx = (int16_t) m->dstX, dy = (int16_t) m->dstY;
        for (y = 0; y < m->height; y++) {
            int sy = m->srcY + y, ty = dy + y;
            if (sy < 0 || sy >= tim->height || ty < 0 || ty >= 240) continue;
            for (x = 0; x < m->width; x++) {
                int sx = m->srcX + x, tx = dx + x;
                if (sx < 0 || sx >= tim->width || tx < 0 || tx >= 320) continue;
                if (!idx[sy * tim->width + sx]) continue;
                deck[ty][tx] = 1;
                if (!tief[ty][tx] || m->depth < tief[ty][tx]) tief[ty][tx] = m->depth;
            }
        }
    }
}

static int cut_laden(cut_t *c, unsigned raum, int cut)
{
    char pfad[700]; size_t sz = 0, msz = 0, tsz = 0; uint32_t off; int n;
    const unsigned mraum = re15_pri_masken_raum(raum);
    memset(c, 0, sizeof *c);
    snprintf(pfad, sizeof pfad, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, raum >> 12, raum);
    c->roh = slurp(pfad, &sz);
    if (!c->roh || re15_rdt_parse(c->roh, sz, &c->rdt) < 0 || cut >= c->rdt.cut_count
        || re15_camera_build_view(&c->rdt.cuts[cut], &c->view) != 0) {
        printf("  FEHLER: ROOM%04X Cut %d: RDT/Kamera (%s)\n", raum, cut, pfad); return 0;
    }
    snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM%04X.MSK", RE15_ASSET_PSX_DIR, mraum);
    c->msk = slurp(pfad, &msz);
    off = c->msk ? re15_pri_msk_section_offset(c->msk, msz, cut) : 0;
    n = off ? re15_pri_parse_section(c->msk, msz, off, &c->pri) : 0;
    snprintf(pfad, sizeof pfad, "%s/MASKS/ROOM%04X_PRI%02d.TIM", RE15_ASSET_PSX_DIR, mraum, cut);
    c->tb = slurp(pfad, &tsz);
    if (n <= 0 || !c->tb || re15_tim_parse(c->tb, (int) tsz, &c->tim) != 0 || c->tim.bpp != 8 || !c->tim.has_clut) {
        printf("  FEHLER: ROOM%04X Cut %d: Sektion (%d) oder Atlas fehlt\n", raum, cut, n); return 0;
    }
    c->kap = c->pri;
    re15_optik_r39_kappen(mraum, cut, &c->kap);     /* derselbe Aufruf wie main.c (PC) / psx/main.c */
    decken(&c->pri, &c->tim, c->deck_roh, c->tief_roh);
    decken(&c->kap, &c->tim, c->deck_neu, c->tief_neu);
    return 1;
}

static void cut_frei(cut_t *c) { free(c->roh); free(c->msk); free(c->tb); }

static uint8_t g_bg[240][320][3];

/* Eckkante neu messen (Verfahren des Dossiers) und gegen die Fenster pruefen. */
static void ecke_messen(int cut)
{
    char t[300]; int y, nz = 0; double s = 0, mn = 1e9, mx = -1e9, e;
    if (!bg_laden(0x1100, cut, g_bg)) { g_fail = 1; return; }
    if (cut == 1) {
        /* Eckleiste: Rueckwand 166..170 gegen Leiste 173 */
        for (y = 40; y <= 110; y++) {
            double l = 0; int x;
            for (x = 166; x <= 170; x++) l += hell(g_bg, x, y);
            l /= 5;
            if (hell(g_bg, 173, y) - l < 30) continue;
            e = kante(g_bg, y, 166, 170, 173, 173, 170, 175);
            if (e < 0) continue;
            s += e; nz++; if (e < mn) mn = e; if (e > mx) mx = e;
        }
        e = nz ? s / nz : -1;
        snprintf(t, sizeof t, "Cut 1 Eckleiste: Halbwert-Kante %.2f (%.2f..%.2f, %d Zeilen) -> erste Wandspalte %d == Fenster (Zeile 80: 172 aussen, 173 innen)",
                 e, mn, mx, nz, (int) floor(e - 0.5) + 1);
        CHECK(t, nz >= 60 && (int) floor(e - 0.5) + 1 == 173 &&
                 re15_optik_r39_in_wand(0x1100, 1, 172, 80) == 0 && re15_optik_r39_in_wand(0x1100, 1, 173, 80) == 1);
        {   /* Sockel: dunkelste Spalte 169..172 in den Zeilen 112..128 */
            int x, best = -1; double bl = 1e9;
            for (x = 169; x <= 172; x++) {
                double l = 0;
                for (y = 112; y <= 128; y++) l += hell(g_bg, x, y);
                l /= 17;
                if (l < bl) { bl = l; best = x; }
            }
            snprintf(t, sizeof t, "Cut 1 Sockel: dunkle Kantenlinie Spalte %d (L %.1f) == erste Wandspalte (Zeile 120: 170 aussen, 171 innen)", best, bl);
            CHECK(t, best == 171 && re15_optik_r39_in_wand(0x1100, 1, 170, 120) == 0 && re15_optik_r39_in_wand(0x1100, 1, 171, 120) == 1);
        }
    } else {
        for (y = 97; y <= 105; y++) {
            e = kante(g_bg, y, 128, 131, 136, 139, 131, 136);
            if (e < 0) continue;
            s += e; nz++; if (e < mn) mn = e; if (e > mx) mx = e;
        }
        e = nz ? s / nz : -1;
        snprintf(t, sizeof t, "Cut 2 Wand: Halbwert-Kante %.2f (%.2f..%.2f, %d Zeilen) -> letzte Wandspalte %d == Fenster (Zeile 100: 133 innen, 134 aussen)",
                 e, mn, mx, nz, (int) floor(e - 0.5));
        CHECK(t, nz >= 8 && (int) floor(e - 0.5) == 133 &&
                 re15_optik_r39_in_wand(0x1100, 2, 133, 100) == 1 && re15_optik_r39_in_wand(0x1100, 2, 134, 100) == 0);
        s = 0; nz = 0; mn = 1e9; mx = -1e9;
        for (y = 108; y <= 122; y++) {
            e = kante(g_bg, y, 129, 132, 137, 140, 132, 137);
            if (e < 0) continue;
            s += e; nz++; if (e < mn) mn = e; if (e > mx) mx = e;
        }
        e = nz ? s / nz : -1;
        snprintf(t, sizeof t, "Cut 2 Sockel: Halbwert-Kante %.2f (%.2f..%.2f, %d Zeilen) -> letzte Wandspalte %d == Fenster (Zeile 115: 134 innen, 135 aussen)",
                 e, mn, mx, nz, (int) floor(e - 0.5));
        CHECK(t, nz >= 12 && (int) floor(e - 0.5) == 134 &&
                 re15_optik_r39_in_wand(0x1100, 2, 134, 115) == 1 && re15_optik_r39_in_wand(0x1100, 2, 135, 115) == 0);
    }
}

static void teil_ecke(void)
{
    static cut_t c; int cut;
    printf("== r39_optik ecke: Fenster gegen die gemessene Eckkante, gekappte Deckung gegen die rohe ==\n");
    for (cut = 1; cut <= 2; cut++) {
        int x, y; long jenseits_roh = 0, jenseits_neu = 0, wand_roh = 0, verloren = 0, dazu = 0, tief_anders = 0;
        char t[300];
        ecke_messen(cut);
        if (!cut_laden(&c, 0x1100, cut)) { g_fail = 1; continue; }
        for (y = 0; y < 240; y++)
            for (x = 0; x < 320; x++) {
                int w = re15_optik_r39_in_wand(0x1100, cut, x, y);
                if (c.deck_roh[y][x] && w == 0) jenseits_roh++;
                if (c.deck_neu[y][x] && w == 0) jenseits_neu++;
                if (c.deck_roh[y][x] && w == 1) {
                    wand_roh++;
                    if (!c.deck_neu[y][x]) verloren++;
                    else if (c.tief_neu[y][x] != c.tief_roh[y][x]) tief_anders++;
                }
                if (c.deck_neu[y][x] && !c.deck_roh[y][x]) dazu++;
            }
        printf("  Cut %d: Rechtecke roh %d -> gekappt %d; Deckung roh jenseits der Ecke %ld, gekappt %ld; Wandpunkte %ld, "
               "verloren %ld, Tiefe geaendert %ld, dazu %ld\n", cut, c.pri.draw_count, c.kap.draw_count,
               jenseits_roh, jenseits_neu, wand_roh, verloren, tief_anders, dazu);
        snprintf(t, sizeof t, "Cut %d Gegenprobe: die ROHEN Masken decken jenseits der gemalten Ecke (%ld Punkte, gemessen %s)",
                 cut, jenseits_roh, cut == 1 ? "474" : "372");
        CHECK(t, jenseits_roh == (cut == 1 ? 474 : 372));
        snprintf(t, sizeof t, "Cut %d gekappt: kein Maskenpunkt jenseits der gemalten Ecke (%ld)", cut, jenseits_neu);
        CHECK(t, jenseits_neu == 0);
        snprintf(t, sizeof t, "Cut %d gekappt: jeder Wandpunkt bleibt mit seiner Tiefe (%ld Punkte, verloren %ld, Tiefe anders %ld, dazu %ld)",
                 cut, wand_roh, verloren, tief_anders, dazu);
        CHECK(t, wand_roh > 20000 && verloren == 0 && tief_anders == 0 && dazu == 0);
        cut_frei(&c);
    }
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Leon an der Lage der Marke F741                                                                                   */
static int proj(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy, double *vz)
{
    double vx = (x * v->rot[0] + y * v->rot[1] + z * v->rot[2]) / 4096.0 + v->trans[0];
    double vy = (x * v->rot[3] + y * v->rot[4] + z * v->rot[5]) / 4096.0 + v->trans[1];
    *vz = (x * v->rot[6] + y * v->rot[7] + z * v->rot[8]) / 4096.0 + v->trans[2];
    if (*vz <= 64) return 0;
    *sx = 160 + vx * v->fov_screen_dist / *vz; *sy = 120 + vy * v->fov_screen_dist / *vz;
    return 1;
}

static void teil_leon(unsigned raum)
{
    static cut_t c; char t[300];
    /* Koerper als Zylinder: Radius 0..300 (Spielerradius 450 = Kollisionsring, der Koerper ist schmaler), Hoehe 0..-1700 */
    int ia, ir, ih;
    long jenseits = 0, jenseits_roh_deckt = 0, jenseits_neu_deckt = 0, hinter = 0, hinter_neu_deckt = 0;
    printf("== r39_optik leon: ROOM%04X Cut 1, Leon (-16007,0,-23658) rot 3023 (Marke F741) ==\n", raum);
    if (!cut_laden(&c, raum, 1)) { g_fail = 1; return; }
    for (ih = 0; ih <= 17; ih++)
        for (ir = 0; ir <= 3; ir++)
            for (ia = 0; ia < 16; ia++) {
                double a = ia * 3.14159265358979 / 8.0, r = ir * 100.0;
                double p[3] = { -16007 + r * cos(a), -100.0 * ih, -23658 + r * sin(a) }, sx, sy, vz;
                int px, py, w;
                if (!proj(&c.view, p[0], p[1], p[2], &sx, &sy, &vz)) continue;
                px = (int) floor(sx); py = (int) floor(sy);
                if (px < 0 || px >= 320 || py < 0 || py >= 240) continue;
                w = re15_optik_r39_in_wand(0x1100, 1, px, py);
                if (w == 0) {
                    jenseits++;
                    if (c.deck_roh[py][px] && re15_pri_mask_occludes((int) c.tief_roh[py][px], (long) vz)) jenseits_roh_deckt++;
                    if (c.deck_neu[py][px] && re15_pri_mask_occludes((int) c.tief_neu[py][px], (long) vz)) jenseits_neu_deckt++;
                } else if (w == 1 && c.deck_roh[py][px]) {
                    hinter++;
                    if (c.deck_neu[py][px] && re15_pri_mask_occludes((int) c.tief_neu[py][px], (long) vz)) hinter_neu_deckt++;
                }
            }
    printf("  Koerperpunkte jenseits der gemalten Ecke %ld: roh verdeckt %ld, gekappt verdeckt %ld | hinter der gemalten Wand %ld: "
           "gekappt verdeckt %ld\n", jenseits, jenseits_roh_deckt, jenseits_neu_deckt, hinter, hinter_neu_deckt);
    snprintf(t, sizeof t, "Gegenprobe: mit den ROHEN Masken liegt Boden ueber Leon jenseits der Ecke (%ld von %ld Punkten)",
             jenseits_roh_deckt, jenseits);
    CHECK(t, jenseits > 100 && jenseits_roh_deckt > 50);
    snprintf(t, sizeof t, "gekappt: jenseits der gemalten Ecke ist Leon frei (%ld verdeckt)", jenseits_neu_deckt);
    CHECK(t, jenseits_neu_deckt == 0);
    snprintf(t, sizeof t, "gekappt: hinter der gemalten Wand bleibt Leon verdeckt (%ld von %ld)", hinter_neu_deckt, hinter);
    CHECK(t, hinter > 100 && hinter_neu_deckt == hinter);
    cut_frei(&c);
}

static void teil_elza(void)
{
    printf("== r39_optik elza: ROOM1101 nutzt die Masken von ROOM1100 ==\n");
    CHECK("re15_pri_masken_raum(0x1101) == 0x1100 (Runde 37 Spur A, RDT bytegleich)", re15_pri_masken_raum(0x1101) == 0x1100);
    teil_leon(0x1101);
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Punkt 2: Schluessel an der Pinnwand ROOM10F0/10F1 "kleiner/subtiler" (NUTZER-VORGABE) — FORM: Bildlaenge wie RE2      */
/* Retail dasselbe Modell an seinen eigenen Platzierungen im Mittel zeigt (Median L_nat*H/vz ueber die RE2-Kameras, die */
/* den Schluessel im Bild haben). Hier aus den RE2-Dateien (info/re2leon) mit der Engine-Kamera nachgerechnet.          */
typedef struct { const char *rdt; long off; } re2_platz_t;

/* RE2 Obj_model_set (0x2D): Ursprung pc[14..19]; Kamera-Saetze @ u32 RDT+0x24, Anzahl RDT+1, 32 B wie RE1.5 (memcpy in
 * re15_camera_cut_t: +0 flag, +2 fov, +4 from, +0x10 to, +0x1C pri). */
static double re2_median_px(const re2_platz_t *pl, int npl, double l_nat, int *n_out, char *liste, size_t ln)
{
    double px[64]; int n = 0, i, j; size_t lp = 0;
    liste[0] = 0;
    for (i = 0; i < npl; i++) {
        char pfad[700]; size_t sz = 0; uint8_t *b; int16_t p[3]; uint32_t cam; int nc, ci;
        snprintf(pfad, sizeof pfad, "%s/%s", RE15_RE2_DIR, pl[i].rdt);
        b = slurp(pfad, &sz);
        if (!b || (size_t) pl[i].off + 20 > sz || b[pl[i].off] != 0x2D) {
            printf("  FEHLER: %s @0x%lX kein Obj_model_set\n", pfad, pl[i].off); g_fail = 1; free(b); continue;
        }
        memcpy(p, b + pl[i].off + 14, 6);
        nc = b[1]; memcpy(&cam, b + 0x24, 4);
        for (ci = 0; ci < nc; ci++) {
            re15_camera_cut_t cut; re15_camera_view_t v; double sx, sy, vz;
            if ((size_t) cam + 32u * (size_t) (ci + 1) > sz) break;
            memcpy(&cut, b + cam + 32 * ci, 32);
            if (re15_camera_build_view(&cut, &v) != 0) continue;
            if (!proj(&v, p[0], p[1], p[2], &sx, &sy, &vz)) continue;
            if (sx < 0 || sx >= 320 || sy < 0 || sy >= 240) continue;
            if (n < 64) px[n++] = l_nat * v.fov_screen_dist / vz;
            if (lp < ln - 40) lp += (size_t) snprintf(liste + lp, ln - lp, "%s C%d %.2f; ", pl[i].rdt + 8, ci, l_nat * v.fov_screen_dist / vz);
        }
        free(b);
    }
    for (i = 1; i < n; i++) { double t = px[i]; for (j = i; j > 0 && px[j - 1] > t; j--) px[j] = px[j - 1]; px[j] = t; }
    if (n_out) *n_out = n;
    if (!n) return -1;
    return (n % 2) ? px[n / 2] : (px[n / 2 - 1] + px[n / 2]) / 2.0;
}

static void md1_x_spanne(const uint8_t *m, int sz, int *x0, int *x1)
{
    static re15_md1_t md; int i;
    *x0 = 32767; *x1 = -32768;
    if (!m || re15_md1_parse(m, sz, &md) != 0 || md.mesh_count < 1) return;
    for (i = 0; i < md.meshes[0].tri_vertex_count; i++) {
        int x = md.meshes[0].tri_vertices[i].x;
        if (x < *x0) *x0 = x;
        if (x > *x1) *x1 = x;
    }
}

static void teil_schluessel(void)
{
    static const re2_platz_t park[] = { { "PL0/RDT/ROOM6150.RDT", 0x2C98 } };
    static const re2_platz_t prison[] = { { "PL0/RDT/ROOM10F0.RDT", 0x0706 }, { "PL0/RDT/ROOM20E0.RDT", 0x18F8 },
                                          { "PL0/RDT/ROOM20E0.RDT", 0x1A96 }, { "PL0/RDT/ROOM2020.RDT", 0x1C1E } };
    char t[400], liste[1200]; int n, x0, x1, sz; double med; int32_t lp[3]; int16_t lr[3];
    printf("== r39_optik schluessel: Parking Lot Key / Prison Key an der Pinnwand - RE2-Bildgroesse (Median) ==\n");
    /* RE2-Beleg aus den Dateien: P. Room Key (ROOM6150 @0x2C98), Small Key (10F0 @0x706, 20E0 @0x18F8/@0x1A96, 2020 @0x1C1E) */
    med = re2_median_px(park, 1, RE15_R39L_PARK_LNAT100 / 100.0, &n, liste, sizeof liste);
    printf("  Park:   %s-> Median %.2f px (%d Kameras)\n", liste, med, n);
    snprintf(t, sizeof t, "Park: RE2-Median %.2f px aus %d Kameras == RE15_R39L_PARK_ZIEL_PX100 %d / RE2_KAMERAS %d",
             med, n, RE15_R39L_PARK_ZIEL_PX100, RE15_R39L_PARK_RE2_KAMERAS);
    CHECK(t, n == RE15_R39L_PARK_RE2_KAMERAS && fabs(med * 100.0 - RE15_R39L_PARK_ZIEL_PX100) <= 1.0);
    med = re2_median_px(prison, 4, RE15_R39L_PRISON_LNAT100 / 100.0, &n, liste, sizeof liste);
    printf("  Prison: %s-> Median %.2f px (%d Kameras)\n", liste, med, n);
    snprintf(t, sizeof t, "Prison: RE2-Median %.2f px aus %d Kameras == RE15_R39L_PRISON_ZIEL_PX100 %d / RE2_KAMERAS %d",
             med, n, RE15_R39L_PRISON_ZIEL_PX100, RE15_R39L_PRISON_RE2_KAMERAS);
    CHECK(t, n == RE15_R39L_PRISON_RE2_KAMERAS && fabs(med * 100.0 - RE15_R39L_PRISON_ZIEL_PX100) <= 1.0);
    /* kleiner als Runde 38, gezeichnete Huelle am Ziel, aber sichtbar (>= 3 px) und nicht mehr die ganze Marke (< 9 px) */
    snprintf(t, sizeof t, "Park: Massstab %d < Runde 38 %d; Huelle %d == Ziel %d (+-2/100), 300 <= Huelle < 900",
             RE15_R38K_PARK_S10000, RE15_R39L_PARK_S38_10000, RE15_R39L_PARK_HUELLE_PX100, RE15_R39L_PARK_ZIEL_PX100);
    CHECK(t, RE15_R38K_PARK_S10000 < RE15_R39L_PARK_S38_10000 && RE15_R38K_PARK_S10000 > 10000 &&
             abs(RE15_R39L_PARK_HUELLE_PX100 - RE15_R39L_PARK_ZIEL_PX100) <= 2 &&
             RE15_R39L_PARK_HUELLE_PX100 >= 300 && RE15_R39L_PARK_HUELLE_PX100 < 900);
    snprintf(t, sizeof t, "Prison: Massstab %d < Runde 38 %d; Huelle %d == Ziel %d (+-2/100), 300 <= Huelle < 900",
             RE15_R38K_PRISON_S10000, RE15_R39L_PRISON_S38_10000, RE15_R39L_PRISON_HUELLE_PX100, RE15_R39L_PRISON_ZIEL_PX100);
    CHECK(t, RE15_R38K_PRISON_S10000 < RE15_R39L_PRISON_S38_10000 && RE15_R38K_PRISON_S10000 > 10000 &&
             abs(RE15_R39L_PRISON_HUELLE_PX100 - RE15_R39L_PRISON_ZIEL_PX100) <= 2 &&
             RE15_R39L_PRISON_HUELLE_PX100 >= 300 && RE15_R39L_PRISON_HUELLE_PX100 < 900);
    /* die gelieferten MD1 tragen den Massstab: x-Spanne der Ecken / s = RE2-Modellbreite 152 (P. Room Key +-76, Runde 38 K 2.6;
     * Small Key gemessen 152) */
    md1_x_spanne(re15_itemorte_r38_schluessel_md1(0x45, &sz), sz, &x0, &x1);
    snprintf(t, sizeof t, "Park-MD1 (%d B): Ecken x %d..%d -> Spanne/s = %.1f (RE2-Modell 152)", sz, x0, x1,
             (x1 - x0) * 10000.0 / RE15_R38K_PARK_S10000);
    CHECK(t, sz > 0 && fabs((x1 - x0) * 10000.0 / RE15_R38K_PARK_S10000 - 152.0) <= 2.0);
    md1_x_spanne(re15_itemorte_r38_schluessel_md1(0x42, &sz), sz, &x0, &x1);
    snprintf(t, sizeof t, "Prison-MD1 (%d B): Ecken x %d..%d -> Spanne/s = %.1f (RE2-Modell 152)", sz, x0, x1,
             (x1 - x0) * 10000.0 / RE15_R38K_PRISON_S10000);
    CHECK(t, sz > 0 && fabs((x1 - x0) * 10000.0 / RE15_R38K_PRISON_S10000 - 152.0) <= 2.0);
    /* Lage, Drehung und Aufhebe-Zone unveraendert (Runde 38 K: Marke key_location.bmp, Zone Slot 8) -> Aufnahme wie bisher */
    CHECK("Park: Lage (1358,-2495,-3400) rot (3072,0,2048) wie Runde 38",
          re15_itemorte_r38_schluessel_lage(0x45, lp, lr) && lp[0] == 1358 && lp[1] == -2495 && lp[2] == -3400 &&
          lr[0] == 3072 && lr[1] == 0 && lr[2] == 2048);
    CHECK("Prison: Lage (1358,-2495,-3400) rot (1024,0,0) wie Runde 38",
          re15_itemorte_r38_schluessel_lage(0x42, lp, lr) && lp[0] == 1358 && lp[1] == -2495 && lp[2] == -3400 &&
          lr[0] == 1024 && lr[1] == 0 && lr[2] == 0);
    CHECK("Aufhebe-Rechteck (798,-3600) 1000x1000, Slot 8 wie Runde 38",
          RE15_R38K_KS_RECT_X == 798 && RE15_R38K_KS_RECT_Z == -3600 && RE15_R38K_KS_RECT_W == 1000 &&
          RE15_R38K_KS_RECT_D == 1000 && RE15_R38K_SCHL_SLOT == 8);
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "kappen") || !strcmp(teil, "alle")) teil_kappen();
    if (!strcmp(teil, "ecke")   || !strcmp(teil, "alle")) teil_ecke();
    if (!strcmp(teil, "leon")   || !strcmp(teil, "alle")) teil_leon(0x1100);
    if (!strcmp(teil, "elza")   || !strcmp(teil, "alle")) teil_elza();
    if (!strcmp(teil, "schluessel") || !strcmp(teil, "alle")) teil_schluessel();
    printf(g_fail ? "r39_optik: FEHLER\n" : "r39_optik: OK\n");
    return g_fail ? 1 : 0;
}
