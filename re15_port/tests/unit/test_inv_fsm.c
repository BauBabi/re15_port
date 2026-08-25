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
#include "re15_inv_ui.h"     /* wave 5: the embedded EXE blob (pair lists @0x80074C88..,
                              * prop table @0x80074DA8) — the tests drive the FULL
                              * combine graph from the shipped bytes, nothing re-typed */
#include "re15_inventory.h"
#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_damage.h"     /* re15_player_equipped_weapon / set */
#include "re15_actor.h"      /* player hp / status_flags (wave-3 heal) */
#include "re15_fade.h"
#include "re15_room.h"       /* g_current_room_id — the MAP entry init reads stage/room */

extern int g_test_core_se_last, g_test_core_se_count;   /* tests/support/test_support.c */

/* wave-5 blob readers (little-endian, PSX-address keyed) */
static uint8_t  tu8(uint32_t addr) { return *RE15_INV_PTR(addr); }
static uint32_t tu32(uint32_t addr)
{
    const unsigned char *p = RE15_INV_PTR(addr);
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}
#define T_PROP 0x80074DA8u   /* prop table stride 12 {cap,pair_ptr,kind,pair_count} */

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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    idle(8);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
}

/* ====================== WAVE 5 helpers (spec shots/inv_wave5_spec.md) ================ */
/* Fresh debug-open with the CURRENT g_inv seed -> ITEM -> GRID (cursor 0). */
static void w5_open_to_grid(void)
{
    re15_inv_set_equipped_slot(0x80);            /* nothing equipped (25c8) */
    re15_inv_set_prev_equip_slot(0x80);
    re15_menu_toggle();
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* tab ITEM confirm */
    idle(8);                                     /* entry slide -> GRID */
}
/* From GRID with the cursor placed: grid confirm (25d6=0 + 25c2=3) -> command slide-in
 * -> RIGHT (25d6=3 EXCHANGE @0x8004a458-47c) -> CROSS (dispatch [3] @0x8004a570:
 * 25c2=7; 25be keeps its grid-mirror value, c3/c4 = 0 by the terminal invariant). */
static void w5_enter_exchange(void)
{
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    idle(8);
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 7, "(w5) EXCHANGE confirm -> 25c2=7 (@0x8004a570)");
}
/* Finish a successful combine: 17-step anim (grow 8 + shrink 8 + terminal
 * @0x80010ff4) -> state 6 -> slide-out -> GRID; then close the debug session. */
