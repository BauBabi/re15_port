/*
 * RE1.5 Rebuilt — the STATUS/INVENTORY screen FSM (wave 2, spec shots/inv_wave2_spec.md).
 *
 * Byte-true port of the original's screen life-cycle (all RE1.5-EXE unless noted):
 *   - OPEN trigger: raw START press-edge sets the request (@0x8001cd64-74), latched when the
 *     player hit-react byte @0x800acae7 == 0 (@0x8001cd94-cdb8); the gameplay task then runs
 *     the transition-stage FSM on DAT_800b5359 (jump table @0x8001069c): stage 1 = screen
 *     fade-out FUN_800217b0(0x200,+0x1800,7,0) (@0x8001ca64-88), stage 2 = hold-black +
 *     2-frame yield + menu-task spawn (@0x8001ca98-cb4c); stages 3-5 = the close-side
 *     gameplay fade-in (@0x8001cbb8-cc94).
 *   - MASTER menu task LAB_8004603c: phase byte DAT_800b25bf -> table @0x80074bdc
 *     {0=init FUN_800460b8, 1=run FUN_80046500, 2=close FUN_80046540}.
 *   - RUN phase, screen 0 (LAB_8004974c): sub-state DAT_800b25c1 {0=tab select, 1=MAP,
 *     2=FILE, 3=ITEM FUN_8004a0cc}; the ITEM mode runs its own FSM on DAT_800b25c2
 *     (slides ±14/frame, grid nav = DEBUG.BIN 0x800c62a0, command stage).
 *   - Gameplay is FULLY SUSPENDED while the menu is open: FUN_80029bf8(0) sets task-0
 *     status |= 0x40 as the FIRST init op (@0x800460bc-c4) — no player/enemy/SCD/AOT/
 *     effects run until FUN_80029c2c(0) resumes at close (@0x8004677c).
 *
 * The screen STATE lives in g_inv_screen (re15_inv_screen.h) — this module IS the
 * original's byte-register writer (25bc tab / 25bd cursor / 25ca dim / 25ea 25ee slides…);
 * the per-frame display list is built from it by re15_inv_screen_build.
 */
#ifndef RE15_MENU_H
#define RE15_MENU_H

#include <stdint.h>

/* Menu task alive (master phases 0..2) — the screen is DRAWN (incl. during its own
 * fade-in/fade-out). The platform draws the inventory instead of the scene while true. */
int  re15_menu_is_open(void);

/* Gameplay fully suspended: open-request latched, transition stages 1-5, or menu open.
 * game_step returns immediately (only the FSM ticks) and the platform 30Hz block
 * (SCD VM / walkers / fx) is gated on this — the byte-true task-suspension model
 * (@0x800460bc task0 |= 0x40; pauseflags |= 0xff000000 during the fades @0x8001cdd4). */
int  re15_menu_gameplay_frozen(void);

/* The gameplay-tail START poll (@0x8001cd64-cde8). Call once per normal gameplay frame
 * BEFORE the frozen check. `hit_react_ok` = the port's hit-react gate (the original
 * gates on u8 @0x800acae7 == 0). The request is STICKY (@0x8001cd7c ori 0x8000): a
 * START pressed during a hit-react opens the menu when the gate clears. */
void re15_menu_start_poll(uint16_t pad_pressed, int hit_react_ok);

/* Advance the transition stages + the menu task one frame (the per-vsync task step).
 * pad_pressed = press-edge word, pad_held = held word (the grid nav reads HELD gated
 * by the D-pad auto-repeat tick, FUN_80030444/DEBUG.BIN 0x800c62c0). */
void re15_menu_fsm_tick(uint16_t pad_pressed, uint16_t pad_held);

/* DEBUG (harnesses only): instant open at tab-select / instant close+commit, skipping
 * the fades and transition stages. RE15_INV_SHOT / RE15_ITEM_USE_TEST. */
void re15_menu_toggle(void);

/* ITEM BOX (RE1.5-hybrid, shots/itembox_spec.md §6 — substate 4 [DESIGN]):
 * world-side open request (the box AOT examine; save-phone precedent) — runs the
 * SHARED byte-true open transition (stage fade-out/hold-black/task spawn) and
 * lands in the box subscreen instead of the tab select. */
void re15_menu_request_box(void);
/* DEBUG/harness: instant box-screen open (fades skipped; RE15_BOX_SHOT/tests). */
void re15_menu_toggle_box(void);

/* Introspection (tests / state log). */
int  re15_menu_stage(void);      /* DAT_800b5359 mirror (0 = normal gameplay)      */
int  re15_menu_phase(void);      /* DAT_800b25bf (0 init / 1 run / 2 close)        */
int  re15_menu_substate(void);   /* DAT_800b25c1 (0 tabs / 1 map / 2 file / 3 item) */
int  re15_menu_item_c3(void);    /* DAT_800b25c3 (state-5 flow selector: 0 classifier /
                                  * 1 equip / 2 unequip / 3 heal / 5 swap / 6 message —
                                  * dispatch table @0x80074c28, wave 3)               */
int  re15_menu_item_c4(void);    /* DAT_800b25c4 (per-flow step/frame counter)       */
int  re15_menu_msg_active(void); /* DAT_800b8520 & 0x80 (cant-use message up)        */

#endif /* RE15_MENU_H */
