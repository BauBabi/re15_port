/*
 * re15_itembox.c — the ITEM BOX (RE1.5-hybrid). Build sheet: shots/itembox_spec.md;
 * dormant-storage background: shots/itembox_verdict.md. Provenance flags per part in
 * re15_itembox.h — [RE2-ported] cites = RE2-Leon retail EXE (FUN_800703b8 family),
 * [RE1.5] cites = RE1.5 PSX.EXE, [DESIGN] = non-canonical hybrid decision (§6).
 */
#include <string.h>
#include <stdlib.h>
#include "re15_itembox.h"
#include "re15_inventory.h"
#include "re15_inv_screen.h"   /* g_inv_screen (box_* mirrors) + icon-cache ops   */
#include "re15_inv_ui.h"       /* embedded RE1.5 EXE blob: prop table @0x80074DA8 */
#include "re15_player.h"       /* RE15_PAD_BIT_*                                  */
#include "re15_engine.h"       /* re15_pad_virtual_word (confirm/cancel roles)    */
#include "re15_audio.h"        /* re15_audio_core_se — SE bank 4 (§6)             */

re15_itembox_t g_itembox;

void re15_itembox_init(void)
{
    /* RE1.5 new-game init shape: FUN_8003e4f4 zeroes all four arrays
     * (sw zero loop @0x8003e52c-554; 8 iterations each). Box starts empty. */
    memset(&g_itembox, 0, sizeof g_itembox);
}

void re15_itembox_export(re15_inv_slot_t out[RE15_BOX_SLOTS])
{
    memcpy(out, &g_itembox, RE15_BOX_SLOTS * sizeof(re15_inv_slot_t));
}

void re15_itembox_import(const re15_inv_slot_t in[RE15_BOX_SLOTS])
{
    memcpy(&g_itembox, in, RE15_BOX_SLOTS * sizeof(re15_inv_slot_t));
}

/* ---------------------------------------------------------------------------- */
/* Trigger registry — the COMPLETE set of shipped "Itembox is not available in  */
/* this preview" messages (exhaustive scan of all room RDT message blocks, msg  */
/* table u32 @RDT+0x3C -> u16 offsets, glyph decode chr = code+0x24 — the same  */
/* method that built the savepoint table; 16/16 rooms, scan 2026-07-21).        */
/* {room, msg index (byte offset of the body within the msg block)}:            */
/*   1150/1151 msg 3  (blk @0x12F8 off 0x12E body @0x1426 / @0x10EC/0x118/0x1204)*/
/*   2010/2011 msg 2  (blk @0x8FC  off 0x4B  body @0x947  / @0x91C/0x4B/0x967)  */
/*   30A0/30A1 msg 0  (blk @0x88C  off 0x14  body @0x8A0  — same bytes both)    */
/*   30B0/30B1 msg 0  (blk @0x9A0  off 0x04  body @0x9A4  — same bytes both)    */
/*   4010 msg 0x2A    (blk @0xFB8  off 0x94E body @0x1906)                      */
/*   4011 msg 0x06    (blk @0x658  off 0x17F body @0x7D7)                       */
/*   5010 msg 0x05    (blk @0x50C  off 0x185 body @0x691)                       */
/*   5011 msg 0x2C    (blk @0xE4C  off 0x8FE body @0x174A)                      */
/*   6020/6021 msg 0  (blk @0xBDC  off 0x02  body @0xBDE  — same bytes both)    */
/*   6030 msg 0x09    (blk @0x1394 off 0x1E6 body @0x157A)                      */
/*   6031 msg 0x03    (blk @0x1130 off 0x9E  body @0x11CE)                      */
/* ROOM1150's identity (msg 3 fired by sub07's Message_on, installed by the     */
/* slot-5 Aot_set @main00+0x70) is ctest-pinned in test_room1150_itembox.c.     */
/* ---------------------------------------------------------------------------- */
static const struct { unsigned room; uint8_t msg; } s_boxpoints[] = {
    { 0x1150, 0x03 }, { 0x1151, 0x03 },   /* STAGE1 */
    { 0x2010, 0x02 }, { 0x2011, 0x02 },   /* STAGE2 */
    { 0x30A0, 0x00 }, { 0x30A1, 0x00 },   /* STAGE3 */
    { 0x30B0, 0x00 }, { 0x30B1, 0x00 },   /* STAGE3 */
    { 0x4010, 0x2A }, { 0x4011, 0x06 },   /* STAGE4 */
    { 0x5010, 0x05 }, { 0x5011, 0x2C },   /* STAGE5 */
    { 0x6020, 0x00 }, { 0x6021, 0x00 },   /* STAGE6 */
    { 0x6030, 0x09 }, { 0x6031, 0x03 },   /* STAGE6 */
};