static void w5_finish_and_close(void)
{
    idle(17);
    CHECK(g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(w5) result-anim terminal -> 25c2=6 + c3/c4=0 (@0x8004b524-b55c)");
    idle(8);
    CHECK(g_inv_screen.item_state == 1, "(w5) slide-out -> GRID");
    re15_menu_toggle();
}
/* One state-7 second-cursor move: a fresh d-pad press edge (auto-repeat fire). */
static void w5_second_move(uint16_t dir)
{
    frame(dir, dir);
}

static void wave5_tests(void)
{
    int i, oi;

    /* (24) FULL herb mix graph, table-driven from the embedded blob: owners
     * 0x24..0x29, pair lists @0x80074d54-...a7 (pair_ptr/pair_count read from the prop
     * rows +4/+9 exactly like the matcher @0x8004e9d8/@0x8004e9e8). Expected counts
     * {6,4,3,3,3,2} = the spec table; every pair is action 1 = MIX-MERGE
     * (@0x8004e1ac): result id + count A+B into the LOWER slot, other slot cleared,
     * MIXITEM tile pic, then compaction (@0x8004b3cc) shifts the knife filler down. */
    {
        static const uint8_t owners[6] = { 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
        static const uint8_t exp_cnt[6] = { 6, 4, 3, 3, 3, 2 };
        int pairs_seen = 0;
        for (oi = 0; oi < 6; oi++) {
            uint8_t  cnt   = tu8(T_PROP + (uint32_t)owners[oi] * 12u + 9u);
            uint32_t plist = tu32(T_PROP + (uint32_t)owners[oi] * 12u + 4u);
            CHECK(cnt == exp_cnt[oi], "(24) owner %02x pair_count %d (blob +9), is %d",
                  owners[oi], exp_cnt[oi], cnt);
            for (i = 0; i < cnt; i++) {
                uint8_t partner = tu8(plist + (uint32_t)i * 4u);
                uint8_t result  = tu8(plist + (uint32_t)i * 4u + 1u);
                uint8_t act     = tu8(plist + (uint32_t)i * 4u + 2u);
                uint8_t pic     = tu8(plist + (uint32_t)i * 4u + 3u);
                CHECK(act == 1, "(24) herb pair %02x+%02x action must be 1, is %d",
                      owners[oi], partner, act);
                pairs_seen++;
                re15_inv_init();
                g_inv.slots[0].id = owners[oi]; g_inv.slots[0].qty = 1;
                g_inv.slots[1].id = partner;    g_inv.slots[1].qty = 2;
                g_inv.slots[2].id = 0x01;       g_inv.slots[2].qty = 0;  /* knife filler */
                w5_open_to_grid();
                w5_enter_exchange();
                w5_second_move(RE15_PAD_BIT_RIGHT);          /* 2nd cursor 0 -> 1 */
                CHECK(g_inv_screen.second_cursor == 1, "(24) 2nd cursor on the partner");
                frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);   /* confirm = mix NOW */
                CHECK(g_inv.slots[0].id == result && g_inv.slots[0].qty == 3,
                      "(24) %02x+%02x -> %02x qty 1+2=3 in the LOWER slot "
                      "(@0x8004e1e8-214), got %02x q%d", owners[oi], partner, result,
                      g_inv.slots[0].id, g_inv.slots[0].qty);
                CHECK(g_inv.slots[1].id == 0x01,
                      "(24) other slot cleared + compaction shifted the knife down "
                      "(@0x8004e2ec-30c + @0x8004b3cc), slot1=%02x", g_inv.slots[1].id);
                CHECK(g_inv.slots[2].id == 0, "(24) tail slot vacated by the compaction");
                CHECK(re15_inv_screen_cache_mix_pic(0) == pic,
                      "(24) MIXITEM tile pic %d uploaded to the result cell "
                      "(@0x8004e224-44), is %d", pic, re15_inv_screen_cache_mix_pic(0));
                CHECK(re15_menu_item_c3() == 1 && re15_menu_item_c4() == 0,
                      "(24) confirm arms the result anim (25c3=1 @0x8004b3e4)");
                w5_finish_and_close();
            }
        }
        CHECK(pairs_seen == 21, "(24) the herb graph has 21 pairs total, saw %d", pairs_seen);
        /* the asymmetric-pic quirk pinned on the shipped bytes: 0x24+0x27 -> 0x2a pic 7
         * (@0x80074d60) vs 0x27+0x24 -> 0x2a pic 4 (@0x80074d8c) — same result id,
         * different tile; ported as-is. */
        CHECK(tu8(0x80074d60u) == 0x27 && tu8(0x80074d61u) == 0x2a && tu8(0x80074d63u) == 7,
              "(24) blob @0x80074d60 = {27,2a,01,07}");
        CHECK(tu8(0x80074d8cu) == 0x24 && tu8(0x80074d8du) == 0x2a && tu8(0x80074d8fu) == 4,
              "(24) blob @0x80074d8c = {24,2a,01,04}");
    }

    /* (24b) LOWER-slot rule with the cursor on the HIGHER slot: owner at slot 2,
     * partner at slot 0 -> the merge lands in slot 0 (sltu picks the branch
     * @0x8004e1b4/@0x8004e250). */
    re15_inv_init();
    g_inv.slots[0].id = 0x25; g_inv.slots[0].qty = 2;    /* RED (partner) */
    g_inv.slots[1].id = 0x01; g_inv.slots[1].qty = 0;    /* knife */
    g_inv.slots[2].id = 0x24; g_inv.slots[2].qty = 1;    /* GREEN (owner, cursor) */
    w5_open_to_grid();
    frame(RE15_PAD_BIT_DOWN, RE15_PAD_BIT_DOWN); idle(1);    /* cursor 0 -> 2 */
    CHECK(g_inv_screen.item_cursor == 2, "(24b) cursor on the Green at slot 2");
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_UP);                     /* 2nd cursor 2 -> 0 */
    CHECK(g_inv_screen.second_cursor == 0, "(24b) 2nd cursor on the Red at slot 0");
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].id == 0x27 && g_inv.slots[0].qty == 3,
          "(24b) G(slot2)+R(slot0) -> 0x27 qty 3 in slot 0 (the LOWER slot), got %02x q%d",
          g_inv.slots[0].id, g_inv.slots[0].qty);
    CHECK(g_inv.slots[1].id == 0x01 && g_inv.slots[2].id == 0,
          "(24b) slot 2 cleared; knife stays at slot 1 (hole was after it)");
    w5_finish_and_close();

    /* (24c) COMBINE-SOUND — NACHGERUESTET nach RE2-Vorbild (Nutzer-Auftrag 2026-08-27:
     * "beim Combine von items fehlen Sound und Effekte ... schaue wie das echte re2 das
     * macht und ruestet Sound und Effekt nach").
     *
     * RE1.5 ist hier byte-true STUMM: der ganze Erfolgspfad @0x8004b37c..@0x8004b404 hat
     * genau vier `jal` (0x80048904 Cursor, 0x8004e900 Matcher, 0x8004e054 Ausfuehrung,
     * 0x8004dadc Kompaktierung) und KEINEN Se_on-Aufruf (FUN_80045024); der Ergebnis-Walker
     * @0x8004b408 hat null `jal`. Der EFFEKT fehlt dagegen nicht — RE1.5 hat seinen eigenen
     * 17-Frame-Puls (`sltiu v0,v1,0x11` @0x8004b414, Tabelle @0x80010ff4), gepinnt in (25).
     *
     * RE2-Vorbild (info/re2leon/PSX.EXE, FUN_8006b358):
     *   @0x8006b57c beq (Cursor unveraendert) / @0x8006b580 lui a0,0x404 -> Record 4
     *   @0x8006b5b0 beq v0,zero / @0x8006b5b4 lui a0,0x407 (FEHLER, Delay-Slot)
     *                            / @0x8006b5b8 lui a0,0x406 (ERFOLG) / @0x8006b5bc jal Se_on
     *   @0x8006b5cc beq (CANCEL) / @0x8006b5d0 lui a0,0x405 -> Record 5
     * Alles RE1.5-EIGENE Bank-4-Records: 4/5/6 spielt das Inventar anderswo bereits
     * (@0x8004a478.., @0x8004a660, @0x8004a51c); Record 7 ist in RE1.5s EXE nie referenziert
     * (Voll-Scan `lui a0,0x0407`: 0 Treffer), existiert aber gueltig in SOUND/CORE00.EDH
     * @0x1c = 00 00 83 00 (erster Leer-Record ist 11 @0x2c = ff ff ff ff). */
    {   int se0, se1;
        /* (a) GELUNGENER Combine -> Record 6 */
        re15_inv_init();
        g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;   /* Green  */
        g_inv.slots[1].id = 0x25; g_inv.slots[1].qty = 1;   /* Red    */
        g_inv.slots[2].id = 0x01; g_inv.slots[2].qty = 0;   /* knife  */
        w5_open_to_grid();
        w5_enter_exchange();
        w5_second_move(RE15_PAD_BIT_RIGHT);
        se0 = g_test_core_se_count;
        frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);    /* confirm = mix */
        se1 = g_test_core_se_count;
        CHECK(se1 > se0 && g_test_core_se_last == 6,
              "(24c) gelungener Combine spielt CORE-Record 6 (RE2 @0x8006b5b8 lui a0,0x406), "
              "gemessen %d Aufrufe / zuletzt %d", se1 - se0, g_test_core_se_last);
        CHECK(g_inv.slots[0].id == 0x27,
              "(24c) der Combine muss auch WIRKLICH gelingen (G+R -> 0x27), sonst prueft (a) "
              "den falschen Zweig — slot0 = %02x", g_inv.slots[0].id);
        w5_finish_and_close();

        /* (b) FEHLGESCHLAGENER Combine (kein Paar) -> Record 7 */
        re15_inv_init();
        g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;   /* Green  */
        g_inv.slots[1].id = 0x01; g_inv.slots[1].qty = 0;   /* knife: kein Partner */
        w5_open_to_grid();
        w5_enter_exchange();
        w5_second_move(RE15_PAD_BIT_RIGHT);
        se0 = g_test_core_se_count;
        frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
        se1 = g_test_core_se_count;
        CHECK(se1 > se0 && g_test_core_se_last == 7,
              "(24c) fehlgeschlagener Combine spielt CORE-Record 7 (RE2 @0x8006b5b4 "
              "lui a0,0x407 im Delay-Slot), gemessen %d Aufrufe / zuletzt %d",
              se1 - se0, g_test_core_se_last);
        CHECK(g_inv_screen.item_state == 6 && g_inv.slots[0].id == 0x24,
              "(24c) und der Combine muss WIRKLICH scheitern (25c2=6 @0x8004b3f0-f4, "
              "Bestand unveraendert) — state=%d slot0=%02x",
              g_inv_screen.item_state, g_inv.slots[0].id);
        idle(8); re15_menu_toggle();

        /* (c) ABBRUCH im Exchange -> Record 5 */
        re15_inv_init();
        g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;
        g_inv.slots[1].id = 0x25; g_inv.slots[1].qty = 1;
        w5_open_to_grid();
        w5_enter_exchange();
        se0 = g_test_core_se_count;
        frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);      /* virtueller Cancel */
        se1 = g_test_core_se_count;
        CHECK(se1 > se0 && g_test_core_se_last == 5,
              "(24c) Abbruch im Exchange spielt CORE-Record 5 (RE2 @0x8006b5d0 lui a0,0x405), "
              "gemessen %d Aufrufe / zuletzt %d", se1 - se0, g_test_core_se_last);
        CHECK(g_inv_screen.item_state == 6,
              "(24c) und der Abbruch muss WIRKLICH greifen (25c2=6) — state=%d",
              g_inv_screen.item_state);
        idle(8); re15_menu_toggle();

        /* (d) CURSOR-Bewegung im Exchange -> Record 4 */
        re15_inv_init();
        g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;
        g_inv.slots[1].id = 0x25; g_inv.slots[1].qty = 1;
        w5_open_to_grid();
        w5_enter_exchange();
        {   uint8_t before = g_inv_screen.second_cursor;
            se0 = g_test_core_se_count;
            w5_second_move(RE15_PAD_BIT_RIGHT);
            se1 = g_test_core_se_count;
            CHECK(g_inv_screen.second_cursor != before,
                  "(24c) der zweite Cursor muss sich WIRKLICH bewegen, sonst prueft (d) nichts "
                  "(%u -> %u)", before, g_inv_screen.second_cursor);
            CHECK(se1 > se0 && g_test_core_se_last == 4,
                  "(24c) Cursor-Bewegung im Exchange spielt CORE-Record 4 "
                  "(RE2 @0x8006b580 lui a0,0x404), gemessen %d Aufrufe / zuletzt %d",
                  se1 - se0, g_test_core_se_last);
        }
        idle(8); re15_menu_toggle();
    }

    /* (25) the 17-step result-anim walker @0x80010ff4 register-exact (fresh disasm):
     * steps 0-7 = 0x8004b43c GROW (d0/d1 +1, d2/d3 -1, c4++ @0x8004b468-49c); steps
     * 8-15 = 0x8004b4a8 SHRINK (25be := 25bd EVERY frame @0x8004b4f0, deltas reversed
     * @0x8004b4d8-b518); step 16 = 0x8004b524 TERMINAL (25c2=6, c3/c4=0, d0-d3=0).
     * Peak = +-8 at step 8. G+G -> 0x28 (pair @0x80074d58 {24,28,01,02}). */
    re15_inv_init();
    g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;
    g_inv.slots[1].id = 0x24; g_inv.slots[1].qty = 1;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].id == 0x28 && g_inv.slots[0].qty == 2,
          "(25) G+G -> 0x28 qty 2 (pair @0x80074d58)");
    CHECK(re15_inv_screen_cache_mix_pic(0) == 2, "(25) MIXITEM pic 2");
    CHECK(g_inv_screen.comb_d0 == 0 && g_inv_screen.comb_d2 == 0,
          "(25) jitter still 0 on the confirm frame (walker starts next frame)");
    for (i = 1; i <= 8; i++) {                           /* GROW steps 0..7 */
        frame(0, 0);
        CHECK(g_inv_screen.comb_d0 == i && g_inv_screen.comb_d1 == i &&
              g_inv_screen.comb_d2 == (uint8_t)-i && g_inv_screen.comb_d3 == (uint8_t)-i,
              "(25) grow step %d: d0/d1=+%d d2/d3=-%d (@0x8004b468-494), got %d/%d",
              i - 1, i, i, g_inv_screen.comb_d0, (int8_t)g_inv_screen.comb_d2);
        CHECK(re15_menu_item_c4() == i, "(25) c4 == %d", i);
        CHECK(g_inv_screen.second_cursor == 1,
              "(25) 2nd cursor NOT snapped during grow (no 25be write in 0x8004b43c)");
    }
    for (i = 1; i <= 8; i++) {                           /* SHRINK steps 8..15 */
        frame(0, 0);
        CHECK(g_inv_screen.comb_d0 == 8 - i &&
              g_inv_screen.comb_d2 == (uint8_t)-(8 - i),
              "(25) shrink step %d: d0=%d (@0x8004b4d8-b510), got %d",
              7 + i, 8 - i, g_inv_screen.comb_d0);
        CHECK(g_inv_screen.second_cursor == g_inv_screen.item_cursor,
              "(25) 25be := 25bd every shrink frame (@0x8004b4f0)");
    }
    frame(0, 0);                                         /* step 16 terminal */
    CHECK(g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0 &&
          re15_menu_item_c4() == 0 && g_inv_screen.comb_d0 == 0 &&
          g_inv_screen.comb_d1 == 0 && g_inv_screen.comb_d2 == 0 &&
          g_inv_screen.comb_d3 == 0,
          "(25) terminal: 25c2=6 + c3/c4=0 + d0-d3=0 (@0x8004b524-b55c)");
    idle(8);
    re15_menu_toggle();

    /* (26) RELOAD cap clamp, both directions (actions 2/3 @0x8004e320/@0x8004e42c;
     * cap = prop[+0] of the id at the RAW cursor slot: s2 @0x8004e338 / s1 @0x8004e444).
     * Browning 0x03: cap 15, pair {15,03,02,00} @0x80074c8c; ammo 0x15 owns the mirror
     * pair {03,03,03,00} @0x80074cd0 (action 3). */
    CHECK(tu8(T_PROP + 0x03 * 12u) == 15, "(26) prop[0x03].cap == 15 (blob)");
    /* weapon-first FULL: 10+5 = 15 <= 15 */
    re15_inv_init();
    g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 10;
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 5;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].id == 0x03 && g_inv.slots[0].qty == 15 && g_inv.slots[1].id == 0,
          "(26) weapon-first FULL reload: qty 15, box deleted (@0x8004e34c-3e0), got %02x q%d",
          g_inv.slots[0].id, g_inv.slots[0].qty);
    CHECK(re15_menu_item_c3() == 1, "(26) action 2 != 0 -> the anim runs");
    w5_finish_and_close();
    /* weapon-first PARTIAL: 10+50 = 60 > 15 -> mag=15, box=45, ids unchanged */
    re15_inv_init();
    g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 10;
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 50;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].qty == 15 && g_inv.slots[1].qty == 45 &&
          g_inv.slots[0].id == 0x03 && g_inv.slots[1].id == 0x15,
          "(26) weapon-first PARTIAL: mag=cap 15, box=rest 45, NO id write "
          "(@0x8004e3e4-420), got q%d/q%d", g_inv.slots[0].qty, g_inv.slots[1].qty);
    CHECK(re15_menu_item_c3() == 1, "(26) partial also animates (returns 2)");
    w5_finish_and_close();
    /* ammo-first PARTIAL (action 3, cap from prop[s1] = the weapon @0x8004e444) */
    re15_inv_init();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 50;
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 10;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[1].qty == 15 && g_inv.slots[0].qty == 45,
          "(26) ammo-first PARTIAL: weapon=15, box=45 (@0x8004e4f0-52c), got q%d/q%d",
          g_inv.slots[1].qty, g_inv.slots[0].qty);
    w5_finish_and_close();
    /* ammo-first FULL: 5+10 = 15 -> weapon takes 15, ammo slot deleted + compaction
     * shifts the weapon into slot 0 */
    re15_inv_init();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 5;
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 10;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].id == 0x03 && g_inv.slots[0].qty == 15 && g_inv.slots[1].id == 0,
          "(26) ammo-first FULL: weapon q15 compacted into slot 0 (@0x8004e458-4ec + "
          "@0x8004b3cc), got %02x q%d", g_inv.slots[0].id, g_inv.slots[0].qty);
    w5_finish_and_close();
    /* wide-weapon kind-2 normalization (matcher @0x8004e910-38 / executor @0x8004e0c8-11c):
     * FLAMETHROWER 0x0e (cells 0/1, kind 1/2) + FLAME FUEL 0x18; cursor on the TAIL
     * cell 1 -> normalized to 0. Pair {18,0e,02,00} @0x80074ca8, cap prop[0x0e]=100. */
    re15_inv_init();
    g_inv.slots[0].id = 0x0e; g_inv.slots[0].qty = 40; g_inv.slots[0].flags = 1;
    g_inv.slots[1].id = 0x0e; g_inv.slots[1].qty = 40; g_inv.slots[1].flags = 2;
    g_inv.slots[2].id = 0x18; g_inv.slots[2].qty = 60;
    w5_open_to_grid();
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT); idle(1);  /* cursor 0 -> 1 (tail) */
    CHECK(g_inv_screen.item_cursor == 1, "(26w) cursor on the tail cell");
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_LEFT);                   /* 2nd 1 -> 0 */
    w5_second_move(RE15_PAD_BIT_DOWN);                   /* 2nd 0 -> 2 (the fuel) */
    CHECK(g_inv_screen.second_cursor == 2, "(26w) 2nd cursor on the fuel");
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].qty == 100 && g_inv.slots[0].id == 0x0e,
          "(26w) FULL into the NORMALIZED head slot: 40+60=100=cap (@0x8004e910-38), q%d",
          g_inv.slots[0].qty);
    CHECK(g_inv.slots[1].qty == 40,
          "(26w) the tail cell's qty byte is NOT written (only the head slot), q%d",
          g_inv.slots[1].qty);
    CHECK(g_inv.slots[2].id == 0, "(26w) fuel deleted");
    w5_finish_and_close();

    /* (27) THE GLOCK-18 QUIRK: pair @0x80074c98 = {15,04,02,00} — the reload RESULT
     * byte is 0x04 SIG P228 (@0x80074c99), not 0x06. A full reload TRANSFORMS the
     * GLOCK into a SIG; the icon cell keeps the GLOCK art (no icon call in the full
     * branch 0x8004e34c-e3e0). The ammo-side list @0x80074ccc has NO 0x06 partner
     * (partners {15,03,04,05,04,0c,13}) -> bullets-first on a GLOCK = NO action. */
    CHECK(tu8(0x80074c98u) == 0x15 && tu8(0x80074c99u) == 0x04 &&
          tu8(0x80074c9au) == 0x02,
          "(27) blob @0x80074c98 = {15,04,02} (the shipped quirk bytes)");
    re15_inv_init();
    g_inv.slots[0].id = 0x06; g_inv.slots[0].qty = 1;    /* GLOCK 18 */
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 5;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].id == 0x04 && g_inv.slots[0].qty == 6,
          "(27) GLOCK full reload -> id 0x04 SIG P228 qty 6 (@0x80074c99 via "
          "@0x8004e370), got %02x q%d", g_inv.slots[0].id, g_inv.slots[0].qty);
    CHECK(re15_inv_screen_cache_tile(0) == 0x06,
          "(27) the icon cell keeps the GLOCK art (no repaint in 0x8004e34c-e3e0), "
          "tile %d", re15_inv_screen_cache_tile(0));
    w5_finish_and_close();
    {   /* ammo-side asymmetry: no 0x06 in the 0x15 partner list (blob scan) */
        uint8_t  cnt   = tu8(T_PROP + 0x15 * 12u + 9u);
        uint32_t plist = tu32(T_PROP + 0x15 * 12u + 4u);
        int found = 0;
        for (i = 0; i < cnt; i++) if (tu8(plist + (uint32_t)i * 4u) == 0x06) found = 1;
        CHECK(cnt == 7 && !found,
              "(27) ammo list @0x80074ccc: 7 partners, none == 0x06 (the quirk's mirror)");
    }
    re15_inv_init();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 5;
    g_inv.slots[1].id = 0x06; g_inv.slots[1].qty = 1;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 6 && g_inv.slots[0].id == 0x15 &&
          g_inv.slots[0].qty == 5 && g_inv.slots[1].id == 0x06,
          "(27) bullets-first on the GLOCK: NO pair -> 25c2=6, nothing mutated "
          "(@0x8004ea38-5c scan miss -> @0x8004b3f0)");
    idle(8);
    re15_menu_toggle();

    /* (28) SELF-STACK (action 5 @0x8004e664): pair {15,15,05,00} @0x80074ccc;
     * delegates to the action-2 body when norm A < norm B (@0x8004e670) else the
     * action-3 body (@0x8004e678); cap = the ammo's own 250. */
    re15_inv_init();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 30;
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 40;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].id == 0x15 && g_inv.slots[0].qty == 70 && g_inv.slots[1].id == 0,
          "(28) self-stack A-lower: 30+40=70 into slot 0, other deleted, got q%d",
          g_inv.slots[0].qty);
    CHECK(re15_menu_item_c3() == 1, "(28) action 5 != 0 -> anim");
    w5_finish_and_close();
    /* partial self-stack over the 250 cap */
    re15_inv_init();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 200;
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 100;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].qty == 250 && g_inv.slots[1].qty == 50,
          "(28) partial self-stack: 250 cap + 50 rest (cap @0x80074da8+0x15*12), got %d/%d",
          g_inv.slots[0].qty, g_inv.slots[1].qty);
    w5_finish_and_close();
    /* cursor on the HIGHER slot -> the action-3 body (merge into B = the lower) */
    re15_inv_init();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 30;
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 40;
    w5_open_to_grid();
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT); idle(1);  /* cursor 0 -> 1 */
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_LEFT);                   /* 2nd 1 -> 0 */
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].qty == 70 && g_inv.slots[1].id == 0,
          "(28) self-stack B-lower (@0x8004e678 -> action-3 body): 70 into slot 0, got q%d",
          g_inv.slots[0].qty);
    w5_finish_and_close();

    /* (29) CRAFTING (action 6 @0x8004e680) — equal-count pairs table-driven over ALL
     * shipped action-6 pairs (owners 0x1c-0x20: detonator/shells + capsules ->
     * grenades 0x09-0x0b pics 9-11 / GL rounds 0x19-0x1b pics 12-14). */
    {
        int crafts = 0;
        for (oi = 0x1c; oi <= 0x20; oi++) {
            uint8_t  cnt   = tu8(T_PROP + (uint32_t)oi * 12u + 9u);
            uint32_t plist = tu32(T_PROP + (uint32_t)oi * 12u + 4u);
            for (i = 0; i < cnt; i++) {
                uint8_t partner = tu8(plist + (uint32_t)i * 4u);
                uint8_t result  = tu8(plist + (uint32_t)i * 4u + 1u);
                uint8_t act     = tu8(plist + (uint32_t)i * 4u + 2u);
                uint8_t pic     = tu8(plist + (uint32_t)i * 4u + 3u);
                if (act != 6) continue;
                crafts++;
                re15_inv_init();
                g_inv.slots[0].id = (uint8_t)oi; g_inv.slots[0].qty = 2;
                g_inv.slots[1].id = partner;     g_inv.slots[1].qty = 2;
                w5_open_to_grid();
                w5_enter_exchange();
                w5_second_move(RE15_PAD_BIT_RIGHT);
                frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
                CHECK(g_inv.slots[0].id == result && g_inv.slots[0].qty == 2 &&
                      g_inv.slots[1].id == 0,
                      "(29) craft equal %02x+%02x -> %02x qty 2, other deleted "
                      "(@0x8004e6d8-758), got %02x q%d", oi, partner, result,
                      g_inv.slots[0].id, g_inv.slots[0].qty);
                CHECK(re15_inv_screen_cache_mix_pic(0) == pic,
                      "(29) MIXITEM pic %d (@0x8004e71c)", pic);
                w5_finish_and_close();
            }
        }
        CHECK(crafts == 12, "(29) 12 shipped action-6 pairs (3+3+2+2+2), saw %d", crafts);
    }
    /* unequal, LOWER stack bigger (qty_lo > qty_hi @0x8004e76c-834): result=min into
     * the lower slot, the leftover takes the SOURCE id in the higher slot + the source
     * cell art (icon copy @0x8004e7d0). */
    re15_inv_init();
    g_inv.slots[0].id = 0x1c; g_inv.slots[0].qty = 5;    /* REMOTE DETON. */
    g_inv.slots[1].id = 0x1e; g_inv.slots[1].qty = 2;    /* NITRO CAPSULE */
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].id == 0x09 && g_inv.slots[0].qty == 2,
          "(29u) lower gets {result, min=2} (@0x8004e7f0/@0x8004e800), got %02x q%d",
          g_inv.slots[0].id, g_inv.slots[0].qty);
    CHECK(g_inv.slots[1].id == 0x1c && g_inv.slots[1].qty == 3,
          "(29u) leftover 5-2=3 keeps the SOURCE id 0x1c in the higher slot "
          "(@0x8004e798/@0x8004e7cc), got %02x q%d", g_inv.slots[1].id, g_inv.slots[1].qty);
    CHECK(re15_inv_screen_cache_mix_pic(0) == 9 && re15_inv_screen_cache_mix_pic(1) == 0,
          "(29u) MIXITEM on the lower; the leftover cell got the source-cell COPY "
          "(identity art, @0x8004e7d0)");
    w5_finish_and_close();
    /* unequal, HIGHER stack bigger (qty_hi > qty_lo @0x8004e83c-8d0): result=min into
     * the lower (kind[lo]:=0 @0x8004e858 — the one-branch-only write), higher keeps
     * its id with the difference. */
    re15_inv_init();
    g_inv.slots[0].id = 0x1c; g_inv.slots[0].qty = 2;
    g_inv.slots[1].id = 0x1e; g_inv.slots[1].qty = 5;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv.slots[0].id == 0x09 && g_inv.slots[0].qty == 2 &&
          g_inv.slots[0].flags == 0,
          "(29u2) lower {result, min=2, kind 0} (@0x8004e858-868), got %02x q%d f%d",
          g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[0].flags);
    CHECK(g_inv.slots[1].id == 0x1e && g_inv.slots[1].qty == 3,
          "(29u2) higher keeps its id with 5-2=3 (@0x8004e8a8-8d0), got %02x q%d",
          g_inv.slots[1].id, g_inv.slots[1].qty);
    w5_finish_and_close();

    /* (30) NO PAIR -> state 6 (executor returns 0 -> sb 6 @0x8004b3f0-f4): knife has
     * pair_count 0 (prop row 0x01). */
    re15_inv_init();
    g_inv.slots[0].id = 0x01; g_inv.slots[0].qty = 0;
    g_inv.slots[1].id = 0x03; g_inv.slots[1].qty = 15;
    w5_open_to_grid();
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0,
          "(30) knife+browning: no pair -> 25c2=6, no anim (pair_count 0 @prop 0x01)");
    CHECK(g_inv.slots[0].id == 0x01 && g_inv.slots[1].id == 0x03,
          "(30) inventory untouched");
    idle(8);
    re15_menu_toggle();

    /* (31) SAME-SLOT + EMPTY-partner rejects (matcher @0x8004e98c / @0x8004e9bc). */
    re15_inv_init();
    g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;
    w5_open_to_grid();
    w5_enter_exchange();
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);       /* 2nd == cursor == 0 */
    CHECK(g_inv_screen.item_state == 6 && g_inv.slots[0].id == 0x24,
          "(31) same-slot confirm -> 25c2=6, Green intact (@0x8004e98c)");
    idle(8);
    w5_enter_exchange();
    w5_second_move(RE15_PAD_BIT_RIGHT);                  /* 2nd -> 1 (EMPTY) */
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 6 && g_inv.slots[0].id == 0x24,
          "(31) empty-partner confirm -> 25c2=6 (idB==0 @0x8004e9bc)");
    idle(8);
    re15_menu_toggle();

    /* (32) DORMANT ACTION 4: the swap+transform pairs @0x80074cb8/bc ship in the data
     * ({1a,10,04,0c}/{1b,11,04,0c}) but their owners 0x10/0x11 — like all orphan-pair
     * owners 0x09-0x0b/0x0f/0x13 — have pair_count 0; and NO reachable list (rows
     * 0x00-0x47) contains an action-4 pair. */
    CHECK(tu8(0x80074cb8u) == 0x1a && tu8(0x80074cb9u) == 0x10 &&
          tu8(0x80074cbau) == 0x04 && tu8(0x80074cbbu) == 0x0c,
          "(32) orphan pair @0x80074cb8 = {1a,10,04,0c} present in the data");
    CHECK(tu8(0x80074cbcu) == 0x1b && tu8(0x80074cbdu) == 0x11 &&
          tu8(0x80074cbeu) == 0x04, "(32) orphan pair @0x80074cbc = {1b,11,04,..}");
    {
        static const uint8_t orphan_owners[7] = { 0x09, 0x0a, 0x0b, 0x0f, 0x10, 0x11, 0x13 };
        int id, a4 = 0;
        for (i = 0; i < 7; i++)
            CHECK(tu8(T_PROP + (uint32_t)orphan_owners[i] * 12u + 9u) == 0,
                  "(32) owner %02x pair_count must be 0 (dormant)", orphan_owners[i]);
        for (id = 0; id <= 0x47; id++) {
            uint8_t  cnt   = tu8(T_PROP + (uint32_t)id * 12u + 9u);
            uint32_t plist = tu32(T_PROP + (uint32_t)id * 12u + 4u);
            for (i = 0; i < cnt; i++)
                if (tu8(plist + (uint32_t)i * 4u + 2u) == 4) a4 = 1;
        }
        CHECK(!a4, "(32) no REACHABLE pair carries action 4 — dormant by data");
    }

    /* (33) second-cursor nav FUN_80048904 (fresh disasm): held word gated by the
     * auto-repeat tick (bgez aca38 @0x80048910); RIGHT +1 only from even below cap-1
     * (@0x80048924-6c), LEFT -1 only from odd (@0x8004897c-b0), DOWN +2 except cap-2/
     * cap-1 (@0x800489c0-a04), UP -2 except <2 (@0x80048a08-38); NO wrap, NO occupancy
     * check, NO SE. */
    re15_inv_init();
    g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;    /* only ONE item — moves are
                                                          * occupancy-free */
    w5_open_to_grid();
    w5_enter_exchange();
    frame(0, 0);                                         /* settle one state-7 frame */
    w5_second_move(RE15_PAD_BIT_RIGHT);
    CHECK(g_inv_screen.second_cursor == 1, "(33) RIGHT 0->1 (even+below cap)");
    w5_second_move(RE15_PAD_BIT_RIGHT);
    CHECK(g_inv_screen.second_cursor == 1, "(33) RIGHT from odd rejected (@0x80048950)");
    w5_second_move(RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.second_cursor == 3, "(33) DOWN 1->3");
    w5_second_move(RE15_PAD_BIT_DOWN);
    w5_second_move(RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.second_cursor == 7, "(33) DOWN 3->5->7");
    w5_second_move(RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.second_cursor == 6, "(33) LEFT 7->6 (odd)");
    w5_second_move(RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.second_cursor == 6, "(33) LEFT from even rejected (@0x800489a0)");
    w5_second_move(RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.second_cursor == 8, "(33) DOWN 6->8 (cap-2 reached)");
    w5_second_move(RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.second_cursor == 8, "(33) DOWN from cap-2 rejected (@0x800489e4)");
    w5_second_move(RE15_PAD_BIT_RIGHT);
    CHECK(g_inv_screen.second_cursor == 9, "(33) RIGHT 8->9");
    w5_second_move(RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.second_cursor == 9, "(33) DOWN from cap-1 rejected (@0x800489ec)");
    w5_second_move(RE15_PAD_BIT_UP);
    CHECK(g_inv_screen.second_cursor == 7, "(33) UP 9->7");
    {   /* auto-repeat cadence: edge, then held — repeats at f16 then every 5
         * (the same FUN_80030444 tick that gates the grid, config (0xf000,15,4)) */
        int f;
        w5_second_move(RE15_PAD_BIT_UP);                 /* 7 -> 5 (edge) */
        CHECK(g_inv_screen.second_cursor == 5, "(33r) UP edge 7->5");
        for (f = 1; f <= 15; f++) frame(0, RE15_PAD_BIT_UP);
        CHECK(g_inv_screen.second_cursor == 5, "(33r) no repeat before f16");
        frame(0, RE15_PAD_BIT_UP);
        CHECK(g_inv_screen.second_cursor == 3, "(33r) first repeat at f16 (delay 15)");
        for (f = 0; f < 4; f++) frame(0, RE15_PAD_BIT_UP);
        CHECK(g_inv_screen.second_cursor == 3, "(33r) no repeat before +5");
        frame(0, RE15_PAD_BIT_UP);
        CHECK(g_inv_screen.second_cursor == 1, "(33r) repeat every 5 (rate 4)");
        idle(1);
        w5_second_move(RE15_PAD_BIT_UP);
        CHECK(g_inv_screen.second_cursor == 1, "(33) UP from <2 rejected (@0x80048a20)");
    }
    /* (34) CANCEL: virtual SQUARE -> 25c2=6, SILENT (checked BEFORE confirm
     * @0x8004b398-3a0; zero SE calls in 0x8004b37c-b404 — fresh disasm). */
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 6 && re15_menu_item_c3() == 0,
          "(34) EXCHANGE cancel -> 25c2=6 (@0x8004b3f0-f4)");
    CHECK(g_inv.slots[0].id == 0x24, "(34) nothing consumed on cancel");
    idle(8);
    CHECK(g_inv_screen.item_state == 1, "(34) back in GRID");
    re15_menu_toggle();
    CHECK(!re15_menu_is_open(), "(34) closed");
}

