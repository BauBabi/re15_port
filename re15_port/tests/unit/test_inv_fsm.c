/**
 * @file test_inv_fsm.c
 * @brief Wave 2 — the byte-true STATUS/INVENTORY screen FSM (menu_common.c).
 *
 * Drives the FSM with scripted pad edges through a full session:
 *   open (START poll -> stage FSM -> fades) -> tab nav -> ITEM mode (entry slide) ->
 *   grid nav (auto-repeat) -> command stage -> equip -> cancel -> exit slide ->
 *   EXIT close (fade-out + equip commit + gameplay fade-in)
 * and asserts the state-byte sequences against the RE'd transitions
 * (spec shots/inv_wave2_spec.md; every constant cited at its assertion).
 *
 * Frame model: one test frame = the port frame order — start_poll + fsm_tick
 * (game_step) then re15_fade_tick (render end_frame). Fades: |step| 0x1800 =
 * exactly 6 drawn frames, done-detection the following frame (FUN_80021880 math).
 */
#include <stdio.h>
#include <string.h>
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_damage.h"     /* re15_player_equipped_weapon / set */
#include "re15_fade.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

/* one port frame: gameplay poll + FSM tick, then the render-side fade tick */
static void frame(uint16_t pressed, uint16_t held)
{
    re15_menu_start_poll(pressed, 1);
    if (re15_menu_gameplay_frozen())
        re15_menu_fsm_tick(pressed, held);
    re15_fade_tick();
}
static void idle(int n) { while (n-- > 0) frame(0, 0); }

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
     * (@0x8004a558-64): 25d6:=0 + 25c2:=9; the wave-3 CHECK stub returns per its
     * terminal contract @0x800c683c-50: 25c2:=4 + 25d6:=1. */
    frame(RE15_PAD_BIT_LEFT, RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.action_dir == 1, "(12) cmd Left -> 25d6=1");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 9, "(12) confirm CHECK -> 25c2=9");
    frame(0, 0);
    CHECK(g_inv_screen.item_state == 4 && g_inv_screen.action_dir == 1,
          "(12) CHECK stub -> 25c2=4 + 25d6=1 (@0x800c683c-50)");

    /* (13) USE on the Browning (wave-3 bridge at the byte-true site @0x8004a674):
     * equip RECORD 25c8 := cursor (the PLAYER commit stays for close @0x80046688) ->
     * state 6 slide-out -> GRID. */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);                /* 25d6 -> 0 USE */
    CHECK(g_inv_screen.action_dir == 0, "(13) cmd Up -> 25d6=0");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 5, "(13) confirm USE -> 25c2=5");
    frame(0, 0);                                            /* bridge: equip + -> state 6 */
    CHECK(re15_inv_equipped_slot() == 1, "(13) equip record 25c8 must be the cursor slot 1");
    CHECK(re15_player_equipped_weapon() == 1,
          "(13) the PLAYER weapon must NOT change in-menu (commit is at close @0x80046688)");
    CHECK(g_inv_screen.item_state == 6, "(13) -> 25c2=6 (slide back to grid)");
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

    if (fails) { fprintf(stderr, "\nINV FSM TEST: %d FAILURES\n", fails); return 1; }
    printf("PASS: status-screen FSM byte-true (open stages + tab FSM + ITEM slides "
           "+/-14 + grid nav w/ auto-repeat 15/4 + command stage + close commit)\n");
    return 0;
}
