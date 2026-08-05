/* test_scd_flag_fade.c — the byte-true SCD 0x59 (indexed flag write) + the 0x56/0x57 fade-channel
 * engine (audio audit wf_1db9c802 AUDIO-OP59-NOT-XA/OP57-IS-FADE + fade trace wf_2c73ab52).
 *
 * 0x59 (@0x800744a8[0x59] -> 0x8003fe90) is NOT Xa_on: it is the WRITE sibling of the 0x58
 * flag-check — idx = work_vars[pc[2]], bank = pc[1], MSB-first bit (0x80000000 >> (idx&0x1f));
 * pc[3] 1=SET / 0=CLEAR / 7=TOGGLE, PC+=4.
 * 0x56 (@0x80042a58 -> FUN_800217b0) = fade-channel CONFIG: ch=pc[1], ABR=pc[2], rgb mask=pc[3],
 * step=s16 pc[4..5], bucket 7 (was mis-routed as "Member_calc2").
 * 0x57 (@0x80042ab4 -> FUN_800216ec) = fade KICK: the u16 is the START LEVEL — written only when
 * the configured step is 0 (static overlay, brightness = level>>7); a nonzero step IGNORES it
 * and auto-starts the ramp at 0 / 0x7fff. Tick FUN_80021880: brightness from the level BEFORE
 * integration, then level += step with u16 wrap = the stop condition (bit15 -> idle).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_fade.h"

#define TEST_SLOT 0
#define OP_EVT_NEXT 0x02

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static void run_op(const uint8_t *bc)
{
    /* the previous run's thread parks ALIVE at the 0x02 sentinel — scd_thread_start refuses an
     * occupied slot (rc=-1) and the tick would run the OLD thread into garbage. Free it first. */
    g_scd.threads[TEST_SLOT].active = 0;
    scd_thread_start(TEST_SLOT, bc);
    scd_vm_tick();
}

