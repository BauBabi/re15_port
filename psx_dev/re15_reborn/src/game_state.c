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
#include "re15_scd.h"

re15_game_state_t g_game;

void re15_game_state_init(void)
{
    /* Phase 4.5.9-D: g_game only holds flags[] now — player lives in
     * g_actors[0]. */
    memset(&g_game, 0, sizeof(g_game));
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
}
