/*
 * RE1.5 Rebuilt — Runde 36 Spur R "dachkiste": Messschrieb des Kisten-Schubs (Diagnose, KEIN Verhalten).
 *
 * RE15_PUSH_LOG=<datei>: je Schubversuch der Schubkette FUN_8002bd44 (aot_common.c re15_prop_push_tick) eine
 * Zeile — Bild, Objekt, Spielerlage, die von FUN_8002cabc(Spieler,obj,1) @0x8002bfa4 gesetzte Ziel-Lage und das
 * Ergebnis von FUN_8003b558(obj,2) @0x8002bfb0 (0 = angenommen @0x8002bfc0, 1 = gesperrt @0x8002c0b0 ->
 * Ruecksetzung @0x8002c0b8-d4). Bei "gesperrt" folgt je getroffener Zelle eine Zeile TREFFER-ZELLE mit
 * Region/Index und Rechteck — das Gegenstueck zum Haltepunkt in FUN_8003da78 (Aufruf @0x8003b79c je Treffer,
 * a0 = Zelle) der Original-Messung (analysis/befunde_runde36/R_belege/orig_1090_push.py), damit Original und
 * Port Versuch fuer Versuch vergleichbar sind (Dossier R_dachkiste.md, Punkt 1-3).
 *
 * Die Zellensuche hier ist dieselbe wie in re15_collision_box_blocked (re15_collision.c), nur mit Ausgabe:
 *   Quadrant       FUN_8003b068 (2-Bit aus sign(pos - ceiling)), Zellenliste des Quadranten
 *   Band           @0x8003b634-644  Bandnibble der Zelle (floor >> 4) == obj+0x82
 *   Klasse         @0x8003b64c-660  (a1 & u0) != 0, a1 = 2 fuer den Schub (@0x8002bfac)
 *   Rechteck       @0x8003b738-768  (u32)(cx - (x0 - r)) < w + 2r, @0x8003b770-794 dito z, r = hx (@0x8003b68c-690)
 */
#include <stdio.h>
#include <stdlib.h>

#include "re15_dachkiste_r36.h"
#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_rdt.h"
#include "re15_room.h"
#include "re15_scd.h"

extern scd_vm_t g_scd;

/* FUN_8003b068 — identisch zu quadrant_of in re15_collision.c (dort static). */
static int quadrant(int32_t px, int32_t pz, int16_t ceil_x, int16_t ceil_z)
{
    unsigned zb = (unsigned)(pz - (int32_t)ceil_z) & 0x80000000u;
    unsigned xb = (unsigned)(px - (int32_t)ceil_x) & 0x80000000u;
    return (int)((zb | (xb >> 1)) >> 0x1e);
}

static void treffer_zellen(FILE *f, int32_t cx, int32_t cz, int band, int32_t r, unsigned mask)
{
    const re15_rdt_t *rdt = &g_room_rdt;
    if (!rdt->sca || rdt->sca_count <= 0 || band < 0) return;
    const int q = quadrant(cx, cz, (int16_t)rdt->ceiling_x, (int16_t)rdt->ceiling_z);
    int start = 0;
    for (int i = 0; i < q && i < 5; i++) start += rdt->sca_rgn[i];
    int end = start + (q < 5 ? rdt->sca_rgn[q] : 0);
    if (end > rdt->sca_count) end = rdt->sca_count;
    for (int i = start; i < end; i++) {
        const re15_sca_entry_t *e = &rdt->sca[i];
        if (band != (e->floor >> 4)) continue;                                     /* @0x8003b634-644 */
        if ((mask & e->u0) == 0) continue;                                         /* @0x8003b64c-660 */
        if ((unsigned)(cx - ((int32_t)e->x - r)) < (unsigned)((int32_t)e->width   + r * 2) &&   /* @0x8003b738-768 */
            (unsigned)(cz - ((int32_t)e->z - r)) < (unsigned)((int32_t)e->density + r * 2)) {   /* @0x8003b770-794 */
            fprintf(f, "  TREFFER-ZELLE r%d[%d]: x %d..%d z %d..%d typ %d u0 0x%02x u1 0x%02x fl 0x%02x\n",
                    q, i - start, (int)e->x, (int)e->x + (int)e->width, (int)e->z, (int)e->z + (int)e->density,
                    (int)e->type, (unsigned)e->u0, (unsigned)e->u1, (unsigned)e->floor);
        }
    }
}

void re15_dachkiste_schub_log(int prop_idx, int32_t ziel_x, int32_t ziel_z, int gesperrt)
{
    static int init = 0;
    static FILE *f = NULL;
    if (!init) {
        init = 1;
        const char *p = getenv("RE15_PUSH_LOG");
        if (p && *p) f = fopen(p, "w");
    }
    if (!f || prop_idx < 0 || prop_idx >= RE15_SCD_MAX_PROPS) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    fprintf(f, "F%u room=%04X p=%d obj=%d SCHUB-%s spieler=(%d,%d) rot=%d lage=(%d,%d) ziel=(%d,%d) band=%d\n",
            g_engine.frame_count, g_current_room_id, prop_idx, (int)g_scd.props[prop_idx].obj_id,
            gesperrt ? "GESPERRT" : "OK", pl->x, pl->z, (int)pl->rot_y,
            g_scd.props[prop_idx].x, g_scd.props[prop_idx].z, ziel_x, ziel_z, (int)g_scd.props[prop_idx].band);
    if (gesperrt)
        treffer_zellen(f, ziel_x + g_scd.props[prop_idx].box_cx, ziel_z + g_scd.props[prop_idx].box_cz,
                       (int)g_scd.props[prop_idx].band, (int32_t)(uint16_t)g_scd.props[prop_idx].box_hx, 2u);
    fflush(f);
}
