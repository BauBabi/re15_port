/*
 * RE1.5 Rebuilt — r42/karte Stufe 3 Runde 2: Positionsmarker auf die gemalte Kachel (Blatt 6..12).
 * Herleitung und Grenzen: include/re15_karte_r42.h. Daten: engine/src/re15_map_kunst.h (tools/gen_map_kunst.py).
 * Rufer: re15_inv_screen.c re15_inv_map_marker (Haken "r42/karte Stufe 3 Runde 2").
 */
#include "re15_karte_r42.h"
#include "re15_map_kunst.h"

static const re15_map_kunst_idx_t *kachel(unsigned page, unsigned rect)
{
    int i;
    for (i = 0; i < RE15_MAP_KUNST_IDX_N; i++)
        if (s_map_kunst_idx[i].page == page && s_map_kunst_idx[i].rect == rect) return &s_map_kunst_idx[i];
    return 0;
}

int re15_karte_r42_kunst_gemalt(unsigned page, unsigned rect, int x, int y)
{
    const re15_map_kunst_idx_t *k = kachel(page, rect);
    int i;
    if (!k) return 0;
    for (i = 0; i < (int)k->anzahl; i++) {
        const re15_map_kunst_spanne_t *s = &s_map_kunst_spannen[k->erst + i];
        int sy = k->ry + s->y, x0 = k->rx + s->x0;
        if (sy == y && x >= x0 && x < x0 + s->len) return 1;
    }
    return 0;
}

/* Der Punkt wird um (mx,my) als 8x8-Quad gezeichnet, sein sichtbarer Ring hat die Mitte bei (mx-1,my-1):
 * TEX.TIM @0x14910 (dieselbe Konstante wie KF_GLYPH_MITTE, karte_fahrstuhl_1080.c; in allen 611 Abzuegen der
 * Stufe 3 Runde 2 liegt die Ring-Mitte genau dort). Geklemmt wird diese sichtbare Mitte, nicht (mx,my). */
#define R42_GLYPH_MITTE 1

int re15_karte_r42_kunst_klemmen(unsigned page, unsigned rect, int16_t *mx, int16_t *my)
{
    const re15_map_kunst_idx_t *k;
    int i, best = -1, bx = 0, by = 0;
    int cx = *mx - R42_GLYPH_MITTE, cy = *my - R42_GLYPH_MITTE;
    if (page < 6 || page > 12) return 0;          /* nur STAGE2-6 (re15_karte_r42.h) */
    k = kachel(page, rect);
    if (!k || k->anzahl == 0) return 0;
    for (i = 0; i < (int)k->anzahl; i++) {
        const re15_map_kunst_spanne_t *s = &s_map_kunst_spannen[k->erst + i];
        int sy = k->ry + s->y, x0 = k->rx + s->x0, x1 = x0 + s->len - 1;
        int px = cx < x0 ? x0 : (cx > x1 ? x1 : cx);
        int dx = px - cx, dy = sy - cy, d = dx * dx + dy * dy;
        if (d == 0) return 0;                       /* liegt schon auf der Kunst */
        if (best < 0 || d < best) { best = d; bx = px; by = sy; }
    }
    *mx = (int16_t)(bx + R42_GLYPH_MITTE);
    *my = (int16_t)(by + R42_GLYPH_MITTE);
    return best;
}

/* ---- r42/karte Stufe 3 Runde 4: der RING des Punkts nicht auf der Nachbarkachel (re15_karte_r42.h) ------------------- */
#include "re15_room.h"

/* Halbe Ringbreite: der sichtbare Ring liegt im 8x8-Quad ab uv(224,128) auf uv 225..229 (DATA/TEX.TIM ab Datei-Byte
 * 0x14910, Zeile v=129; karte_fahrstuhl_1080.c KF_GLYPH_MITTE) - 5 Texel, Mitte +-2. */
#define R42_RING_HALB 2