/* ====================== MAP wave unit tests (no session) ============================ */
static void wavemap_tests(void)
{
    int16_t mx, my;

    /* (M1) per-stage init tables (FUN_8004b568/b8a0/b9d4/bc9c/bdd4/bf70, dispatch
     * @0x80074c0c). Every case: 260d = room + stage base {0,38,50,65,77,98}
     * (@0x8004b5ac../@0x8004b9c0/@0x8004bc88/@0x8004bdc0/@0x8004bf5c/@0x8004c044);
     * page tails cited per branch in re15_inv_map_stage_init. */
    static const struct { int st, rm, room, page; } cases[] = {
        { 0,  0,   0,  2 }, { 0, 11,  11,  2 },     /* @0x8004b680-684 */
        { 0, 12,  12,  3 }, { 0, 17,  17,  3 },     /* @0x8004b6f4-6f8 */
        { 0, 18,  18,  4 }, { 0, 20,  20,  4 }, { 0, 22, 22, 4 },  /* @0x8004b754-758 */
        { 0, 23,  23,  5 },                         /* @0x8004b764-768 */
        { 0, 24,  24,  0 }, { 0, 29,  29,  0 },     /* sb zero @0x8004b7dc */
        { 0, 30,  30,  1 }, { 0, 37,  37,  1 },     /* @0x8004b884-88c */
        { 1,  0,  38,  6 }, { 1,  9,  47,  6 },     /* @0x8004b990-994 */
        { 1, 10,  48, 13 }, { 1, 11,  49, 13 },     /* @0x8004b9b4 (page 0xd) */
        { 2,  0,  50,  7 }, { 2, 31,  81,  7 },     /* @0x8004bc7c */
        { 3,  0,  65,  8 }, { 3, 11,  76,  8 },     /* @0x8004bdb4 */
        { 4,  0,  77,  9 }, { 4, 11,  88,  9 },     /* @0x8004bf50 via @0x8004bea8 */
        { 4, 12,  89, 10 }, { 4, 14,  91, 10 },     /* @0x8004bee4 */
        { 4, 15,  92, 11 }, { 4, 16,  93, 11 },     /* @0x8004bf08 */
        { 4, 17,  94,  9 }, { 4, 20,  97,  9 },     /* table [17..20] -> 9 */
        { 5,  0,  98, 12 }, { 5,  7, 105, 12 },     /* @0x8004c038-44 */
    };
    unsigned i;
    for (i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        re15_inv_map_stage_init(cases[i].st, cases[i].rm);
        CHECK(re15_inv_map_room() == cases[i].room && re15_inv_map_page() == cases[i].page,
              "(M1) stage %d room %d -> slot %d page %d, are %d/%d",
              cases[i].st, cases[i].rm, cases[i].room, cases[i].page,
              re15_inv_map_room(), re15_inv_map_page());
    }

    /* (M2) STALE quirk: out-of-range rooms return WITHOUT writing (stage-1 bound
     * sltiu 0x26 @0x8004b574 -> jr @0x8004b898; likewise 0xc/0x20/0xc/0x15/0x8). */
    re15_inv_map_stage_init(0, 20);                  /* known state: 20/4 */
    re15_inv_map_stage_init(0, 0x26);
    CHECK(re15_inv_map_room() == 20 && re15_inv_map_page() == 4,
          "(M2) stage-1 room 0x26 leaves 260d/260e stale (@0x8004b578/0x8004b898)");
    re15_inv_map_stage_init(1, 12);
    re15_inv_map_stage_init(4, 0x15);
    re15_inv_map_stage_init(5, 8);
    CHECK(re15_inv_map_room() == 20 && re15_inv_map_page() == 4,
          "(M2) out-of-range rooms of stages 2/5/6 also leave them stale");

    /* (M3) page-0xd CD file id overruns the 13-entry table @0x80074c4c: u16[13] = 0
     * (bytes @0x80074c66, inside the region-1 blob — byte-true OOB read). */
    {
        const unsigned char *p = RE15_INV_PTR(0x80074c4cu + 13u * 2u);
        CHECK((p[0] | (p[1] << 8)) == 0, "(M3) file-id table overrun u16[13] == 0");
        p = RE15_INV_PTR(0x80074c4cu);
        CHECK((p[0] | (p[1] << 8)) == 12, "(M3) file-id table[0] == 12 (MAP01.PIX)");
    }

    /* (M4) marker formula (FUN_800473f8 @0x8004741c-0x80047528; the settled DISPLAYED
     * path — the builder's (world+25000)/scale div @0x80047010-14 is overwritten
     * before every AddPrim). Scale row 2 @0x800768C0 = {100,136,2287,2287}:
     *   world (0,0):        t=(320000*2287)>>20=697, +5=702, /10=70 -> mx=170;
     *                       my=trunc(-702/10)+136 = -70+136 = 66.
     *   world (-32000,..):  t=0+5=5, /10=0 -> mx=100 / my=136.
     *   world (-40000,..):  (-80000*2287)>>20 = floor(-174.48) = -175 (arithmetic
     *                       shift), +5=-170, trunc(/10)=-17 -> mx=83;
     *                       y: -(-170)=170, /10=17 -> my=153. */
    re15_inv_map_marker(0, 0, 2, &mx, &my);
    CHECK(mx == 170 && my == 66, "(M4) row2 world(0,0) -> (170,66), is (%d,%d)", mx, my);
    re15_inv_map_marker(-32000, -32000, 2, &mx, &my);
    CHECK(mx == 100 && my == 136, "(M4) row2 world(-32000) -> offsets (100,136), is (%d,%d)",
          mx, my);
    re15_inv_map_marker(-40000, -40000, 2, &mx, &my);
    CHECK(mx == 83 && my == 153, "(M4) row2 world(-40000) -> (83,153) — floor-shift + "
          "trunc-div mix, is (%d,%d)", mx, my);
    /* placeholder row 0 {0,0,1,1} (@0x800768B0): everything collapses to (0,0) */
    re15_inv_map_marker(0, 0, 0, &mx, &my);
    CHECK(mx == 0 && my == 0, "(M4) row0 placeholder -> (0,0), is (%d,%d)", mx, my);

    /* restore the session state (ROOM1140) for any later consumer */
    re15_inv_map_stage_init(0, 20);
}

