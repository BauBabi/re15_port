/*
 * RE1.5 Rebuilt — Item-get "You got X" presentation MODAL.
 *
 * Byte-true port of FUN_8001db28 (@0x8001db28, state byte DAT_80072d3b, jump table @0x800106b4) —
 * the hardcoded item-pickup presentation. On pickup the game FREEZES (g_pauseflags |= 0xff000000),
 * the picked item's PICTURE ZOOMS+SPINS in (17 frames), FLIPS like a coin (9 frames), then a MESSAGE
 * BOX opens — "WILL YOU TAKE THE <item>." (Yes/No) if there's room, or "YOU CAN'T CARRY ANY MORE
 * ITEMS" when full — and the item is GRANTED only on Yes (deferred to state 7, gated on
 * inventory-room). On a FULL inventory or a "No" the box SHRINKS away (17 frames) and the item stays
 * in the world. RE'd 2026-07-12 (workflows wq41xdnn2 + wf_9e42f4cb, disasm-verified).
 *
 * The picture is the per-item 112×72 TIM at byte offset `item_id × 0x3000` in ITEM/ITPS.ITP (file id
 * 0x2a — see re15_itps.h), NOT the 40×30 ITEMALL icon. There is NO pickup SE anywhere in the modal
 * (adversarial full-jal audit @0x8001db28–0x8001e4b0: zero Se_on/SPU — the click is the room's own SCD
 * Se_on). CORRECTION: an earlier note claimed "image-only, no text" — WRONG. The modal DOES show the
 * Yes/No take-prompt; FUN_80027e68 (a1=0x100) is the message-box opener, not a fade (wf_9e42f4cb).
 *
 * Faithful-line (flagged): the exact prompt GLYPHS + typewriter cadence + the terminal-wait frame
 * count live in BSS @0x800c4fc6 (past the EXE image) — savestate-derived, not statically byte-true;
 * the port renders the savestate-decoded strings ("WILL YOU TAKE THE" / "YOU CAN'T CARRY ANY MORE
 * ITEMS" + re15_item_name) in the debug font. The MECHANISM (message box, Yes/No, player-gated
 * dismiss, No -> item stays) IS byte-true.
 */
#ifndef RE15_ITEM_MODAL_H
#define RE15_ITEM_MODAL_H

#include <stdint.h>

/* Arm the modal for a just-picked-up item (mirrors LAB_80043328 @0x80043328): records the item +
 * DEFERS the grant to the modal's end. Ignored if a modal is already running (byte-true guard
 * @0x80043334: `if (DAT_80072d3b != 0) return`). `aot_slot` = the g_aot slot to deactivate on
 * confirm; `taken_bit` = the zone-9 flag payload to set on confirm (0 = none). */
void re15_item_modal_start(uint8_t item_type, uint8_t amount, uint8_t taken_bit, int aot_slot);

/* Advance the FSM one 30 Hz game tick (one FUN_8001db28 dispatch). `pad_edge` = the newly-pressed pad
 * bits this tick (DAT_800ac76c) — read only by the message-box wait (state 6): & 0x3000 toggles Yes/No,
 * & 0x4000 confirms, & 0xc000 dismisses the can't-carry line. Call ONLY while active, from the 30 Hz
 * gameplay tick — the caller must FREEZE the rest of gameplay while active (byte-true g_pauseflags |=
 * 0xff000000: player move, enemy AI, SCD/event, model anim all halt). */
void re15_item_modal_tick(uint16_t pad_edge);

/* Non-zero while the modal is presenting (== DAT_80072d3b != 0). */
int re15_item_modal_active(void);

/* RENDER query: on return 1, fills the 4 screen corners (TL,TR,BL,BR order, 320×240 screen space) of
 * the item quad for the CURRENT frame + the item type + the visible face (0=front, 1=back). Returns 0
 * when no quad should be drawn this frame (idle / init / the 1-frame grant tail). */
int re15_item_modal_quad(int out_x[4], int out_y[4], uint8_t *out_type, int *out_face);

/* RENDER query for the message box (states 5/6). Returns 0 if no prompt is up, else 1 = the
 * "WILL YOU TAKE THE <item>." Yes/No prompt (fills *out_choice: 0=Yes, 1=No), or 2 = the
 * "YOU CAN'T CARRY ANY MORE ITEMS" line. *out_type = the item id (for the name). */
int re15_item_modal_prompt(uint8_t *out_type, int *out_choice);

/* Typewriter reveal: the number of prompt glyphs shown so far (1 per 2 frames). The renderer clamps
 * each prompt line to this budget. `_ready` = the text has fully revealed (Yes/No is now selectable). */
int re15_item_modal_reveal(void);
int re15_item_modal_prompt_ready(void);

/* Inspection (tests): the raw state byte (0..8, mirrors DAT_80072d3b) + counters. */
uint8_t re15_item_modal_state(void);
int     re15_item_modal_frame(void);   /* DAT_8008f630 */

#endif /* RE15_ITEM_MODAL_H */