/* 1, wenn (x,y) auf IRGENDEINEM Texel der Kachel (Index != 0, samt Wandlinie) liegt. */
static int kunst_alles(unsigned page, unsigned rect, int x, int y)
{
    const re15_map_kunst_idx_t *k = kachel(page, rect);
    int i;
    if (!k) return 0;
    for (i = 0; i < (int)k->anzahl_a; i++) {
        const re15_map_kunst_spanne_t *s = &s_map_kunst_alles[k->erst_a + i];
        int sy = k->ry + s->y, x0 = k->rx + s->x0;
        if (sy == y && x >= x0 && x < x0 + s->len) return 1;
    }
    return 0;
}

/* Kacheln DESSELBEN Orts auf diesem Blatt: Zonen desselben Raums mit derselben Zeile (re15_map_zones.c rect_state_impl,
 * Regel "ein Ort auf zwei Kacheln", 4040 r4+r12, 4080 r8+r13, 5140 r10+r12). */
static int ort_kacheln(const re15_map_zone_t *zn, unsigned page, unsigned rect, unsigned char *out, int max)
{
    int i, n = 0, j;
    out[n++] = (unsigned char)rect;
    for (i = 0; zn && i < re15_map_zone_count() && n < max; i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        if (!z || z->room != zn->room || z->page != page || z->etage || z->rect == 255) continue;
        if (z->ox != zn->ox || z->oy != zn->oy || z->sx != zn->sx || z->sy != zn->sy) continue;
        if (z->flip_x != zn->flip_x || z->flip_z != zn->flip_z || z->tausch != zn->tausch) continue;
        for (j = 0; j < n && out[j] != z->rect; j++) ;
        if (j == n) out[n++] = z->rect;
    }
    return n;
}

static int im_ort(unsigned page, const unsigned char *k, int nk, int x, int y, int nur_koerper)
{
    int j;
    for (j = 0; j < nk; j++)
        if (nur_koerper ? re15_karte_r42_kunst_gemalt(page, k[j], x, y) : kunst_alles(page, k[j], x, y)) return 1;
    return 0;
}

/* Ring um (cx,cy): *fremd = Texel auf der Kunst einer ANDEREN Kachel des Blatts (nicht dieses Orts), Rueckgabe = Texel auf
 * dem Raumkoerper dieses Orts. */
static int ring_zaehlen(unsigned page, const unsigned char *k, int nk, int cx, int cy, int *fremd)
{
    int dx, dy, i, n = 0, f = 0;
    for (dy = -R42_RING_HALB; dy <= R42_RING_HALB; dy++)
        for (dx = -R42_RING_HALB; dx <= R42_RING_HALB; dx++) {
            int x = cx + dx, y = cy + dy;
            if (im_ort(page, k, nk, x, y, 1)) { n++; continue; }
            if (im_ort(page, k, nk, x, y, 0)) continue;          /* eigene Wandlinie: weder noch */
            for (i = 0; i < RE15_MAP_KUNST_IDX_N; i++) {
                const re15_map_kunst_idx_t *q = &s_map_kunst_idx[i];
                if (q->page != page) continue;
                if (kunst_alles(page, q->rect, x, y)) { f++; break; }
            }
        }
    *fremd = f;
    return n;
}

/* Raumkoerper des Orts im 2-px-RAND um den Ring (9x9-Kasten ohne den 5x5-Ring): liegt der Punkt IM Raum oder an dessen
 * Rand/auf einer Treppenlinie? */
static int rand_zaehlen(unsigned page, const unsigned char *k, int nk, int cx, int cy)
{
    int dx, dy, n = 0, r = R42_RING_HALB + 2;
    for (dy = -r; dy <= r; dy++)
        for (dx = -r; dx <= r; dx++) {
            if (dx >= -R42_RING_HALB && dx <= R42_RING_HALB && dy >= -R42_RING_HALB && dy <= R42_RING_HALB) continue;
            if (im_ort(page, k, nk, cx + dx, cy + dy, 1)) n++;
        }
    return n;
}

