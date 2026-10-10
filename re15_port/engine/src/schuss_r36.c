/*
 * schuss_r36.c — Runde 36 Spur C: Sichtlinie des Spieler-Schusses (RE1.5 FUN_8001b9b4).
 * Belege und Einordnung: include/re15_schuss_r36.h, Dossier analysis/befunde_runde36/C_schuss.md §1.
 */
#include "re15_schuss_r36.h"
#include "re15_room.h"       /* g_room_rdt_ok */

static int s_verworfen = 0;

int  re15_schuss_r36_sichtlinie_verworfen(void) { return s_verworfen; }
void re15_schuss_r36_sichtlinie_reset(void)     { s_verworfen = 0; }

int re15_schuss_r36_sichtlinie_blockiert(const re15_actor_t *pl, const re15_actor_t *ziel)
{
    int region;
    if (!pl || !ziel) return 0;
    if (!g_room_rdt_ok) return 0;                  /* keine SCA -> leere Regionen -> 0 (@0x8003dd94) */
    /* FUN_8001b9b4: `ori s0,zero,0x3` @0x8001ba08, Schleife `addiu s0,s0,-1` / `bne` @0x8001ba34-38,
     * je Region FUN_8003dcc4(delta, region, 0xf00, 0x300) @0x8001ba1c-24; das Ergebnis wird
     * ODER-verknuepft (`or s1,s1,v0` @0x8001ba28) — der erste Treffer entscheidet. Ursprung und
     * Band (+0x82) sind die des Spielers (DAT_800ac784, @0x8003dd00/@0x8003de60). */
    for (region = 3; region >= 0; region--) {
        if (re15_los_ray_blocked(pl, ziel, region, 0u)) {
            s_verworfen++;
            return 1;
        }
    }
    return 0;
}
