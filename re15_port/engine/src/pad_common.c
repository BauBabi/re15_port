/*
 * pad_common.c — the VIRTUAL (config-remapped) pad-word builder (wave-6 finding 4).
 * Kept dependency-free so unit tests that link menu/msg/scd objects do not drag the
 * whole game-step closure in.
 */
#include "re15_engine.h"
#include "re15_player.h"

/* ---- VIRTUAL (config-remapped) pad word — byte-true FUN_80030444 @0x800304b8-e4:
 * for each virtual bit i (15..0): `lhu raw_mask = table[i*2]; and raw; bne -> or (1<<i)`.
 * Active table = PTR_DAT_80073e1c[DAT_800b0fcc]; MZD default preset 0 @0x80073dbc
 * (bytes 00 10 00 20 00 40 00 80 00 10 00 40 80 00 80 00 / 08 00 40 00 08 00 04 00
 *  00 80 00 20 80 00 40 00, re-read this pass):
 *   [0..3]  UP RIGHT DOWN LEFT      (raw 0x1000/0x2000/0x4000/0x8000)
 *   [4..5]  UP DOWN                 [6..7]  SQUARE SQUARE (raw 0x0080)
 *   [8]     R1        [9]  CROSS (raw 0x0040)   [10] R1   [11] L1
 *   [12..13] LEFT RIGHT (menu-L/R)  [14] SQUARE = CONFIRM  [15] CROSS = CANCEL
 * i.e. virtual confirm 0x4000 <- RAW SQUARE and virtual cancel 0x8000 <- RAW CROSS
 * (wave-6 finding 4). The table below is preset 0 translated to the PORT's physical
 * bit layout (re15_player.h); TYPE B/C/EDIT presets are normalized to TYPE-A physical
 * by the platform's pc_pad_config BEFORE this word is built, which composes to the
 * same virtual result as the PSX per-preset tables. Every entry is a single bit, so
 * building the word from a press-EDGE mask yields the exact virtual edge word
 * (DAT_800ac76c); from the held mask, the virtual held word (DAT_800ac768). */
uint16_t re15_pad_virtual_word(uint16_t phys)
{
    static const uint16_t vtbl[16] = {
        RE15_PAD_BIT_UP,   RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_DOWN, RE15_PAD_BIT_LEFT,
        RE15_PAD_BIT_UP,   RE15_PAD_BIT_DOWN,
        RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE,
        RE15_PAD_BIT_R1,   RE15_PAD_BIT_CROSS, RE15_PAD_BIT_R1,   RE15_PAD_BIT_L1,
        RE15_PAD_BIT_LEFT, RE15_PAD_BIT_RIGHT,
        RE15_PAD_BIT_SQUARE /* 0x4000 confirm @0x80073dbc[14]=0x0080 */,
        RE15_PAD_BIT_CROSS  /* 0x8000 cancel  @0x80073dbc[15]=0x0040 */
    };
    uint16_t out = 0;
    int i;
    for (i = 0; i < 16; i++)
        if (phys & vtbl[i]) out |= (uint16_t)(1u << i);
    return out;
}
