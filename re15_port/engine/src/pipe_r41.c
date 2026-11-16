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
