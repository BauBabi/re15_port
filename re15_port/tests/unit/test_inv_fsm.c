/**
 * @file test_inv_fsm.c
 * @brief Waves 2+3 — the byte-true STATUS/INVENTORY screen FSM (menu_common.c).
 *
 * Drives the FSM with scripted pad edges through a full session:
 *   open (START poll -> stage FSM -> fades) -> tab nav -> ITEM mode (entry slide) ->
 *   grid nav (auto-repeat) -> command stage -> USE (swap anim) -> cancel -> exit slide ->
 *   EXIT close (fade-out + equip commit + gameplay fade-in)
 * plus the wave-3 state-5 flows (sections 17+): unequip/equip/swap per-step register
 * sequences, the 25c9 stale-gate cd=0x5c quirk, the prompt-less direct heal and the
 * cant-use message — asserted against the RE'd transitions (specs shots/inv_wave2_spec.md
 * + shots/inv_wave3_spec.md; every constant cited at its assertion).
 *
 * Frame model: one test frame = the port frame order — start_poll + fsm_tick
 * (game_step), then the render-side per-drawn-frame advances (re15_inv_screen_ecg_tick
 * = the FUN_80048a44 sweep/wipe head, exactly the main.c order) and re15_fade_tick.
 * Fades: |step| 0x1800 = exactly 6 drawn frames, done-detection the following frame
 * (FUN_80021880 math).
 */
#include <stdio.h>
#include <string.h>
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_damage.h"     /* re15_player_equipped_weapon / set */
#include "re15_actor.h"      /* player hp / status_flags (wave-3 heal) */
#include "re15_fade.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

/* one port frame: gameplay poll + FSM tick, then the render-side advances (the ECG/wipe
 * head FUN_80048a44 runs per DRAWN frame while the menu is open — main.c:2785 order;
 * the heal c4==1 wait polls the wipe self-clear, so the test must advance it too). */
static void frame(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen())
        re15_menu_fsm_tick(pressed, held);
    if (re15_menu_is_open())
        re15_inv_screen_ecg_tick();
    re15_fade_tick();
}
static void idle(int n) { while (n-- > 0) frame(0, 0); }

/* wave 3: from GRID with the cursor placed — grid confirm (25d6=0 + 25c2=3
 * @0x800c64c0-c8), command slide-in (7f -14 @0x8004a41c-454) + state 4, then the
 * state-4 confirm on USE (SE(4,6) + 25c2=5 via table [0] @0x8004a550). */
static void use_confirm(void)
{
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    idle(8);
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
}