/* ---- Stufe 3 Runde 5 (Pruefbericht pruef_s3_r4 ROT 4080_t0: "Der Punkt steht an der Spitze der Diagonale direkt an der
 * Kachel von 4030", Rot-Mass 0,29 gegen 0,32 in v0.8.30) -------------------------------------------------------------------
 * Das Ziel der Klemme wird jetzt am SICHTBAREN Raumkoerper gemessen: Texel, die eine Tuer- oder Treppenmarke dieses Blatts
 * uebermalt, sind auf dem Schirm kein Raum (re15_inv_screen.c, Tuer kind 0..3 = 5x1/1x5-Balken um (mx,my), Treppe kind 4/5
 * = drei 5-px-Sprossen im Abstand 2). Bisher zaehlte die Klemme sie mit und hielt (170,117) fuer die beste Lage - auf dem
 * Schirm liegt dort der Tuerbalken 4080<->4030 (Marke (165,113) kind 3, x165 y111..115) im Rand. PORT-WAHL (Anzeige, keine
 * Spielkonstante). Ausgeloest wird die Klemme wie bisher (Ring auf fremder Kachel oder Rand der Kunst < 30 %); nur das Mass
 * des Ziels (sichtbar) und die Suchweite (halbe Ringbreite + 2 = 4 px) aendern sich. Gemessen ueber alle 397 Tuer-Ankuenfte
 * STAGE1-6 (unit_r42_karte_lauf mit R42_DUMP, vorher/nachher): es wandert NUR 4080/4081 aus 4030, (170,117) ->
 * (171,118); dort liegt im 9x9-Feld um den Ring keine Kunst einer fremden Kachel und kein Tuerbalken. Ein zusaetzlicher
 * Vorrang "9x9-Feld frei von fremder Kunst" wurde gemessen und verworfen: er zog auch 4080 aus 4090 (213,129) -> (215,129)
 * und 4040 aus 4050 um 2 px und brach das Mitlaufen im Ostarm (Riegel [M1]). */
/* Die gezeichneten Marken des Blatts werden je Aufruf EINMAL gesammelt (re15_map_mark_get prueft die Sichtbarkeit ueber den
 * Zonen-Zustand und ist teuer; je Texel aufgerufen machte es integration_map_raum_live 14 s -> 304 s langsam). */
#define R42_MARKEN_MAX 96
typedef struct { short x, y; unsigned char kind; } r42_marke_t;
static r42_marke_t s_marken[R42_MARKEN_MAX];
static int s_marken_n;

static void marken_sammeln(unsigned page)
{
    int k, n = re15_map_mark_count();
    s_marken_n = 0;
    for (k = 0; k < n && s_marken_n < R42_MARKEN_MAX; k++) {
        int mpg, mrc, mx, my, kind;
        if (!re15_map_mark_get(k, &mpg, &mrc, &mx, &my, &kind)) continue;   /* nur gezeichnete Marken */
        if ((unsigned)mpg != page) continue;
        s_marken[s_marken_n].x = (short)mx; s_marken[s_marken_n].y = (short)my;
        s_marken[s_marken_n].kind = (unsigned char)kind; s_marken_n++;
    }
}

static int marke_deckt(int x, int y)
{
    int k;
    for (k = 0; k < s_marken_n; k++) {
        int mx = s_marken[k].x, my = s_marken[k].y, kind = s_marken[k].kind;
        if (kind <= 3) {
            int laengs_x = (kind == 0 || kind == 2);
            if (laengs_x ? (y == my && x >= mx - 2 && x <= mx + 2) : (x == mx && y >= my - 2 && y <= my + 2)) return 1;
        } else {
            int row;
            for (row = -2; row <= 2; row += 2) {
                if (kind == 5 ? (x == mx + row && y >= my - 2 && y <= my + 2) : (y == my + row && x >= mx - 2 && x <= mx + 2))
                    return 1;
            }
        }
    }
    return 0;
}

