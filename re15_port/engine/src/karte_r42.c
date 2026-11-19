/*
 * RE1.5 Rebuilt — r42/karte Stufe 3 Runde 2: Positionsmarker auf die gemalte Kachel (Blatt 6..12).
 * Herleitung und Grenzen: include/re15_karte_r42.h. Daten: engine/src/re15_map_kunst.h (tools/gen_map_kunst.py).
 * Rufer: re15_inv_screen.c re15_inv_map_marker (Haken "r42/karte Stufe 3 Runde 2").
 */
#include "re15_karte_r42.h"
#include "re15_map_kunst.h"

/* Messhaken fuer Riegel (unit_r42_karte_glatt): 1 = Punkt ohne die Klemmen und ohne Gang-Zeile, also nur Zeile + Rechteck.
 * Im Spiel immer 0. */
int g_re15_karte_r42_roh = 0;

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
    if (g_re15_karte_r42_roh) return 0;
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
    if (g_re15_karte_r42_roh) return 0;
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

/* ---- r42/karte Stufe 3 Runde 7 ----------------------------------------------------------------------------------------------
 * (A) Die allgemeine Klemme der Runde 6 ("sauberer Ring", Suchweite 7) ist ZURUECKGENOMMEN (Orchestrator-Vorgabe ab Runde 7,
 *     Punkt A; Pruefbericht pruef_s3_r6 ROT 2: Spruenge >= 4 px auf dem begehbaren 500er-Raster R5 406 -> R6 646, u. a.
 *     2050/2051, 20B0/20B1, 11A0, 2030, 3050, 50D0, 6000). re15_karte_r42_ring_klemmen ist wieder der Stand von Runde 5
 *     (02972374c). Riegel: unit_r42_karte_glatt_s2..s6 (Spruenge je Raum nicht mehr als in Runde 5).
 * (B) GANG-ZEILE fuer den Schraeggang von ROOM4080/4081 (Zone 1 = Kachel r13, Blatt 8, DATA/MAP09.PIX). PORT-ERGAENZUNG
 *     (die Zeile des Raums @0x800768b0 + 8*(65+8) = @0x80076AF8 ist der Stub {0,0,1,1}), Formel des Originals FUN_800473f8
 *     @0x8004741c-0x80047528, ohne Klemme. Pruefbericht pruef_s3_r6 ROT 1: "beim ersten Schritt in den Gang springt der Punkt
 *     6 Bildpunkte zurueck ... steht er wieder auf der Wandlinie, ueber blauem Grund, direkt am Uebergang zu 4030".
 *     Welt (ROOM4080.RDT, Kollisionszellen 12 B ab 0x0055C):
 *       Zelle 6 @0x005A4 = 6c20 6c20 6ae6 ebf6 04: Dreieck Typ 4 (-6550,-2325) 8300x8300 -> Wand x+z = -575
 *       Zelle 11 @0x005E0 = 6c20 6c20 37e2 b8f2 07: Dreieck Typ 7 (-7625,-3400) 8300x8300 -> Wand x+z = -2725
 *       (re15_collision.c: Typ 7 fest fuer (z-Z)*W < (X+W-x)*D). Der Gang liegt genau unter 45 Grad (Hypotenusen quadratischer
 *       Zellen), seine Mitte ist x+z = -1650. Er beginnt an der Tuer nach 4030 (Slot 0, Mitte (-4250,2550), x-z = -6800) und
 *       endet, wo beide Dreiecke enden (Ecken (1750,-2325) und (675,-3400), x-z = 4075).
 *     Kunst (Blatt 8, r13 'd' Index != 0/4): Gang-Inneres je Zeile y bei x = y+49..y+53 (Wandlinien x-y = 48 und 54, Mitte
 *       x-y = 51), Spitze an der Tuer (166,112), Ende (172,123); dahinter die Nahtlinie x+y = 296..297 (Rahmenlinie von r3 und
 *       Wandlinien r13/r8), ab x+y = 298 die breite Diagonale r8.
 *     Zeile: Mitte x+z = -1650 -> Punkt mx-my = 51 (Gangmitte); Tuer (x-z = -6800) -> Punkt mx+my = 279, Gangende (x-z = 4075)
 *       -> mx+my = 297. Daraus sx = sy = round(18 * 2^20 / 10875) = 1736 (gleich in x und z: sonst kippt die Mittellinie des
 *       Gangs gegen die gemalte), ox = 119, oy = 171. Zum Vergleich die Zeile des Raums (Zone 0, aus den Waenden): 1833/1905.
 *     HALTESTELLEN (gemessen an DATA/MAP09.PIX, Messhilfe scratchpad karte/s3r7/mitte.py): der Ring (5x5 ohne Ecken, TEX.TIM
 *       @0x14910 uv 225..229) ist so breit wie der Gang. Sein Inneres (3x3) liegt nur auf der Mittellinie ganz auf dem Raum
 *       (Ring-Mitten (167,116) .. (172,121)); die Spitze an der Tuer ist zu schmal. Rand (9x9 ohne 5x5, sichtbarer Raum von
 *       r8/r13 ohne Tuerbalken) auf der Mittellinie: (167,116) 15, (168,117) 17, (169,118) 18, (170,119) 17, (171,120) 15,
 *       (172,121) 20 von 56. Das Mass der Pruefung verlangt mindestens 18/56 = 0,32 (v0.8.30, Abzug der Ankunft). Es bleiben
 *       ZWEI Lagen: Ring-Mitte (169,118) und (172,121), Punkt (170,119) und (173,122). Der Punkt nimmt die Haltestelle, die
 *       der Gang-Zeile am naechsten liegt (gemessen entlang mx+my; bei Gleichstand die zur Tuer). Ein Rueckwaertssprung ist
 *       damit ausgeschlossen: mx+my der Zeile waechst mit x-z, die Haltestellen auch.
 *     HINTER DEM GANGENDE (Ring-Mitte auf oder hinter der Naht, Punkt mx+my >= 298) rechnet der Punkt mit der Zone der breiten
 *       Diagonale (Zone 0, r8, Zeile aus den Waenden) samt der Klemmen von Runde 4/5 - dort wie bisher.
 *     PORT-WAHL (Anzeige, keine Spielkonstante). Grenzen (offen benannt im Bericht s3 r7): die beiden Haltestellen liegen
 *     drei Bildpunkte auseinander; Ring-Mitte (169,118) hat den Tuerbalken 4080<->4030 (x165 y111..115) 2 Texel weit im 9x9-Feld,
 *     der Ring selbst beruehrt ihn nicht. */
