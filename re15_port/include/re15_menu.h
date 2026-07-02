/*
 * RE1.5 Rebuilt — Inventory menu (Phase 8.20/8.21, 2026-07-02).
 *
 * The g_inv-driven inventory screen: press START (PC: I) to open a paused overlay showing the
 * carried items in the byte-true 2-col × 5-row grid; move the cursor with the D-pad (byte-true
 * column-major ±1-parity / ±2 nav, FUN_800487b0), SQUARE = EQUIP the selected WEAPON (id < 0x15;
 * sets DAT_800aca5d + re-primes its ARMS SE bank) and close, CROSS = close.
 *
 * This is the LOGIC half (engine-side, testable headless); the RENDER half is platform-side
 * (re15_menu_render in the PC main loop, reading the getters below + re15_item_name / g_inv).
 *
 * BYTE-TRUE grounding (RE15_INVENTORY_SUBSYSTEM.md):
 *  - Open on the newly-pressed START edge (state-1 poll @0x8001cd68); pause = an inline game_step gate.
 *  - Grid: 2 cols {4,44} × 5 rows {32,62,92,122,152} (DAT_80076274), panel origin (215,26); 10 cells.
 *  - Cursor nav column-major 2-wide (FUN_800487b0): Right +1 (even & <count-1), Left -1 (odd & >0),
 *    Down +2 (row below exists), Up -2 (>=2); no wrap.
 *  - EQUIP = DAT_800aca5d = item id then FUN_80043d8c (@0x80046688/@0x800466c4); gated to weapons (id<0x15).
 */
#ifndef RE15_MENU_H
#define RE15_MENU_H

#include <stdint.h>

/* Is the inventory open this frame? (game_step pauses gameplay while true.) */
int  re15_menu_is_open(void);

/* Toggle open/closed — call on the START press-edge. On open it snapshots the occupied g_inv slots
 * into the display list and snaps the cursor to the currently-equipped weapon if present. START is
 * owned by the toggle (NOT re15_menu_tick). */
void re15_menu_toggle(void);

/* Per-frame menu input (only acts while open): the byte-true D-pad grid nav, SQUARE = equip the
 * selected weapon (+ re-prime ARMS) and close, CROSS = close. Pass the port's pad_pressed (edge). */
void re15_menu_tick(uint16_t pad_pressed);

/* Getters for the platform renderer. The display list is the occupied g_inv slots, compacted into
 * grid cells 0..count-1 (column-major: even = left col, odd = right col). */
int     re15_menu_count(void);        /* number of displayed items (occupied slots, <= 10) */
int     re15_menu_cursor(void);       /* selected display index 0..count-1                  */
uint8_t re15_menu_disp_id(int i);     /* item id at display index i (0 = none)              */
uint8_t re15_menu_disp_qty(int i);    /* quantity at display index i                        */

#endif /* RE15_MENU_H */