int re15_itembox_is(unsigned room_id, uint8_t msg_id)
{
    for (unsigned i = 0; i < sizeof(s_boxpoints) / sizeof(s_boxpoints[0]); i++)
        if (s_boxpoints[i].room == room_id && s_boxpoints[i].msg == msg_id)
            return 1;
    return 0;
}

static int s_box_pending = 0;
int  re15_itembox_pending(void)       { return s_box_pending; }
void re15_itembox_set_pending(int on) { s_box_pending = on ? 1 : 0; }
void re15_itembox_reset(void)         { s_box_pending = 0; }

/* ---------------------------------------------------------------------------- */
/* Transfer engine — RE2 FUN_800703b8 (spec §3), RE1.5 kind byte for RE2 Size.  */
/* ---------------------------------------------------------------------------- */

/* RE1.5 prop table @0x80074DA8, stride 12, +0 = max stack / reload cap (the
 * byte the reload merges read: lbu @0x8004e338 / @0x8004e444; RE1.5-EXE) —
 * standing in for RE2's cap table DAT_800a9e1c[id*8] (@0x800704c4). */
static uint8_t box_ammo_cap(uint8_t id)
{
    return *RE15_INV_PTR(0x80074DA8u + (uint32_t)id * 12u);
}

/* RE1.5 ammo id gate 0x15..0x21 (`sltiu id,0x22` @0x80049124 + weapon bound
 * 0x15 @0x80047d54, RE1.5-EXE) — stands in for RE2's box-ammo gate
 * id ∈ 0x14..0x1f (@0x80070490-94). [RE1.5-adapted] */
static int box_is_ammo(uint8_t id) { return re15_item_is_ammo(id); }

static void clear_slot(re15_inv_slot_t *s)
{
    s->id = 0; s->qty = 0; s->flags = 0; s->pad = 0;
}

static int inv_first_free(void)
{
    /* RE2 FUN_80069668(0): first id==0 else 0xffffffff (spec §0). Port: -1. */
    for (int i = 0; i < RE15_INV_MAX_SLOTS - 1; i++)   /* pack slots 0..9 (RE1.5
                                                        * capacity DAT_800b0fbc=10) */
        if (g_inv.slots[i].id == 0) return i;
    return -1;
}

static int inv_count_empties(void)
{
    /* RE2 FUN_80069668(1): count of empty slots (spec §0). */
    int n = 0;
    for (int i = 0; i < RE15_INV_MAX_SLOTS - 1; i++)
        if (g_inv.slots[i].id == 0) n++;
    return n;
}

/* Silent auto-unequip when the deposited slot is equipped — RE2: EVERY branch
 * (equip := 0x80, weapon id := 0; spec §3 tail = quirk 8). Port: the equip SLOT
 * clears here; the live weapon id (DAT_800aca5d) is committed at screen close by
 * the shared snapshot-compare (menu close_phase — RE2's close-time weapon-model
 * reload analog, quirk 14). */