/* ====================== FILE wave (DEBUG.BIN FUN_800c6ca0 @0x800c6ca0) =============== */
/* All addresses = DEBUG.BIN (maps 1:1 @0x800c0000, file==RAM). SE spy = test_support. */
extern int g_test_core_se_last;
extern int g_test_core_se_count;
#include "font_width.h"          /* @0x800c4416 — the center-control width sums */

#define FILEDOC_PAGES 7          /* u16 @0x800ccd34 = 0xe -> >>1 = 7 (@0x800c7124-30) */

static void fframe(uint16_t p) { frame(p, p); }

/* run one full page-turn animation after its input frame: 10 slide frames + commit +
 * 10 slide frames + return-to-3 frame = 22 (driver 0x800c77bc). */
static void file_turn_settle(void) { idle(22); }

static void file_wave_tests(void)
{
    int i, se0;

    /* (F1) fresh debug-open at tab-select; R1 = the instant FILE launch
     * (@0x80049834-4c: tab=3 + 25c1=2, NO common resets, no SE). */
    re15_inv_set_equipped_slot(0x80);       /* equip regs at the kind-0 defaults */
    re15_inv_set_prev_equip_slot(0x80);
    re15_menu_toggle();
    re15_inv_screen_sync_equip();
    se0 = g_test_core_se_count;
    fframe(RE15_PAD_BIT_R1);
    CHECK(g_inv_screen.tab == 3 && re15_menu_substate() == 2,
          "(F1) R1 -> tab=3 + 25c1=2 (@0x80049834-4c)");
    CHECK(g_inv_screen.item_state == 0, "(F1) 25c2=0 (enter slide)");
    CHECK(g_test_core_se_count == se0, "(F1) tab launch is SILENT (no SE site)");

    /* (F2) enter slide: 30 frames (sltiu 0x1e @0x800c6cdc), deltas @0x800c6d08-60:
     * 25e0-15, 25e6-9, 25d8-7, 25dc-7, 25ea+7, 25f2-8; every slide frame zeroes
     * [0x800c6c94..97] (sw zero @0x800c6cec). Bases: 215/82/126/0x96/166/26. */
    fframe(0);
    CHECK(g_inv_screen.list_x == 200 && g_inv_screen.ecg_y == 73 &&
          g_inv_screen.arms_x == 119 && g_inv_screen.equip_x == 0x96 - 7 &&
          g_inv_screen.tab_base_y == 173 && g_inv_screen.idcard_y == 18,
          "(F2) slide frame 1 deltas: %d/%d/%d/%d/%d/%d",
          g_inv_screen.list_x, g_inv_screen.ecg_y, g_inv_screen.arms_x,
          g_inv_screen.equip_x, g_inv_screen.tab_base_y, g_inv_screen.idcard_y);
    idle(29);
    CHECK(g_inv_screen.list_x == 215 - 30 * 15 && g_inv_screen.ecg_y == 82 - 30 * 9 &&
          g_inv_screen.arms_x == 126 - 30 * 7 && g_inv_screen.equip_x == 0x96 - 30 * 7 &&
          g_inv_screen.tab_base_y == 166 + 30 * 7 && g_inv_screen.idcard_y == 26 - 30 * 8,
          "(F2) slide end after 30 frames (30*{-15,-9,-7,-7,+7,-8})");
    CHECK(g_inv_screen.item_state == 0, "(F2) 25c2 still 0 on the last slide frame");
    fframe(0);
    CHECK(g_inv_screen.item_state == 1 && re15_menu_item_c3() == 0,
          "(F2) transition frame -> 25c2=1 + 25c3=0 (@0x800c6d64-6e20)");
    CHECK(g_inv_screen.file_sub == 0 && g_inv_screen.file_page == 0 &&
          g_inv_screen.file_row == 0 && g_inv_screen.file_reader_page == 0,
          "(F2) [0x800c6c94..97] zeroed by the slide (sw zero @0x800c6cec)");

    /* (F3) page nav @0x800c6dbc: RIGHT wraps 0->1->2->0 (sltiu 3 @0x800c6e78-84),
     * LEFT wraps 0->2 (bgez @0x800c6e3c-48); each with SE(4,4). */
    se0 = g_test_core_se_count;
    fframe(RE15_PAD_BIT_RIGHT);
    CHECK(g_inv_screen.file_page == 1, "(F3) Right -> page 1 (@0x800c6e64-88)");
    CHECK(g_test_core_se_count == se0 + 1 && g_test_core_se_last == 4,
          "(F3) SE(4,4) (lui 0x404 @0x800c6e8c)");
    fframe(RE15_PAD_BIT_RIGHT); fframe(RE15_PAD_BIT_RIGHT);
    CHECK(g_inv_screen.file_page == 0, "(F3) Right x3 wraps to 0");
    fframe(RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.file_page == 2 && g_test_core_se_last == 4,
          "(F3) Left wraps 0 -> 2 (@0x800c6e3c-48) + SE(4,4)");
    fframe(RE15_PAD_BIT_RIGHT);                 /* back to page 0 for (F4) */
    CHECK(g_inv_screen.file_page == 0, "(F3) back on page 0");

    /* (F4) list display list, page 0 (mask 0x0001 @0x800c6c98): row 0 = the name
     * (12 glyphs "Chris' Diary", a3=0 -> clut row 0), rows 1-9 = 21 underscores each
     * (a3=0x30 -> clut row 6), title "Files" 5 glyphs (a3=0x10 -> clut row 2), the
     * 19x19 tab icon uv(0x72,0x8e) clut @0x800c7420[0]=0x7a90=UI2, the page-level
     * highlight TILE (0x11,0x19) 140x26 (@0x800c7480-98). */
    {
        static re15_inv_op_t ops[RE15_INV_MAX_OPS];
        int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
        int n_name = 0, n_us = 0, n_title = 0, n_icon = 0, n_tile = 0;
        for (i = 0; i < n; i++) {
            if (ops[i].kind == RE15_INV_OP_TILE) {
                n_tile++;
                CHECK(ops[i].x == 0x11 && ops[i].y == 0x19 &&
                      ops[i].w == 0x8c && ops[i].h == 0x1a &&
                      ops[i].r == 0x20 && ops[i].abe == 1,
                      "(F4) page-level highlight (17,25) 140x26 subtractive rgb 0x20 "
                      "(@0x800c7464-98)");
            }
            if (ops[i].kind != RE15_INV_OP_SPRT) continue;
            if (ops[i].page == RE15_INV_PAGE_FONT4) {
                if (ops[i].clut == RE15_INV_CLUT_TEXROW0) n_name++;
                if (ops[i].clut == RE15_INV_CLUT_TEXROW6) n_us++;
                if (ops[i].clut == RE15_INV_CLUT_TEXROW2) n_title++;
            }
            if (ops[i].page == RE15_INV_PAGE_TEX4 && ops[i].w == 0x13 &&
                ops[i].h == 0x13 && ops[i].u == 0x72 && ops[i].v == 0x8e) {
                n_icon++;
                CHECK(ops[i].x == 0x12 && ops[i].y == 0x1a && ops[i].clut == 2,
                      "(F4) tab icon at (18,26) clut UI2=0x7a90 (@0x800c73b8-f0/7420)");
            }
        }
        CHECK(n_name == 12, "(F4) row 0 'Chris' Diary' = 12 glyphs clut row 0, is %d", n_name);
        CHECK(n_us == 9 * 21, "(F4) 9 hidden rows x 21 underscores clut row 6 "
              "(@0x800c7916, a3=0x30 @0x800c731c), is %d", n_us);
        CHECK(n_title == 5, "(F4) title 'Files' = 5 glyphs clut row 2 (a3=0x10 "
              "@0x800c729c), is %d", n_title);
        CHECK(n_icon == 1, "(F4) exactly one tab icon");
        CHECK(n_tile == 1, "(F4) exactly one highlight TILE");
    }
    /* page 1 (mask 0xffff): all 10 rows are names; rows 0x59-0x5b carry the '&'
     * digraph 0x64 -> SIGNED pair-table read @0x800c4440 = codes {9,8} -> the
     * FUN_80028ec4 printer runs CENTER (eating 2 raw bytes) + NEWLINE — the row
     * bleeds one 16px line DOWN (byte-true quirk; @0x800131c0-c4 + @0x80028fe8).
     * Row 7 (id 0x59) sits at y=0x35+7*16=0xc5 -> its tail lands at y=0xd5. */
    fframe(RE15_PAD_BIT_RIGHT);                 /* page 1 */
    {
        static re15_inv_op_t ops[RE15_INV_MAX_OPS];
        int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
        int n_us = 0, bleed = 0;
        for (i = 0; i < n; i++) {
            if (ops[i].kind != RE15_INV_OP_SPRT || ops[i].page != RE15_INV_PAGE_FONT4)
                continue;
            if (ops[i].clut == RE15_INV_CLUT_TEXROW6) n_us++;
            if (ops[i].clut == RE15_INV_CLUT_TEXROW0 && ops[i].y == 0xc5 + 0x10) bleed++;
        }
        CHECK(n_us == 0, "(F4) page 1 mask 0xffff -> no underscores, is %d", n_us);
        CHECK(bleed > 0, "(F4) '&' digraph newline-bleed at y=0xd5 present (row 0x59)");
    }
    fframe(RE15_PAD_BIT_LEFT);                  /* back to page 0 */

    /* (F5) row select @0x800c7010: SQUARE enters (SE(4,6) + sub=1 + row=0
     * @0x800c6ea0-bc); DOWN/UP wrap 10 (@0x800c7098-70fc) with SE(4,4); CROSS backs
     * out (SE(4,5) + sub=0 @0x800c707c-8c). The mask is NOT checked. */
    fframe(RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.file_sub == 1 && g_inv_screen.file_row == 0 &&
          g_test_core_se_last == 6,
          "(F5) SQUARE -> row select + SE(4,6) (@0x800c6ea0-bc)");
    fframe(RE15_PAD_BIT_DOWN); fframe(RE15_PAD_BIT_DOWN); fframe(RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.file_row == 3 && g_test_core_se_last == 4,
          "(F5) Down x3 -> row 3 + SE(4,4)");
    {   /* row-level highlight bar (0x2b, 0x34+row*16) 154x16 (@0x800c749c-c0) */
        static re15_inv_op_t ops[RE15_INV_MAX_OPS];
        int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
        int ok = 0;
        for (i = 0; i < n; i++)
            if (ops[i].kind == RE15_INV_OP_TILE &&
                ops[i].x == 0x2b && ops[i].y == 0x34 + 3 * 16 &&
                ops[i].w == 0x9a && ops[i].h == 0x10) ok = 1;
        CHECK(ok, "(F5) row-3 highlight bar (43,100) 154x16");
    }
    for (i = 0; i < 7; i++) fframe(RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.file_row == 0, "(F5) Down wraps 9 -> 0 (sltiu 0xa @0x800c70b4-c4)");
    fframe(RE15_PAD_BIT_UP);
    CHECK(g_inv_screen.file_row == 9, "(F5) Up wraps 0 -> 9 (bgez @0x800c70ec-f8)");
    fframe(RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.file_sub == 0 && g_test_core_se_last == 5,
          "(F5) CROSS -> back to page level + SE(4,5) (@0x800c707c-8c)");

    /* (F6) reader open: SQUARE (rows) + SQUARE (open @0x800c704c-70: SE(4,6), 25c2=3,
     * reader page 0, bob word zeroed). Selection-INDEPENDENT: any row opens the one
     * embedded document. */
    fframe(RE15_PAD_BIT_SQUARE);
    fframe(RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 3 && g_inv_screen.file_reader_page == 0 &&
          g_test_core_se_last == 6,
          "(F6) reader open: 25c2=3 + page 0 + SE(4,6) (@0x800c704c-70)");
    /* one reader frame: page-0 display list = 5 newlines then the CENTERED title line
     * (control 9 op 0x78 @doc: x = 0x28 + 0x78 - width('Operation Report')/2, glyphs
     * at y = 0x20 + 5*16 = 0xa0), footer '1/7' (3 glyphs clut row 4, y=0xd2, centered
     * on 0xa0 @0x800c778c-98), right arrow only (page 0: no left @0x800c7554) at
     * (0x11c, 0x70) uv(0x70,0x48) clut UI7 (type 1 @0x800c7594). */
    fframe(0);
    {
        static const uint8_t title_codes[16] = {   /* doc bytes @0x800ccd45-54 */
            0x2b,0x4c,0x41,0x4e,0x3d,0x50,0x45,0x4b,0x4a,0x00,
            0x2e,0x41,0x4c,0x4b,0x4e,0x50 };
        static re15_inv_op_t ops[RE15_INV_MAX_OPS];
        int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
        int wsum = 0, x_exp, n_txt = 0, n_foot = 0, n_arrow_l = 0, n_arrow_r = 0;
        int first_x = -1, first_y = -1;
        for (i = 0; i < 16; i++) wsum += re15_font_width[title_codes[i]];
        x_exp = 0x28 + 0x78 - wsum / 2;
        for (i = 0; i < n; i++) {
            if (ops[i].kind != RE15_INV_OP_SPRT) continue;
            if (ops[i].page == RE15_INV_PAGE_FONT4) {
                if (ops[i].clut == RE15_INV_CLUT_TEXROW0) {
                    if (n_txt == 0) { first_x = ops[i].x; first_y = ops[i].y; }
                    n_txt++;
                }
                if (ops[i].clut == RE15_INV_CLUT_TEXROW4) n_foot++;
            }
            if (ops[i].page == RE15_INV_PAGE_TEX4 && ops[i].w == 16 && ops[i].h == 16) {
                if (ops[i].u == 0x70 && ops[i].v == 0x38) n_arrow_l++;
                if (ops[i].u == 0x70 && ops[i].v == 0x48) {
                    n_arrow_r++;
                    CHECK(ops[i].x == 0x11c && ops[i].y == 0x70 && ops[i].clut == 7,
                          "(F6) right arrow (0x11c,0x70) clut UI7=0x7bd0 (type 1 "
                          "@0x800c7594/@0x800c7734)");
                }
            }
        }
        CHECK(n_txt == 16, "(F6) page 0 = 16 title glyphs, is %d", n_txt);
        CHECK(first_y == 0x70, "(F6) title line y = 0x20+5*16 = 0x70 (controls 08 x5), "
              "is %d", first_y);
        CHECK(first_x == x_exp, "(F6) CENTER control x = 0x28+0x78-w/2 = %d "
              "(@0x80028fe8-9010), is %d", x_exp, first_x);
        CHECK(n_foot == 3, "(F6) footer '1/7' = 3 glyphs clut row 4 (@0x800c7744), is %d",
              n_foot);
        CHECK(n_arrow_l == 0 && n_arrow_r == 1,
              "(F6) page 0: right arrow only (@0x800c7554 gate)");
    }

    /* (F7) bob cycle @0x800c75ac-e4: offset 0 for the first 31 reader frames (counter
     * hits 0x1e on frame 31 AFTER drawing), 4 from frame 32, reset after 0x3c. */
    idle(29);                                    /* frames 2..30 of state 3 */
    CHECK(g_inv_screen.file_bob_off == 0, "(F7) bob offset 0 through frame 30");
    fframe(0);                                   /* frame 31: draws 0, then off=4 */
    CHECK(g_inv_screen.file_bob_off == 0, "(F7) frame 31 still draws offset 0");
    fframe(0);                                   /* frame 32 */
    CHECK(g_inv_screen.file_bob_off == 4, "(F7) frame 32 draws offset 4 (ctr 0x1e hit)");

    /* (F8) forward page turn @0x800c7190-71e0 + driver 0x800c77bc: RIGHT on page 0
     * (0+1 != end) -> 25c2=6, phase 0, x=0xc, SE(4,8). Then 10 frames x -= 28
     * (@0x800c77fc), commit frame: page 1 + x=320 + state 7 (@0x800c7868-78), 10
     * frames x -= 28 -> 40, return frame -> state 3 (@0x800c787c). */
    se0 = g_test_core_se_count;
    fframe(RE15_PAD_BIT_RIGHT);
    CHECK(g_inv_screen.item_state == 6 && g_inv_screen.file_anim_phase == 0 &&
          g_inv_screen.file_text_x == 0xc,
          "(F8) fwd turn armed: 25c2=6, regs {0,0xc} (@0x800c71b4-d0)");
    CHECK(g_test_core_se_count == se0 + 1 && g_test_core_se_last == 8,
          "(F8) SE(4,8) (lui 0x408 @0x800c71d4)");
    fframe(0);
    CHECK(g_inv_screen.item_state == 6 && g_inv_screen.file_text_x == 0xc - 28 &&
          g_inv_screen.file_anim_phase == 1,
          "(F8) slide frame 1: x=-16 (x-=28 @0x800c77fc), is x=%d",
          g_inv_screen.file_text_x);
    idle(9);
    CHECK(g_inv_screen.file_text_x == 0xc - 280 && g_inv_screen.file_anim_phase == 10 &&
          g_inv_screen.item_state == 6, "(F8) half 1 done: x=-268 phase 10");
    fframe(0);
    CHECK(g_inv_screen.item_state == 7 && g_inv_screen.file_reader_page == 1 &&
          g_inv_screen.file_text_x == 320 && g_inv_screen.file_anim_phase == 0,
          "(F8) commit: page++ + x=0x140 + state 7 (@0x800c7868-78)");
    idle(10);
    CHECK(g_inv_screen.file_text_x == 320 - 280 && g_inv_screen.item_state == 7,
          "(F8) half 2 done: x=40 (= the reader rest x 0x28)");
    fframe(0);
    CHECK(g_inv_screen.item_state == 3 && g_inv_screen.file_anim_phase == 0,
          "(F8) return frame -> state 3 (@0x800c787c-80)");

    /* (F9) backward turn: LEFT on page 1 -> 25c2=4, x=0x44, SE(4,8); commit: page 0 +
     * x=-240 + state 5 (@0x800c7854-64); settle to state 3. */
    fframe(RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.item_state == 4 && g_inv_screen.file_text_x == 0x44 &&
          g_test_core_se_last == 8,
          "(F9) bwd turn armed: 25c2=4, x=0x44 (@0x800c7240-5c) + SE(4,8)");
    idle(10);
    CHECK(g_inv_screen.file_text_x == 0x44 + 280, "(F9) half 1: x=68+280 (x+=28 @0x800c77f0)");
    fframe(0);
    CHECK(g_inv_screen.item_state == 5 && g_inv_screen.file_reader_page == 0 &&
          g_inv_screen.file_text_x == -240,
          "(F9) commit: page-- + x=-240 + state 5 (@0x800c7854-64)");
    idle(10);
    CHECK(g_inv_screen.file_text_x == 40, "(F9) half 2 lands on x=40");
    fframe(0);
    CHECK(g_inv_screen.item_state == 3, "(F9) back in the reader");
    /* LEFT on page 0: sb zero only, NO SE, NO anim (@0x800c7218-24). */
    se0 = g_test_core_se_count;
    fframe(RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.item_state == 3 && g_inv_screen.file_reader_page == 0 &&
          g_test_core_se_count == se0,
          "(F9) Left at page 0 = dead write (sb zero @0x800c7224, no SE)");

    /* (F10) END position: page can reach 7 = one-past-last (@0x800c71a4/71e8); the
     * drawer clamps to 6 (@0x800c7628-34) and the footer shows 7/7; right arrow type 2
     * (clut 0x7b50=UI5 @0x800c7580-84). SQUARE closes ONLY there (@0x800c715c-68);
     * RIGHT there closes too (t1==end+1 @0x800c71ac). */
    for (i = 0; i < 5; i++) {                    /* pages 1..5 via 5 fwd turns */
        fframe(RE15_PAD_BIT_RIGHT); file_turn_settle();
    }
    CHECK(g_inv_screen.file_reader_page == 5, "(F10) on page 5 after 5 fwd turns, is %d",
          g_inv_screen.file_reader_page);
    fframe(RE15_PAD_BIT_RIGHT); file_turn_settle();
    CHECK(g_inv_screen.file_reader_page == 6 && g_inv_screen.item_state == 3,
          "(F10) page 6 (last text page)");
    se0 = g_test_core_se_count;
    fframe(RE15_PAD_BIT_RIGHT);                  /* 6+1 == end -> page=7, SE(4,4), NO anim */
    CHECK(g_inv_screen.file_reader_page == 7 && g_inv_screen.item_state == 3 &&
          g_test_core_se_last == 4 && g_test_core_se_count == se0 + 1,
          "(F10) Right on page 6 -> END position 7 + SE(4,4), no anim (@0x800c71e8-f4)");
    {   /* end-position display: text clamped to page 6, footer '7/7', arrow type 2 */
        static re15_inv_op_t ops[RE15_INV_MAX_OPS];
        int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
        int ok2 = 0, okl = 0;
        for (i = 0; i < n; i++) {
            if (ops[i].kind != RE15_INV_OP_SPRT) continue;
            if (ops[i].page == RE15_INV_PAGE_TEX4 && ops[i].w == 16 && ops[i].h == 16) {
                if (ops[i].u == 0x70 && ops[i].v == 0x48 && ops[i].clut == 5) ok2 = 1;
                if (ops[i].u == 0x70 && ops[i].v == 0x38) okl = 1;
            }
        }
        CHECK(ok2, "(F10) end-position right arrow clut UI5=0x7b50 (type 2 "
              "@0x800c7580-84/@0x800c7734)");
        CHECK(okl, "(F10) left arrow present (page != 0 @0x800c7554)");
    }
    fframe(RE15_PAD_BIT_LEFT);                   /* end -> page-- + SE(4,4), no anim */
    CHECK(g_inv_screen.file_reader_page == 6 && g_inv_screen.item_state == 3 &&
          g_test_core_se_last == 4,
          "(F10) Left at END -> page 6 direct (@0x800c7228-34)");
    fframe(RE15_PAD_BIT_RIGHT);                  /* back to END */
    fframe(RE15_PAD_BIT_SQUARE);                 /* SQUARE at END closes */
    CHECK(g_inv_screen.item_state == 1 && g_test_core_se_last == 5,
          "(F10) SQUARE at END -> close + SE(4,5) (@0x800c715c-84)");
    /* SQUARE elsewhere is dead: reopen, SQUARE on page 0 -> still reading */
    fframe(RE15_PAD_BIT_SQUARE);                 /* rows */
    fframe(RE15_PAD_BIT_SQUARE);                 /* reader, page 0 */
    se0 = g_test_core_se_count;
    fframe(RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 3 && g_test_core_se_count == se0,
          "(F10) SQUARE off the END position is dead (bne @0x800c7168)");
    fframe(RE15_PAD_BIT_CROSS);                  /* CROSS closes from anywhere */
    CHECK(g_inv_screen.item_state == 1 && g_test_core_se_last == 5,
          "(F10) CROSS closes the reader + SE(4,5) (@0x800c7170-84)");

    /* (F11) exit. BYTE-TRUE: the reader close writes ONLY 25c2 (@0x800c7170-84) —
     * [0x800c6c94] stays 1, so the list returns at the ROW-SELECT level; the first
     * CROSS backs out to page level (@0x800c707c-8c), the second exits. Exit =
     * SE(4,5) + 25c2=2 (@0x800c6e00-20); 30-frame reverse slide restores every base;
     * exit contract @0x800c6f74-8c (25ca=0, 25c1=0, 25c2=0, 25c3=0, tab kept). */
    CHECK(g_inv_screen.file_sub == 1,
          "(F11) after reader close the list is at ROW level (6c94 untouched)");
    fframe(RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.file_sub == 0 && g_inv_screen.item_state == 1,
          "(F11) CROSS -> back to page level first");
    fframe(RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 2 && g_test_core_se_last == 5,
          "(F11) CROSS -> exit slide + SE(4,5)");
    idle(30);                                    /* 30 slide frames (sltiu 0x1e) */
    CHECK(g_inv_screen.item_state == 2, "(F11) still sliding on frame 30");
    fframe(0);                                   /* contract frame @0x800c6f74-8c */
    CHECK(re15_menu_substate() == 0 && g_inv_screen.item_state == 0 &&
          re15_menu_item_c3() == 0 && g_inv_screen.highlight == 0 &&
          g_inv_screen.tab == 3,
          "(F11) exit contract @0x800c6f74-8c: 25ca=0 25c1=0 25c2=0 25c3=0, tab kept");
    CHECK(g_inv_screen.list_x == 215 && g_inv_screen.ecg_y == 82 &&
          g_inv_screen.arms_x == 126 && g_inv_screen.equip_x == 0x96 &&
          g_inv_screen.tab_base_y == 166 && g_inv_screen.idcard_y == 26,
          "(F11) reverse slide restores all bases exactly");

    /* (F12) R1 also closes the list (the 0x8 branch @0x800c6df4-df8 falls into the
     * CROSS close @0x800c6e00); tab-3 confirm re-enters (25c1=2 @0x800499c8-cc). */
    fframe(RE15_PAD_BIT_SQUARE);                 /* tab-3 confirm (virtual 0x4000 <- raw
                                                  * SQUARE, wave-6 f4) re-enters FILE */
    CHECK(re15_menu_substate() == 2, "(F12) tab-3 confirm re-enters FILE (@0x800499c8-cc)");
    idle(31);                                    /* slide + transition -> list */
    CHECK(g_inv_screen.item_state == 1, "(F12) list after slide");
    fframe(RE15_PAD_BIT_R1);
    CHECK(g_inv_screen.item_state == 2 && g_test_core_se_last == 5,
          "(F12) R1 closes too (@0x800c6df4-6e20)");
    idle(31);                                    /* 30 slides + contract frame */
    CHECK(re15_menu_substate() == 0 && g_inv_screen.tab == 3,
          "(F12) second session exits clean");
    re15_menu_toggle();                          /* close the debug session */
    CHECK(!re15_menu_is_open(), "(F12) closed");
}

