/**
 * @file test_itembox_screen.c
 * @brief ITEM BOX subscreen FSM (re15_itembox.c + menu_common.c substate 4):
 *        cursor side split (RE2 state 0/1, spec §2.4), EXIT row (panel 2, §2.3),
 *        L1/R1 page flip (§6 DECISION — ≙ RE2's ±5 shoulder keys @0x8007005c),
 *        swap through the FSM, reject -> state 5 message wait, EXIT close with
 *        the shared fade/equip-commit close phase, compaction-on-open (quirk 9).
 *
 * Frame model = the test_inv_fsm harness (start_poll + fsm_tick + ecg_tick +
 * fade_tick per frame).
 */
#include <stdio.h>
#include <string.h>
#include "re15_menu.h"
#include "re15_itembox.h"
#include "re15_inv_screen.h"
#include "re15_inventory.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_fade.h"
#include "re15_actor.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

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
static void press(uint16_t bits) { frame(bits, bits); }

static void close_if_open(void)
{
    if (re15_menu_is_open()) re15_menu_toggle();
    idle(2);
}

int main(void)
{
    printf("=== ITEM BOX screen FSM (RE2 shape, spec §2.3/§2.4 + §6 hybrid) ===\n");

    /* ---- (1) open: substate 4, compaction-on-open (quirk 9), cursor 0 ---- */
    re15_inv_init();
    re15_itembox_init();
    re15_inv_set_equipped_slot(0x80);
    re15_inv_set_prev_equip_slot(0x80);
    g_inv.slots[0].id = 0x01;                       /* knife            */
    g_inv.slots[2].id = 0x15; g_inv.slots[2].qty = 50;  /* hole at slot 1 */
    re15_menu_toggle_box();
    idle(1);
    CHECK(re15_menu_is_open() && re15_menu_substate() == 4,
          "(1) box open = menu substate 4, got open=%d sub=%d",
          re15_menu_is_open(), re15_menu_substate());
    CHECK(g_inv_screen.box_mode == 1, "(1) box_mode set for the builder");
    CHECK(g_inv.slots[1].id == 0x15,
          "(1) compaction-on-open closed the hole (quirk 9: RE2 FUN_80069714 "
          "@0x80068c60 == RE1.5 @0x800464a0), got slot1 %02x", g_inv.slots[1].id);
    CHECK(re15_itembox_screen_panel() == 3 && re15_itembox_screen_state() == 0 &&
          g_inv_screen.item_cursor == 0,
          "(1) panel 3 / state 0 / inventory cursor 0 (RE2 §2.2 'cursor starts 0')");

    /* ---- (2) state-0 nav + EXIT row (RE2 §2.4 state 0 / §2.3 panel 2) ---- */
    press(RE15_PAD_BIT_DOWN);
    CHECK(g_inv_screen.item_cursor == 2, "(2) down +2, got %d", g_inv_screen.item_cursor);
    press(RE15_PAD_BIT_RIGHT);
    CHECK(g_inv_screen.item_cursor == 3, "(2) right +1");
    press(RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.item_cursor == 2, "(2) left -1");
    press(RE15_PAD_BIT_UP);
    CHECK(g_inv_screen.item_cursor == 0, "(2) up -2");
    press(RE15_PAD_BIT_UP);
    CHECK(re15_itembox_screen_panel() == 2,
          "(2) up at cursor<2 -> EXIT row (panel 2), got panel %d",
          re15_itembox_screen_panel());
    CHECK(g_inv_screen.box_side == 2, "(2) box_side mirror = 2 (EXIT)");
    press(RE15_PAD_BIT_DOWN);
    CHECK(re15_itembox_screen_panel() == 3 && g_inv_screen.item_cursor == 1,
          "(2) EXIT row DOWN -> panel 3 with cursor := 1 (RE2 §2.3), got p%d c%d",
          re15_itembox_screen_panel(), g_inv_screen.item_cursor);

    /* ---- (3) side split: confirm -> box side; cancel -> back (RE2 §2.4) ---- */
    press(RE15_PAD_BIT_SQUARE);                    /* virtual confirm 0x4000 */
    CHECK(re15_itembox_screen_state() == 1 && g_inv_screen.box_side == 1,
          "(3) confirm -> state 1 (box side), got %d", re15_itembox_screen_state());
    press(RE15_PAD_BIT_CROSS);                     /* virtual cancel 0x8000  */
    CHECK(re15_itembox_screen_state() == 0,
          "(3) cancel -> state 0 (inventory side), got %d", re15_itembox_screen_state());

    /* ---- (4) page flip: L1/R1 wrap over 4 pages (§6, ≙ RE2 ±5 keys) ---- */
    press(RE15_PAD_BIT_SQUARE);                    /* back to the box side   */
    CHECK(re15_itembox_screen_page() == 0, "(4) page starts 0");
    press(RE15_PAD_BIT_R1);
    CHECK(re15_itembox_screen_page() == 1, "(4) R1 -> page 1");
    press(RE15_PAD_BIT_R1); press(RE15_PAD_BIT_R1); press(RE15_PAD_BIT_R1);
    CHECK(re15_itembox_screen_page() == 0, "(4) R1 x4 wraps to page 0");
    press(RE15_PAD_BIT_L1);
    CHECK(re15_itembox_screen_page() == 3, "(4) L1 wraps back to page 3");
    press(RE15_PAD_BIT_L1); press(RE15_PAD_BIT_L1); press(RE15_PAD_BIT_L1);
    CHECK(re15_itembox_screen_page() == 0, "(4) L1 x4 wraps to page 0");

    /* ---- (5) box-side cell nav bounds (2×4 grid, §6) ---- */
    press(RE15_PAD_BIT_DOWN); press(RE15_PAD_BIT_DOWN);
    press(RE15_PAD_BIT_DOWN); press(RE15_PAD_BIT_DOWN);
    CHECK(re15_itembox_screen_cursor() == 6, "(5) down clamps at 6, got %d",
          re15_itembox_screen_cursor());
    press(RE15_PAD_BIT_RIGHT);
    CHECK(re15_itembox_screen_cursor() == 7, "(5) right -> 7");
    press(RE15_PAD_BIT_UP); press(RE15_PAD_BIT_UP); press(RE15_PAD_BIT_UP);
    CHECK(re15_itembox_screen_cursor() == 1, "(5) up clamps at row 0 (cell 1), got %d",
          re15_itembox_screen_cursor());
    press(RE15_PAD_BIT_LEFT);
    CHECK(re15_itembox_screen_cursor() == 0, "(5) left -> 0");

    /* ---- (6) swap through the FSM: deposit the knife (cursor 1) ---- */
    /* cursor is at inventory slot 1 (the ammo after compaction); move to 0 */
    press(RE15_PAD_BIT_CROSS);                     /* back to inventory side */
    press(RE15_PAD_BIT_LEFT);
    CHECK(g_inv_screen.item_cursor == 0, "(6) cursor on slot 0 (knife)");
    press(RE15_PAD_BIT_SQUARE);                    /* -> box side            */
    press(RE15_PAD_BIT_SQUARE);                    /* confirm = swap (RE2 §2.4
                                                    * state 4: state:=0 + call) */
    CHECK(re15_itembox_screen_state() == 0,
          "(6) swap returns to state 0, got %d", re15_itembox_screen_state());
    CHECK(g_itembox.pages[0].slots[0].id == 0x01,
          "(6) knife deposited into box page 0 slot 0, got %02x",
          g_itembox.pages[0].slots[0].id);
    CHECK(g_inv.slots[0].id == 0x15,
          "(6) inventory compacted after the deposit (ammo to slot 0), got %02x",
          g_inv.slots[0].id);
    close_if_open();

    /* ---- (7) reject -> state 5 message wait -> state 1 (RE2 §2.4 states 4/5) */
    re15_inv_init();
    re15_itembox_init();
    re15_inv_set_equipped_slot(0x80);
    for (int i = 0; i < 10; i++) { g_inv.slots[i].id = 0x24; g_inv.slots[i].qty = 1; }
    g_itembox.pages[0].slots[0].id = 0x0e;
    g_itembox.pages[0].slots[0].qty = 100;
    g_itembox.pages[0].slots[0].flags = RE15_BOX_KIND_WIDE;
    re15_menu_toggle_box();
    idle(1);
    press(RE15_PAD_BIT_SQUARE);                    /* -> box side            */
    press(RE15_PAD_BIT_SQUARE);                    /* swap -> REJECT         */
    CHECK(re15_itembox_screen_state() == 5,
          "(7) 2-cell withdraw with empties==0 -> state 5 (@0x800707dc-ec), got %d",
          re15_itembox_screen_state());
    CHECK(re15_menu_msg_active(),
          "(7) the reject message is up (RE1.5 cant-use infra for RE2's "
          "FUN_8002fe38 site)");
    CHECK(g_inv.slots[0].id == 0x24 && g_itembox.pages[0].slots[0].id == 0x0e,
          "(7) nothing moved on the reject");
    idle(200);                                     /* typewriter completes    */
    CHECK(re15_itembox_screen_state() == 5, "(7) still waiting while msg is up");
    press(RE15_PAD_BIT_SQUARE);                    /* dismiss (PRESS-WAIT)    */
    idle(2);
    CHECK(!re15_menu_msg_active(), "(7) message dismissed");
    CHECK(re15_itembox_screen_state() == 1,
          "(7) msg gone -> back to state 1 (box side, RE2 §2.4 state 5), got %d",
          re15_itembox_screen_state());
    close_if_open();

    /* ---- (8) EXIT close through the shared close phase + equip commit ---- */
    re15_inv_init();
    re15_itembox_init();
    g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 15;   /* handgun          */
    re15_inv_set_equipped_slot(0);
    re15_player_set_equipped_weapon(0x03);
    re15_menu_toggle_box();
    idle(1);
    press(RE15_PAD_BIT_SQUARE);                    /* -> box side            */
    press(RE15_PAD_BIT_SQUARE);                    /* deposit the equipped gun */
    CHECK(re15_inv_equipped_slot() == 0x80,
          "(8) silent auto-unequip (quirk 8), got %02x", re15_inv_equipped_slot());
    press(RE15_PAD_BIT_UP);                        /* cursor 0 -> EXIT row   */
    CHECK(re15_itembox_screen_panel() == 2, "(8) EXIT row reached");
    press(RE15_PAD_BIT_SQUARE);                    /* EXIT confirm -> close  */
    idle(30);                                      /* fade-out + commit + fade-in */
    CHECK(!re15_menu_is_open(), "(8) box screen closed via EXIT");
    CHECK(g_inv_screen.box_mode == 0, "(8) box_mode cleared at teardown");
    CHECK(re15_player_equipped_weapon() == 1,
          "(8) close commit: equip 0x80 -> weapon 1 (the RE1.5 status-screen "
          "close commit shape @0x80046668-88; ≙ RE2's close-time weapon-model "
          "reload, quirk 14), got %d", re15_player_equipped_weapon());
    idle(20);                                      /* stage 4/5 fade-in settle */
    CHECK(re15_menu_stage() == 0, "(8) transition stages settled, got %d",
          re15_menu_stage());

    if (fails) { printf("ITEMBOX SCREEN: %d FAIL\n", fails); return 1; }
    printf("ITEMBOX SCREEN: all checks passed\n");
    return 0;
}
