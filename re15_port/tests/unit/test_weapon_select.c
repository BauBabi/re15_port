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
#include "re15_damage.h"     /* re15_player_equipped_weapon / re15_player_set_equipped_weapon */
#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_inventory.h"  /* re15_inv_load_briefing / re15_item_is_weapon / g_inv */
#include <stdio.h>

int main(void)
{
    int fail = 0;
    printf("=== weapon-select menu (Phase 8.20) ===\n");

    /* (0) byte-true STAGE1 briefing loadout: knife (id 1) + handgun (id 3, 15) + h.gun bullets
     * (id 0x15, 50), savestate-confirmed (DAT_800b10ac). Classification = id-range gate. */
    re15_inv_load_briefing();
    if (g_inv.slots[0].id != 0x01 || g_inv.slots[0].qty != 0 ||
        g_inv.slots[1].id != 0x03 || g_inv.slots[1].qty != 15 ||
        g_inv.slots[2].id != 0x15 || g_inv.slots[2].qty != 50) {
        fprintf(stderr, "FAIL: (0) briefing loadout mismatch: [%02x:%d][%02x:%d][%02x:%d]\n",
                g_inv.slots[0].id, g_inv.slots[0].qty, g_inv.slots[1].id, g_inv.slots[1].qty,
                g_inv.slots[2].id, g_inv.slots[2].qty); fail = 1; }
    /* byte-true: weapon = id < 0x15 (knife 1 + handgun 3 = weapons; h.gun bullets 0x15 = ammo). */
    if (!re15_item_is_weapon(0x01) || !re15_item_is_weapon(0x03) || re15_item_is_weapon(0x15)) {
        fprintf(stderr, "FAIL: (0) is-weapon: knife(1)+handgun(3)=weapon, bullets(0x15)=not\n"); fail = 1; }
    if (re15_item_is_ammo(0x03) || !re15_item_is_ammo(0x15) || !re15_item_is_key(0x22)) {
        fprintf(stderr, "FAIL: (0) is-ammo(0x15)=1, is-ammo(3)=0, is-key(0x22)=1\n"); fail = 1; }
    /* byte-true catalog names */
    if (re15_item_name(0x01)[0] != 'C' /*COMBAT KNIFE*/ || re15_item_name(0x03)[0] != 'B' /*BROWNING*/) {
        fprintf(stderr, "FAIL: (0) item names: 1='%s' 3='%s'\n", re15_item_name(0x01), re15_item_name(0x03)); fail = 1; }
    if (!fail)
        printf("  (0) briefing: KNIFE(1)+BROWNING HP(3,x15)+H.GUN BULLETS(0x15,x50); "
               "weapon=id<0x15, ammo 0x15..0x21, key>=0x22\n");

    /* The menu is g_inv-driven: display list = [0:KNIFE(1), 1:BROWNING HP(3), 2:H.GUN BULLETS(0x15)]. */
    re15_player_set_equipped_weapon(1);          /* equipped = the knife (briefing default) */
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: menu must start closed\n"); fail = 1; }

    /* (1) OPEN: toggle -> open. Byte-true (audit wf_1c0e60d8): the menu is a FIXED capacity-cell grid
     * (count == 10, NOT the occupied count — FUN_800487b0 bounds the cursor by the 0x800b0fbc capacity),
     * items at their fixed slots (0=knife,1=handgun,2=bullets, 3..9 empty), cursor snaps to the equipped
     * weapon (id 1 -> slot 0). Empty cells 3..9 are cursor-reachable but have no id/action. */
    re15_menu_toggle();
    if (!re15_menu_is_open()) { fprintf(stderr, "FAIL: (1) toggle must open the menu\n"); fail = 1; }
    if (re15_menu_count() != 10) {
        fprintf(stderr, "FAIL: (1) count must be 10 (fixed capacity grid), ist %d\n", re15_menu_count()); fail = 1; }
    if (re15_menu_disp_id(0) != 0x01 || re15_menu_disp_id(3) != 0x00) {
        fprintf(stderr, "FAIL: (1) fixed grid: slot0=knife(1) slot3=empty(0), ist 0x%02x/0x%02x\n",
                re15_menu_disp_id(0), re15_menu_disp_id(3)); fail = 1; }
    if (re15_menu_disp_id(re15_menu_cursor()) != 0x01) {
        fprintf(stderr, "FAIL: (1) cursor must snap to the equipped item 1 (knife), ist id 0x%02x\n",
                re15_menu_disp_id(re15_menu_cursor())); fail = 1; }

    /* (2) START is owned by the toggle, NOT the tick: ticking START while open must NOT close it. */
    re15_menu_tick(RE15_PAD_BIT_START);
    if (!re15_menu_is_open()) { fprintf(stderr, "FAIL: (2) tick(START) must not close the menu\n"); fail = 1; }

    /* (3) NAV byte-true 2-col grid (column-major): from 0, RIGHT->1 (handgun), LEFT->0; DOWN 0->2 (bullets);
     * clamps at the last item + at 0 (no wrap). */
    re15_menu_tick(RE15_PAD_BIT_RIGHT);
    if (re15_menu_cursor() != 1 || re15_menu_disp_id(1) != 0x03) {
        fprintf(stderr, "FAIL: (3) RIGHT from 0 -> 1 (handgun 3), cursor=%d id=0x%02x\n",
                re15_menu_cursor(), re15_menu_disp_id(1)); fail = 1; }
    re15_menu_tick(RE15_PAD_BIT_LEFT);
    if (re15_menu_cursor() != 0) { fprintf(stderr, "FAIL: (3) LEFT from 1 -> 0, ist %d\n", re15_menu_cursor()); fail = 1; }
    re15_menu_tick(RE15_PAD_BIT_DOWN);           /* 0 -> 2 (bullets), row below */
    if (re15_menu_cursor() != 2) { fprintf(stderr, "FAIL: (3) DOWN from 0 -> 2, ist %d\n", re15_menu_cursor()); fail = 1; }
    for (int i = 0; i < 6; i++) re15_menu_tick(RE15_PAD_BIT_DOWN);    /* byte-true: DOWN traverses the FULL
                                                                      * capacity grid into the empty cells;
                                                                      * clamps at 8 (8+2=10 not < 10), NOT
                                                                      * at the last occupied item. */
    if (re15_menu_cursor() != 8) { fprintf(stderr, "FAIL: (3) DOWN must clamp at 8 (grid end), ist %d\n", re15_menu_cursor()); fail = 1; }
    for (int i = 0; i < 6; i++) re15_menu_tick(RE15_PAD_BIT_UP);      /* 8 -> 0, clamp at 0 */
    if (re15_menu_cursor() != 0) { fprintf(stderr, "FAIL: (3) UP must clamp at 0, ist %d\n", re15_menu_cursor()); fail = 1; }

    /* (4) EQUIP a WEAPON: cursor to the handgun (index 1, id 3), SQUARE -> equipped == 3 AND the menu closes. */
    re15_menu_tick(RE15_PAD_BIT_RIGHT);          /* -> 1 (handgun) */
    re15_menu_tick(RE15_PAD_BIT_SQUARE);
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: (4) EQUIP (SQUARE) must close the menu\n"); fail = 1; }
    if (re15_player_equipped_weapon() != 0x03) {
        fprintf(stderr, "FAIL: (4) EQUIP handgun must set equipped=3, ist %d\n", re15_player_equipped_weapon()); fail = 1; }

    /* (5) EQUIP a NON-weapon is a NO-OP: cursor to the bullets (index 2, ammo), SQUARE -> equipped UNCHANGED,
     * menu stays open (ammo is not equippable). */
    re15_menu_toggle();                          /* re-open (cursor snaps to the equipped handgun = index 1) */
    re15_menu_tick(RE15_PAD_BIT_LEFT);           /* 1 -> 0 */
    re15_menu_tick(RE15_PAD_BIT_DOWN);           /* 0 -> 2 (bullets, ammo) */
    if (re15_menu_disp_id(re15_menu_cursor()) != 0x15) {
        fprintf(stderr, "FAIL: (5) cursor must be on the bullets (0x15), ist id 0x%02x\n",
                re15_menu_disp_id(re15_menu_cursor())); fail = 1; }
    int eq_before = re15_player_equipped_weapon();
    re15_menu_tick(RE15_PAD_BIT_SQUARE);
    if (!re15_menu_is_open()) { fprintf(stderr, "FAIL: (5) SQUARE on ammo must NOT close (not equippable)\n"); fail = 1; }
    if (re15_player_equipped_weapon() != eq_before) {
        fprintf(stderr, "FAIL: (5) SQUARE on ammo must not change the equipped weapon\n"); fail = 1; }

    /* (5b) CANCEL: CROSS closes without changing the equipped weapon. */
    re15_menu_tick(RE15_PAD_BIT_CROSS);
    if (re15_menu_is_open() || re15_player_equipped_weapon() != eq_before) {
        fprintf(stderr, "FAIL: (5b) CROSS must cancel/close without changing the weapon\n"); fail = 1; }

    /* (6) tick while closed is a no-op (defensive). */
    re15_menu_tick(RE15_PAD_BIT_SQUARE | RE15_PAD_BIT_DOWN);
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: (6) tick while closed must not open\n"); fail = 1; }

    if (fail) { fprintf(stderr, "\nINVENTORY MENU TEST FAILED\n"); return 1; }
    printf("PASS: g_inv inventory menu (byte-true classification + catalog; 2-col grid nav; "
           "equip weapons only [id<0x15]; ammo/cancel no-op)\n");
    return 0;
}