int main(void)
{
    printf("=== status-screen FSM (wave 2, spec shots/inv_wave2_spec.md) ===\n");

    re15_fade_init();
    re15_inv_load_briefing();            /* knife(1)/browning(3,15)/bullets(0x15,50) */
    re15_inv_set_equipped_slot(0);       /* DAT_800b25c8 = 0 (knife equipped; savestate 25c8=0) */
    re15_player_set_equipped_weapon(1);  /* DAT_800aca5d = 1 */

    /* (0) closed idle */
    CHECK(!re15_menu_is_open() && !re15_menu_gameplay_frozen(), "(0) must start closed");

    /* (1) OPEN: raw START edge sets the request; hit-react gate ok -> latch + stage 1
     * (@0x8001cd64-cde8). Stage 1 arms the gameplay fade-out (FUN_800217b0(0x200,
     * +0x1800,7,0) @0x8001ca64-88) and falls into the stage-2 poll -> stage 2. */
    frame(RE15_PAD_BIT_START, RE15_PAD_BIT_START);
    CHECK(re15_menu_gameplay_frozen(), "(1) START must freeze gameplay");
    CHECK(re15_menu_stage() == 2, "(1) stage must be 2 after the stage-1 arm, is %d",
          re15_menu_stage());
    CHECK(!re15_menu_is_open(), "(1) menu task must not be alive during the fade-out");

    /* (2) fade-out = 6 drawn frames (levels 0..0x7800, rate +0x1800 @0x8001ca6c),
     * done-detection the following frame; then hold-black + FUN_80029ac8(2) 2-frame
     * yield @0x8001caa8-cad4; then the task spawn @0x8001cb34-44. Frame budget:
     * 5 more fade frames + 1 done/arm-hold + 2 hold + 1 spawn = 9. */
    idle(9);
    CHECK(re15_menu_is_open(), "(2) menu task must be alive after fade-out+hold+spawn");
    CHECK(re15_menu_phase() == 0, "(2) master phase must be 0 (init), is %d", re15_menu_phase());

    /* (3) phase 0 = init + menu fade-in (LAB_80049524 @0x800496c4-4972c, -0x1800 =
     * 6 drawn frames) -> phase 1 (25bf++ @0x800464d4-e8). Init state (FUN_800460b8):
     * 25bc/bd/be=0, 25ca=0, 25ea=0xa6, 25ee=0x108. */
    idle(8);
    CHECK(re15_menu_phase() == 1, "(3) master phase must be 1 (run), is %d", re15_menu_phase());
    CHECK(re15_menu_substate() == 0, "(3) sub-state must be 0 (tab select)");
    CHECK(g_inv_screen.tab == 0 && g_inv_screen.item_cursor == 0 &&
          g_inv_screen.highlight == 0, "(3) init: tab=0 cursor=0 dim=0 (@0x800463e0-f8)");
    CHECK(g_inv_screen.tab_base_y == 0xa6 && g_inv_screen.act_base_y == 0x108,
          "(3) init: 25ea=0xa6 25ee=0x108 (@0x800495b0/c8), are %d/%d",
          g_inv_screen.tab_base_y, g_inv_screen.act_base_y);

    /* (4) TAB NAV is ABSOLUTE (no wrap/cycle; @0x80049850-b8): Right->3 FILE,
     * Left->1 MAP, Down->2 EXIT, Up->0 ITEM; L1->MAP launch, R1->FILE launch. */
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);
    CHECK(g_inv_screen.tab == 3, "(4) Right -> FILE(3) (@0x80049850-64), is %d", g_inv_screen.tab);
    frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.tab == 2, "(4) Down -> EXIT(2) (@0x80049888-9c), is %d", g_inv_screen.tab);
    frame(RE15_PAD_BIT_LEFT, RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.tab == 1, "(4) Left -> MAP(1) (@0x8004986c-80), is %d", g_inv_screen.tab);
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);
    CHECK(g_inv_screen.tab == 0, "(4) Up -> ITEM(0) (@0x800498a4-b8), is %d", g_inv_screen.tab);

    /* (5) L1 = instant MAP launch (tab=1 AND 25c1=1, @0x8004980c-30); the wave-3 MAP
     * stub returns to tab-select next frame per the exit contract @0x8004c2c4-304
     * (25ca=0, 25c1=0), tab KEPT. */
    frame(RE15_PAD_BIT_L1, RE15_PAD_BIT_L1);
    CHECK(g_inv_screen.tab == 1 && re15_menu_substate() == 1, "(5) L1 -> tab=1 + 25c1=1");
    frame(0, 0);
    CHECK(re15_menu_substate() == 0 && g_inv_screen.tab == 1,
          "(5) MAP stub must return to tab-select with tab kept");

    /* (6) confirm ITEM: Up -> tab 0; virtual CROSS -> 25ca=1 (@0x800498e4-f4, the ONE
     * set-to-1 site), cursors reset (@0x800498f8-918), 25c1=3 (@0x80049960-64),
     * ITEM state 0 = entry slide. */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(re15_menu_substate() == 3, "(6) CROSS on ITEM -> 25c1=3, is %d", re15_menu_substate());
    CHECK(g_inv_screen.highlight == 1, "(6) 25ca must be 1 (dim flag)");
    CHECK(g_inv_screen.item_state == 0, "(6) 25c2 must be 0 (entry slide)");

    /* (7) entry slide (@0x8004a394-3c0): 25ea +14/frame while <251 — exactly 7 steps
     * 166->264 (166+14*7), then 25c2:=1 on the 8th frame. */
    {
        int i, expect = 0xa6;
        for (i = 0; i < 7; i++) {
            frame(0, 0);
            expect += 14;
            CHECK(g_inv_screen.tab_base_y == expect,
                  "(7) slide frame %d: 25ea must be %d (+14/frame), is %d",
                  i, expect, g_inv_screen.tab_base_y);
        }
        CHECK(g_inv_screen.tab_base_y == 264, "(7) slide must land exactly 264");
        frame(0, 0);
        CHECK(g_inv_screen.item_state == 1, "(7) 25c2 must be 1 (GRID) after the slide");
    }

    /* (8) GRID: name print queued per frame (FUN_80028c1c(0x18,0xa8,1,id)
     * @0x800c65a8-d8) — cursor 0 = knife id 1. */
    frame(0, 0);
    CHECK(g_inv_screen.name_item == 0x01, "(8) grid name print id must be 1 (knife), is %d",
          g_inv_screen.name_item);
    CHECK(g_inv_screen.second_cursor == g_inv_screen.item_cursor,
          "(8) 25be := 25bd mirror (@0x800c65dc-ec)");

    /* (9) grid nav (DEBUG.BIN 0x800c62a0; HELD word gated by the auto-repeat tick
     * FUN_80030444, config (mask,15,4) @0x800460cc): press edge moves immediately;
     * held repeats first after 16 frames, then every 5. RIGHT only from even cells;
     * DOWN +2 except from cap-2/cap-1 (cap=10 @0x800c63e0); UP -2 except <2; no wrap. */
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);          /* edge: 0 -> 1 */
    CHECK(g_inv_screen.item_cursor == 1, "(9) RIGHT edge 0->1, is %d", g_inv_screen.item_cursor);
    CHECK(g_inv_screen.name_item == 0x03, "(9) name follows cursor: Browning id 3");
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);          /* edge from odd: rejected */
    CHECK(g_inv_screen.item_cursor == 1, "(9) RIGHT from odd cell must be rejected");
    frame(RE15_PAD_BIT_LEFT, RE15_PAD_BIT_LEFT);            /* edge: 1 -> 0 */
    CHECK(g_inv_screen.item_cursor == 0, "(9) LEFT edge 1->0");
    frame(RE15_PAD_BIT_LEFT, RE15_PAD_BIT_LEFT);            /* LEFT from even: patched-dead
                                                             * 0xA jump @0x800c637c -> no-op */
    CHECK(g_inv_screen.item_cursor == 0, "(9) LEFT from even cell = no-op (equip cell "
          "route patched dead @0x800c637c = 0x1000003b)");
    {   /* auto-repeat: DOWN edge then hold — moves at frames 0, 16, 21, 26 (delay 15 /
         * rate 4, test-then-decrement @0x80030570-628), then cap-1 rejects. */
        int f;
        frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN);        /* f0 edge: 0 -> 2 */
        CHECK(g_inv_screen.item_cursor == 2, "(9r) DOWN edge 0->2");
        for (f = 1; f <= 15; f++) frame(0, RE15_PAD_BIT_DOWN);
        CHECK(g_inv_screen.item_cursor == 2, "(9r) held DOWN must not repeat before f16, is %d",
              g_inv_screen.item_cursor);
        frame(0, RE15_PAD_BIT_DOWN);                        /* f16: first repeat 2 -> 4 */
        CHECK(g_inv_screen.item_cursor == 4, "(9r) first repeat at f16 (delay 15), is %d",
              g_inv_screen.item_cursor);
        for (f = 0; f < 4; f++) frame(0, RE15_PAD_BIT_DOWN);
        CHECK(g_inv_screen.item_cursor == 4, "(9r) no repeat before +5");
        frame(0, RE15_PAD_BIT_DOWN);                        /* f21: 4 -> 6 */
        CHECK(g_inv_screen.item_cursor == 6, "(9r) repeat every 5 (rate 4), is %d",
              g_inv_screen.item_cursor);
        idle(1);                                            /* release */
        frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN);        /* 6 -> 8 == cap-2 */
        CHECK(g_inv_screen.item_cursor == 8, "(9r) DOWN 6->8");
        frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN);        /* cap-2: rejected @0x800c63e8 */
        CHECK(g_inv_screen.item_cursor == 8, "(9r) DOWN from cap-2 (8) must be rejected");
        while (g_inv_screen.item_cursor > 0)                /* UP back to 0 (edge presses) */
            { frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP); idle(1); }
        frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);            /* <2: rejected @0x800c6438 */
        CHECK(g_inv_screen.item_cursor == 0, "(9r) UP from 0 must be rejected");
    }

    /* (10) grid confirm on an EMPTY cell: nothing (id==0 check @0x800c64b4). */
    frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN); idle(1);   /* 0 -> 2 */
    frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN); idle(1);   /* 2 -> 4 (empty) */
    CHECK(g_inv_screen.item_cursor == 4, "(10) cursor on empty cell 4");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 1, "(10) confirm on empty must stay in GRID");

    /* (11) grid confirm on the Browning (slot 1): 25d6:=0 + 25c2:=3 (@0x800c64c0-c8),
     * command-cluster slide-in 25ee 264->166 (-14/frame, @0x8004a41c-454) -> state 4. */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP); idle(1);       /* 4 -> 2 */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP); idle(1);       /* 2 -> 0 */
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);          /* 0 -> 1 (Browning) */
    CHECK(g_inv_screen.item_cursor == 1, "(11) cursor on Browning");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 3 && g_inv_screen.action_dir == 0,
          "(11) confirm -> 25c2=3 + 25d6=0, are %d/%d",
          g_inv_screen.item_state, g_inv_screen.action_dir);
    {
        int i, expect = 264;
        for (i = 0; i < 7; i++) {
            frame(0, 0);
            expect -= 14;
            CHECK(g_inv_screen.act_base_y == expect,
                  "(11) cmd slide frame %d: 25ee must be %d (-14/frame), is %d",
                  i, expect, g_inv_screen.act_base_y);
        }
        CHECK(g_inv_screen.act_base_y == 166, "(11) cmd slide must land exactly 166");
        frame(0, 0);
        CHECK(g_inv_screen.item_state == 4, "(11) 25c2 must be 4 (command select)");
    }

    /* (12) command nav (@0x8004a458-4fc): Left->1 CHECK; confirm dispatch [1]
     * (@0x8004a558-64): 25d6:=0 + 25c2:=9 = the WAVE-4 examine FSM FUN_800c6630
     * (DEBUG.BIN, sub-state = 25d6, table @0x800c6864). Cursor = Browning (id 3,
     * desc entry 3 = the 2-PAGE description — exercises the page-wait). */
    frame(RE15_PAD_BIT_LEFT, RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.action_dir == 1, "(12) cmd Left -> 25d6=1");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 9, "(12) confirm CHECK -> 25c2=9");
    CHECK(g_inv_screen.action_dir == 0, "(12) dispatch [1] pre-writes 25d6=0 (@0x8004a55c)");

    /* (12a) sub-state 0 @0x800c6664-66bc: 7 frames — panels 25d8/25dc/25e4/25f0 -=36
     * (@0x800c6688-66b4) while 25ee +=14 (@0x800c6684/66bc; slti 251 @0x800c667c). */
    {
        int k;
        for (k = 1; k <= 7; k++) {
            frame(0, 0);
            CHECK(g_inv_screen.act_base_y == 166 + 14 * k &&
                  g_inv_screen.arms_x   == 126 - 36 * k &&
                  g_inv_screen.equip_x  == 0x96 - 36 * k &&
                  g_inv_screen.cond_x   == 13 - 36 * k &&
                  g_inv_screen.idcard_x == 14 - 36 * k,
                  "(12a) slide f%d: 25ee=%d 25d8=%d 25dc=%d 25e4=%d 25f0=%d",
                  k, g_inv_screen.act_base_y, g_inv_screen.arms_x, g_inv_screen.equip_x,
                  g_inv_screen.cond_x, g_inv_screen.idcard_x);
            CHECK(g_inv_screen.exam_visible == 0, "(12a) photo not yet visible");
        }
    }
    /* (12b) slide done (25ee=264): photo load frame @0x800c66c0-6700 — id = helper
     * 0x800c6600 (Browning = 3), struct @0x800c6220 := {1,-207,26}, 25d6:=1. */
    frame(0, 0);
    CHECK(g_inv_screen.exam_visible == 1 && g_inv_screen.exam_x == -207 &&
          g_inv_screen.exam_y == 26 && g_inv_screen.exam_item == 3 &&
          g_inv_screen.action_dir == 1,
          "(12b) photo init {1,-207,26} + id 3 + 25d6=1 (@0x800c66d0-66f8), x=%d d6=%d",
          g_inv_screen.exam_x, g_inv_screen.action_dir);

    /* (12c) sub-state 1 @0x800c6704-675c: x += 22/frame (delay-slot store @0x800c671c);
     * 10 frames -207 -> +13; the x==13 frame ALSO opens the desc msg (@0x800c6728-3c:
     * FUN_80027e68(0x00a00013, 0x8400, id, 0xff000000)) and sets 25d6=2. */
    {
        int k;
        for (k = 1; k <= 9; k++) {
            frame(0, 0);
            CHECK(g_inv_screen.exam_x == -207 + 22 * k,
                  "(12c) slide-in f%d: x=%d != %d", k, g_inv_screen.exam_x, -207 + 22 * k);
            CHECK(!re15_menu_msg_active(), "(12c) msg not yet open at f%d", k);
        }
        frame(0, 0);
        CHECK(g_inv_screen.exam_x == 13 && g_inv_screen.action_dir == 2,
              "(12c) x lands 13 (ori 0xd @0x800c6710) + 25d6=2, x=%d", g_inv_screen.exam_x);
        CHECK(re15_menu_msg_active() && g_inv_screen.msg_entry == 3 &&
              g_inv_screen.msg_x == 0x13 && g_inv_screen.msg_y == 0xa0,
              "(12c) desc msg open: entry 3 at (0x13,0xa0) (@0x800c6728-3c)");
    }
    /* (12d) typewriter page 1 (VM FUN_80028134 case 1, menu pacing 8525=2/8524=4):
     * the open frame's own VM tick took 8525 2->1 (the flush runs the VM the same
     * frame), the 2nd tick reveals glyph 1, then 4 frames/glyph; entry 3 page 1 =
     * 27 tick-codes ("Manufactured by FN, / Belgium." — 0x08 newline is free). */
    CHECK(g_inv_screen.msg_reveal == 0, "(12d) no glyph on the open frame");
    frame(0, 0);
    CHECK(g_inv_screen.msg_reveal == 1, "(12d) first glyph on the 2nd VM tick, reveal=%d",
          g_inv_screen.msg_reveal);
    { int n = 0; while (g_inv_screen.msg_reveal < 27 && n++ < 200) frame(0, 0); }
    CHECK(g_inv_screen.msg_reveal == 27,
          "(12d) page 1 = 27 tick-codes (bank entry 3), reveal=%d", g_inv_screen.msg_reveal);
    /* 4 frames later the advance hits the 02 00 control -> VM state 2 PAGE-WAIT;
     * the arrow blinks from the NEXT wait frame ((8525&0x30)!=0, count 0-1=0xff). */
    idle(4);
    CHECK(re15_menu_msg_active() && g_inv_screen.msg_reveal == 27,
          "(12d) page-wait holds the full page 1");
    CHECK(g_inv_screen.msg_arrow == 0, "(12d) arrow not yet on the entry frame");
    frame(0, 0);
    CHECK(g_inv_screen.msg_arrow == 1, "(12d) arrow on (count 0xff & 0x30)");
    idle(47);                                   /* counts 0xfe..0xd0 stay visible */
    CHECK(g_inv_screen.msg_arrow == 1, "(12d) arrow still on at count 0xd0");
    frame(0, 0);                                /* count 0xcf -> bits 4-5 clear */
    CHECK(g_inv_screen.msg_arrow == 0, "(12d) arrow blinks off at count 0xcf");
    /* (12e) page-wait confirm (edge & 0xc000): display start := past the 02 00
     * (= computed from the SHIPPED bank stream, not hand-typed), reveal reset,
     * next glyph after 2 frames (8525 := 1<<1). Page 2 = 29 tick-codes. */
    {
        const uint8_t *s3 = re15_inv_desc_entry(3);
        int off = 0;
        while (s3[off] != 0x02) off++;
        off += 2;
        frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
        CHECK(g_inv_screen.msg_page_off == off && g_inv_screen.msg_reveal == 0,
              "(12e) page restart: 8528 := past 02 00 (off %d, is %d)",
              off, g_inv_screen.msg_page_off);
        frame(0, 0); frame(0, 0);
        CHECK(g_inv_screen.msg_reveal == 1, "(12e) page-2 first glyph after 2f");
        { int n = 0; while (g_inv_screen.msg_reveal < 29 && n++ < 200) frame(0, 0); }
        CHECK(g_inv_screen.msg_reveal == 29, "(12e) page 2 = 29 tick-codes, reveal=%d",
              g_inv_screen.msg_reveal);
        idle(4);                                /* advance hits 01 00 -> state 5 */
        CHECK(re15_menu_msg_active(), "(12e) PRESS-WAIT holds (no auto-dismiss)");
    }
    /* (12f) dismiss (state 5, edge & 0xc000 -> 8520&=0x7f); the CHECK FSM's msg-gone
     * poll fires SE(4,5) + 25d6=3 the NEXT frame (@0x800c6760-67b0); then 10 frames
     * x -= 22 back to -207 (@0x800c67b8-67e8: photo cleared + 25d6=4). */
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(!re15_menu_msg_active(), "(12f) dismissed on the confirm edge");
    CHECK(g_inv_screen.action_dir == 2, "(12f) FSM still in sub-state 2 this frame");
    frame(0, 0);
    CHECK(g_inv_screen.action_dir == 3, "(12f) msg-gone poll -> 25d6=3 (@0x800c67a4-b0)");
    {
        int k;
        for (k = 1; k <= 9; k++) {
            frame(0, 0);
            CHECK(g_inv_screen.exam_x == 13 - 22 * k,
                  "(12f) spin-out f%d: x=%d", k, g_inv_screen.exam_x);
            CHECK(g_inv_screen.exam_visible == 1, "(12f) photo visible until -207");
        }
        frame(0, 0);
        CHECK(g_inv_screen.exam_x == -207 && g_inv_screen.exam_visible == 0 &&
              g_inv_screen.action_dir == 4,
              "(12f) x=-207 -> visible=0 + 25d6=4 (@0x800c67d4-e0)");
    }
    /* (12g) sub-state 4 @0x800c67ec-6850: 7 frames panels += 36 / 25ee -= 14; the
     * 25ee==166 frame exits with 25c2=4 + 25d6=1 (@0x800c683c-50). */
    {
        int k;
        for (k = 1; k <= 7; k++) {
            frame(0, 0);
            CHECK(g_inv_screen.act_base_y == 264 - 14 * k &&
                  g_inv_screen.arms_x   == -126 + 36 * k &&
                  g_inv_screen.equip_x  == 0x96 - 252 + 36 * k &&
                  g_inv_screen.cond_x   == -239 + 36 * k &&
                  g_inv_screen.idcard_x == -238 + 36 * k,
                  "(12g) slide-back f%d: 25ee=%d 25d8=%d", k,
                  g_inv_screen.act_base_y, g_inv_screen.arms_x);
            CHECK(g_inv_screen.item_state == 9, "(12g) still in CHECK during the slide");
        }
        frame(0, 0);
        CHECK(g_inv_screen.item_state == 4 && g_inv_screen.action_dir == 1,
              "(12g) exit contract 25c2=4 + 25d6=1 (@0x800c6844/6850)");
        CHECK(g_inv_screen.arms_x == 126 && g_inv_screen.equip_x == 0x96 &&
              g_inv_screen.cond_x == 13 && g_inv_screen.idcard_x == 14,
              "(12g) panel registers restored exactly (are %d/%d/%d/%d)",
              g_inv_screen.arms_x, g_inv_screen.equip_x, g_inv_screen.cond_x,
              g_inv_screen.idcard_x);
    }
    /* (12h) desc bank id-keying (gen/inv_desc_bank.inc == DEBUG.BIN @0x800c50de):
     * entry 0 == the wave-3 cant-use bytes; knife 0x01 = "A combat knife." ('A'=0x1d
     * @0x800c5186); Green 0x24 = "Medicine derived..." ('M'=0x29 @0x800c5908). */
    {
        const uint8_t *s0 = re15_inv_desc_entry(0);
        const uint8_t *s1 = re15_inv_desc_entry(1);
        const uint8_t *sg = re15_inv_desc_entry(0x24);
        int i, ok = 1;
        for (i = 0; i < RE15_INV_CANTUSE_GLYPHS; i++)
            if (s0[i] != re15_inv_cantuse_text[i]) ok = 0;
        CHECK(ok && s0[RE15_INV_CANTUSE_GLYPHS] == 0x01,
              "(12h) bank entry 0 == cant-use text + 01-terminator");
        CHECK(s1[0] == 0x1d && s1[1] == 0x00 && s1[2] == 0x3f,
              "(12h) knife desc starts 'A c' (0x1d 0x00 0x3f @0x800c5186)");
        CHECK(sg[0] == 0x29 && sg[1] == 0x41 && sg[2] == 0x40,
              "(12h) Green desc starts 'Med' (0x29 0x41 0x40 @0x800c5908)");
        CHECK(re15_inv_desc_entry(0x47) != 0 && re15_inv_desc_entry(0x48) == 0,
              "(12h) bank spans exactly ids 0x00..0x47 (72 entries @0x800c50de)");
    }

    /* (13) USE on the Browning = the byte-true SWAP anim (wave 3, state 5 @0x8004a674 ->
     * FUN_8004aa24): knife equipped (slot 0, id 1) != Browning (id 3) and 25c8 != 0x80
     * -> classifier c3=5 SWAP + cd=0x3e + c4=0 (@0x8004ab2c-3c + @0x8004aaa8). Commit at
     * anim STEP 0 (@0x8004b0c4: 25c8:=cursor, 25c9:=old, kind0 -> 25dc/25de = 0xac/0x3a
     * @0x8004b108-11c), 11 ticks animate 25dc -2/frame (0xac->0x96 @0x8004b1b4-c4), cd
     * stays 0x3e (the 25c9 gate @0x8004b1ec is dead — step 0 wrote 25c9=0), 2608 is
     * NEVER touched (no gun slide in a swap); terminal at c4==11 (@0x8004b234-44). */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);                /* 25d6 -> 0 USE */
    CHECK(g_inv_screen.action_dir == 0, "(13) cmd Up -> 25d6=0");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 5, "(13) confirm USE -> 25c2=5");
    frame(0, 0);                                            /* frame 1: classifier */
    CHECK(re15_menu_item_c3() == 5 && re15_menu_item_c4() == 0,
          "(13) classifier -> c3=5 SWAP + c4=0 (@0x8004ab3c/@0x8004aaa8), c3=%d",
          re15_menu_item_c3());
    CHECK(g_inv_screen.arms_rgb == 0x3e, "(13) cd primed 0x3e (@0x8004ab2c)");
    CHECK(re15_inv_equipped_slot() == 0, "(13) NO commit at confirm/classify (25c8 still 0)");
    frame(0, 0);                                            /* swap step 0: commit + tick */
    CHECK(re15_inv_equipped_slot() == 1, "(13) step-0 commit: 25c8 := cursor 1 (@0x8004b0d8)");
    CHECK(re15_inv_prev_equip_slot() == 0, "(13) 25c9 := old slot 0 (@0x8004b0e8)");
    CHECK(g_inv_screen.equip_x == 0xac - 2,
          "(13) step 0: 25dc = 0xac (commit) - 2 (tick), is 0x%02x", g_inv_screen.equip_x);
    CHECK(re15_player_equipped_weapon() == 1,
          "(13) the PLAYER weapon must NOT change in-menu (commit is at close @0x80046688)");
    {
        int k;
        for (k = 1; k <= 10; k++) {                         /* ticks 1..10 */
            frame(0, 0);
            CHECK(g_inv_screen.equip_x == 0xac - 2 * (k + 1),
                  "(13) tick %d: 25dc -2/frame (@0x8004b1c4), is 0x%02x",
                  k, g_inv_screen.equip_x);
            CHECK(g_inv_screen.arms_rgb == 0x3e && g_inv_screen.arms_slide == 0,
                  "(13) tick %d: cd stays 0x3e (dead gate) + 2608 untouched", k);
        }
    }
    CHECK(g_inv_screen.equip_x == 0x96 && g_inv_screen.equip_y == 0x3a,
          "(13) swap lands exactly on the kind0 rest pair (0x96,0x3a), are 0x%02x/0x%02x",
          g_inv_screen.equip_x, g_inv_screen.equip_y);
    frame(0, 0);                                            /* c4==11 terminal */
    CHECK(g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(13) terminal invariant: 25c2=6 + c3=0 + c4=0 (@0x8004b234-44)");
    idle(8);                                                /* 25ee 166->264 (+14 x7) + state 1 */
    CHECK(g_inv_screen.item_state == 1 && g_inv_screen.act_base_y == 264,
          "(13) slide-out must return to GRID with 25ee=264, are %d/%d",
          g_inv_screen.item_state, g_inv_screen.act_base_y);

    /* (14) grid cancel (virtual SQUARE @0x800c6538-58): 25c2=2 exit slide 264->166,
     * then 25ca=0 + 25c1=0 (@0x8004a3d4-418) — back to tab select, tab kept (0). */
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 2, "(14) cancel -> 25c2=2");
    idle(8);
    CHECK(re15_menu_substate() == 0 && g_inv_screen.highlight == 0 &&
          g_inv_screen.tab_base_y == 0xa6,
          "(14) exit slide -> tab select, 25ca=0, 25ea=0xa6 (are %d/%d/%d)",
          re15_menu_substate(), g_inv_screen.highlight, g_inv_screen.tab_base_y);
    CHECK(g_inv_screen.tab == 0, "(14) tab byte keeps its value (ITEM)");

    /* (15) EXIT close: Down -> tab 2; confirm -> 25ca=0 + 25bf++ (@0x800499b4-c4).
     * Close phase (@0x80046540): 6-frame fade-out (screen keeps drawing = stays open),
     * equip CHANGED (snapshot knife id 1 vs Browning id 3 @0x800465f4-661c) -> 2-frame
     * hold-black + commit DAT_800aca5d=3 (@0x80046688) -> teardown + task kill; then
     * gameplay fade-in stages 3-5 (@0x8001cbb8-cc94) -> stage 0. */
    frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.tab == 2, "(15) Down -> EXIT tab");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(re15_menu_phase() == 2, "(15) EXIT confirm -> phase 2 (close)");
    CHECK(re15_menu_is_open(), "(15) the screen keeps drawing during the close fade");
    idle(12);                                               /* arm+6 fade + done + 2 hold + commit */
    CHECK(!re15_menu_is_open(), "(15) menu task must be dead after the close");
    CHECK(re15_player_equipped_weapon() == 3,
          "(15) close-phase equip commit: DAT_800aca5d must be 3 (Browning), is %d",
          re15_player_equipped_weapon());
    CHECK(re15_menu_gameplay_frozen(), "(15) gameplay still frozen during the fade-in");
    idle(10);                                               /* stages 4/5 + 6 fade frames */
    CHECK(re15_menu_stage() == 0 && !re15_menu_gameplay_frozen(),
          "(15) stage must return to 0 (normal gameplay), is %d", re15_menu_stage());

    /* (16) grid START = immediate close (raw 0x800 @0x800c6560-98: SE(4,5) + 25bf++,
     * NO slide-back), and the sticky request re-opens after a blocked gate. */
    frame(RE15_PAD_BIT_START, 0);                           /* re-open */
    idle(40);
    CHECK(re15_menu_is_open() && re15_menu_phase() == 1, "(16) re-open reaches the run phase");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);          /* ITEM */
    idle(8);                                                /* entry slide */
    CHECK(g_inv_screen.item_state == 1, "(16) in GRID");
    frame(RE15_PAD_BIT_START, RE15_PAD_BIT_START);
    CHECK(re15_menu_phase() == 2, "(16) grid START -> immediate close phase (25bf++)");
    idle(30);
    CHECK(!re15_menu_is_open() && re15_menu_stage() == 0, "(16) closed + settled");

    /* ====================== WAVE 3 (spec shots/inv_wave3_spec.md) ====================== */
    /* Fresh byte-true game-start state: the briefing loadout also runs the equip-history
     * init 25c9:=0x80 (@0x80045fe0). Debug-open (fades skipped) -> ITEM -> GRID. */
    re15_inv_load_briefing();
    re15_inv_set_equipped_slot(0);
    re15_player_set_equipped_weapon(1);
    re15_menu_toggle();
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);          /* tab ITEM confirm */
    idle(8);                                                /* entry slide -> GRID */
    CHECK(g_inv_screen.item_state == 1, "(17) reopen reaches GRID");
    CHECK(re15_inv_prev_equip_slot() == 0x80,
          "(17) game-start 25c9 == 0x80 (@0x80045fe0; savestate mzd_inv_open.sav)");

    /* (17) UNEQUIP via USE on the EQUIPPED weapon — the classifier's ID compare
     * (inventory[equip].id == inventory[cursor].id @0x8004aadc, NOT slot equality) ->
     * c3=2 + cd=0x3e + c4=0 (@0x8004aae4-aaf8/@0x8004aaa8). 12-step table @0x80010f54:
     * 11 UNCONDITIONAL ticks (cd+6 @0x8004ad64 / 2608-5 @0x8004ad88 / 25de-1
     * @0x8004ad8c), 25c8:=0x80 at step 5 (@0x8004ad44), terminal at step 11
     * (@0x8004ada8-adc0). Net: cd 0x3e->0x80, 2608 0->-55 = the nothing-equipped rest. */
    use_confirm();                                          /* cursor 0 = the knife */
    CHECK(g_inv_screen.item_state == 5, "(17) USE confirm -> 25c2=5");
    frame(0, 0);                                            /* frame 1: classifier */
    CHECK(re15_menu_item_c3() == 2 && re15_menu_item_c4() == 0,
          "(17) classifier ID-compare -> c3=2 UNEQUIP (@0x8004aadc-aaec), c3=%d",
          re15_menu_item_c3());
    CHECK(g_inv_screen.arms_rgb == 0x3e, "(17) cd primed 0x3e (@0x8004aaf8)");
    {
        int k;
        for (k = 0; k <= 10; k++) {                         /* ticks 0..10 */
            frame(0, 0);
            CHECK(g_inv_screen.arms_rgb == 0x3e + 6 * (k + 1),
                  "(17) tick %d: cd +6/frame (@0x8004ad64), is 0x%02x",
                  k, g_inv_screen.arms_rgb);
            CHECK(g_inv_screen.arms_slide == -5 * (k + 1),
                  "(17) tick %d: 2608 -5/frame (@0x8004ad88), is %d",
                  k, g_inv_screen.arms_slide);
            CHECK(g_inv_screen.equip_y == 0x3a - (k + 1),
                  "(17) tick %d: 25de -1/frame (@0x8004ad8c), is 0x%02x",
                  k, g_inv_screen.equip_y);
            if (k < 5)
                CHECK(re15_inv_equipped_slot() == 0,
                      "(17) tick %d: 25c8 unchanged before step 5", k);
            else
                CHECK(re15_inv_equipped_slot() == 0x80,
                      "(17) tick %d: 25c8 == 0x80 from step 5 (@0x8004ad44)", k);
        }
    }
    CHECK(g_inv_screen.arms_rgb == 0x80 && g_inv_screen.arms_slide == -55,
          "(17) unequip lands cd=0x80 / 2608=-55 (rest @0x800495f8-9604)");
    frame(0, 0);                                            /* terminal */
    CHECK(g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(17) terminal invariant: 25c2=6, c3=0, c4=0 (@0x8004ada8-adc0)");
    idle(8);
    CHECK(g_inv_screen.item_state == 1, "(17) back in GRID");

    /* (18) EQUIP from empty — classifier: the equipped-id compare first reads the
     * OUT-of-inventory byte inv[0x80*4] @0x800b12ac (live 0x00, measured — never a
     * weapon id) then 25c8==0x80 (delay-slot ori 0x80 @0x8004aae0) -> c3=1 + cd=0x80
     * (@0x8004ab04-20). Table @0x80010f24: commit at step 5 (25c8:=cursor, 25c9:=0x80,
     * kind0 ammo pair (0x96,0x35) @0x8004ac04-14); ticks: cd -6 gated on 25c9==0x80
     * (@0x8004ac88-aca4 — fires all 11 here), 2608 +5, 25de +1. Terminal 25de = 0x35+6
     * = 0x3b — the byte-true off-by-one vs the reopen rest 0x3a (persists until the
     * next screen open; do NOT converge it). */
    CHECK(re15_inv_prev_equip_slot() == 0x80,
          "(18) 25c9 STILL 0x80 (unequip has no 25c9 writer — xref list)");
    use_confirm();                                          /* cursor 0 = the knife */
    frame(0, 0);                                            /* classifier */
    CHECK(re15_menu_item_c3() == 1 && re15_menu_item_c4() == 0,
          "(18) classifier -> c3=1 EQUIP (@0x8004ab20), c3=%d", re15_menu_item_c3());
    CHECK(g_inv_screen.arms_rgb == 0x80, "(18) cd primed 0x80 (@0x8004ab14)");
    {
        int k;
        for (k = 0; k <= 10; k++) {                         /* ticks 0..10 */
            frame(0, 0);
            CHECK(g_inv_screen.arms_rgb == (uint8_t)(0x80 - 6 * (k + 1)),
                  "(18) tick %d: cd -6 (25c9==0x80 gate @0x8004ac88-aca4), is 0x%02x",
                  k, g_inv_screen.arms_rgb);
            CHECK(g_inv_screen.arms_slide == -55 + 5 * (k + 1),
                  "(18) tick %d: 2608 +5/frame (@0x8004acc4), is %d",
                  k, g_inv_screen.arms_slide);
        }
    }
    CHECK(g_inv_screen.arms_rgb == 0x3e && g_inv_screen.arms_slide == 0,
          "(18) equip lands cd=0x3e / 2608=0 (the equipped rest), are 0x%02x/%d",
          g_inv_screen.arms_rgb, g_inv_screen.arms_slide);
    CHECK(g_inv_screen.equip_y == 0x3b,
          "(18) TERMINAL 25de = 0x35+6 = 0x3b QUIRK (vs reopen rest 0x3a), is 0x%02x",
          g_inv_screen.equip_y);
    CHECK(re15_inv_equipped_slot() == 0 && re15_inv_prev_equip_slot() == 0x80,
          "(18) step-5 commit: 25c8=cursor 0 (@0x8004abd0), 25c9=old 0x80 (@0x8004abe0)");
    frame(0, 0);
    CHECK(g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(18) terminal invariant (@0x8004acec-ad04)");
    idle(8);

    /* (19) THE 25c9 STALE-GATE QUIRK (spec fact "25c9 quirk": writers are ONLY the
     * game-start init/equip-step5/swap-step0): swap (25c9:=0) -> unequip (25c9
     * untouched) -> equip: ticks 0-4 SKIP the cd fade (25c9==0 != 0x80), the step-5
     * commit rewrites 25c9:=0x80, ticks 5-10 fade -> cd TERMINALS AT 0x5c (0x80-36),
     * not 0x3e. Byte-derived (@0x8004ac88 gate); the port must reproduce, not fix. */
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);          /* cursor 0 -> 1 (Browning) */
    CHECK(g_inv_screen.item_cursor == 1, "(19) cursor on the Browning");
    use_confirm();
    idle(14);                                               /* classify + 11 ticks + terminal + 1 */
    CHECK(re15_inv_equipped_slot() == 1 && re15_inv_prev_equip_slot() == 0,
          "(19) swap done: 25c8=1, 25c9=0 (@0x8004b0d8/@0x8004b0e8)");
    idle(8);
    CHECK(g_inv_screen.item_state == 1, "(19) grid after the swap");
    use_confirm();                                          /* USE on the equipped Browning */
    idle(14);                                               /* the 12-step unequip anim */
    CHECK(re15_inv_equipped_slot() == 0x80 && re15_inv_prev_equip_slot() == 0,
          "(19) unequip done: 25c8=0x80, 25c9 STAYS 0 (no unequip writer)");
    idle(8);
    use_confirm();                                          /* re-equip the Browning */
    frame(0, 0);                                            /* classifier: c3=1, cd=0x80 */
    CHECK(re15_menu_item_c3() == 1 && g_inv_screen.arms_rgb == 0x80,
          "(19) equip classifier primed (c3=1, cd=0x80)");
    idle(5);                                                /* ticks 0-4 */
    CHECK(g_inv_screen.arms_rgb == 0x80,
          "(19) ticks 0-4 SKIP the fade (stale 25c9==0 @0x8004ac88), cd=0x%02x",
          g_inv_screen.arms_rgb);
    idle(6);                                                /* ticks 5-10 (step-5 commit) */
    CHECK(g_inv_screen.arms_rgb == 0x5c,
          "(19) THE QUIRK: cd terminals at 0x5c (only 6 fades: 0x80-36), is 0x%02x",
          g_inv_screen.arms_rgb);
    CHECK(re15_inv_prev_equip_slot() == 0x80, "(19) step-5 rewrote 25c9=0x80 (@0x8004abe0)");
    frame(0, 0);
    CHECK(g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(19) terminal invariant");
    idle(8);
    CHECK(g_inv_screen.item_state == 1, "(19) grid");

    /* (20) HEAL is DIRECT — NO prompt, NO message (@0x8004adcc-0x8004b070 contains no
     * jal 0x80027e68 and no pad reads): c3=3 (@0x8004ab60, c4 via the terminal
     * invariant) -> c4=0 wipe-arm (table @0x80010f84: Green 0x24 -> 25d4=1 @0x8004ae84)
     * -> c4=1 wait for the renderer self-clear (32 frames mode 1) -> c4=2 consume
     * (@0x8004aee4-af28 + compaction) + apply (@0x80010fbc[2] = +25, srl v0,s0,2
     * @0x8004af78) + ECG sweep reset (@0x8004b038) -> 25c2=6. Wipe BEFORE consume. */
    g_inv.slots[3].id = 0x24; g_inv.slots[3].qty = 1;       /* seed a Green Medicine */
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 40;
    g_actors[RE15_ACTOR_SLOT_PLAYER].status_flags = 0;
    frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN); idle(1);   /* cursor 1 -> 3 */
    CHECK(g_inv_screen.item_cursor == 3, "(20) cursor on the Green");
    use_confirm();
    frame(0, 0);                                            /* classifier */
    CHECK(re15_menu_item_c3() == 3 && re15_menu_item_c4() == 0,
          "(20) classifier -> c3=3 HEAL (@0x8004ab60), c3=%d", re15_menu_item_c3());
    CHECK(!re15_menu_msg_active(), "(20) NO prompt and NO message in the heal flow");
    frame(0, 0);                                            /* c4==0: wipe-arm */
    CHECK(g_inv_screen.wipe_mode == 1 && re15_menu_item_c4() == 1,
          "(20) wipe armed at c4==0, BEFORE the consume (mode 1 @0x8004ae84)");
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 40 && g_inv.slots[3].id == 0x24,
          "(20) hp + slot UNTOUCHED while the wipe runs");
    {   /* byte cadence: 31 more c4==1 poll frames (renderer +3/frame 0x20->0x80 = 32
         * wipe ticks incl. the arm frame's), 1 detect frame (c4:=2), 1 consume frame. */
        int n = 0;
        while (g_inv_screen.item_state == 5 && n++ < 60) frame(0, 0);
        CHECK(n == 33, "(20) heal cadence: 31 wait + detect + consume frames, is %d", n);
    }
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 65,
          "(20) Green heals +25 (100>>2 @0x8004af78) with no prompt — hp 40->65, is %d",
          g_actors[RE15_ACTOR_SLOT_PLAYER].hp);
    CHECK(g_inv.slots[3].id == 0, "(20) slot consumed at c4==2 (@0x8004aee4-af28)");
    CHECK(g_inv_screen.ecg_sweep == 0x21,
          "(20) ECG sweep reset 2600:=0x20 (@0x8004b038) + the same-frame renderer "
          "advance +1 (@0x80048ab4) = 0x21, is 0x%02x", g_inv_screen.ecg_sweep);
    CHECK(re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(20) terminal invariant (@0x8004b044-58)");
    idle(8);
    CHECK(g_inv_screen.item_state == 1, "(20) grid");

    /* (21) consume even at FULL HP + no clamp (@0x8004afa8-b0 raw addu, consume
     * unconditional at c4==2). */
    g_inv.slots[3].id = 0x24; g_inv.slots[3].qty = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
    use_confirm();                                          /* cursor still 3 */
    { int n = 0; while (g_inv_screen.item_state == 5 && n++ < 60) frame(0, 0); }
    CHECK(g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 125,
          "(21) full-HP heal: 100+25=125 (NO clamp) AND consumed, hp %d",
          g_actors[RE15_ACTOR_SLOT_PLAYER].hp);
    CHECK(g_inv.slots[3].id == 0, "(21) consumed even at full HP");
    idle(8);

    /* (22) Red Medicine 0x25 -> CANT-USE message (the classifier's beq a0,0x25
     * exclusion @0x8004ab54 -> c3=6 @0x8004ab6c-7c): slide the command cluster out
     * (25ee 166->264 @0x8004b2a0-d4), open "You can't use it here." (FUN_80027e68
     * (0x00a80018,0x8400,0,0) @0x8004b2d8 = desc-bank entry 0 at (0x18,0xa8)), menu
     * typewriter 2f lead-in then 4f/glyph (8525=1<<1 / 8524=2<<1 @0x80028194-281c4),
     * NO fast-forward (FF gated on 8522!=0 @0x800281ec-f4), NO auto-dismiss; dismissed
     * only by the confirm/cancel EDGE (&0xc000 @0x8002868c-86d0, no SE), then 25c2:=1
     * = GRID DIRECTLY (@0x8004b30c-24). */
    g_inv.slots[3].id = 0x25; g_inv.slots[3].qty = 1;       /* seed Red at the cursor */
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 40;
    use_confirm();
    frame(0, 0);                                            /* classifier */
    CHECK(re15_menu_item_c3() == 6 && re15_menu_item_c4() == 0,
          "(22) Red -> c3=6 cant-use (@0x8004ab54/@0x8004ab6c), c3=%d", re15_menu_item_c3());
    {
        int i;
        for (i = 0; i < 7; i++) {                           /* 7 slide frames 166->264 */
            frame(0, 0);
            CHECK(g_inv_screen.act_base_y == 166 + 14 * (i + 1),
                  "(22) slide %d: 25ee +14 (@0x8004b2c8), is %d",
                  i, g_inv_screen.act_base_y);
        }
        frame(0, 0);                                        /* detect: 25ee>=251 -> c4=1 */
        CHECK(re15_menu_item_c4() == 1, "(22) slide done -> c4=1 (@0x8004b2b0)");
    }
    frame(0, 0);                                            /* c4==1: msg open + 1st VM tick */
    CHECK(re15_menu_msg_active() && re15_menu_item_c4() == 2,
          "(22) message opened (8520|=0x80) + c4=2 (@0x8004b2d8-f8)");
    CHECK(g_inv_screen.msg_reveal == 0, "(22) no glyph yet (2-frame lead-in)");
    frame(0, 0);                                            /* 2nd VM tick */
    CHECK(g_inv_screen.msg_reveal == 1,
          "(22) first glyph after 2 frames (8525=1<<1), reveal=%d", g_inv_screen.msg_reveal);
    idle(4);
    CHECK(g_inv_screen.msg_reveal == 2,
          "(22) then 4 frames/glyph (8524=2<<1), reveal=%d", g_inv_screen.msg_reveal);
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);          /* confirm DURING typing */
    CHECK(re15_menu_msg_active(),
          "(22) no dismiss and no FF while typing (FF gate 8522==0 @0x800281ec-f4)");
    { int n = 0;
      while (g_inv_screen.msg_reveal < RE15_INV_CANTUSE_GLYPHS && n++ < 200) frame(0, 0); }
    CHECK(g_inv_screen.msg_reveal == RE15_INV_CANTUSE_GLYPHS,
          "(22) all 22 glyphs typed (DEBUG.BIN entry-0 byte count)");
    /* Byte-true terminator cadence (wave-4 VM alignment): the last glyph reloads
     * 8525=4; the 01 00 -> state-5 transition happens on the 4th frame after it, and
     * an edge pressed ON that transition frame is LOST (the state-5 dismiss check
     * @0x8002868c only runs on a LATER VM invocation; press edges don't persist). */
    idle(4);
    CHECK(re15_menu_msg_active(), "(22) PRESS-WAIT holds (VM state 5, no auto-dismiss)");
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);        /* cancel edge dismisses too */
    CHECK(!re15_menu_msg_active(),
          "(22) dismissed on the cancel edge (&0xc000 @0x8002868c-86d0, no SE)");
    frame(0, 0);                                            /* c4==2 poll sees the clear */
    CHECK(g_inv_screen.item_state == 1 && re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(22) return DIRECTLY to GRID (25c2:=1, not state 6) + terminal invariant");
    CHECK(g_inv.slots[3].id == 0x25 && g_actors[RE15_ACTOR_SLOT_PLAYER].hp == 40,
          "(22) Red NOT consumed, hp untouched");
    CHECK(g_inv_screen.act_base_y == 264, "(22) command cluster stays out (grid rest)");

    /* (23) ammo (0x15 H.GUN BULLETS) hits the same c3=6 verdict (all non-weapon
     * non-heal ids @0x8004ab6c). Quick pass: message + confirm-edge dismissal. */
    frame(RE15_PAD_BIT_LEFT, RE15_PAD_BIT_LEFT); idle(1);   /* cursor 3 -> 2 (odd-1) */
    CHECK(g_inv_screen.item_cursor == 2, "(23) cursor on the bullets");
    use_confirm();
    frame(0, 0);
    CHECK(re15_menu_item_c3() == 6, "(23) ammo -> c3=6 (@0x8004ab6c)");
    { int n = 0; while (!re15_menu_msg_active() && n++ < 20) frame(0, 0); }
    CHECK(re15_menu_msg_active(), "(23) message up");
    { int n = 0;
      while (g_inv_screen.msg_reveal < RE15_INV_CANTUSE_GLYPHS && n++ < 200) frame(0, 0); }
    idle(4);                                    /* terminator -> VM state 5 (see (22)) */
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(!re15_menu_msg_active(), "(23) dismissed on the confirm edge");
    frame(0, 0);
    CHECK(g_inv_screen.item_state == 1 && re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(23) grid + terminal invariant; ammo intact: %02x", g_inv.slots[2].id);
    CHECK(g_inv.slots[2].id == 0x15, "(23) bullets NOT consumed");
    re15_menu_toggle();                                     /* close the debug session */
    CHECK(!re15_menu_is_open(), "(23) closed");

    if (fails) { fprintf(stderr, "\nINV FSM TEST: %d FAILURES\n", fails); return 1; }
    printf("PASS: status-screen FSM byte-true (open stages + tab FSM + ITEM slides "
           "+/-14 + grid nav w/ auto-repeat 15/4 + command stage + close commit; "
           "wave 3: equip/unequip/swap per-step anims + 25c9 stale-gate 0x5c quirk + "
           "prompt-less heal (+25 Green) + cant-use message)\n");
    return 0;
}
