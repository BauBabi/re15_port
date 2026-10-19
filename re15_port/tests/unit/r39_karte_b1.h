/* =============================================================================
 * Runde 39 Spur G "karten" - Pruefhilfen fuer B1 (Blatt 0) als SCHEMA-ZEICHNUNG
 * =============================================================================
 * Nutzer 2026-10-17 (analysis/befunde_runde39/AUFTRAG.md): "Der Room B1 den habe ich nicht gemacht. Da muss du
 * generell noch einmal schauen, da passt nichts so richtig zusammen mit dem wie man sich bewegt im Vergleich zu
 * der Karte." Seit Runde 39 zeichnet Blatt 0 keine gemalten Kacheln mehr, sondern je Ort eine Schema-Zeichnung
 * (re15_map_zones.h, B1-Kopf in s_map_zones; Rechnung analysis/befunde_runde39/G_karten_werkzeug/b1_plan.py).
 *
 * Die Riegel der Runden 35/36 pruefen dieselben ABSICHTEN ("aktuell ist NUR der Ort X", "der Marker steht auf
 * der Zeichnung von X", "die Treppe sitzt auf der Projektion ihres STAIR-AOT") - frueher an gemalten Rechtecken
 * und Kachel-Texeln, jetzt an der Zeichnung: gemessen wird die ECHTE Op-Liste des Kartenschirms
 * (re15_inv_screen_build), nicht eine Nachbildung.
 * ========================================================================== */
#ifndef R39_KARTE_B1_H
#define R39_KARTE_B1_H

#include <string.h>
#include "re15_room.h"
#include "re15_inv_screen.h"

/* Schema-Nummern (zone.synth) der B1-Orte, re15_map_zones.h s_map_synth Index 3..10 (synth = Index + 1). */
enum { R39B1_GANG = 4, R39B1_HALLE = 5, R39B1_OST = 6, R39B1_11B0 = 7, R39B1_11C0 = 8, R39B1_11D0 = 9,
       R39B1_10A0 = 10, R39B1_KAMMER = 11 };

/* Liegt (x,y) in einer Zelle der Zeichnung <synth>?  innen = 1: nicht auf der Randlinie der Zelle. */
static inline int r39b1_in(int synth, int x, int y, int innen)
{
    int i, n = re15_map_zone_count();
    for (i = 0; i < n; i++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(i);
        int bx, by, bw, bh, erste, nz, k;
        if (!zn || zn->synth != synth) continue;
        if (!re15_map_zone_synth(zn, &bx, &by, &bw, &bh, &erste, &nz)) continue;
        for (k = 0; k < nz; k++) {
            int cx, cy, cw, ch;
            if (!re15_map_synth_cell(erste + k, &cx, &cy, &cw, &ch)) continue;
            if (innen ? (x > cx && x < cx + cw - 1 && y > cy && y < cy + ch - 1)
                      : (x >= cx && x < cx + cw && y >= cy && y < cy + ch)) return 1;
        }
        return 0;
    }
    return 0;
}

/* Kasten (Huelle) der Zeichnung <synth>; 0 = gibt es nicht. */
static inline int r39b1_kasten(int synth, int *x, int *y, int *w, int *h)
{
    int i, n = re15_map_zone_count();
    for (i = 0; i < n; i++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(i);
        if (zn && zn->synth == synth) return re15_map_zone_synth(zn, x, y, w, h, 0, 0);
    }
    return 0;
}

/* Zustand der Zeichnung <synth> im gebauten Kartenschirm (g_inv_screen, Blatt schon gesetzt): 2 = AKTUELL
 * (Fuellung RE15_KARTE_AKTUELL), 1 = besucht (RE15_KARTE_BESUCHT), 0 = keine Fuellung (unbesucht/gar nicht).
 * Gemessen an der FILL-Op (abe 1) genau auf der ersten Zelle - so zeichnet sie der Schema-Durchgang. */
static inline int r39b1_zustand(int synth)
{
    static re15_inv_op_t ops[RE15_INV_MAX_OPS];
    int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS), i, zi, nzone = re15_map_zone_count();
    int cx = -1, cy = -1, cw = 0, ch = 0;
    for (zi = 0; zi < nzone; zi++) {
        const re15_map_zone_t *zn = re15_map_zone_by_index(zi);
        int bx, by, bw, bh, erste, nz;
        if (!zn || zn->synth != synth) continue;
        if (re15_map_zone_synth(zn, &bx, &by, &bw, &bh, &erste, &nz) && nz > 0)
            re15_map_synth_cell(erste, &cx, &cy, &cw, &ch);
        break;
    }
    if (cx < 0) return 0;
    for (i = 0; i < n; i++) {
        const re15_inv_op_t *q = &ops[i];
        if (q->kind != RE15_INV_OP_FILL || !q->abe) continue;
        if (q->x != cx || q->y != cy || q->w != cw || q->h != ch) continue;
        if (q->r == (uint8_t)((RE15_KARTE_AKTUELL & 31u) << 3) &&
            q->g == (uint8_t)(((RE15_KARTE_AKTUELL >> 5) & 31u) << 3)) return 2;
        if (q->r == (uint8_t)((RE15_KARTE_BESUCHT & 31u) << 3) &&
            q->g == (uint8_t)(((RE15_KARTE_BESUCHT >> 5) & 31u) << 3)) return 1;
    }
    return 0;
}

/* "Aktuell ist NUR die Zeichnung <synth>": sie AKTUELL, alle anderen B1-Zeichnungen nicht, und keine gemalte
 * Kachel von Blatt 0 aktuell (die gibt es nicht mehr). Ausnahme <auch>: eine zweite Zeichnung DESSELBEN Raums,
 * die nach der Regel "rot gilt fuer jede besuchte Zeichnung des Raums" (re15_inv_screen.c) mitleuchtet (0 = keine). */
static inline int r39b1_nur_aktuell(int synth, int auch)
{
    int s, ok = (r39b1_zustand(synth) == 2);
    for (s = R39B1_GANG; s <= R39B1_KAMMER; s++)
        if (s != synth && s != auch && r39b1_zustand(s) == 2) ok = 0;
    for (s = 0; s < re15_map_rect_count(0); s++)
        if (re15_map_rect_state(0, (unsigned)s) == RE15_MAP_RECT_CURRENT) ok = 0;
    return ok;
}

#endif
