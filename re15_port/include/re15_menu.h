/*
 * RE1.5 Rebuilt — Inventory / weapon-select menu (Phase 8.20, 2026-07-02).
 *
 * PHASE 1 = a minimal, playable/testable weapon-select. Press START (PC: I) to open a
 * paused overlay listing the weapons; move the cursor (UP/DOWN), confirm with SQUARE to
 * EQUIP (+ re-prime the weapon's ARMS SE bank) and close, or cancel with CROSS.
 *
 * This is the LOGIC half (engine-side, testable headless); the RENDER half is platform-side
 * (re15_menu_render in the PC main loop, reading the getters below).
 *
 * BYTE-TRUE grounding (RE15_INVENTORY_SUBSYSTEM.md, the inventory-weapon-select-re workflow):
 *  - Open on the newly-pressed START edge — mirrors the state-1 START poll @0x8001cd68
 *    (DAT_800ac762 & 0x800); the port reads its own pad_pressed edge (RE15_PAD_BIT_START 0x0008).
 *  - EQUIP = `DAT_800aca5d = inventory[cursor].id` then re-load the ARMS bank via FUN_80043d8c —
 *    the byte-true equip-commit @0x80046688 + @0x800466c4 (same order as room-init FUN_800314b0).
 *  - Gameplay PAUSE while open = an inline game_step gate (the workflow OVERTURNED the "PSX thread
 *    scheduler" attribution as unproven; the inline gate mirrors the stair/dead/grabbed skip pattern).
 * FAITHFUL-LINE (Phase 1, flagged): the OWNED-weapon set is a fixed test list (the general
 * inventory item-id -> weapon-id derivation passes through the undecompilable FUN_800c00a8); the
 * presentation is a debug-text list (the byte-true 2-col grid + ITEMALL icons + sub-action FSM are
 * Phase 2, RE15_INVENTORY_SUBSYSTEM.md §3).
 */
#ifndef RE15_MENU_H
#define RE15_MENU_H

#include <stdint.h>

/* Is the weapon-select menu open this frame? (game_step pauses gameplay while true.) */
int  re15_menu_is_open(void);

/* Toggle the menu open/closed — call on the START press-edge. On open, the cursor snaps to the
 * currently-equipped weapon if it is in the list. START is owned by the toggle (NOT re15_menu_tick),
 * so opening + ticking on the same frame does not immediately close it. */
void re15_menu_toggle(void);

/* Per-frame menu input (only acts while open): UP/DOWN move the cursor (clamped, no wrap),
 * SQUARE = EQUIP the selected weapon (re15_player_set_equipped_weapon + re15_audio_prime_weapon)
 * and close, CROSS = cancel/close. Pass the port's pad_pressed (edge) word. */
void re15_menu_tick(uint16_t pad_pressed);

/* Getters for the platform renderer (re15_menu_render). */
int         re15_menu_count(void);            /* number of selectable weapons  */
int         re15_menu_cursor(void);           /* selected index 0..count-1      */
const char *re15_menu_entry_name(int i);      /* display name of entry i        */
int         re15_menu_entry_weapon(int i);    /* weapon id of entry i (0..21)   */

#endif /* RE15_MENU_H */