static void unequip_if(int slot_a, int slot_b)
{
    int eq = re15_inv_equipped_slot();
    if (eq != 0x80 && (eq == slot_a || eq == slot_b))
        re15_inv_set_equipped_slot(0x80);
}

/* RE1.5 wide-weapon FRONT-SHIFT (reused from the world-pickup insert
 * FUN_8004dc4c @0x8004dc98-de3c, RE1.5-EXE — the same shape the box withdraw
 * calls in RE2 as FUN_800698b4(1) @0x80070830): slots 0..7 -> 2..9 (icon cells
 * move along, jal 0x80049390 per cell), then the equip index += 2 UNCONDITIONALLY
 * (@0x8004dc84-9c has no 0x80 guard — the RE1.5 0x80->0x82 rawness, byte-true
 * as disassembled; RE2's site is the equip += 2 @0x80070830). */
static void inv_front_shift2(void)
{
    for (int i = 7; i >= 0; i--) {
        g_inv.slots[i + 2] = g_inv.slots[i];
        re15_inv_icon_copy(i, i + 2);
    }
    clear_slot(&g_inv.slots[0]);
    clear_slot(&g_inv.slots[1]);
    re15_inv_icon_blank(0);
    re15_inv_icon_blank(1);
    re15_inv_set_equipped_slot(re15_inv_equipped_slot() + 2);
}

