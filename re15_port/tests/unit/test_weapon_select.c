/**
 * @file test_weapon_select.c
 * @brief Phase 8.20 — the inventory / weapon-select menu LOGIC (menu_common.c).
 *
 * Headless test of the engine-side menu FSM (open/close, cursor nav, EQUIP). The render half
 * (re15_menu_render, PC main loop) is not exercised here. Byte-true anchors: EQUIP sets the
 * equipped weapon (DAT_800aca5d) exactly as the game's equip-commit @0x80046688; START is owned
 * by re15_menu_toggle (open+tick on one frame must NOT self-close). See RE15_INVENTORY_SUBSYSTEM.md.
 */
#include "re15_menu.h"
#include "re15_damage.h"   /* re15_player_equipped_weapon / re15_player_set_equipped_weapon */
#include "re15_player.h"   /* RE15_PAD_BIT_* */
#include <stdio.h>

int main(void)
{
    int fail = 0;
    printf("=== weapon-select menu (Phase 8.20) ===\n");

    /* Start closed, equipped = the byte-true default (weapon 1 = the handgun). */
    re15_player_set_equipped_weapon(1);
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: menu must start closed\n"); fail = 1; }
    if (re15_menu_count() < 2) { fprintf(stderr, "FAIL: need >=2 weapons to select\n"); fail = 1; }

    /* (1) OPEN: toggle -> open, and the cursor snaps to the currently-equipped weapon (1 -> index 0). */
    re15_menu_toggle();
    if (!re15_menu_is_open()) { fprintf(stderr, "FAIL: (1) toggle must open the menu\n"); fail = 1; }
    if (re15_menu_entry_weapon(re15_menu_cursor()) != 1) {
        fprintf(stderr, "FAIL: (1) cursor must snap to the equipped weapon 1, ist weapon %d\n",
                re15_menu_entry_weapon(re15_menu_cursor())); fail = 1; }

    /* (2) START is owned by the toggle, NOT the tick: ticking START while open must NOT close it. */
    re15_menu_tick(RE15_PAD_BIT_START);
    if (!re15_menu_is_open()) { fprintf(stderr, "FAIL: (2) tick(START) must not close the menu\n"); fail = 1; }

    /* (3) NAV: DOWN moves to entry 1 (weapon 2); UP at top / DOWN at bottom clamp (no wrap). */
    re15_menu_tick(RE15_PAD_BIT_DOWN);
    int c1 = re15_menu_cursor();
    if (c1 != 1) { fprintf(stderr, "FAIL: (3) DOWN must move cursor 0->1, ist %d\n", c1); fail = 1; }
    for (int i = 0; i < 10; i++) re15_menu_tick(RE15_PAD_BIT_DOWN);   /* clamp at the last entry */
    if (re15_menu_cursor() != re15_menu_count() - 1) {
        fprintf(stderr, "FAIL: (3) DOWN must clamp at the last entry, ist %d\n", re15_menu_cursor()); fail = 1; }
    for (int i = 0; i < 10; i++) re15_menu_tick(RE15_PAD_BIT_UP);     /* clamp at the first entry */
    if (re15_menu_cursor() != 0) {
        fprintf(stderr, "FAIL: (3) UP must clamp at entry 0, ist %d\n", re15_menu_cursor()); fail = 1; }

    /* (4) EQUIP: move to entry 1 (weapon 2), press SQUARE -> equipped == 2 AND the menu closes. */
    re15_menu_tick(RE15_PAD_BIT_DOWN);           /* cursor -> 1 */
    int want = re15_menu_entry_weapon(1);        /* weapon 2 */
    re15_menu_tick(RE15_PAD_BIT_SQUARE);
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: (4) EQUIP (SQUARE) must close the menu\n"); fail = 1; }
    if (re15_player_equipped_weapon() != want) {
        fprintf(stderr, "FAIL: (4) EQUIP must set the equipped weapon to %d, ist %d\n",
                want, re15_player_equipped_weapon()); fail = 1; }

    /* (5) CANCEL: open, move the cursor, press CROSS -> closes WITHOUT changing the equipped weapon. */
    int before = re15_player_equipped_weapon();
    re15_menu_toggle();
    re15_menu_tick(RE15_PAD_BIT_DOWN);
    re15_menu_tick(RE15_PAD_BIT_CROSS);
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: (5) CROSS must cancel/close the menu\n"); fail = 1; }
    if (re15_player_equipped_weapon() != before) {
        fprintf(stderr, "FAIL: (5) CANCEL must not change the equipped weapon (%d -> %d)\n",
                before, re15_player_equipped_weapon()); fail = 1; }

    /* (6) tick while closed is a no-op (defensive). */
    re15_menu_tick(RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_DOWN);
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: (6) tick while closed must not open\n"); fail = 1; }

    if (fail) { fprintf(stderr, "\nWEAPON-SELECT MENU TEST FAILED\n"); return 1; }
    printf("PASS: weapon-select menu (open snaps to equipped; START toggle-only; nav clamps; "
           "SQUARE equips+closes; CROSS cancels)\n");
    return 0;
}
