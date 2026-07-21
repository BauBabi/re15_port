/*
 * re15_itembox.h — the ITEM BOX (RE1.5-hybrid, per shots/itembox_spec.md §6).
 *
 * PROVENANCE (every part flagged — no silent invention):
 *   [RE2-ported]      Transfer mechanics = RE2-Leon FUN_800703b8 VERBATIM where
 *                     applicable (all @0x8007xxxx cites = RE2-Leon retail EXE),
 *                     incl. the 14-quirk catalog (spec §7): ammo qty-SWAP,
 *                     whole-stack redirect, min(cursor,first-free) landing,
 *                     2-cell reject/front-shift/Size-3 analog, double
 *                     compaction, silent auto-unequip, compaction-on-open.
 *   [RE1.5-dormant]   Storage SHAPE = the four dormant init-zeroed 8-slot × 4-byte
 *                     arrays @0x800b1444/0x800b1484/0x800b14a4/0x800b14c4 inside
 *                     the RE1.5 save block (zero loop @0x8003e52c-554, sole
 *                     caller @0x8001d570; shots/itembox_verdict.md). The 0x20
 *                     un-zeroed gap 0x800b1464-1484 means the arrays are NOT
 *                     contiguous — modeled as 4 DISCRETE pages of 8, not a
 *                     32-ring. Their original purpose is UNDECIDABLE (could be
 *                     flag banks); the use as box pages is RE1.5-plausible only.
 *   [DESIGN, non-canonical] 4 pages × 8 with L1/R1 page flip REPLACES RE2's
 *                     64-slot scroll ring (§2.4: selection fixed at (scroll+2)&0x3f)
 *                     — decision §6. The L1/R1 flip stands in for RE2's ±5 page
 *                     keys (raw 0x4/0x8 shoulder, instant commit @0x8007005c).
 *   [RE1.5-adapted]   RE2's Size byte ↔ RE1.5's kind byte (+2 of the 4-byte slot;
 *                     DEFINITIVE semantics shots/inv_wave2_spec.md fact (2):
 *                     0 = 1-cell, 1 = wide HEAD, 2 = wide TAIL). Boxed-wide =
 *                     ONE box slot with kind 3 (RE2's `li v0,3; sb → Size`
 *                     @0x80070ad0-dc). Ammo id range = RE1.5's 0x15..0x21
 *                     (`sltiu id,0x22` @0x80049124 + ARMS-head table @0x8007492c;
 *                     stands in for RE2's 0x14..0x1f gate @0x80070490-94).
 *                     Ammo caps = the RE1.5 prop table @0x80074DA8 +0 (stride 12;
 *                     the same +0 byte the reload merge FUN_8004ebdc/FUN_8004e054
 *                     read @0x8004e338) standing in for RE2's DAT_800a9e1c[id*8]
 *                     (@0x800704c4).
 *   [Trigger: invented, precedent = the save phone] The 16 safe-room box AOTs
 *                     (message-only in the shipped MZD build) open the box screen
 *                     default-on; the shipped flavor message stays byte-true
 *                     reachable under RE15_BOX_PREVIEW_MSG=1.
 */
#ifndef RE15_ITEMBOX_H
#define RE15_ITEMBOX_H

#include <stdint.h>
#include "re15_inventory.h"   /* re15_inv_slot_t {id,qty,flags(kind),pad} */

#define RE15_BOX_PAGES      4   /* the 4 dormant arrays @0x800b1444/1484/14a4/14c4 */
#define RE15_BOX_PAGE_SLOTS 8   /* 8 × 4-byte slots per array (zero loop bound
                                 * sltiu v0,a0,0x8 @0x8003e54c, RE1.5 PSX.EXE)   */
#define RE15_BOX_SLOTS      (RE15_BOX_PAGES * RE15_BOX_PAGE_SLOTS)   /* 32 (§6) */

