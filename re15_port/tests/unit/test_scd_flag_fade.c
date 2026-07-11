/* test_scd_flag_fade.c — the byte-true SCD 0x59 (indexed flag write) + 0x57 (fade step) opcodes
 * (audio audit wf_1db9c802: AUDIO-OP59-NOT-XA + AUDIO-OP57-IS-FADE-NOT-BGM).
 *
 * 0x59 (@0x800744a8[0x59] -> 0x8003fe90) is NOT Xa_on: it is the WRITE sibling of the 0x58
 * flag-check — idx = work_vars[pc[2]], bank = pc[1], MSB-first bit (0x80000000 >> (idx&0x1f));
 * pc[3] 1=SET / 0=CLEAR / 7=TOGGLE, PC+=4. The old handler queued a spurious XA event and lost
 * ROOM1030/1031's scripted flag write.
 * 0x57 (@0x80042ab4) is NOT Sce_bgmtbl_set: ch = pc[1], step = u16 LE @pc[2..3] into the screen-
 * fade channel model (FUN_800216ec -> DAT_800b5458); no audio event.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_scd.h"
#include "re15_aot.h"

#define TEST_SLOT 0
#define OP_EVT_NEXT 0x02

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static void run_op(const uint8_t *bc)
{
    scd_thread_start(TEST_SLOT, bc);
    scd_vm_tick();
}

int main(void)
{
    printf("=== SCD 0x59 flag write + 0x57 fade step (audit wf_1db9c802) ===\n");

    /* 0x59 SET: work_vars[4] = 37 -> bank 5, idx 37 (word 1, bit 5) set. No audio event. */
    {
        scd_vm_init(); re15_aot_init();
        g_scd.work_vars[4] = 37;
        unsigned audio_before = g_scd.audio_count;
        uint8_t bc[] = { 0x59, 0x05, 0x04, 0x01, OP_EVT_NEXT };
        run_op(bc);
        CHECK("0x59 SET: flag (5, 37) set", re15_game_flag_get(5, 37) == 1);
        CHECK("0x59 SET: no audio event queued", g_scd.audio_count == audio_before);
    }

    /* 0x59 CLEAR (op 0) + TOGGLE (op 7). */
    {
        scd_vm_init(); re15_aot_init();
        g_scd.work_vars[4] = 37;
        re15_game_flag_set(5, 37, 1);
        uint8_t bc_clear[] = { 0x59, 0x05, 0x04, 0x00, OP_EVT_NEXT };
        run_op(bc_clear);
        CHECK("0x59 CLEAR: flag (5, 37) cleared", re15_game_flag_get(5, 37) == 0);
        uint8_t bc_tgl[] = { 0x59, 0x05, 0x04, 0x07, OP_EVT_NEXT };
        run_op(bc_tgl);
        CHECK("0x59 TOGGLE: flag (5, 37) toggled on", re15_game_flag_get(5, 37) == 1);
    }

    /* 0x59 advances 4 bytes (the length that always matched — no desync either way). */
    {
        scd_vm_init(); re15_aot_init();
        uint8_t bc[] = { 0x59, 0x00, 0x00, 0x01, OP_EVT_NEXT };
        run_op(bc);
        CHECK("0x59: PC advanced 4 (+1 sentinel)", g_scd.threads[TEST_SLOT].pc == bc + 5);
    }

    /* 0x57 fade step: ch = pc[1], step = u16 LE @pc[2..3]; stored in the model, no audio event. */
    {
        scd_vm_init(); re15_aot_init();
        unsigned audio_before = g_scd.audio_count;
        uint8_t bc[] = { 0x57, 0x00, 0x00, 0x08, OP_EVT_NEXT };   /* ch 0, step 0x0800 */
        run_op(bc);
        CHECK("0x57: fade_step[0] == 0x0800 (u16 LE, not two bytes)", g_scd.fade_step[0] == 0x0800);
        CHECK("0x57: no audio event queued", g_scd.audio_count == audio_before);
        CHECK("0x57: PC advanced 4 (+1 sentinel)", g_scd.threads[TEST_SLOT].pc == bc + 5);
    }

    if (g_fail) { printf("SCD-FLAG-FADE: FAIL\n"); return 1; }
    printf("SCD-FLAG-FADE: all checks passed\n");
    return 0;
}
