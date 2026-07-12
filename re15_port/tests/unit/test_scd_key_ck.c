/* test_scd_key_ck.c — byte-true SCD Sce_key_ck (0x51) input predicate.
 *
 * LAB_80042920: param=pc[1], mask=LE u16 @pc[2]; cond = (mask & DAT_800ac768) != 0 ? param
 * : (param^1); PC+=4. DAT_800ac768 = the per-frame HELD (logical/remapped) pad register, which
 * the port publishes to the VM as g_scd_pad_held = pad_current (game_step_common.c:163) in the
 * exact PSX bit convention (re15_player.h: CROSS 0x4000 / SQUARE 0x8000 / D-pad 0x10-0x80 — the
 * same word the dialog FSM reads as & 0x4000 = CROSS, audit wf_6aad95ad). The op returns the
 * dispatcher boolean the enclosing Ifel_ck consumes: CONTINUE(1)=predicate TRUE (run the If body),
 * IF_FALSE(3)=predicate FALSE (pop the block). The no-input case stays param^1 (byte-identical to
 * the prior hard-coded behavior), so only an actually-held mask button changes anything — e.g.
 * ROOM1080's 8 per-frame `Ifel_ck{Sce_key_ck(1,<bit>)}` input-poll predicates now respond.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_scd.h"

extern int op_sce_key_ck(scd_thread_t *t);
extern int op_sce_espr_control(scd_thread_t *t);   /* 0x52 — pressed-edge sibling of 0x51 */
extern uint16_t g_scd_pad_held;
extern uint16_t g_scd_pad_edge;

/* scd_vm.c dispatcher codes: #define SCD_R_CONTINUE 1 / SCD_R_IF_FALSE 3. */
#define R_TRUE  1
#define R_FALSE 3

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static int keyck(uint8_t param, uint16_t mask, uint16_t held)
{
    uint8_t bc[4] = { 0x51, param, (uint8_t)(mask & 0xff), (uint8_t)(mask >> 8) };
    scd_thread_t th; memset(&th, 0, sizeof th); th.pc = bc;
    g_scd_pad_held = held;
    return op_sce_key_ck(&th);
}

/* 0x52 = the pressed-EDGE sibling: identical predicate on g_scd_pad_edge. */
static int esprck(uint8_t param, uint16_t mask, uint16_t edge)
{
    uint8_t bc[4] = { 0x52, param, (uint8_t)(mask & 0xff), (uint8_t)(mask >> 8) };
    scd_thread_t th; memset(&th, 0, sizeof th); th.pc = bc;
    g_scd_pad_edge = edge;
    return op_sce_espr_control(&th);
}

int main(void)
{
    printf("=== byte-true Sce_key_ck (0x51): (mask & DAT_800ac768) ? param : param^1 ===\n");

    /* param=1: held mask bit -> TRUE (param); not held -> FALSE (param^1 = 0). */
    CHECK("CROSS(0x4000) held, param=1 -> TRUE",              keyck(1, 0x4000, 0x4000) == R_TRUE);
    CHECK("CROSS not held, param=1 -> FALSE (no-key=param^1)", keyck(1, 0x4000, 0x0000) == R_FALSE);
    CHECK("wrong button held (SQUARE) vs CROSS mask -> FALSE", keyck(1, 0x4000, 0x8000) == R_FALSE);

    /* param=0 inverts the sense: held -> FALSE (param), not held -> TRUE (param^1 = 1). */
    CHECK("DOWN(0x40) held, param=0 -> FALSE",                keyck(0, 0x0040, 0x0040) == R_FALSE);
    CHECK("DOWN not held, param=0 -> TRUE (param^1)",         keyck(0, 0x0040, 0x0000) == R_TRUE);

    /* multi-bit mask: ANY set bit in the held word matches (`& != 0`). */
    CHECK("mask 0x00f0 (dpad), UP(0x10) held, param=1 -> TRUE",       keyck(1, 0x00f0, 0x0010) == R_TRUE);
    CHECK("mask 0x00f0, only START(0x08) held, param=1 -> FALSE",     keyck(1, 0x00f0, 0x0008) == R_FALSE);

    /* the byte-true no-input default (held=0) is unchanged vs the old hard-coded path for both
     * param senses — this is why wiring the register is regression-free without live input. */
    CHECK("no-input default param=1 -> FALSE (unchanged)",   keyck(1, 0xffff, 0x0000) == R_FALSE);
    CHECK("no-input default param=0 -> TRUE (unchanged)",    keyck(0, 0xffff, 0x0000) == R_TRUE);

    /* PC advances exactly 4 (LAB_80042920 PC+=4), regardless of the branch taken. */
    {
        uint8_t bc[4] = { 0x51, 1, 0x00, 0x40 };
        scd_thread_t th; memset(&th, 0, sizeof th); th.pc = bc;
        g_scd_pad_held = 0;
        op_sce_key_ck(&th);
        CHECK("PC advanced 4", th.pc == bc + 4);
    }

    /* ---- 0x52 pressed-edge sibling (op_sce_espr_control, LAB_8004295c) ---- */
    printf("--- 0x52 pressed-edge predicate: (mask & DAT_800ac76c=g_scd_pad_edge) ---\n");
    CHECK("0x52 CROSS press-edge, param=1 -> TRUE",          esprck(1, 0x4000, 0x4000) == R_TRUE);
    CHECK("0x52 no press-edge, param=1 -> FALSE (param^1)",  esprck(1, 0x4000, 0x0000) == R_FALSE);
    CHECK("0x52 START(0x08) edge, mask 0x00f0 -> FALSE",     esprck(1, 0x00f0, 0x0008) == R_FALSE);
    CHECK("0x52 UP(0x10) edge, mask 0x00f0 -> TRUE",         esprck(1, 0x00f0, 0x0010) == R_TRUE);
    CHECK("0x52 no-input default param=0 -> TRUE (unchanged)", esprck(0, 0xffff, 0x0000) == R_TRUE);

    if (g_fail) { printf("SCD-KEY-CK: FAIL\n"); return 1; }
    printf("SCD-KEY-CK: all checks passed\n");
    return 0;
}
