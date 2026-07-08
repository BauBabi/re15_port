/* test_scd_for2.c — the byte-true SCD For2 loop opcode (0x1B, LAB_8003f5d0).
 *
 * For2 is the dynamic-count sibling of For (0x0D): its iteration count comes from a script WORK
 * VARIABLE (count = work_vars[pc[5]], `lhu 0x800b0fd0 + var*2` @0x8003f620) instead of a literal
 * u16. It was unmapped -> op_unknown, which skipped the loop set-up entirely. Everything else (the
 * loop-frame push, and the Next/Break that pop it) is identical to For. Asserts:
 *   (1) the pushed For-count == work_vars[var_idx] (the ONLY byte-true difference from For).
 *   (2) a different var_idx reads a different work variable.
 *   (3) count==0 skips the block (no loop frame pushed — the defensive early-out, like op_for).
 *   (4) end-to-end: the loop body executes exactly work_vars[var_idx] times.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_scd.h"
#include "re15_aot.h"

#define TEST_SLOT 0
#define OP_EVT_NEXT 0x02   /* 1-byte yield sentinel — parks the thread so we can read its state */

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static void vm_reset(void)
{
    scd_vm_init();      /* zeroes g_scd (incl. work_vars), registers the opcodes */
    re15_aot_init();
}

int main(void)
{
    printf("=== SCD For2 (0x1B) — For loop with a work-variable count (LAB_8003f5d0) ===\n");

    /* (1) the For-count is read from work_vars[var_idx]. block_len=0 -> body (pc+6) is the sentinel,
     *     so the frame is pushed and the thread parks without iterating. */
    {
        vm_reset();
        g_scd.work_vars[5] = 7;
        uint8_t bc[] = { 0x1B, 0x00, 0x00, 0x00, 0x00, 0x05, OP_EVT_NEXT };  /* blk_len=0, var_idx=5 */
        scd_thread_start(TEST_SLOT, bc);
        scd_vm_tick();
        const scd_thread_t *th = &g_scd.threads[TEST_SLOT];
        CHECK("(1) loop frame pushed", th->loop_count == 1);
        CHECK("(1) For-count == work_vars[5] (7)", th->loop_for_cnt[0] == 7);
    }

    /* (2) a different var_idx selects a different work variable. */
    {
        vm_reset();
        g_scd.work_vars[5] = 99;    /* decoy */
        g_scd.work_vars[9] = 3;
        uint8_t bc[] = { 0x1B, 0x00, 0x00, 0x00, 0x00, 0x09, OP_EVT_NEXT };  /* var_idx=9 */
        scd_thread_start(TEST_SLOT, bc);
        scd_vm_tick();
        const scd_thread_t *th = &g_scd.threads[TEST_SLOT];
        CHECK("(2) For-count == work_vars[9] (3), not work_vars[5]", th->loop_for_cnt[0] == 3);
    }

    /* (3) count==0 skips the block — no loop frame pushed (defensive early-out, same as op_for). */
    {
        vm_reset();
        g_scd.work_vars[5] = 0;
        uint8_t bc[] = { 0x1B, 0x00, 0x00, 0x00, 0x00, 0x05, OP_EVT_NEXT };  /* blk_len=0 -> skip lands on sentinel */
        scd_thread_start(TEST_SLOT, bc);
        scd_vm_tick();
        const scd_thread_t *th = &g_scd.threads[TEST_SLOT];
        CHECK("(3) count==0 -> no frame pushed", th->loop_count == 0);
    }

    /* (4) end-to-end: the loop body runs exactly work_vars[var_idx] times. Body = Calc (0x26) ADD
     *     work_vars[10] += 1; Next (0x0E) closes the loop; OP_EVT_NEXT terminates. */
    {
        vm_reset();
        g_scd.work_vars[5]  = 5;    /* count */
        g_scd.work_vars[10] = 0;    /* accumulator */
        uint8_t bc[] = {
            0x1B, 0x00, 0x08, 0x00, 0x00, 0x05,   /* For2: block_len=8 (body..after-Next), var_idx=5 */
            0x26, 0x00, 0x00, 0x0A, 0x01, 0x00,   /* Calc: work_vars[10] += 1 (op ADD=0, dst=10, imm=1) */
            0x0E, 0x00,                           /* Next */
            OP_EVT_NEXT                           /* loop exit / park */
        };
        scd_thread_start(TEST_SLOT, bc);
        scd_vm_tick();
        CHECK("(4) loop body ran work_vars[5]==5 times", g_scd.work_vars[10] == 5);
        CHECK("(4) loop frame popped after completion", g_scd.threads[TEST_SLOT].loop_count == 0);
    }

    if (g_fail) { printf("SCD-FOR2: FAIL\n"); return 1; }
    printf("SCD-FOR2: all checks passed\n");
    return 0;
}