typedef struct {
    unsigned short room;
    unsigned char idx, weiter_idx;      /* Zone des Gangs, Zone hinter dem Gangende */
    short ox, oy;
    unsigned short sx, sy;              /* Formel FUN_800473f8 @0x8004741c-0x80047528 */
    short naht;                         /* Punkt mx+my, ab dem der Ring auf/hinter der Naht liegt */
    unsigned char n;
    short halt[2][2];                   /* Haltestellen (Punkt, nicht Ring-Mitte) */
} r42_gang_t;

static const r42_gang_t s_r42_gang[] = {
    { 0x4080, 1, 0, 119, 171, 1736, 1736, 298, 2, { { 170, 119 }, { 173, 122 } } },
    { 0x4081, 1, 0, 119, 171, 1736, 1736, 298, 2, { { 170, 119 }, { 173, 122 } } },
};

const void *re15_karte_r42_gang(const void *zone, int32_t wx, int32_t wz, int16_t *mx, int16_t *my, int *fertig)
{
    const re15_map_zone_t *zn = (const re15_map_zone_t *)zone;
    int i, j;
    *fertig = 0;
    if (!zn || zn->etage || g_re15_karte_r42_roh) return 0;
    for (i = 0; i < (int)(sizeof s_r42_gang / sizeof s_r42_gang[0]); i++) {
        const r42_gang_t *g = &s_r42_gang[i];
        int32_t t, t2, px, py, s, beste = 0, bd = 0;
        if (g->room != zn->room || g->idx != zn->idx) continue;
        t  = ((wx + 32000) * 10 * (int32_t)g->sx) >> 20;
        t2 = ((wz + 32000) * 10 * (int32_t)g->sy) >> 20;
        px = (t + 5) / 10 + g->ox;
        py = -((t2 + 5) / 10) + g->oy;
        s = px + py;
        if (s >= g->naht) {                       /* hinter dem Gangende: breite Diagonale */
            for (j = 0; j < re15_map_zone_count(); j++) {
                const re15_map_zone_t *z = re15_map_zone_by_index(j);
                if (z && z->room == zn->room && z->idx == g->weiter_idx && !z->etage && z->page == zn->page) return z;
            }
            return 0;
        }
        for (j = 0; j < g->n; j++) {
            int32_t d = s - (g->halt[j][0] + g->halt[j][1]);
            if (d < 0) d = -d;
            if (j == 0 || d < bd) { bd = d; beste = j; }
        }
        *mx = g->halt[beste][0];
        *my = g->halt[beste][1];
        *fertig = 1;
        return 0;
    }
    return 0;
}
