/**
 * @file test_weapon_select.c
 * @brief Inventory classification + the byte-true EQUIP-at-CLOSE commit (wave 2).
 *
 * Wave-2 REWRITE: the old test asserted the port's INVENTED menu behavior (SQUARE =
 * immediate equip + menu close; START owned by a toggle). The RE'd original
 * (spec shots/inv_wave2_spec.md) works differently and the assertions moved:
 *   - equip in-menu only changes the equip RECORD DAT_800b25c8 (grid confirm ->
 *     command stage -> USE @0x8004a674);
 *   - the PLAYER weapon DAT_800aca5d is committed at the CLOSE phase (@0x80046654-88:
 *     0x80 -> id 1 knife) iff the equipped id changed vs the open snapshot
 *     DAT_800b25ce (@0x800465f4-661c);
 *   - START closes (25bf++ @0x800499d0-a18 / @0x800c6560-98), it does not toggle.
 * The full FSM walk lives in test_inv_fsm.c; this test keeps the classification
 * checks + drives the equip commit through the DEBUG toggle (instant open/close).
 */
#include "re15_menu.h"
#include "re15_inv_screen.h"
#include "re15_damage.h"     /* re15_player_equipped_weapon / re15_player_set_equipped_weapon */
#include "re15_player.h"     /* RE15_PAD_BIT_* */
#include "re15_inventory.h"  /* re15_inv_load_briefing / re15_item_is_weapon / g_inv */
#include <stdio.h>

int main(void)
{
    int fail = 0;
    printf("=== weapon-select / equip commit (wave 2) ===\n");

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

    re15_inv_set_equipped_slot(0);       /* DAT_800b25c8 = 0 (knife, briefing default)  */
    re15_player_set_equipped_weapon(1);  /* DAT_800aca5d = 1                            */
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: menu must start closed\n"); fail = 1; }

    /* (1) DEBUG toggle-open (harness path): instant phase-1 tab-select; the open snapshot
     * DAT_800b25ce (@0x8004649c) records the equipped id (knife 1). */
    re15_menu_toggle();
    if (!re15_menu_is_open() || re15_menu_phase() != 1) {
        fprintf(stderr, "FAIL: (1) toggle must open into the run phase\n"); fail = 1; }
    if (g_inv_screen.tab != 0 || g_inv_screen.item_cursor != 0) {
        fprintf(stderr, "FAIL: (1) init cursors: tab=0, grid cursor=0 (@0x800463e0-f8)\n"); fail = 1; }

    /* (2) change the equip RECORD (as the USE bridge does: 25c8 := cursor slot) — the
     * PLAYER weapon must NOT change while the menu is open (commit @0x80046688 is in
     * the close phase). */
    re15_inv_set_equipped_slot(1);       /* the Browning slot */
    re15_inv_screen_sync_equip();
    if (re15_player_equipped_weapon() != 1) {
        fprintf(stderr, "FAIL: (2) DAT_800aca5d must stay 1 while the menu is open\n"); fail = 1; }

    /* (3) toggle-close: the equip-changed check (25ce vs inv[25c8].id @0x800465f4-661c)
     * fires -> commit DAT_800aca5d = 3 (@0x80046654-88). */
    re15_menu_toggle();
    if (re15_menu_is_open()) { fprintf(stderr, "FAIL: (3) toggle must close\n"); fail = 1; }
    if (re15_player_equipped_weapon() != 0x03) {
        fprintf(stderr, "FAIL: (3) close commit must set equipped=3, ist %d\n",
                re15_player_equipped_weapon()); fail = 1; }

    /* (4) unchanged close: open + close without touching 25c8 -> no commit (compare
     * equal @0x800465f4 -> branch to the common teardown @0x800466cc). */
    re15_player_set_equipped_weapon(3);
    re15_menu_toggle();
    re15_menu_toggle();
    if (re15_player_equipped_weapon() != 3) {
        fprintf(stderr, "FAIL: (4) unchanged equip must not re-commit\n"); fail = 1; }

    /* (5) nothing-equipped commit quirk: 25c8=0x80 -> DAT_800aca5d := 1 (the knife;
     * @0x80046668-6c 'bne 0x80 / ori v0,1'). */
    re15_menu_toggle();
    re15_inv_set_equipped_slot(0x80);
    re15_inv_screen_sync_equip();
    re15_menu_toggle();
    if (re15_player_equipped_weapon() != 1) {
        fprintf(stderr, "FAIL: (5) unequip close must map 0x80 -> weapon id 1, ist %d\n",
                re15_player_equipped_weapon()); fail = 1; }

    if (fail) { fprintf(stderr, "\nWEAPON-SELECT TEST FAILED\n"); return 1; }
    printf("PASS: classification + byte-true equip-at-close commit (25c8 record in-menu, "
           "DAT_800aca5d commit @0x80046688 at close; 0x80 -> knife 1)\n");
    return 0;
}