int re15_itembox_transfer(int inv_cursor, int page, int slot)
{
    re15_inv_slot_t *ci, *bs;
    int cur = inv_cursor;

    if (cur < 0 || cur >= RE15_INV_MAX_SLOTS - 1) return RE15_BOX_XFER_OK;
    if (page < 0 || page >= RE15_BOX_PAGES) return RE15_BOX_XFER_OK;
    if (slot < 0 || slot >= RE15_BOX_PAGE_SLOTS) return RE15_BOX_XFER_OK;

    /* cursor lands only on heads via the nav; normalize a tail defensively
     * (kind==2 -> slot-1, the RE1.5 normalization shape @0x8004e910-38). */
    if (g_inv.slots[cur].flags == 2 && cur > 0) cur--;
    ci = &g_inv.slots[cur];
    bs = &g_itembox.pages[page].slots[slot];

    /* ---- ammo pre-pass (RE2 @0x80070490-94 gate; spec §3 first block) ---- */
    if (bs->id != 0 && box_is_ammo(bs->id)) {
        uint8_t cap = box_ammo_cap(bs->id);          /* RE2 cap read @0x800704c4 */
        if (ci->id == bs->id) {
            int sum = (int)ci->qty + (int)bs->qty;
            if (ci->qty == cap) {
                /* (a) inventory stack already full -> quantities SWAP
                 * (inv := boxqty, box := cap) @0x800704e0-f4 — QUIRK 1. */
                ci->qty = bs->qty;
                bs->qty = cap;
            } else if (sum > cap) {
                /* (b) sum > cap -> inv := cap, box := sum - cap (spec §3 b). */
                ci->qty = cap;
                bs->qty = (uint8_t)(sum - cap);
            } else {
                /* (c) sum <= cap -> inv += box, box zeroed (spec §3 c). */
                ci->qty = (uint8_t)sum;
                clear_slot(bs);
            }
            return RE15_BOX_XFER_OK;   /* merge complete — QUIRK 2: ammo always
                                        * merges box->inventory, even on a
                                        * deposit intent */
        }
        if (ci->id == 0) {
            /* empty cursor: scan ALL inventory for the same ammo id, merge only
             * if the WHOLE box stack fits (@0x80070554-98 — QUIRK 3, whole-
             * stack-only redirect); else fall through to the normal swap. */
            for (int k = 0; k < RE15_INV_MAX_SLOTS - 1; k++) {
                if (g_inv.slots[k].id == bs->id &&
                    (int)g_inv.slots[k].qty + (int)bs->qty <= (int)cap) {
                    g_inv.slots[k].qty = (uint8_t)(g_inv.slots[k].qty + bs->qty);
                    clear_slot(bs);
                    return RE15_BOX_XFER_OK;
                }
            }
        }
    }

    {
        int inv_wide = (ci->id != 0 && (ci->flags == 1 || ci->flags == 2));
        int box_wide = (bs->id != 0 && bs->flags == RE15_BOX_KIND_WIDE);

        if (!inv_wide && !box_wide) {
            /* ---- case 0: 1-cell ⇄ 1-cell/empty (spec §3 case 0) ---- */
            re15_inv_slot_t old_inv = *ci;
            re15_inv_slot_t old_box = *bs;
            int ff, dest;
            unequip_if(cur, cur);                    /* silent unequip (quirk 8) */
            clear_slot(ci);
            re15_inv_icon_blank(cur);
            ff = inv_first_free();                   /* FUN_80069668(0)          */
            dest = (ff >= 0 && ff < cur) ? ff : cur; /* min(cursor, first-free)
                                                      * landing — QUIRK 4        */
            if (old_box.id != 0) {
                g_inv.slots[dest].id    = old_box.id;
                g_inv.slots[dest].qty   = old_box.qty;
                g_inv.slots[dest].flags = 0;
                re15_inv_icon_blank(dest);           /* fresh upload = identity  */
            }
            if (old_inv.id != 0) {
                bs->id = old_inv.id; bs->qty = old_inv.qty; bs->flags = 0;
            } else {
                clear_slot(bs);
            }
            re15_inv_compact();                      /* compact after (spec §3)  */
            return RE15_BOX_XFER_OK;
        }

        if (!inv_wide && box_wide) {
            /* ---- case 1: box holds a 2-cell weapon (spec §3 case 1) ---- */
            int empties = inv_count_empties();       /* FUN_80069668(1)          */
            re15_inv_slot_t old_inv = *ci;
            if (empties == 0 || (empties == 1 && ci->id == 0)) {
                /* THE ONLY REJECT (@0x800707dc-ec) — caller opens the message
                 * + enters state 5 (RE2: FUN_8002fe38(0xaf0010,0xe400,8,0)). */
                return RE15_BOX_XFER_REJECT;
            }
            unequip_if(cur, cur);                    /* unequip-if-needed        */
            clear_slot(ci);                          /* clear cursor             */
            re15_inv_icon_blank(cur);
            re15_inv_compact();                      /* compact                  */
            inv_front_shift2();                      /* front-shift-by-2 (equip
                                                      * += 2 @0x80070830 / RE1.5
                                                      * @0x8004dc84-9c)          */
            g_inv.slots[0].id = bs->id;  g_inv.slots[0].qty = bs->qty;
            g_inv.slots[0].flags = 1;                /* kind 1/2 = RE2 Size 1/2  */
            g_inv.slots[1].id = bs->id;  g_inv.slots[1].qty = bs->qty;
            g_inv.slots[1].flags = 2;
            if (old_inv.id != 0) {                   /* old item into box        */
                bs->id = old_inv.id; bs->qty = old_inv.qty; bs->flags = 0;
            } else {
                clear_slot(bs);
            }
            return RE15_BOX_XFER_OK;
        }

        if (inv_wide && !box_wide) {
            /* ---- case 2: inventory 2-cell out (spec §3 case 2) ---- */
            re15_inv_slot_t old_box = *bs;
            uint8_t wid = ci->id, wqty = ci->qty;
            unequip_if(cur, cur + 1);                /* silent unequip           */
            clear_slot(&g_inv.slots[cur]);           /* clear BOTH cells         */
            clear_slot(&g_inv.slots[cur + 1]);
            re15_inv_icon_blank(cur);
            re15_inv_icon_blank(cur + 1);
            re15_inv_compact();                      /* compaction TWICE          */
            re15_inv_compact();                      /* (@0x800709bc + @0x80070a24
                                                      * /a38 — QUIRK 7)          */
            bs->id = wid; bs->qty = wqty;
            bs->flags = RE15_BOX_KIND_WIDE;          /* Size := 3 (`li v0,3; sb`
                                                      * @0x80070ad0-dc)          */
            if (old_box.id != 0) {                   /* box item into first-free */
                int ff = inv_first_free();
                if (ff >= 0) {
                    g_inv.slots[ff].id    = old_box.id;
                    g_inv.slots[ff].qty   = old_box.qty;
                    g_inv.slots[ff].flags = 0;
                    re15_inv_icon_blank(ff);
                }
            }
            return RE15_BOX_XFER_OK;
        }

        /* ---- case 3: 2-cell ⇄ 2-cell, in-place (spec §3 case 3) ---- */
        {
            uint8_t bid = bs->id, bqty = bs->qty;
            uint8_t wid = ci->id, wqty = ci->qty;
            unequip_if(cur, cur + 1);                /* silent unequip           */
            g_inv.slots[cur].id = bid;      g_inv.slots[cur].qty = bqty;
            g_inv.slots[cur].flags = 1;              /* box weapon -> both cells */
            g_inv.slots[cur + 1].id = bid;  g_inv.slots[cur + 1].qty = bqty;
            g_inv.slots[cur + 1].flags = 2;          /* (@0x80070c30-3c)         */
            re15_inv_icon_blank(cur);
            re15_inv_icon_blank(cur + 1);
            bs->id = wid; bs->qty = wqty;
            bs->flags = RE15_BOX_KIND_WIDE;          /* inv weapon -> box Size 3
                                                      * (@0x80070d98)            */
            return RE15_BOX_XFER_OK;
        }
    }
}