int main(void)
{
    printf("=== SCD 0x59 flag write + 0x57 fade step (audit wf_1db9c802) ===\n");

    /* 0x59 SET: work_vars[4] = 37 -> bank 3, idx 37 (word 1, bit 5) set. No audio event.
     * BANK 3, NICHT 5: Bank-5-Wort-1 (Bits 0x20..0x3F) ist Ein-Frame-Scratch und wird am ENDE
     * jedes VM-Laufs genullt (byte-true FUN_8003ebf4 @0x8003ec1c, gerufen vom VM-Executor
     * FUN_8003f0a0 @0x8003f18c) — dort waere nach run_op() IMMER 0 zu lesen, im Original wie im
     * Port. Der Zweck dieses Tests ist die Wort-Mathematik idx>>5 fuer idx >= 32, und die
     * prueft Bank 3 mit demselben Opcode-Pfad genauso. */
    {
        scd_vm_init(); re15_aot_init();
        g_scd.work_vars[4] = 37;
        unsigned audio_before = g_scd.audio_count;
        uint8_t bc[] = { 0x59, 0x03, 0x04, 0x01, OP_EVT_NEXT };
        run_op(bc);
        CHECK("0x59 SET: flag (3, 37) set", re15_game_flag_get(3, 37) == 1);
        CHECK("0x59 SET: no audio event queued", g_scd.audio_count == audio_before);
    }

    /* 0x59 CLEAR (op 0) + TOGGLE (op 7). */
    {
        scd_vm_init(); re15_aot_init();
        g_scd.work_vars[4] = 37;
        re15_game_flag_set(3, 37, 1);
        uint8_t bc_clear[] = { 0x59, 0x03, 0x04, 0x00, OP_EVT_NEXT };
        run_op(bc_clear);
        CHECK("0x59 CLEAR: flag (3, 37) cleared", re15_game_flag_get(3, 37) == 0);
        uint8_t bc_tgl[] = { 0x59, 0x03, 0x04, 0x07, OP_EVT_NEXT };
        run_op(bc_tgl);
        CHECK("0x59 TOGGLE: flag (3, 37) toggled on", re15_game_flag_get(3, 37) == 1);
    }

    /* 0x59 advances 4 bytes (the length that always matched — no desync either way). */
    {
        scd_vm_init(); re15_aot_init();
        uint8_t bc[] = { 0x59, 0x00, 0x00, 0x01, OP_EVT_NEXT };
        run_op(bc);
        CHECK("0x59: PC advanced 4 (+1 sentinel)", g_scd.threads[TEST_SLOT].pc == bc + 5);
    }

    /* 0x56 + 0x57: the exact ROOM11F0 sequence (@0x174a: `56 00 02 07 00 00` + `57 00 00 08`) —
     * ch0 configured subtractive white static, then the kick SETS the level (0x0800 -> the tick
     * outputs brightness 16 on all three channels). No audio event on either opcode. */
    {
        scd_vm_init(); re15_aot_init();
        unsigned audio_before = g_scd.audio_count;
        uint8_t bc[] = { 0x56, 0x00, 0x02, 0x07, 0x00, 0x00,      /* config: ch0 ABR2 mask7 step0 */
                         0x57, 0x00, 0x00, 0x08,                  /* kick: level 0x0800 */
                         OP_EVT_NEXT };
        run_op(bc);
        CHECK("0x56: ch0 ABR 2 (subtractive), masks 0xff, step 0",
              g_fade_ch[0].abr == 2 && g_fade_ch[0].mask_r == 0xFF &&
              g_fade_ch[0].mask_g == 0xFF && g_fade_ch[0].mask_b == 0xFF && g_fade_ch[0].step == 0);
        CHECK("0x57: level = 0x0800 (the u16 is the START LEVEL, static overlay)",
              g_fade_ch[0].level == 0x0800);
        re15_fade_tick();
        CHECK("tick: brightness 16 on rgb (level>>7), channel drawn",
              g_fade_ch[0].drawn && g_fade_ch[0].out_r == 16 &&
              g_fade_ch[0].out_g == 16 && g_fade_ch[0].out_b == 16);
        CHECK("static channel stays active (step 0 never terminates)", re15_fade_done(0) == 0);
        CHECK("0x56/0x57: no audio event queued", g_scd.audio_count == audio_before);
        CHECK("0x56+0x57: PC advanced 10 (+1 sentinel)", g_scd.threads[TEST_SLOT].pc == bc + 11);
    }

    /* RAMP semantics (FUN_800216ec + FUN_80021880): step +0x1800 -> level starts 0 (kick value
     * IGNORED), 6 drawn frames (brightness 0,0x30,0x60,0x90,0xc0,0xf0), then the u16 wrap past
     * 0x8000 sets bit15 = done. step -0x1800 -> starts 0x7fff (brightness 0xff first). */
    {
        scd_vm_init(); re15_aot_init();
        re15_fade_config(1, 2, 7, 0x1800, 7);
        re15_fade_kick(1, 0x1234);                                /* value ignored on the ramp path */
        CHECK("ramp up: level auto-starts at 0", g_fade_ch[1].level == 0);
        static const uint8_t exp[6] = { 0x00, 0x30, 0x60, 0x90, 0xC0, 0xF0 };
        int ok = 1;
        for (int f = 0; f < 6; f++) {
            re15_fade_tick();
            if (!g_fade_ch[1].drawn || g_fade_ch[1].out_r != exp[f]) ok = 0;
        }
        CHECK("ramp up: 6 frames 0x00..0xF0 (br = level>>7 BEFORE integration)", ok);
        CHECK("ramp up: wrap past 0x8000 -> done", re15_fade_done(1) == 1);
        re15_fade_tick();
        CHECK("done channel not drawn + level frozen", g_fade_ch[1].drawn == 0);

        re15_fade_config(2, 2, 7, (int16_t)-0x1800, 7);
        re15_fade_kick(2, 0);
        CHECK("ramp down: level auto-starts at 0x7fff", g_fade_ch[2].level == 0x7FFF);
        re15_fade_tick();
        CHECK("ramp down: first frame brightness 0xff", g_fade_ch[2].out_r == 0xFF);
        re15_fade_kill(2);
        CHECK("kill: done immediately", re15_fade_done(2) == 1);
    }

    /* LETTERBOX counter (FUN_80021a0c, trace wf_bba41002 #21): the SCD `Set(1,0x1b,1)` byte form
     * (22 01 1b 01 — 247 shipped sites) sets flag(1,27) = the direction bit; the per-frame tick
     * ramps the level ±0x10 within [0, 0xF0] (15 frames full open/close). */
    {
        scd_vm_init(); re15_aot_init();
        g_letterbox_level = 0;
        uint8_t bc_on[] = { 0x22, 0x01, 0x1b, 0x01, OP_EVT_NEXT };
        run_op(bc_on);
        CHECK("Set(1,0x1b,1): flag(1,27) set (the letterbox bit 0x10)", re15_game_flag_get(1, 27) == 1);
        for (int f = 0; f < 3; f++) re15_letterbox_tick(re15_game_flag_get(1, 27));
        CHECK("letterbox: 3 frames up -> level 0x30", g_letterbox_level == 0x30);
        for (int f = 0; f < 20; f++) re15_letterbox_tick(re15_game_flag_get(1, 27));
        CHECK("letterbox: clamps at 0xF0 (fully open)", g_letterbox_level == 0xF0);
        uint8_t bc_off[] = { 0x22, 0x01, 0x1b, 0x00, OP_EVT_NEXT };
        run_op(bc_off);
        CHECK("Set(1,0x1b,0): flag(1,27) cleared", re15_game_flag_get(1, 27) == 0);
        for (int f = 0; f < 15; f++) re15_letterbox_tick(re15_game_flag_get(1, 27));
        CHECK("letterbox: 15 frames down -> level 0 (never underflows)", g_letterbox_level == 0);
        re15_letterbox_tick(0);
        CHECK("letterbox: stays 0 when closed", g_letterbox_level == 0);
    }

    if (g_fail) { printf("SCD-FLAG-FADE: FAIL\n"); return 1; }
    printf("SCD-FLAG-FADE: all checks passed\n");
    return 0;
}
