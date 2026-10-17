/*
 * RE1.5 Rebuilt — Runde 38 Spur W "rolltor20a0" (PC): ein Raum-Objekt-Viereck, von dessen zwei GPU-Dreiecken nur
 * EINES die Groessengrenze besteht, zeichnet die PSX-GPU als genau dieses eine Dreieck.
 *
 * psx-spx graphicsprocessingunitgpu.md: Vierecke rastert die GPU als zwei Dreiecke aus den Scheiteln (0,1,2) und
 * (1,2,3); "The maximum distance between two vertices is 1023 horizontally, and 511 vertically. Polygons and lines
 * that are exceeding that dimensions are NOT rendered." — je Dreieck (Original-Messung ROOM20A0 Cut 11, Viereck
 * Q07: Dreieck (0,1,2) zu gross, (1,2,3) nicht; analysis/befunde_runde38/W_rolltor20a0.md §4).
 * Die Prop-Schleife in main.c zeichnet ein Viereck mit BEIDEN bestandenen Dreiecken unveraendert (ihre Teilung
 * (0,1,3)+(0,3,2) deckt dieselbe Flaeche); nur den Halbfall gibt sie hierher.
 */
#include <stdint.h>

#include "re15_objpoly_r38.h"
#include "re15_objabe_r38.h" /* Nachbesserung 1: halbtransparente Objekte (ABE/ABR/STP) zeichnen ueber denselben Weg */

void re15_objpoly_r38_halbviereck_pc(uint16_t attr, const int ax[4], const int ay[4], const re15_md1_quad_uv_t *uv,
                                     int page_off, int wz, const uint8_t gpu_ok[2], const uint8_t rgb[12])
{
    const int u[4] = { uv->u0, uv->u1, uv->u2, uv->u3 };
    const int v[4] = { uv->v0, uv->v1, uv->v2, uv->v3 };
    for (int t = 0; t < 2; t++) {
        if (!gpu_ok[t]) continue;
        const int a = t, b = t + 1, c = t + 2;      /* (0,1,2) bzw. (1,2,3) */
        re15_objabe_r38_pc_dreieck(attr, uv->page,
                                     ax[a], ay[a], u[a] + page_off, v[a],
                                     ax[b], ay[b], u[b] + page_off, v[b],
                                     ax[c], ay[c], u[c] + page_off, v[c],
                                     0, (int)uv->clut, wz,
                                     rgb[3 * a], rgb[3 * a + 1], rgb[3 * a + 2],
                                     rgb[3 * b], rgb[3 * b + 1], rgb[3 * b + 2],
                                     rgb[3 * c], rgb[3 * c + 1], rgb[3 * c + 2]);
    }
}