/* ---------------------------------------------------------------------------- */
/* Box screen FSM — RE2 panel/state shape (spec §2.3/§2.4) on the RE1.5 grid.   */
/* Input adaptation [DESIGN]: confirm/cancel = the port's virtual roles 0x4000/  */
/* 0x8000 (RE2 reads the action word DAT_800ce310 0x1000/0x2000 roles); d-pad =  */
/* press edges (RE2's DAT_800ce304 repeat cadence not replicated); L1/R1 page   */
/* flip ≙ RE2's raw 0x4/0x8 shoulder ±5 instant commit (@0x8007005c).            */
/* ---------------------------------------------------------------------------- */
static uint8_t s_bx_panel  = 3;   /* RE2 DAT_800d5bf1: 3 = main FSM, 2 = EXIT row */
static uint8_t s_bx_state  = 0;   /* RE2 DAT_800d5bf2: 0 inv / 1 box / 5 msg wait */
static uint8_t s_bx_page   = 0;   /* [DESIGN §6] page 0..3 (≙ RE2 scroll window)  */
static uint8_t s_bx_cursor = 0;   /* [DESIGN §6] box cell 0..7 (≙ scroll+2 pick)  */

int re15_itembox_screen_state(void)  { return s_bx_state; }
int re15_itembox_screen_panel(void)  { return s_bx_panel; }
int re15_itembox_screen_page(void)   { return s_bx_page; }
int re15_itembox_screen_cursor(void) { return s_bx_cursor; }

/* menu_common.c bridges (the shared message VM + SEs). */
void re15_menu_box_reject_msg(void);   /* opens desc-bank entry 0 at (0x18,0xa8) —
                                        * the RE1.5 cant-use infra @0x8004b2d8,
                                        * standing in for RE2's FUN_8002fe38 box
                                        * reject message [RE1.5-adapted §6] */
int  re15_menu_msg_active(void);

static void bse(int id) { re15_audio_core_se(id); }   /* SE bank 4 (§6) */

