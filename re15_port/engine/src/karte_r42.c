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

int re15_karte_r42_kunst_klemmen(unsigned page, unsigned rect, int16_t *mx, int16_t *my)
{
    const re15_map_kunst_idx_t *k;
    int i, best = -1, bx = 0, by = 0;
    if (page < 6 || page > 12) return 0;          /* nur STAGE2-6 (re15_karte_r42.h) */
    k = kachel(page, rect);
    if (!k || k->anzahl == 0) return 0;
    for (i = 0; i < (int)k->anzahl; i++) {
        const re15_map_kunst_spanne_t *s = &s_map_kunst_spannen[k->erst + i];
        int sy = k->ry + s->y, x0 = k->rx + s->x0, x1 = x0 + s->len - 1;
        int px = *mx < x0 ? x0 : (*mx > x1 ? x1 : *mx);
        int dx = px - *mx, dy = sy - *my, d = dx * dx + dy * dy;
        if (d == 0) return 0;                       /* liegt schon auf der Kunst */
        if (best < 0 || d < best) { best = d; bx = px; by = sy; }
    }
    *mx = (int16_t)bx;
    *my = (int16_t)by;
    return best;
}