static int rand_sichtbar(unsigned page, const unsigned char *k, int nk, int cx, int cy)
{
    int dx, dy, n = 0, r = R42_RING_HALB + 2;
    for (dy = -r; dy <= r; dy++)
        for (dx = -r; dx <= r; dx++) {
            if (dx >= -R42_RING_HALB && dx <= R42_RING_HALB && dy >= -R42_RING_HALB && dy <= R42_RING_HALB) continue;
            if (im_ort(page, k, nk, cx + dx, cy + dy, 1) && !marke_deckt(cx + dx, cy + dy)) n++;
        }
    return n;
}

int re15_karte_r42_ring_klemmen(const void *zone, unsigned page, unsigned rect, int16_t *mx, int16_t *my)
{
    const re15_map_zone_t *zn = (const re15_map_zone_t *)zone;
    unsigned char k[8];
    int nk, cx, cy, f0, n0, v0, dx, dy, bf = 0, bn = 0, bd = 0, bx = 0, by = 0, hat = 0;
    const int rand_n = (2 * R42_RING_HALB + 5) * (2 * R42_RING_HALB + 5) - (2 * R42_RING_HALB + 1) * (2 * R42_RING_HALB + 1);
    const int weit = R42_RING_HALB + 2;
    if (page < 6 || page > 12) return 0;
    nk = ort_kacheln(zn, page, rect, k, (int)sizeof k);
    cx = *mx - R42_GLYPH_MITTE; cy = *my - R42_GLYPH_MITTE;
    (void)ring_zaehlen(page, k, nk, cx, cy, &f0);
    n0 = rand_zaehlen(page, k, nk, cx, cy);
    /* Ausloeser unveraendert seit Runde 4 (pruef_s3_r3 ROT 4070_t0/4080_t0): Ring auf fremder Kachel oder < 30 % Raumkoerper
     * der Kunst im 2-px-Rand (Mass der Pruefung "Rot um den Punkt", Grenze 0.3). Eine Klemme, die jeden Punkt verschoebe,
     * setzte ihn in schmalen Kacheln auf die Mittellinie (Riegel unit_r42_karte_st456 [C], gemessen Runde 4). */
    if (f0 == 0 && n0 * 10 >= rand_n * 3) return 0;
    marken_sammeln(page);
    v0 = rand_sichtbar(page, k, nk, cx, cy);
    /* Reihenfolge: wenig fremde Texel im Ring, sichtbarer Rand >= Haelfte, dann kurzer Weg (sonst mehr Rand). Gemessen an 4080_t0 (Ankunft aus 4030, Rohlage (167,114)): Suchweite 3 erreicht
     * hoechstens 0,29 sichtbar (dieselbe Lage wie Runde 4), Suchweite 4 -> (171,118), im Abzug 0,36 (v0.8.30: 0,32). */
    for (dy = -weit; dy <= weit; dy++)
        for (dx = -weit; dx <= weit; dx++) {
            int n, f, d, h, bh;
            if (!im_ort(page, k, nk, cx + dx, cy + dy, 1)) continue;   /* Mitte bleibt auf dem Raumkoerper des eigenen ORTS */
            (void)ring_zaehlen(page, k, nk, cx + dx, cy + dy, &f);
            n = rand_sichtbar(page, k, nk, cx + dx, cy + dy);
            d = dx * dx + dy * dy;
            h = (n * 2 >= rand_n); bh = (bn * 2 >= rand_n);
            if (!hat || f < bf ||
                (f == bf && (h > bh ||
                 (h == bh && (h ? (d < bd || (d == bd && n > bn)) : (n > bn || (n == bn && d < bd))))))) {
                hat = 1; bf = f; bn = n; bd = d; bx = cx + dx; by = cy + dy;
            }
        }
    if (!hat || bf > f0 || (bf == f0 && bn <= v0)) return 0;
    *mx = (int16_t)(bx + R42_GLYPH_MITTE);
    *my = (int16_t)(by + R42_GLYPH_MITTE);
    return 1;
}