void re15_itembox_screen_open(void)
{
    /* RE2 box task open: FSM struct zeroed + cursors zeroed (@0x80068be8-fc +
     * DAT_800d5bfc/fd; "Inventory cursor starts 0" §2.2 = quirk 14 head).
     * Page 0 on entry [DESIGN — RE2 scroll persists in DAT_800d5c14 only within
     * the screen; init writes scroll:=0 @§2.2]. */
    s_bx_panel = 3; s_bx_state = 0; s_bx_page = 0; s_bx_cursor = 0;
    g_inv_screen.item_cursor = 0;
    g_inv_screen.box_mode = 1;
    g_inv_screen.box_page = 0;
    g_inv_screen.box_cursor = 0;
    g_inv_screen.box_side = 0;
    g_inv_screen.highlight = 1;    /* draw the grid cursor (g8 gate, wave-1 model) */
}

/* State 0 — inventory grid, RE2 nav semantics (§2.4 state 0) on the RE1.5
 * 2-column grid: right +1 (skip 2 when on a Size/kind-1 head), left -1 (skip a
 * kind-2 tail), down +2, up -2 (cursor<2 -> EXIT row). The RE1.5 personal-slot
 * 0xa special cases (quirk 12) have NO analog — RE1.5 has no personal slot
 * [RE1.5-adapted]. SE(4,4) plays on the press, before the accept checks (the
 * RE1.5 grid precedent @0x800c62d8: SE also on a rejected move). */
