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

int re15_karte_r42_ring_klemmen(const void *zone, unsigned page, unsigned rect, int16_t *mx, int16_t *my)
{
    const re15_map_zone_t *zn = (const re15_map_zone_t *)zone;
    unsigned char k[8];
    int nk, cx, cy, f0, n0, dx, dy, bf = 0, bn = 0, bd = 0, bx = 0, by = 0, hat = 0;
    const int rand_n = (2 * R42_RING_HALB + 5) * (2 * R42_RING_HALB + 5) - (2 * R42_RING_HALB + 1) * (2 * R42_RING_HALB + 1);
    if (page < 6 || page > 12) return 0;
    nk = ort_kacheln(zn, page, rect, k, (int)sizeof k);
    cx = *mx - R42_GLYPH_MITTE; cy = *my - R42_GLYPH_MITTE;
    (void)ring_zaehlen(page, k, nk, cx, cy, &f0);
    n0 = rand_zaehlen(page, k, nk, cx, cy);
    /* Stufe 3 Runde 4, Nachbesserung (Pruefbericht pruef_s3_r3 ROT 4070_t0/4080_t0, Mass "Rot um den Punkt"): auch
     * wenn der Ring keine fremde Kachel beruehrt, aber im 2-px-RAND um ihn (56 Texel) WENIGER ALS 30 % auf dem
     * Raumkoerper des eigenen Orts liegen (Treppengitter r7 von 4070, Spitze der Diagonale r13 von 4080), wandert die
     * Mitte um hoechstens die halbe Ringbreite zur NAECHSTEN Lage, deren Rand zur Haelfte Raumkoerper ist (gibt es keine:
     * zur Lage mit dem meisten Raumkoerper). PORT-WAHL (Anzeige, keine Spielkonstante); 30 % ist das Mass der Pruefung
     * ("Rot um den Punkt", pruef_s3_r1 eigen.py, Grenze 0.3), die Haelfte das Ziel, weil auf dem Schirm Treppen- und
     * Tuerstriche den Rand weiter schmaelern (gemessen: 30 % in der Kunst = 0,25 im Abzug L4070_t0). Ausgeloest wird nur
     * unter 30 %: eine Klemme, die jeden Punkt auf die beste Lage zoege, setzte ihn in schmalen Kacheln auf die Mittellinie,
     * und er liefe quer nicht mehr mit (Riegel unit_r42_karte_st456 [C], gemessen 4040 Zone 1 8/19, 4070 21/49).
     * Reihenfolge: wenig fremde Texel im Ring, Rand >= Haelfte, kurzer Weg, mehr Rand. */
    if (f0 == 0 && n0 * 10 >= rand_n * 3) return 0;
    /* Suchweite = halbe Ringbreite + 1. Gemessen an der Spitze der Diagonale r13 (4080_t0, Rand im Abzug): Suchweite 2 ->
     * 0,27, 3 -> 0,29, 4 -> 0,29 (dieselbe Lage (170,117) = Maximum des Raumkoerpers in der Kunst); 4070_t0: 2 -> 0,38,
     * 3 -> 0,57. PORT-WAHL. */
    for (dy = -(R42_RING_HALB + 1); dy <= R42_RING_HALB + 1; dy++)
        for (dx = -(R42_RING_HALB + 1); dx <= R42_RING_HALB + 1; dx++) {
            int n, f, d;
            if (!im_ort(page, k, nk, cx + dx, cy + dy, 1)) continue;   /* Mitte bleibt auf dem Raumkoerper des eigenen ORTS */
            (void)ring_zaehlen(page, k, nk, cx + dx, cy + dy, &f);
            n = rand_zaehlen(page, k, nk, cx + dx, cy + dy);
            d = dx * dx + dy * dy;
            if (!hat || f < bf ||
                (f == bf && ((n * 2 >= rand_n) > (bn * 2 >= rand_n) ||
                             ((n * 2 >= rand_n) == (bn * 2 >= rand_n) &&
                              ((n * 2 >= rand_n) ? (d < bd || (d == bd && n > bn)) : (n > bn || (n == bn && d < bd))))))) {
                hat = 1; bf = f; bn = n; bd = d; bx = cx + dx; by = cy + dy;
            }
        }
    if (!hat || bf > f0 || (bf == f0 && bn <= n0)) return 0;
    *mx = (int16_t)(bx + R42_GLYPH_MITTE);
    *my = (int16_t)(by + R42_GLYPH_MITTE);
    return 1;
}