int main(void)
{
    printf("=== status-screen FSM (wave 2, spec shots/inv_wave2_spec.md) ===\n");

    re15_fade_init();
    g_current_room_id = 0x1140;          /* stage 0 room 0x14 (lh 0x800b0fe0/0fe2) */
    wavemap_tests();                     /* MAP-wave unit block (tables + marker math) */
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

    /* (5) MAP wave — L1 = instant MAP launch (tab=1 AND 25c1=1, @0x8004980c-30; jumps
     * to the entry dispatch @0x8004997c: per-stage init + CD-load spawn). Then the
     * FUN_8004c058 4-state FSM: 25-frame slide-out (sltiu 0x19 @0x8004c0bc) ->
     * upload+arena frame (c2 0->1 @0x8004c204-210) -> interactive -> cancel ->
     * 25-frame reverse -> exit contract @0x8004c2f0-304. */
    frame(RE15_PAD_BIT_L1, RE15_PAD_BIT_L1);
    CHECK(g_inv_screen.tab == 1 && re15_menu_substate() == 1, "(5) L1 -> tab=1 + 25c1=1");
    /* entry init for ROOM1140 = stage 0, room 0x14=20: FUN_8004b568 case 20 (rooms
     * 18-22 tail @0x8004b754-758) -> 260d=20, 260e=4 */
    CHECK(re15_inv_map_room() == 20 && re15_inv_map_page() == 4,
          "(5) ROOM1140 entry init -> 260d=20 260e=4 (@0x8004b754-758), are %d/%d",
          re15_inv_map_room(), re15_inv_map_page());
    /* slide-out: 25 frames, deltas @0x8004c0c8-154 (25e0+15, 25e6+9, 25e4-9, 25de-7,
     * 25da-7, 25f0-8, 25ea+7). Bases: 25e0=215, 25e6=82, 25e4=13, 25de=0x3a (knife
     * equipped, kind 0), 25da=26, 25f0=14, 25ea=166. */
    frame(0, 0);
    CHECK(g_inv_screen.list_x == 230 && g_inv_screen.ecg_y == 91 &&
          g_inv_screen.cond_x == 4 && g_inv_screen.equip_y == 0x3a - 7 &&
          g_inv_screen.arms_y == 19 && g_inv_screen.idcard_x == 6 &&
          g_inv_screen.tab_base_y == 173,
          "(5) slide frame 1 deltas (@0x8004c0c8-154): %d/%d/%d/%d/%d/%d/%d",
          g_inv_screen.list_x, g_inv_screen.ecg_y, g_inv_screen.cond_x,
          g_inv_screen.equip_y, g_inv_screen.arms_y, g_inv_screen.idcard_x,
          g_inv_screen.tab_base_y);
    CHECK(g_inv_screen.item_state == 0, "(5) still sliding: 25c2=0");
    idle(24);
    CHECK(g_inv_screen.list_x == 215 + 25 * 15 && g_inv_screen.ecg_y == 82 + 25 * 9 &&
          g_inv_screen.cond_x == 13 - 25 * 9 && g_inv_screen.equip_y == 0x3a - 25 * 7 &&
          g_inv_screen.arms_y == 26 - 25 * 7 && g_inv_screen.idcard_x == 14 - 25 * 8 &&
          g_inv_screen.tab_base_y == 166 + 25 * 7,
          "(5) slide end after 25 frames (25*{15,9,-9,-7,-7,-8,7})");
    CHECK(g_inv_screen.item_state == 0, "(5) c2 still 0 on the last slide frame");
    /* upload frame: c2 0->1 (@0x8004c1c4/0x8004c204-210), c3=0 (@0x8004c1cc); the
     * draw gate (word==0x00010100 @0x80049bb4-cc) turns on THIS frame and the marker
     * is recomputed (FUN_800473f8) — room slot 20 scale row @0x800768B0+160 =
     * {0,0,1,1} (raw dump), player world (0,0): t=(32000*10*1)>>20=0, +5=5, /10=0
     * -> mx=0; t2 likewise, negated (-5/10=0) -> my=0. */
    frame(0, 0);
    CHECK(g_inv_screen.item_state == 1 && re15_menu_item_c3() == 0,
          "(5) upload frame -> 25c2=1 + 25c3=0 (@0x8004c204-210/0x8004c1cc)");
    CHECK(g_inv_screen.substate == 1, "(5) substate mirror for the draw gate");
    CHECK(g_inv_screen.map_marker_x == 0 && g_inv_screen.map_marker_y == 0,
          "(5) marker for slot-20 row {0,0,1,1} at world (0,0) = (0,0), is (%d,%d)",
          g_inv_screen.map_marker_x, g_inv_screen.map_marker_y);
    /* display list while the gate is on: the MAP set (marker on TEX4 + 2 fixed
     * sprites + page-4 rect count 7 on the MAP page) + the 4 g11 screws. */
    {
        static re15_inv_op_t ops[RE15_INV_MAX_OPS];
        int n = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
        int i, nmap = 0, nscrew = 0, have_marker = 0, have_s1 = 0, have_s2 = 0;
        for (i = 0; i < n; i++) {
            if (ops[i].kind != RE15_INV_OP_SPRT) continue;
            if (ops[i].page == RE15_INV_PAGE_MAP4) {
                nmap++;
                CHECK(ops[i].clut == RE15_INV_CLUT_TEXROW21,
                      "(5) MAP-page op clut must be row 21 (0x7d50 @0x800473cc)");
                if (ops[i].x == 0x1e && ops[i].y == 0x1e && ops[i].w == 0x58 &&
                    ops[i].h == 0x20 && ops[i].u == 0 && ops[i].v == 0) have_s1 = 1;
                if (ops[i].x == 0x10e && ops[i].y == 0x28 && ops[i].w == 0x20 &&
                    ops[i].h == 0x30 && ops[i].u == 0x60 && ops[i].v == 0) have_s2 = 1;
            }
            if (ops[i].page == RE15_INV_PAGE_TEX4 && ops[i].w == 8 && ops[i].h == 8 &&
                ops[i].u == 224 && ops[i].v == 128 && ops[i].clut == 6)
                have_marker = 1;
            if (ops[i].page == RE15_INV_PAGE_TEX4 && ops[i].w == 16 && ops[i].h == 16 &&
                ops[i].u == 112)
                nscrew++;
        }
        CHECK(nmap == 2 + 7, "(5) MAP-page ops = 2 sprites + 7 page-4 rects "
              "(count @0x80076840+4*8), is %d", nmap);
        CHECK(have_s1 && have_s2, "(5) fixed sprites (30,30)88x32 uv(0,0) + "
              "(270,40)32x48 uv(96,0) (@0x80047204-2c0)");
        CHECK(have_marker, "(5) marker quad 8x8 uv(224,128) clut 6 on the TEX page "
              "(@0x80047130-50/@0x8004714c)");
        CHECK(nscrew == 4, "(5) 4 g11 screws uv(112,..) (case 0xb @0x80048010-38, "
              "tmpl @0x80075630), is %d", nscrew);
        /* gate OFF -> no MAP set, no screws (word check @0x80049bb4-cc/@0x8004801c-2c) */
        {
            re15_inv_screen_t off = g_inv_screen;
            off.item_state = 0;
            n = re15_inv_screen_build(&off, ops, RE15_INV_MAX_OPS);
            for (i = 0, nmap = 0, nscrew = 0; i < n; i++) {
                if (ops[i].page == RE15_INV_PAGE_MAP4) nmap++;
                if (ops[i].page == RE15_INV_PAGE_TEX4 && ops[i].kind == RE15_INV_OP_SPRT &&
                    ops[i].w == 16 && ops[i].h == 16 && ops[i].u == 112) nscrew++;
            }
            CHECK(nmap == 0 && nscrew == 0, "(5) gate off -> no map ops/screws");
        }
    }
    /* interactive: only virtual cancel 0x8000 (@0x8004c1d0-e0) or raw L1 0x4
     * (@0x8004c1e8-f8) do anything; other pads are dead (no pan/step). */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 1, "(5) Up/CROSS dead in interactive state");
    frame(RE15_PAD_BIT_L1, RE15_PAD_BIT_L1);      /* L1 toggles out too (@0x8004c1f4) */
    CHECK(g_inv_screen.item_state == 2, "(5) L1 -> c2=2 (reverse slide)");
    /* reverse slide 25 frames (deltas mirrored @0x8004c240-2b8) -> exit contract */
    idle(25);
    CHECK(g_inv_screen.list_x == 215 && g_inv_screen.ecg_y == 82 &&
          g_inv_screen.cond_x == 13 && g_inv_screen.equip_y == 0x3a &&
          g_inv_screen.arms_y == 26 && g_inv_screen.idcard_x == 14 &&
          g_inv_screen.tab_base_y == 166,
          "(5) reverse slide restores all bases exactly");
    CHECK(g_inv_screen.item_state == 2, "(5) c2 still 2 on the last reverse frame");
    frame(0, 0);
    CHECK(re15_menu_substate() == 0 && g_inv_screen.tab == 1 &&
          g_inv_screen.item_state == 0 && re15_menu_item_c3() == 0 &&
          g_inv_screen.highlight == 0,
          "(5) exit contract @0x8004c2f0-304: 25ca=0 25c1=0 25c2=0 25c3=0, tab kept");
    /* re-enter via tab confirm (25c1=1 @0x80049968-78 + the same entry dispatch),
     * cancel out via SQUARE during interactive (@0x8004c1d0-e0) */
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(re15_menu_substate() == 1, "(5) tab-1 confirm re-enters MAP");
    idle(26);
    CHECK(g_inv_screen.item_state == 1, "(5) interactive after slide+upload");
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
    CHECK(g_inv_screen.item_state == 2, "(5) virtual cancel -> c2=2");
    idle(26);
    CHECK(re15_menu_substate() == 0 && g_inv_screen.tab == 1,
          "(5) second session exits clean");

    /* (6) confirm ITEM: Up -> tab 0; virtual CROSS -> 25ca=1 (@0x800498e4-f4, the ONE
     * set-to-1 site), cursors reset (@0x800498f8-918), 25c1=3 (@0x80049960-64),
     * ITEM state 0 = entry slide. */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP);
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(g_inv_screen.item_state == 1, "(10) confirm on empty must stay in GRID");

    /* (11) grid confirm on the Browning (slot 1): 25d6:=0 + 25c2:=3 (@0x800c64c0-c8),
     * command-cluster slide-in 25ee 264->166 (-14/frame, @0x8004a41c-454) -> state 4. */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP); idle(1);       /* 4 -> 2 */
    frame(RE15_PAD_BIT_UP, RE15_PAD_BIT_UP); idle(1);       /* 2 -> 0 */
    frame(RE15_PAD_BIT_RIGHT, RE15_PAD_BIT_RIGHT);          /* 0 -> 1 (Browning) */
    CHECK(g_inv_screen.item_cursor == 1, "(11) cursor on Browning");
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
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
        frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
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
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);          /* ITEM */
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);          /* tab ITEM confirm */
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);          /* confirm DURING typing */
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
    frame(RE15_PAD_BIT_CROSS, RE15_PAD_BIT_CROSS);        /* cancel edge dismisses too */
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
    frame(RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
    CHECK(!re15_menu_msg_active(), "(23) dismissed on the confirm edge");
    frame(0, 0);
    CHECK(g_inv_screen.item_state == 1 && re15_menu_item_c3() == 0 && re15_menu_item_c4() == 0,
          "(23) grid + terminal invariant; ammo intact: %02x", g_inv.slots[2].id);
    CHECK(g_inv.slots[2].id == 0x15, "(23) bullets NOT consumed");
    re15_menu_toggle();                                     /* close the debug session */
    CHECK(!re15_menu_is_open(), "(23) closed");

    /* ====================== WAVE 5 (spec shots/inv_wave5_spec.md) ====================== */
    /* EXCHANGE/combine: command dispatch [3] @0x8004a570 -> 25c2=7; state 7 = per-frame
     * FUN_8004b33c (sub table @0x80074c44: [0]=0x8004b37c select, [1]=0x8004b408 result
     * anim). All expectations below come from the EMBEDDED blob (pair lists / prop rows)
     * and the fresh wave-5 disasm cited in menu_common.c. */
    wave5_tests();

    /* ====================== FILE wave (DEBUG.BIN FUN_800c6ca0) ======================== */
    file_wave_tests();

    if (fails) { fprintf(stderr, "\nINV FSM TEST: %d FAILURES\n", fails); return 1; }
    printf("PASS: status-screen FSM byte-true (open stages + tab FSM + ITEM slides "
           "+/-14 + grid nav w/ auto-repeat 15/4 + command stage + close commit; "
           "wave 3: equip/unequip/swap per-step anims + 25c9 stale-gate 0x5c quirk + "
           "prompt-less heal (+25 Green) + cant-use message; "
           "wave 5: EXCHANGE pair engine — full 21-pair herb graph + 12 crafts + "
           "reload clamps both directions + self-stack + GLOCK 06->04 quirk + "
           "17-step result anim + 2nd-cursor nav + dormant action 4; "
           "FILE wave: 30f slides + 3-page list w/ masks + row select + reader open + "
           "22f page-turn curve + END-position closes + SE(4,4/5/6/8) sites + exit "
           "contract)\n");
    return 0;
}
