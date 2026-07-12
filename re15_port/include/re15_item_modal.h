/*
 * RE1.5 Rebuilt — Item-get "You got X" presentation MODAL.
 *
 * Byte-true port of FUN_8001db28 (@0x8001db28, state byte DAT_80072d3b, jump table @0x800106b4) —
 * the hardcoded item-pickup presentation. On pickup the game FREEZES (g_pauseflags |= 0xff000000),
 * the picked item's icon ZOOMS+SPINS in (17 frames), FLIPS like a coin (9 frames), the item is
 * GRANTED (deferred to the end, gated on inventory-room), and — on a FULL inventory — the box
 * SHRINKS away (17 frames) with NO grant (the item stays in the world). There is NO "You got the X"
 * TEXT anywhere in the FSM (image-only, like RE2 retail — proven: zero Fnt/MSG calls in the 260-instr
 * disasm, workflow wq41xdnn2). RE'd + arbitrated 2026-07-12 (3 finders + self-verified against the
 * bytes; the exact per-frame corner math is FUN_8001e1c8 scale/rotate + FUN_8004f008 2D-rotate).
 *
 * Faithful-line gaps (flagged, NOT byte-traced): the exact modal TIM (the port reuses the 40×30
 * ITEMALL icon scaled onto the 112×72 quad); the pickup SE (DAT_800b8523→SE not traced — silent,
 * NOT fabricated); the state-5/6 fade-out timing (the port's load/fade DMA gates are 1-frame).
 */
#ifndef RE15_ITEM_MODAL_H
#define RE15_ITEM_MODAL_H

#include <stdint.h>

/* Arm the modal for a just-picked-up item (mirrors LAB_80043328 @0x80043328): records the item +
 * DEFERS the grant to the modal's end. Ignored if a modal is already running (byte-true guard
 * @0x80043334: `if (DAT_80072d3b != 0) return`). `aot_slot` = the g_aot slot to deactivate on
 * confirm; `taken_bit` = the zone-9 flag payload to set on confirm (0 = none). */
void re15_item_modal_start(uint8_t item_type, uint8_t amount, uint8_t taken_bit, int aot_slot);

/* Advance the FSM one 30 Hz game tick (one FUN_8001db28 dispatch). Call ONLY while active, from the
 * 30 Hz gameplay tick — the caller must FREEZE the rest of gameplay while active (byte-true
 * g_pauseflags |= 0xff000000: player move, enemy AI, SCD/event, model anim all halt). */
void re15_item_modal_tick(void);

/* Non-zero while the modal is presenting (== DAT_80072d3b != 0). */
int re15_item_modal_active(void);

/* RENDER query: on return 1, fills the 4 screen corners (TL,TR,BL,BR order, 320×240 screen space) of
 * the item quad for the CURRENT frame + the item type + the visible face (0=front, 1=back). Returns 0
 * when no quad should be drawn this frame (idle / init / the 1-frame grant tail). */
int re15_item_modal_quad(int out_x[4], int out_y[4], uint8_t *out_type, int *out_face);

/* Inspection (tests): the raw state byte (0..8, mirrors DAT_80072d3b) + counters. */
uint8_t re15_item_modal_state(void);
int     re15_item_modal_frame(void);   /* DAT_8008f630 */

#endif /* RE15_ITEM_MODAL_H */
