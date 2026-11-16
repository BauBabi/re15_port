/*
 * RE1.5 Rebuilt — Runde 41 Spur "pipe" (Herleitung und Belege: include/re15_pipe_r41.h,
 * Bank-Bau tools/pipe_r41/gen_pipe_plw.py, Riegel tests/unit/test_pipe_r41.c).
 */
#include "re15_pipe_r41.h"
#include "gen/pipe_r41.inc"   /* RE15_PIPE_R41_REICHWEITE (generiert aus den Netzen) */

int re15_pipe_r41_ohne_zahl(uint8_t id)
{
    /* NUTZERWUNSCH: Messer (1) und Rohr (2) ohne Zahl — die beiden Ids der Nahkampf-Spalte @0x8006E54C/@0x8006E550. */
    return id == RE15_PIPE_R41_MESSER || id == RE15_PIPE_R41_ID;
}

uint32_t re15_pipe_r41_reichweite(int weapon_id, uint32_t tabelle)
{
    /* Original @0x8006E5A8 = 1000 (Grundwert); NUTZERWUNSCH "mehr Reichweite als das Messer" (@0x8006E5A4 = 1100). */
    if (weapon_id == RE15_PIPE_R41_ID) return (uint32_t)RE15_PIPE_R41_REICHWEITE;
    return tabelle;
}

int32_t re15_pipe_r41_re2_tiefe(int weapon_id)
{
    /* Vorsprung des Rohrs ueber das Messer (1100 @0x8006E5A4) — dieselbe Zahl wie im RE1.5-Kegel. */
    return (weapon_id == RE15_PIPE_R41_ID) ? (int32_t)RE15_PIPE_R41_REICHWEITE - 1100 : 0;
}

int re15_pipe_r41_arms_bank(int weapon_id)
{
    /* PORT-WAHL Vorlage Messer: ARMS02 ist auf der Disc byte-gleich ARMS03..06 (Pistole) -> Messer-Bank ARMS01. */
    return (weapon_id == RE15_PIPE_R41_ID) ? RE15_PIPE_R41_MESSER : weapon_id;
}

int re15_pipe_r41_treffer_ton(int weapon_id)
{
    /* Original: nur Waffe 1 (`ori v0,zero,0x1` @0x800123C0, `bne s0,v0` @0x800123C4); PORT-WAHL: Rohr wie Messer. */
    return weapon_id == RE15_PIPE_R41_MESSER || weapon_id == RE15_PIPE_R41_ID;
}

/* ---- Messhaken RE15_PIPE_LOG (kein Spielverhalten) ------------------------------------------------------------- */
#include "re15_actor.h"
#include "re15_engine.h"
#include <stdio.h>
#include <stdlib.h>

void re15_pipe_r41_log_treffer(int weapon_id, int slot, uint32_t abstand_hand, uint32_t reichweite)
{
    static const char *pfad = NULL;
    static int geprueft = 0;
    if (!geprueft) { pfad = getenv("RE15_PIPE_LOG"); geprueft = 1; }
    if (!pfad || !*pfad || (weapon_id != RE15_PIPE_R41_MESSER && weapon_id != RE15_PIPE_R41_ID)) return;
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    FILE *f = fopen(pfad, "a");
    if (!f) return;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER], *e = &g_actors[slot];
    int64_t dx = (int64_t)e->x - pl->x, dz = (int64_t)e->z - pl->z;
    int64_t q = dx * dx + dz * dz, r = 0;
    while ((r + 1) * (r + 1) <= q) r++;
    fprintf(f, "[pipe] F%u waffe=%d slot=%d abstand_hand=%u abstand_spieler=%lld reichweite=%u hp=%d "
               "spieler=(%d,%d,rot=%d) gegner=(%d,%d)\n",
            (unsigned)g_engine.frame_count, weapon_id, slot, (unsigned)abstand_hand, (long long)r,
            (unsigned)reichweite, (int)e->hp, (int)pl->x, (int)pl->z, (int)pl->rot_y, (int)e->x, (int)e->z);
    fclose(f);
}