/* Boxed 2-cell weapon kind analog (RE2 Size 3 @0x80070ad0-dc / @0x80070c30-3c). */
#define RE15_BOX_KIND_WIDE  3

typedef struct {
    /* pages[p].slots[i] mirrors the dormant array @0x800b1444 + p-th array
     * (0x800b1444/0x800b1484/0x800b14a4/0x800b14c4), slot stride 4
     * {id,qty,kind,pad} — RE1.5's inventory slot format (DAT_800b10ac shape). */
    struct { re15_inv_slot_t slots[RE15_BOX_PAGE_SLOTS]; } pages[RE15_BOX_PAGES];
} re15_itembox_t;

extern re15_itembox_t g_itembox;

/* Zero the whole box — the RE1.5 new-game init shape (FUN_8003e4f4 zero loop
 * @0x8003e52c-554 clears all four arrays; box starts empty like RE2's
 * new-game zero region, spec §1). */
void re15_itembox_init(void);

/* ---- transfer engine — RE2 FUN_800703b8 semantics (ONE operation: SWAP
 * inv[cursor] ⇄ box[page][slot]; no deposit/withdraw commands, spec §3) -------- */
enum {
    RE15_BOX_XFER_OK     = 0,
    RE15_BOX_XFER_REJECT = 1   /* THE ONLY reject: box holds a 2-cell weapon and
                                * empties==0 || (empties==1 && cursor empty)
                                * (@0x800707dc-ec). No box-full reject exists
                                * (fixed slots, 1:1 swap — spec §3 tail). */
};
int re15_itembox_transfer(int inv_cursor, int page, int slot);

/* Compaction-on-open (RE2 box task: FUN_80069714 @0x80068c60 = quirk 9) is done
 * by the shared menu phase-0 init (re15_inv_compact @0x800464a0 — the RE1.5
 * status screen compacts at open too, so ONE call covers both cites). */

/* ---- trigger registry (the 16 safe-room box AOTs) --------------------------- */
/* 1 if a Message_on of msg_id in room_id is the shipped "Itembox is not
 * available in this preview" flavor message (the box AOT identity). */
int  re15_itembox_is(unsigned room_id, uint8_t msg_id);

/* One-shot open request (save-phone precedent, re15_savepoint_pending shape). */
int  re15_itembox_pending(void);
void re15_itembox_set_pending(int on);
void re15_itembox_reset(void);   /* clear pending on room change */

/* ---- box screen FSM (status-screen-style subscreen; menu substate 4) --------
 * Cursor model per RE2's box FSM (spec §2.3/§2.4, panels @0x800a9bb4 + sub-state
 * DAT_800d5bf2): panel 2 = EXIT row, panel 3 = main; main states 0 = inventory
 * grid, 1 = box side, 4 = swap (transient), 5 = message wait. RE2 states 2/3
 * (6-frame scroll anim) have NO analog — replaced by the instant page flip
 * [DESIGN, §6]. Ticked by menu_common.c when substate==4. */
void re15_itembox_screen_open(void);                    /* on subscreen entry   */
int  re15_itembox_screen_tick(uint16_t pressed, uint16_t held); /* 1 = close req */

/* introspection (tests) */
int  re15_itembox_screen_state(void);   /* main state 0/1/5 (RE2 DAT_800d5bf2)  */
int  re15_itembox_screen_panel(void);   /* 3 = main, 2 = EXIT row (DAT_800d5bf1)*/
int  re15_itembox_screen_page(void);    /* current page 0..3                    */
int  re15_itembox_screen_cursor(void);  /* box-side cell cursor 0..7            */

/* flat accessors for persistence (savedata v4 box[32]: page*8+i order). */
void re15_itembox_export(re15_inv_slot_t out[RE15_BOX_SLOTS]);
void re15_itembox_import(const re15_inv_slot_t in[RE15_BOX_SLOTS]);

#endif /* RE15_ITEMBOX_H */
