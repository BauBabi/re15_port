/*
 * RE1.5 Rebuilt — Game state (Phase 4.4.4, 2026-05-19).
 *
 * Hosts the SCD-driven world state: player position/heading/animation
 * and 512 game flags. Written by opcode handlers (Pos_set, Dir_set,
 * Plc_motion, Set), read by the renderer + conditional opcodes (Ck).
 *
 * Target-agnostic — same on PSX and PC since g_game is just data.
 */

#include <string.h>
#include <stdio.h>     /* Flag-Trace (RE15_FLAG_TRACE) */
#include <stdlib.h>
#include "re15_scd.h"

re15_game_state_t g_game;

/*=========================================================================
 * DAT_800aca40 / DAT_800b853c — die globalen Pause-Flags.  Semantik + alle
 * Adress-Belege stehen bei den RE15_PAUSE_*-Defines in re15_scd.h.
 *=========================================================================*/
uint32_t g_re15_pauseflags       = 0;   /* DAT_800aca40 */
uint32_t g_re15_pauseflags_saved = 0;   /* DAT_800b853c */

/* DAT_800b8520 & 0x80 — "eine Message ist offen".  Der Port hat sein eigenes
 * Message-Lifetime-Modell (g_scd.message_*), aber der Open-GUARD @0x80027e74
 * braucht genau dieses eine Bit: der Port fuehrt den Message_on-Opcode bzw. den
 * Examine-AOT waehrend eines offenen Textes erneut aus, und ohne Guard wuerde
 * @0x80027ec8 den Snapshot mit dem BEREITS EINGEFRORENEN Wert ueberschreiben —
 * der Restore koennte den Freeze dann nie mehr aufheben. */
static int s_pause_open = 0;

void re15_pauseflags_open(uint32_t mask)
{
    /* Open-Guard: FUN_80027e68 @0x80027e74 `lbu v0,DAT_800b8520` / @0x80027e7c
     * `andi v0,v0,0x80` / @0x80027e80 `beq` -> sonst `j 0x800280ac` mit v0 = -1.
     * Ein zweiter Open waehrend eines offenen Textes speichert NICHTS und
     * ver-ODERt NICHTS. */
    if (s_pause_open) return;
    s_pause_open = 1;
    g_re15_pauseflags_saved = g_re15_pauseflags;   /* @0x80027eb4 lw + @0x80027ec8 sw */
    g_re15_pauseflags      |= mask;                /* @0x80027ecc or + @0x80027ed0 sw */
}

void re15_pauseflags_close(void)
{
    /* FUN_80028134: `lw a0,DAT_800b853c` -> `sw a0,0x800aca40` an drei Stellen —
     * @0x80028594/@0x800285a4 (Select-Confirm), @0x800286bc/@0x800286cc (End-Wait),
     * @0x80028708/@0x8002871c (Hold-N-Timeout).  Alle drei loeschen zusaetzlich
     * das Open-Bit (`andi v0,v0,0x7f` @0x80028598/@0x800286c0/@0x8002870c). */
    if (!s_pause_open) return;
    s_pause_open      = 0;
    g_re15_pauseflags = g_re15_pauseflags_saved;
}

void re15_pauseflags_clear(void)
{
    /* Raumwechsel-FSM: @0x8001ca44 und @0x8001caec `sw zero,-13760(at)` = 0x800aca40. */
    g_re15_pauseflags       = 0;
    g_re15_pauseflags_saved = 0;
    s_pause_open            = 0;
}

void re15_game_state_init(void)
{
    /* Phase 4.5.9-D: g_game only holds flags[] now — player lives in
     * g_actors[0]. */
    memset(&g_game, 0, sizeof(g_game));
    re15_pauseflags_clear();
}

/* Byte-faithful flag store: per-zone u32 array, MSB-first WITHIN the 32-bit word.
 *   word = idx >> 5,  mask = 0x80000000 >> (idx & 0x1f)
 * Matches the original FUN_8004ef90 (set) / FUN_8004efe4 (check) exactly
 * (= RE2 FUN_80077360 / FUN_8007730c). idx spans 0..255 per zone. */
int re15_game_flag_get(uint8_t zone, uint8_t idx)
{
    if (zone >= RE15_FLAG_ZONES) return 0;
    uint32_t word = (uint32_t)(idx >> 5);             /* 0..7 */
    uint32_t mask = 0x80000000u >> (idx & 0x1f);      /* MSB-first in the word */
    return (g_game.flags[zone][word] & mask) ? 1 : 0;
}

void re15_game_flag_set(uint8_t zone, uint8_t idx, int value)
{
    if (zone >= RE15_FLAG_ZONES) return;
    uint32_t word = (uint32_t)(idx >> 5);
    uint32_t mask = 0x80000000u >> (idx & 0x1f);
    if (value) g_game.flags[zone][word] |= mask;
    else       g_game.flags[zone][word] &= ~mask;
    /* RE15_FLAG_TRACE=1: die beiden Intro-Ende-Flags mitschreiben. Solange flag(1,27) oder
     * flag(2,7) steht, zieht main.c:2890 player_mode jeden Frame auf 2 und der Spieler bleibt
     * skript-gefuehrt — im nicht-interaktiven Lauf endet das Helipad-Intro genau deshalb nie. */
    if ((zone == 1 && idx == 27) || (zone == 2 && idx == 7)) {
        extern int re15_flag_trace_enabled(void);
        if (re15_flag_trace_enabled())
            fprintf(stderr, "[flag] z%u/%u = %d\n", zone, idx, value ? 1 : 0);
    }
}

int re15_flag_trace_enabled(void)
{
    static int on = -1;
    if (on < 0) on = getenv("RE15_FLAG_TRACE") ? 1 : 0;
    return on;
}