static void bx_state0(uint16_t pressed)
{
    uint8_t *cur = &g_inv_screen.item_cursor;
    if (pressed & (RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN |
                   RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) bse(4);
    if (pressed & RE15_PAD_BIT_RIGHT) {
        int step = (g_inv.slots[*cur].flags == 1) ? 2 : 1;   /* skip own tail */
        if (*cur + step <= 9) *cur = (uint8_t)(*cur + step);
    } else if (pressed & RE15_PAD_BIT_LEFT) {
        if (*cur > 0) {
            int n = *cur - 1;
            if (g_inv.slots[n].flags == 2 && n > 0) n--;     /* skip a tail   */
            *cur = (uint8_t)n;
        }
    } else if (pressed & RE15_PAD_BIT_DOWN) {
        if (*cur + 2 <= 9) *cur = (uint8_t)(*cur + 2);
    } else if (pressed & RE15_PAD_BIT_UP) {
        if (*cur < 2) { s_bx_panel = 2; return; }            /* -> EXIT row
                                                              * (RE2 §2.4)    */
        *cur = (uint8_t)(*cur - 2);
    }
    if (re15_pad_virtual_word(pressed) & 0x4000) {           /* confirm role  */
        bse(6);                                              /* RE2 SE 0x406  */
        s_bx_state = 1;                                      /* -> box side   */
        return;
    }
    if (re15_pad_virtual_word(pressed) & 0x8000) {           /* cancel role   */
        bse(5);                                              /* RE2 SE 0x405  */
        s_bx_panel = 0;                                      /* panel 0 = exit
                                                              * start (§2.3)  */
    }
}

/* EXIT row (RE2 panel 2 @0x800a9bb4[2], §2.3): DOWN -> cursor:=1, panel:=3
 * SE 0x404; action -> panel:=0 (exit) SE 0x406. */
static void bx_exit_row(uint16_t pressed)
{
    if (pressed & RE15_PAD_BIT_DOWN) {
        bse(4);
        g_inv_screen.item_cursor = 1;                        /* cursor := 1   */
        s_bx_panel = 3; s_bx_state = 0;
        return;
    }
    if (re15_pad_virtual_word(pressed) & (0x4000 | 0x8000)) {
        /* RE2 gates on action &0x3000 (either role) -> exit. */
        bse(6);
        s_bx_panel = 0;
    }
}

/* State 1 — box side. [DESIGN §6]: page-local 2×4 cell cursor + L1/R1 page flip
 * replace RE2's fixed-middle-row 64-ring scroll (§2.4 states 1/2/3). The flip is
 * INSTANT like RE2's ±5 shoulder keys (commit tail @0x8007005c); the 6-frame
 * ±3px scroll anim (quirk 11) has no analog here. */
static void bx_state1(uint16_t pressed)
{
    uint8_t *cur = &s_bx_cursor;
    if (pressed & (RE15_PAD_BIT_UP | RE15_PAD_BIT_DOWN |
                   RE15_PAD_BIT_LEFT | RE15_PAD_BIT_RIGHT)) bse(4);
    if      (pressed & RE15_PAD_BIT_RIGHT) { if (!(*cur & 1)) (*cur)++; }
    else if (pressed & RE15_PAD_BIT_LEFT)  { if (*cur & 1) (*cur)--; }
    else if (pressed & RE15_PAD_BIT_DOWN)  { if (*cur + 2 <= 7) *cur += 2; }
    else if (pressed & RE15_PAD_BIT_UP)    { if (*cur >= 2) *cur -= 2; }
    if (pressed & RE15_PAD_BIT_L1) { bse(4); s_bx_page = (uint8_t)((s_bx_page + 3) & 3); }
    if (pressed & RE15_PAD_BIT_R1) { bse(4); s_bx_page = (uint8_t)((s_bx_page + 1) & 3); }

    if (re15_pad_virtual_word(pressed) & 0x8000) {           /* cancel role   */
        bse(5);
        s_bx_state = 0;                                      /* RE2: -> state0 */
        return;
    }
    if (re15_pad_virtual_word(pressed) & 0x4000) {           /* confirm role  */
        /* RE2 state 4 (swap): state := 0, call FUN_800703b8 — the transfer may
         * set state 5 on the reject (§2.4 state 4). */
        bse(6);
        s_bx_state = 0;
        if (re15_itembox_transfer(g_inv_screen.item_cursor,
                                  s_bx_page, s_bx_cursor) == RE15_BOX_XFER_REJECT) {
            re15_menu_box_reject_msg();                      /* the RE1.5 cant-use
                                                              * message infra  */
            s_bx_state = 5;                                  /* @0x800707ec    */
        }
    }
}

int re15_itembox_screen_tick(uint16_t pressed, uint16_t held)
{
    (void)held;
    if (s_bx_panel == 2) {
        bx_exit_row(pressed);
    } else if (s_bx_panel == 3) {
        switch (s_bx_state) {
        case 0: bx_state0(pressed); break;
        case 1: bx_state1(pressed); break;
        case 5:
            /* RE2 state 5: wait until the message bit clears -> state 1
             * (DAT_800e873c bit 0x80 poll, §2.4 state 5). */
            if (!re15_menu_msg_active()) s_bx_state = 1;
            break;
        default:
            s_bx_state = 0;
            break;
        }
    }

    /* common tail — name caption of the hovered item. RE2's tail captions
     * inv[cursor].id at (0x10,0xaf) (FUN_800693d0 @§2.4 tail); the box rows
     * carry their own name labels in RE2's list draw (§2.5). The icon-grid
     * hybrid has no per-row labels, so the caption follows the ACTIVE side
     * [DESIGN]: box side = the hovered box slot's id. Drawn through the
     * campaign's byte-true name-bank pipeline (FUN_80028c1c glyphs — the same
     * id catalog re15_item_name mirrors in ASCII). */
    if (s_bx_panel == 3 && s_bx_state == 1)
        g_inv_screen.name_item =
            (int16_t)g_itembox.pages[s_bx_page].slots[s_bx_cursor].id;
    else
        g_inv_screen.name_item = (int16_t)g_inv.slots[g_inv_screen.item_cursor].id;

    /* builder mirrors */
    g_inv_screen.box_page   = s_bx_page;
    g_inv_screen.box_cursor = s_bx_cursor;
    g_inv_screen.box_side   = (s_bx_panel == 2) ? 2
                            : (s_bx_state == 1 || s_bx_state == 5) ? 1 : 0;

    return (s_bx_panel == 0) ? 1 : 0;   /* panel 0 = exit start -> menu close */
}
