/*
 * re15_inv_screen.c — byte-true RE1.5 status/inventory screen display-list builder
 * (Wave 1; spec shots/inv_plan.md §1 + shots/inv_wave1_blockers.md Q1-Q4, raw RE
 * shots/inv_re_reports.md report B; RE1.5-EXE throughout).
 *
 * Reproduces the original per-frame prim build:
 *   FUN_80049a5c @0x80049a5c  draw chain (AddPrim order = ops[] order, earliest=topmost)
 *   FUN_80047648 @0x80047648  master-group animator (per-group anchor/clut/rgb rules;
 *                             case bodies DISASM-VERIFIED — the Ghidra decompile hides
 *                             the capacity-dependent ITEM-LIST layout, see g1 below)
 *   FUN_80048a44 @0x80048a44  ECG trace renderer (32 vertical ABE LineF2)
 *   FUN_80048f28 @0x80048f28  qty digit drawer (leading-zero suppression, clut by the
 *                             item's prop8 color class @0x80074DA8+id*12+8 — blocker Q1)
 *   FUN_80046a1c @0x80046a1c  icon-cell/card/background arena (build-time geometry)
 *
 * Every constant is read from the embedded EXE blob (re15_inv_ui.h) at its original
 * address, or carries its @0x citation inline.
 */
#include <string.h>
#include "re15_inv_screen.h"
#include "re15_inv_ui.h"
#include "re15_inventory.h"
#include "re15_room.h"           /* RE2-Kartensystem: re15_map_rect_state (Port-Erweiterung) */
#include "re15_itembox.h"        /* ITEM BOX subscreen (box_mode display list) */
#include "font_width.h"          /* per-glyph advance u8 @0x800c4416 (DEBUG.BIN, vendored) */
#include "gen/inv_name_bank.inc" /* item-name bank @0x800c495c/4a28 + digraph pairs @0x800c4438 */
#include "gen/inv_desc_bank.inc" /* item-desc bank @0x800c50de (wave 4; entry idx = item id) */
#include "gen/inv_file_doc.inc"  /* FILE wave: 7-page document @0x800ccd34 + row names
                                  * 0x48-0x65 + masks/bases/titles/underscores (census-
                                  * asserted; tools/gen_inv_file_doc.py) */

const uint8_t *re15_inv_desc_entry(int id)
{
    if (id < 0 || id >= RE15_INV_DESC_NIDS) return 0;
    return re15_inv_desc_blob + re15_inv_desc_off[id];
}
int re15_inv_desc_nids(void) { return RE15_INV_DESC_NIDS; }

/* ---- little-endian blob readers (blob = verbatim PSX.EXE image bytes) ---- */
static uint16_t bu16(uint32_t addr)
{
    const unsigned char *p = RE15_INV_PTR(addr);
    return (uint16_t)(p[0] | (p[1] << 8));
}
static int16_t bs16(uint32_t addr) { return (int16_t)bu16(addr); }
static uint32_t bu32(uint32_t addr)
{
    const unsigned char *p = RE15_INV_PTR(addr);
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static uint8_t bu8(uint32_t addr) { return *RE15_INV_PTR(addr); }

/* ---- region-2 readers (MAP blob [0x800762A0, 0x80076C00), re15_inv_ui.h) ---- */
static uint16_t mu16(uint32_t addr)
{
    const unsigned char *p = RE15_INV_MAP_PTR(addr);
    return (uint16_t)(p[0] | (p[1] << 8));
}
static uint32_t mu32(uint32_t addr)
{
    const unsigned char *p = RE15_INV_MAP_PTR(addr);
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static uint8_t mu8(uint32_t addr) { return *RE15_INV_MAP_PTR(addr); }

/* ---- table bases (all inside the blob) ---- */
#define MASTER_TBL   0x80074A8Cu  /* stride 0xC {clut_idx u16, count u16, tmpl u32, buf u32} */
#define CELL_TBL     0x80076274u  /* 11 x {s16 x, s16 y} grid cells (+ entry 10 Std-Arms)    */
#define ICON_UV_TBL  0x80076244u  /* 12 x {u16 u, u16 v} icon-cache cell uv                  */
#define BUTTON_TBL   0x80075390u  /* stride 0xC {s16 dx, dy, w, h, u8 u @+8, u8 v @+10}      */
#define ECG_PTR_TBL  0x80076214u  /* 6 x u32 -> waveform tables (PSX addrs; rebase in blob)  */
#define ECG_COL_TBL  0x8007622Cu  /* 6 x {r,g,b,fade-mask}                                   */
#define PROP_TBL     0x80074DA8u  /* stride 12; +8 = digit color class (blocker Q1)          */

/* layout registers, init values FUN_800460b8 @0x800460b8 + LAB_80049524 @0x80049584-95d0:
 * ID card (25f0,25f2)=(14,26); ECG (25e4,25e6)=(13,82); ARMS (25d8,25da)=(126,26);
 * ITEM LIST (25e0,25e2)=(215,26); tabs (25e8)=126; action (25ec)=142 */
#define ID_BX    14
#define ID_BY    26
#define ECG_BX   13
#define ECG_BY   82
#define ARMS_BX  126
#define ARMS_BY  26
#define LIST_BX  215
#define LIST_BY  26
#define TAB_BX   126
#define ACT_BX   142

#define CAPACITY 10   /* DAT_800b0fbc = 0x0a (live savestate; grid cell table has 10 cells) */

re15_inv_screen_t g_inv_screen;

static void g7_label_reset(void);   /* wave 3: forget the frozen g7 label face (see below) */

/* MAP wave — DAT_800b260d / DAT_800b260e: PERSISTENT globals (BSS initial 0). The menu
 * init never writes them; the ONLY writers in the whole EXE are the six per-stage inits
 * (ghidra xref scan, wave-3 spec: "No other writer of 260e exists"). A MAP entry in an
 * out-of-range room keeps the previous values (stale-previous, byte-true). */
static uint8_t s_map_room = 0;
static uint8_t s_map_page = 0;

void re15_inv_screen_sync_equip(void)
{
    /* LAB_80049524 @0x800495e8-0x8004961c: 25cd/2608 by equipped-or-not */
    int eq = re15_inv_equipped_slot();
    if (eq == 0x80) { g_inv_screen.arms_rgb = 0x80; g_inv_screen.arms_slide = -55; }
    else            { g_inv_screen.arms_rgb = 0x3e; g_inv_screen.arms_slide = 0;   }
    /* @0x80049620-0x800496dc: 25dc/25de from the equipped slot's kind byte */
    g_inv_screen.equip_x = 0x96; g_inv_screen.equip_y = 0x3a;
    if (eq != 0x80) {
        uint8_t kind = g_inv.slots[eq & 0x0f].flags;
        if (kind == 1) g_inv_screen.equip_x = 0x82;   /* @0x80049688 */
        if (kind == 2) g_inv_screen.equip_x = 0xaa;   /* @0x800496c8 */
    }
    g_inv_screen.equipped_slot = (uint8_t)eq;
}

void re15_inv_screen_open(void)
{
    memset(&g_inv_screen, 0, sizeof g_inv_screen);
    /* FUN_800460b8 @0x800460b8: cursors bc/bd/be=0, 25ca=0, sweep 2600=0x20,
     * glow 2602=0, phase 2603=0; wipe 25d4=0 @0x8004645c */
    g_inv_screen.ecg_sweep = 0x20;
    g_inv_screen.tab_base_y = 0xa6;    /* DAT_800b25ea init @0x800495b0 */
    g_inv_screen.act_base_y = 0x108;   /* DAT_800b25ee init @0x800495c8 */
    /* wipe cursors parked at their steady-state values (the renderer's self-reset writes
     * 25fe:=0x20 / 25fc:=0x6a on wipe completion, FUN_80048a44.c:61-125; the dedicated
     * init-site store was not separately traced — OPEN, live savestate shows the parked
     * values). */
    g_inv_screen.wipe_v = 0x20;
    g_inv_screen.wipe_h = 0x6a;
    g_inv_screen.name_item = -1;
    g_inv_screen.msg_entry = -1;
    /* wave 4: the CHECK-slid layout registers at their init values (FUN_800460b8 /
     * LAB_80049524 register block — same source as the #define bases below) */
    g_inv_screen.arms_x   = ARMS_BX;   /* DAT_800b25d8 = 126 */
    g_inv_screen.cond_x   = ECG_BX;    /* DAT_800b25e4 = 13  */
    g_inv_screen.idcard_x = ID_BX;     /* DAT_800b25f0 = 14  */
    g_inv_screen.idcard_y = ID_BY;     /* DAT_800b25f2 = 26 (25f0=0xe,25f2=0x1a in the
                                        * FUN_800460b8 register block — FILE wave: the
                                        * FILE slide moves 25f2 @0x800c6d60/0x800c6f58) */
    /* MAP wave: the remaining slid base registers (same init block LAB_80049524 /
     * FUN_800460b8 register writes as the #define bases) */
    g_inv_screen.list_x   = LIST_BX;   /* DAT_800b25e0 = 215 */
    g_inv_screen.ecg_y    = ECG_BY;    /* DAT_800b25e6 = 82  */
    g_inv_screen.arms_y   = ARMS_BY;   /* DAT_800b25da = 26  */
    /* MAP page/room-slot mirrors of the PERSISTENT globals 260d/260e (the menu init
     * writes neither — stale-previous across opens, byte-true) */
    g_inv_screen.map_room = s_map_room;
    g_inv_screen.map_page = s_map_page;
    g7_label_reset();                  /* prim arenas rebuilt at init -> template label */
    re15_inv_screen_sync_equip();
}

void re15_inv_screen_heal_wipe(uint8_t id)
{
    /* Wipe dispatch @0x8004ae24-ae68 via table @0x80010f84, index id-0x22 (sltiu 0xd —
     * ids outside 0x22..0x2e, e.g. 0x2f NUT, get no wipe): [1]=0x23 Antidote / [4]=0x26
     * Blue -> 25d4=2 (@0x8004ae70); [12]=0x2e G+R+B -> none (jumps to the c4++ tail,
     * byte-true prototype inconsistency); [0],[2],[3],[5..11] -> 25d4=1 (@0x8004ae84).
     * (WAVE 3: the ECG sweep reset 2600:=0x20 @0x8004b038 moved to the heal c4==2 exit
     * in menu_common.c — the original arms the wipe at c4==0, BEFORE the consume.) */
    if (id == 0x23 || id == 0x26) { g_inv_screen.wipe_mode = 2; g_inv_screen.wipe_h = 0x6a; }
    else if (id >= 0x22 && id <= 0x2d) { g_inv_screen.wipe_mode = 1; g_inv_screen.wipe_v = 0x20; }
}

void re15_inv_screen_ecg_tick(void)
{
    /* FUN_80048a44 head @0x80048ab4-ac4: prev>0x7f -> 0x20, else +1 */
    if (g_inv_screen.ecg_sweep > 0x7f) g_inv_screen.ecg_sweep = 0x20;
    else                               g_inv_screen.ecg_sweep++;
    /* @0x80048ac8-b34: glow +=4 while phase<0x20 else -=4; phase wraps at 0x40 */
    if ((int8_t)g_inv_screen.ecg_phase < 0x20) g_inv_screen.ecg_glow = (uint8_t)(g_inv_screen.ecg_glow + 4);
    else                                       g_inv_screen.ecg_glow = (uint8_t)(g_inv_screen.ecg_glow - 4);
    g_inv_screen.ecg_phase++;
    if ((int8_t)g_inv_screen.ecg_phase >= 0x40) g_inv_screen.ecg_phase = 0;
    /* Condition-change WIPE cursor advance (FUN_80048a44 wipe modes, FUN_80048a44.c:61-125,
     * self-clear @0x80048eec): mode 1 = 25fe +3/frame from 0x20, >=0x80 -> reset 0x20 +
     * 25d4:=0 (32 frames); mode 2 = 25fc -2/frame from 0x6a, <=0x22 -> reset 0x6a +
     * 25d4:=0 (36 frames). Advanced here (once per drawn frame) like the sweep above. */
    if (g_inv_screen.wipe_mode == 1) {
        g_inv_screen.wipe_v = (uint8_t)(g_inv_screen.wipe_v + 3);
        if (g_inv_screen.wipe_v >= 0x80) { g_inv_screen.wipe_v = 0x20; g_inv_screen.wipe_mode = 0; }
    } else if (g_inv_screen.wipe_mode == 2) {
        g_inv_screen.wipe_h = (uint8_t)(g_inv_screen.wipe_h - 2);
        if (g_inv_screen.wipe_h <= 0x22) { g_inv_screen.wipe_h = 0x6a; g_inv_screen.wipe_mode = 0; }
    }
}

int re15_inv_screen_condition(int hp, int poisoned)
{
    /* FUN_8004ed6c @0x8004ed6c: u16 @0x800acaec bit1 -> 3 (poison);
     * thresholds lb @0x800112b4 (=0x50=80) / @0x800112b5 (=0x14=20) */
    if (poisoned) return 3;
    if (hp >= 80) return 0;
    if (hp >= 20) return 1;
    return 2;
}

/* ==================================================================================== */
/* MAP wave — per-stage entry inits + the per-frame marker formula                      */
/* ==================================================================================== */

uint8_t re15_inv_map_room(void) { return s_map_room; }
uint8_t re15_inv_map_page(void) { return s_map_page; }

void re15_inv_map_stage_init(int stage, int room)
{
    /* Dispatch table @0x80074c0c = {0x8004b568, 0x8004b8a0, 0x8004b9d4, 0x8004bc9c,
     * 0x8004bdd4, 0x8004bf70}, indexed by the 0-based stage (lh DAT_800b0fe0), called
     * at MAP entry @0x8004997c-98 (both the tab-1 confirm and the L1 instant launch
     * @0x8004980c-30 j 0x8004997c). Each init switches on the 0-based room index
     * (lh DAT_800b0fe2) via its own jump table; every in-range case chain ends in a
     * tail that writes 260e (page) and 260d (room + per-stage base). */
    switch (stage) {
    case 0:
        /* FUN_8004b568: bound sltiu 0x26 @0x8004b574 (out-of-range: jr @0x8004b898,
         * NO write); jump table @0x8001103c (38 entries, fall-through stub chains);
         * 260d = the raw room index (every stub: lbu 0x800b0fe2 -> sb 0x800b260d,
         * e.g. @0x8004b5a0-ac; join @0x8004b894). Page tails: rooms 0-11 -> 2
         * (@0x8004b680-684), 12-17 -> 3 (@0x8004b6f4-6f8), 18-22 -> 4 (@0x8004b754-758),
         * 23 -> 5 (@0x8004b764-768), 24-29 -> 0 (sb zero -> 260e @0x8004b7dc, own exit
         * @0x8004b7e8), 30-37 -> 1 (@0x8004b884 + join @0x8004b888-894). */
        if ((unsigned)room >= 0x26) return;
        s_map_room = (uint8_t)room;
        if      (room <= 11) s_map_page = 2;
        else if (room <= 17) s_map_page = 3;
        else if (room <= 22) s_map_page = 4;
        else if (room == 23) s_map_page = 5;
        else if (room <= 29) s_map_page = 0;
        else                 s_map_page = 1;
        return;
    case 1:
        /* FUN_8004b8a0: bound sltiu 0xc @0x8004b8ac; table @0x800110d4; join
         * @0x8004b9b8-c8: 260e = v1, 260d = room + 38 (addiu +38 @0x8004b9c0).
         * Tails: rooms 0-9 -> 6 (@0x8004b990-994), 10-11 -> 0xd (@0x8004b9b4).
         * NOTE page 0xd's CD file id overruns the 13-entry table @0x80074c4c:
         * u16[13] = 0 (bytes @0x80074c66, in-blob) -> CD file 0 (byte-true quirk). */
        if ((unsigned)room >= 0xc) return;
        s_map_room = (uint8_t)(room + 38);
        s_map_page = (room <= 9) ? 6 : 0xd;
        return;
    case 2:
        /* FUN_8004b9d4: bound sltiu 0x20 @0x8004b9e0; table @0x80011104 (all 32 cases
         * fall through); single tail @0x8004bc74-90: page 7 (ori v1,7 @0x8004bc7c),
         * 260d = room + 50 (addiu +50 @0x8004bc88). */
        if ((unsigned)room >= 0x20) return;
        s_map_room = (uint8_t)(room + 50);
        s_map_page = 7;
        return;
    case 3:
        /* FUN_8004bc9c: bound sltiu 0xc @0x8004bca8; table @0x80011184; single tail:
         * page 8 (ori v1,8 @0x8004bdb4, sb @0x8004bdbc), 260d = room + 65
         * (addiu +65 @0x8004bdc0). */
        if ((unsigned)room >= 0xc) return;
        s_map_room = (uint8_t)(room + 65);
        s_map_page = 8;
        return;
    case 4:
        /* FUN_8004bdd4: bound sltiu 0x15 @0x8004bde0; table @0x800111b4; join
         * @0x8004bf54-64: 260e = v1, 260d = room + 77 (addiu +77 @0x8004bf5c).
         * Tails: rooms 12-14 -> 0xa (cases 12/13 stubs @0x8004beb0/bec4 fall into the
         * case-14 tail @0x8004bed8-ee4 ori v1,0xa), 15-16 -> 0xb (case-15 stub
         * @0x8004bee8 falls into case-16 tail @0x8004befc-f08 ori v1,0xb), all others
         * -> 9 (rooms 0-7 chain @0x8004be08-bea4 ends j 0x8004bf0c @0x8004bea8; rooms
         * 8-11 and 17-20 map onto the same stubs 0x8004bf0c/bf20/bf34/bf48 per the
         * table; final tail ori v1,9 @0x8004bf50). */
        if ((unsigned)room >= 0x15) return;
        s_map_room = (uint8_t)(room + 77);
        if      (room >= 12 && room <= 14) s_map_page = 0xa;
        else if (room == 15 || room == 16) s_map_page = 0xb;
        else                               s_map_page = 9;
        return;
    case 5:
        /* FUN_8004bf70: bound sltiu 0x8 @0x8004bf7c; table @0x8001120c (8 consecutive
         * stubs); tail @0x8004c030-4c: page 0xc (ori v1,0xc @0x8004c038), 260d =
         * room + 98 (addiu +98 @0x8004c044). */
        if ((unsigned)room >= 0x8) return;
        s_map_room = (uint8_t)(room + 98);
        s_map_page = 0xc;
        return;
    default:
        /* The dispatch table @0x80074c0c has exactly 6 entries; the stage register is
         * only ever written 0..5 by the room setup (@0x8001d808). */
        return;
    }
}

void re15_inv_map_marker(int32_t world_x, int32_t world_z, uint8_t room_slot,
                         int16_t *mx, int16_t *my)
{
    /* FUN_800473f8 @0x8004741c-0x80047528 (raw MIPS, settles the wave-3 flagged
     * discrepancy): THIS formula is the displayed one — it rewrites the marker quad's
     * xy every gated frame (stores @0x80047568/757c/7590/75a8 x, @0x80047584/7598/
     * 75b0/75c4 y) BEFORE the AddPrim @0x800475d8, so the builder FUN_80046fd8's
     * (world+25000)/scale DIV result (@0x80047010-14, parked in DAT_800b2604/2606
     * @0x80047098/@0x80047124) is never displayed (2604 has zero readers outside the
     * builder itself, ghidra xref list).
     *   x: t = mflo((world_x+32000)*10 * x_scale) sra 20 (@0x80047428-5c: addiu 32000,
     *      *5<<1, mult, mflo, sra 20 — 32-bit wrapping product, arithmetic shift);
     *      t += 5 (@0x80047460); t /= 10 (magic 0x66666667 @0x80047450-54, mfhi sra 2
     *      minus sign bit @0x80047488/0x800474d0-d8 = C truncation toward zero);
     *      t += x_off (lh SIGNED @0x800474e8).
     *   y: t2 likewise from world_z * z_scale (@0x8004746c-a8), t2 += 5 (@0x800474ac),
     *      NEGATED (subu zero @0x800474b0), /10 (@0x800474b4/0x80047500-508),
     *      += y_off (lhu @0x80047518); stored to DAT_800b2606 (@0x80047528). */
    uint32_t row  = 0x800768B0u + (uint32_t)room_slot * 8u;
    int16_t  xoff = (int16_t)mu16(row);          /* lh  @0x800474e8 */
    uint16_t yoff = mu16(row + 2u);              /* lhu @0x80047518 */
    uint16_t xscl = mu16(row + 4u);              /* lhu @0x80047444 */
    uint16_t zscl = mu16(row + 6u);              /* lhu @0x8004747c */
    int32_t t, t2;
    t  = (int32_t)((uint32_t)(world_x + 32000) * 10u * xscl) >> 20;  /* sra @0x8004745c */
    t += 5;                                                          /* @0x80047460 */
    t  = t / 10;                                                     /* @0x80047464-d8 */
    *mx = (int16_t)(t + xoff);
    t2  = (int32_t)((uint32_t)(world_z + 32000) * 10u * zscl) >> 20; /* sra @0x800474a8 */
    t2 += 5;                                                         /* @0x800474ac */
    t2  = -t2;                                                       /* @0x800474b0 */
    t2  = t2 / 10;                                                   /* @0x800474b4-508 */
    *my = (int16_t)(t2 + (int32_t)yoff);
}

/* ---- WAVE 5: icon-cache cell art override (see re15_inv_screen.h for the model +
 * citations). 0 = identity; 1..14 = MIXITEM pic; 0x80|tile = frozen ITEMALL tile. */
static uint8_t s_icon_ovr[10];

void re15_inv_icon_reset(void) { memset(s_icon_ovr, 0, sizeof s_icon_ovr); }

void re15_inv_icon_mix_upload(int cell, int pic)
{
    /* FUN_800492b8(cell, 0, 0x801a0000 + (pic-1)*1200) — the EXCHANGE executor's
     * result-tile upload (a2 math sll2+add/sll4-sub/sll4 = (pic-1)*1200 @0x8004e224-38
     * et al.). pic is always 1..14 in the shipped pair data (every action-1/4/6 pair
     * carries a nonzero pic byte; pic==0 exists only on the action-2/3/5 reload pairs,
     * which never reach an upload site). */
    if (cell >= 0 && cell <= 9) s_icon_ovr[cell] = (uint8_t)(pic & 0x7f);
}

void re15_inv_icon_copy(int src, int dst)
{
    /* FUN_80049390(a0=src, a1=dst): MoveImage of the 40x30 cell rect (RECT built from
     * a0 @0x80049448/0x80049460, dest xy from a1 @0x800493f4-408) — the destination
     * cell now shows the source cell's CURRENT art, so the override value moves with
     * it (identity stays identity because the slot bytes are copied by the same ops). */
    if (src >= 0 && src <= 9 && dst >= 0 && dst <= 9) s_icon_ovr[dst] = s_icon_ovr[src];
}

void re15_inv_icon_blank(int cell)
{
    /* FUN_8004947c(cell): MoveImage from the static blank rect (680,346) 40x30
     * (@0x80049494-b4: {0x2a8,0x15a,0x14,0x1e}) — always paired with the slot's
     * id:=0 write, so identity (empty -> ITEMALL tile 0 = blank) re-holds. */
    if (cell >= 0 && cell <= 9) s_icon_ovr[cell] = 0;
}

void re15_inv_icon_freeze_tile(int cell, int tile)
{
    /* Reload-full writes the id byte (@0x8004e370/@0x8004e47c) with ZERO icon calls in
     * its branch (no jal in 0x8004e34c-e3e0 / 0x8004e458-e4ec) — the cell keeps the OLD
     * art. Freeze the identity cell at its pre-change tile; an already-overridden cell
     * keeps its override (== "no VRAM write"). */
    if (cell >= 0 && cell <= 9 && s_icon_ovr[cell] == 0)
        s_icon_ovr[cell] = (uint8_t)(0x80 | (tile & 0x7f));
}

int re15_inv_screen_cache_mix_pic(int cell)
{
    if (cell < 0 || cell > 9) return 0;
    return (s_icon_ovr[cell] != 0 && !(s_icon_ovr[cell] & 0x80)) ? s_icon_ovr[cell] : 0;
}

int re15_inv_screen_cache_tile(int cell)
{
    if (cell < 0 || cell > 9) return -1;      /* cells 10/11 = ST_00 static art (Q3) */
    {
        uint8_t id = g_inv.slots[cell].id;
        uint8_t kind = g_inv.slots[cell].flags;
        if (kind == 1 || kind == 2) return -2; /* wide weapon: 80x30 from ITPS blk+0x21A0
                                                * (FUN_800492b8 mode1) — OPEN in wave 1 */
        if (s_icon_ovr[cell] & 0x80)           /* wave 5: frozen art (reload-full id
                                                * change without a VRAM write)          */
            return s_icon_ovr[cell] & 0x7f;
        return id;                             /* identity map incl. 0x15 (Q3 byte-proof);
                                                * empty slot -> tile 0 = blank navy */
    }
}

/* ---- op emission helpers ---- */
typedef struct {
    re15_inv_op_t *ops;
    int n, max;
} emit_t;

static void sprt(emit_t *e, int page, int clut, int x, int y, int w, int h,
                 int u, int v, int r, int g, int b, int abe)
{
    re15_inv_op_t *o;
    if (e->n >= e->max) return;
    o = &e->ops[e->n++];
    o->kind = RE15_INV_OP_SPRT; o->page = (uint8_t)page; o->clut = (uint8_t)clut;
    o->abe = (uint8_t)abe;
    o->x = (int16_t)x; o->y = (int16_t)y; o->w = (int16_t)w; o->h = (int16_t)h;
    o->u = (uint8_t)u; o->v = (uint8_t)v;
    o->r = (uint8_t)r; o->g = (uint8_t)g; o->b = (uint8_t)b;
}

/* group template row: stride 12 {s16 x,y,w,h; u8 u @+8; u8 v @+10} (FUN_800467a8
 * builder walk @0x800467a8: x=tmpl+0, y=+2, w=+4, h=+6, u=+8, v=+10) */
static void tmpl_row(uint32_t tmpl, int i, int *x, int *y, int *w, int *h, int *u, int *v)
{
    uint32_t a = tmpl + (uint32_t)i * 12u;
    *x = bs16(a); *y = bs16(a + 2); *w = bs16(a + 4); *h = bs16(a + 6);
    *u = bu8(a + 8); *v = bu8(a + 10);
}

static void master_row(int gr, int *clut_idx, int *count, uint32_t *tmpl)
{
    uint32_t a = MASTER_TBL + (uint32_t)gr * 12u;
    *clut_idx = bu16(a); *count = bu16(a + 2); *tmpl = bu32(a + 4);
}

/* FUN_80048f28 @0x80048f28 (disasm-verified): qty digits for one slot.
 * equip_mode = the a3 flag (0 = grid entry, 1 = ARMS equip-panel echo). */
static void emit_digits(emit_t *e, const re15_inv_screen_t *st, int slot, int equip_mode)
{
    uint8_t id = g_inv.slots[slot].id;
    uint8_t kind, qty;
    int hund, tens, ones, sx, di;
    if (id == 0) return;                            /* @0x80048f84 */
    kind = g_inv.slots[slot].flags;
    if (kind == 2) {                                /* @0x80048f9c-fb8: wide tail cell */
        if (!equip_mode) return;                    /* grid: tail draws no digits */
        slot -= 1;                                  /* equip mode: use the head slot */
        id = g_inv.slots[slot].id;
    }
    if (slot >= CAPACITY) return;                   /* @0x80048fbc-fcc */
    if (id >= 0x22) return;                         /* sltiu id,0x22 @0x80049124 */
    qty = g_inv.slots[slot].qty;
    hund = qty / 100; tens = (qty % 100) / 10; ones = qty % 10;  /* magic divides
                                                     * 0x51eb851f/0xcccccccd @0x80048fd0+ */
    sx = 3;                                         /* x accumulator init 3 @0x80048f44 */
    for (di = 2; di >= 0; di--) {
        int dv = (di == 2) ? hund : (di == 1) ? tens : ones;
        int x, y;
        if (di == 2 && hund == 0) continue;         /* @0x800490d8-e4 */
        if (di == 1 && tens == 0 && hund == 0) continue;  /* @0x800490ec-108 */
        if (!equip_mode) {
            /* @0x80049138-180: cell table + list base 25e0/25e2 LIVE (lhu 0(s6) =
             * DAT_800b25e0 @0x80049148, ghidra-resolved), y+20 */
            x = bs16(CELL_TBL + (uint32_t)slot * 4u) + st->list_x + sx;
            y = bs16(CELL_TBL + (uint32_t)slot * 4u + 2u) + LIST_BY + 20;
        } else {
            /* @0x80049184-1e0: equip panel 25dc/25de (kind2: x-40) */
            uint8_t eq = st->equipped_slot;
            if (eq == 0x80) return;
            x = st->equip_x + sx - ((g_inv.slots[eq & 0x0f].flags == 2) ? 40 : 0);
            y = st->equip_y + 20;
        }
        /* glyph: 8x8 SPRT code 0x66, u=digit*8, v=0xf0 (@0x80049214-28), rgb neutral
         * (never written — Q1), clut = DAT_800b2610[2 + prop8] (@0x8004922c-4c) */
        sprt(e, RE15_INV_PAGE_TEX4,
             2 + bu8(PROP_TBL + (uint32_t)id * 12u + 8u),
             x, y, 8, 8, dv * 8, 0xf0, 128, 128, 128, 1);
        sx += 6;                                    /* @0x80049230 */
    }
}

/* ---- grid-mode ITEM-NAME print (wave 2) --------------------------------------------
 * FUN_80028c1c @0x80028c1c, called ONLY from the grid tail @0x800c65a8-65d8 as
 * (x=0x18, y=0xa8, flags=1, id). String resolve FUN_80028840 @0x80028844:
 * ptr = 0x800c4a28 + u16[0x800c495c + id*2]. Stream decoder FUN_80013160 @0x80013160
 * (decompile RE_15_Quellcode_V2/FUN_80013160.c): pending u16 @0x800c44b6 (0xffff = none)
 * returned first (advances the string); direct codes (c+0xa0)&0xff > 0x58 pass through;
 * else digraph pair @0x800c44b8/b9 + (c-0xa0)*2 — first half returned WITHOUT advancing,
 * second stashed as pending. Loop ends on decoded code 7 (@0x80028ce4-ce8). */
static uint16_t s_name_pending = 0xffff;   /* DAT_800c44b6 (file init value 0xffff) */

static int name_next_code(const uint8_t **pp)
{
    unsigned c;
    if (s_name_pending != 0xffff) {
        c = s_name_pending; s_name_pending = 0xffff; (*pp)++;
        return (int)c;
    }
    c = **pp;
    if (((c + 0xa0) & 0xff) > 0x58) { (*pp)++; return (int)c; }
    {
        int idx = ((int)c - 0xa0) * 2 + RE15_INV_NAME_DIGRAPH_BIAS;
        s_name_pending = re15_inv_name_digraph[idx + 1];
        return re15_inv_name_digraph[idx];
    }
}

static void emit_name(emit_t *e, int id)
{
    const uint8_t *p;
    int x = 0x18, code, guard = 64;              /* a0=0x18 @0x800c65ac; guard = port safety */
    if (id < 0 || id >= RE15_INV_NAME_NIDS) return;
    p = re15_inv_name_blob + re15_inv_name_off[id];
    while ((code = name_next_code(&p)) != 7 && guard-- > 0) {   /* terminator @0x80028ce4-ce8 */
        /* 16x16 SPRT, prim word 0x7c808080 (rgb neutral) @0x80028d14-1c; glyph cell
         * u=(code&0xf)<<4, v=((code>>4)<<4)+0x20 @0x80028d04-3c on the message-FONT
         * page (TEX.TIM texel cols 256-511, font_atlas_psx.h region y[32..128]);
         * clut row 0 (0x7810) @0x80028cb8-c0 (flags=1). */
        sprt(e, RE15_INV_PAGE_FONT4, RE15_INV_CLUT_TEXROW0,
             x, 0xa8, 16, 16, (code & 0xf) << 4, ((code >> 4) << 4) + 0x20,
             128, 128, 128, 0);
        x += re15_font_width[code & 0xff];       /* advance @0x80028d78-94 */
        if (e->n >= e->max) break;
    }
}

/* ---- cant-use message "You can't use it here." (wave 3) ----------------------------
 * DEBUG.BIN desc-bank entry 0, resolved by FUN_80027e68 (a1&0xc00==0x400 path): ptr =
 * 0x800c50de + u16[0x800c50de] (= off 0x90). Bytes re-read from shared_assets/PSX/BIN/
 * DEBUG.BIN @0x50de+0x90 this wave (terminator 01 00 = msg-VM end-mode -> state 5
 * PRESS-WAIT; 0x3a = apostrophe glyph, 0x57 = period glyph, chr = code+0x24). */
const uint8_t re15_inv_cantuse_text[RE15_INV_CANTUSE_GLYPHS] = {
    0x35,0x4b,0x51,0x00,0x3f,0x3d,0x4a,0x3a,0x50,0x00,0x51,0x4f,0x41,0x00,
    0x45,0x50,0x00,0x44,0x41,0x4e,0x41,0x57
};

static void emit_msg(emit_t *e, const re15_inv_screen_t *st)
{
    /* Generalized msg-slice emitter (wave 4; supersedes the wave-3 cant-use-only path —
     * entry 0 of the same bank IS the cant-use text). Position (msg_x,msg_y) =
     * DAT_800b8534/8536 (FUN_80027e68 a0 = y<<16|x). Emitted by the msg VM into the
     * shared text ring (FUN_80028868); per code: 0x00 = space (pen += width[0], emitter
     * LAB_80028bc8), 0x08 = NEWLINE (x := 8534, y += 0x10 — FUN_80028868 case 8:
     * `uVar11 = 8534 | ((y>>16)+0x10)<<16`), glyphs = 16x16 atlas cell u=(code&0xf)<<4,
     * v=((code>>4)<<4)+0x20 (prim word 0x64808080, clut default 0x7810 = TEX.TIM CLUT
     * row 0 — FUN_80028868 `uVar12 = 0x7810`; the bank uses no 05 color controls),
     * pen advance = (&DAT_800c4416)[code]. Only the current PAGE is drawn (display
     * start DAT_800b8528 resets on the page-wait confirm — VM state 2). */
    const uint8_t *s = re15_inv_desc_entry(st->msg_entry);
    int x, y, i, left = st->msg_reveal;
    if (!s) return;
    x = st->msg_x; y = st->msg_y; i = st->msg_page_off;
    while (left > 0 && e->n < e->max) {
        uint8_t c = s[i];
        if (c == 0x01 || c == 0x02) break;            /* end / page break (safety) */
        if (c == 0x08) { x = st->msg_x; y += 0x10; i++; continue; }
        if (c == 0x00) { x += re15_font_width[0]; i++; left--; continue; }
        sprt(e, RE15_INV_PAGE_FONT4, RE15_INV_CLUT_TEXROW0,
             x, y, 16, 16, (c & 0xf) << 4, ((c >> 4) << 4) + 0x20,
             128, 128, 128, 0);
        x += re15_font_width[c];
        i++; left--;
    }
    /* PAGE-WAIT arrow (VM state 2 @FUN_80028134 case 2: blink (8525&0x30)!=0 ->
     * FUN_800c69bc(8536,...) pos = ((y+44)<<16)|0x6d (8542==0, savestate) +
     * FUN_800279c8(pos, 0x80, "\x2f") -> debug-text drawer FUN_80029214: 8x8 SPRT
     * (prim 0x74808080), cell u=(0x2f-0x20)%32*8=120, v=(0x2f-0x20)/32*8=0, clut
     * ((0x80&0x30)>>3)+0x1e0 -> row 0 = 0x7810). */
    if (st->msg_arrow)
        sprt(e, RE15_INV_PAGE_FONT4, RE15_INV_CLUT_TEXROW0,
             0x6d, st->msg_y + 44, 8, 8, 120, 0, 128, 128, 128, 0);
}

/* ==================================================================================== */
/* FILE wave — the FUN_800c6ca0 screen's draw set (DEBUG.BIN, file==RAM).               */
/* ==================================================================================== */

/* Line-width sum = DEBUG.BIN 0x800c0214 (called by the printer's center/right controls
 * @0x80028fe8/0x80029014 and the footer @0x800c7784): walks the RAW bytes (NO digraph
 * decode), sums (&0x800c4416)[byte], stops on 7 / 8 / 1 (@0x800c0224-38). */
static int file_line_width(const uint8_t *p)
{
    int wsum = 0;
    while (*p != 7 && *p != 8 && *p != 1)
        wsum += re15_font_width[*p++];
    return wsum;
}

/* Byte-true FUN_80028ec4 @0x80028ec4 (raw-verified this wave) — the glyph-code text
 * printer the FILE screen uses for rows/title/reader/footer. Per decoded code
 * (FUN_80013160 digraph stream @0x80028f6c, shared pending s_name_pending):
 *   7 / 1        end (@0x80028f88-90)
 *   8            newline: y += 0x10, x = x0 (@0x80028fdc-e4)
 *   9            center: u16 LE operand (lbu 0(s2)/1(s2) @0x80028ff4-9000, s2 += 2),
 *                x = x0 + op - width(rest)/2 (@0x80028fe8-9010)
 *   10 (0x0a)    right-align: x = x0 + op - width(rest) (@0x80029014-38)
 *   5            inline palette op (1 byte @0x8002903c-5c) — absent from the vendored
 *                FILE data (generator census); operand skipped, clut kept
 *   0xfb         nothing (@0x80028fc0-c4); 0xfc -> x += 7 (@0x80029060-70)
 *   else         glyph: SPRT_16 prim 0x7c808080 (@0x8002909c-a0), u=(c&0xf)<<4
 *                (@0x800290d0-d8), v=((c>>4)<<4)+0x20 (@0x8002908c-98), clut = t8,
 *                x += width[c] (@0x8002910c-24). Code 0 (space) IS a glyph (blank
 *                cell (0,0x20), width 4) — it falls through the control chain.
 * clut = 0x7810 | (a3&0x30)<<3 (@0x80028f5c-64) -> TEX.TIM CLUT rows 0/2/4/6.
 * BYTE-TRUE QUIRK (rows 0x59-0x5b '&'): digraph byte 0x64 indexes the pair table
 * SIGNED (@0x800131c0-c4) into width bytes @0x800c4440 = codes {9,8} -> the printer
 * runs them as CENTER (consuming the next 2 RAW bytes as the operand, desyncing the
 * pending) + NEWLINE. Reproduced exactly by sharing the raw pointer between the
 * digraph decoder and the operand reads. */
static void emit_text(emit_t *e, int x0, int y0, const uint8_t *p, int flags)
{
    int clut = ((flags & 0x30) == 0x00) ? RE15_INV_CLUT_TEXROW0
             : ((flags & 0x30) == 0x10) ? RE15_INV_CLUT_TEXROW2
             : ((flags & 0x30) == 0x20) ? RE15_INV_CLUT_TEXROW4
             :                            RE15_INV_CLUT_TEXROW6;
    int x = x0, y = y0, code, guard = 512;      /* guard = port safety only */
    while (guard-- > 0 && e->n < e->max) {
        code = name_next_code(&p);              /* FUN_80013160 @0x80028f6c */
        if (code == 7 || code == 1) break;
        if (code <= 10) {
            if (code == 8) { y += 0x10; x = x0; continue; }
            if (code == 9) {
                int op = p[0] | (p[1] << 8); p += 2;
                x = x0 + op - file_line_width(p) / 2;
                continue;
            }
            if (code == 10) {
                int op = p[0] | (p[1] << 8); p += 2;
                x = x0 + op - file_line_width(p);
                continue;
            }
            if (code == 5) { p += 1; continue; }
            /* 0,2,3,4,6 fall through to the glyph draw (@0x80028fb8) */
        } else if (code == 0xfb) {
            continue;
        } else if (code == 0xfc) {
            x += 7;
            continue;
        }
        sprt(e, RE15_INV_PAGE_FONT4, clut, x, y, 16, 16,
             (code & 0xf) << 4, (((code >> 4) << 4) + 0x20) & 0xff,
             128, 128, 128, 0);
        x += re15_font_width[code & 0xff];
    }
}

/* Reader footer "page/total" = DEBUG.BIN 0x800c7744: number formatter 0x800c78a8
 * (tens+0xc only when nonzero @0x800c78bc-c8, ones+0xc @0x800c78d0-d4), separator
 * glyph 0x38 (@0x800c775c-60), terminator 7 (@0x800c7778-7c); centered x =
 * 0xa0 - width/2 (jal 0x800c0214 @0x800c7784 + srl/subu @0x800c778c-94), y = 0xd2,
 * a3 = 0x20 (@0x800c7798-a8). page = the CLAMPED reader page (caller s0). */
static void emit_file_footer(emit_t *e, int page_clamped, int total)
{
    uint8_t buf[8];
    int n = 0, v;
    v = page_clamped + 1;                       /* a1 = s0+1 @0x800c774c */
    if (v >= 10) buf[n++] = (uint8_t)(v / 10 + 0xc);
    buf[n++] = (uint8_t)(v % 10 + 0xc);
    buf[n++] = 0x38;
    v = total;                                  /* a1 = u16[0xcd34]>>1 @0x800c7768-74 */
    if (v >= 10) buf[n++] = (uint8_t)(v / 10 + 0xc);
    buf[n++] = (uint8_t)(v % 10 + 0xc);
    buf[n++] = 0x07;
    emit_text(e, 0xa0 - file_line_width(buf) / 2, 0xd2, buf, 0x20);
}

/* Reader page text = DEBUG.BIN 0x800c7600: page clamped to count-1 (slt/addiu
 * @0x800c7628-34), text = base + u16[base + page*2] (@0x800c7638-48), printed at
 * (x, 0x20) a3=0 (@0x800c7644-50), then the footer (jal 0x800c7744 @0x800c7654). */
static void emit_file_reader(emit_t *e, const re15_inv_screen_t *st, int x)
{
    int pg = st->file_reader_page;
    if (pg >= RE15_INV_FILEDOC_PAGES) pg = RE15_INV_FILEDOC_PAGES - 1;
    emit_text(e, x, 0x20, re15_inv_filedoc_blob + re15_inv_filedoc_off[pg], 0);
    emit_file_footer(e, pg, RE15_INV_FILEDOC_PAGES);
}

/* Corner arrows = DEBUG.BIN 0x800c7528 (draw part) + glyph drawer 0x800c7670:
 * 16x16 SPRTs, DR_MODE 0xe100001b = the 4bpp TEX page (@0x800c76b0-b8), prim
 * 0x64808080 (@0x800c76bc-c4), uv+clut from the type table @0x800c7734 =
 * {0x7bd03870, 0x7bd04870, 0x7b504870, 0x7b104870}: type0 (left, page>0
 * @0x800c7554-70) uv(0x70,0x38) clut 0x7bd0 = UI row 15 = selector 7; right arrow
 * ALWAYS at x=0x11c+off (@0x800c7598-a8) with type 2 when page==end / 3 when
 * page==end-1 / else 1 (@0x800c7580-94) -> cluts 0x7b50=UI5 / 0x7b10=UI4 / 0x7bd0=UI7,
 * all uv(0x70,0x48). off = the bob offset drawn BEFORE the counter update. */
static void emit_file_arrows(emit_t *e, const re15_inv_screen_t *st)
{
    int end = RE15_INV_FILEDOC_PAGES;           /* s0 = u16[0xcd34]>>1 @0x800c7544-50 */
    int off = (int)st->file_bob_off;
    int pg = st->file_reader_page;
    if (pg != 0)
        sprt(e, RE15_INV_PAGE_TEX4, 7, 0x14 - off, 0x70, 16, 16,
             0x70, 0x38, 128, 128, 128, 0);
    sprt(e, RE15_INV_PAGE_TEX4,
         (pg == end) ? 5 : (pg == end - 1) ? 4 : 7,
         0x11c + off, 0x70, 16, 16, 0x70, 0x48, 128, 128, 128, 0);
}

/* FILE list (state 1) draw set, in the original per-frame call order @0x800c6d70-8c:
 * rows 0x800c72b8 -> title 0x800c727c (+ tab icon 0x800c7374) -> highlight 0x800c742c.
 * Rows: 10 lines at x=0x2c, y=0x35+row*16 (@0x800c7300/0x800c733c); visibility mask
 * u16 @0x800c6c98[page] (lhu @0x800c72f0), bit set -> name FUN_80028840(base+row)
 * printed a3=0 (@0x800c7320-28), clear -> the 21-underscore string a3=0x30
 * (@0x800c7310-1c). Title: table @0x800c78e4[page] at (0x2c,0x1f) a3=0x10
 * (@0x800c7284-9c). Tab icon: 19x19 SPRT at (0x12,0x1a) uv(0x72,0x8e) (@0x800c73b8-f0:
 * xy word 0x001a0012, wh 0x00130013), clut @0x800c7420[page] = {0x7a90,0x7ad0,0x7b10}
 * = UI rows 10/11/12 = selectors 2/3/4. Highlight: subtractive TILE rgb 0x20
 * (@0x800c7464-6c): page level box (0x11,0x19) 140x26 (@0x800c7480-98), row level
 * (0x2b, 0x34+row*16) 154x16 (@0x800c749c-c0; sll row,20 = +y). AddPrim order icon ->
 * highlight prepends into the bucket @0x800aa6a8 (getter 0x800c74f0) => highlight
 * drawn FIRST = behind the icon; the text ring draws topmost (wave-2 model). */
static void emit_file_list(emit_t *e, const re15_inv_screen_t *st)
{
    int pg = st->file_page;
    uint16_t mask;
    int base, row;
    if (pg > 2) pg = 2;                          /* port safety; original indexes raw */
    mask = re15_inv_file_mask[pg];
    base = re15_inv_file_rowbase[pg];
    for (row = 0; row < 10; row++) {
        int y = 0x35 + row * 16;
        if (mask & (1u << row))
            emit_text(e, 0x2c, y,
                      re15_inv_file_name_blob +
                      re15_inv_file_name_off[base + row - 0x48], 0);
        else
            emit_text(e, 0x2c, y, re15_inv_file_underscores, 0x30);
    }
    emit_text(e, 0x2c, 0x1f,
              re15_inv_file_title_blob + re15_inv_file_title_off[pg], 0x10);
    sprt(e, RE15_INV_PAGE_TEX4, 2 + pg, 0x12, 0x1a, 0x13, 0x13,
         0x72, 0x8e, 128, 128, 128, 0);
    if (e->n < e->max) {
        re15_inv_op_t *o = &e->ops[e->n++];
        memset(o, 0, sizeof *o);
        o->kind = RE15_INV_OP_TILE;
        o->abe = 1;
        o->r = o->g = o->b = 0x20;               /* TILE word 0x62202020 @0x800c7464-6c */
        if (st->file_sub == 0) {
            o->x = 0x11; o->y = 0x19; o->w = 0x8c; o->h = 0x1a;   /* @0x800c7480-98 */
        } else {
            o->x = 0x2b; o->y = (int16_t)(0x34 + st->file_row * 16);
            o->w = 0x9a; o->h = 0x10;                              /* @0x800c749c-c0 */
        }
    }
}

/* wave 4: POLY_G4 navy gradient box (DEBUG.BIN 0x800c6b84; corner colors in the
 * rasterizer — prim words @0x800c6bd0/6bdc/6be8/6bf4). */
static void gbox(emit_t *e, int x, int y, int w, int h)
{
    re15_inv_op_t *o;
    if (e->n >= e->max) return;
    o = &e->ops[e->n++];
    memset(o, 0, sizeof *o);
    o->kind = RE15_INV_OP_GBOX;
    o->x = (int16_t)x; o->y = (int16_t)y; o->w = (int16_t)w; o->h = (int16_t)h;
}

/* wave 4: CHECK examine-panel chrome element (DEBUG.BIN 0x800c6aac): 24-byte packet
 * {hdr, DR_MODE 0xe100001d (4bpp texpage (832,256)), SPRT 0x64808080, xy, uv+clut,
 * wh} with uv/clut/wh from the table @0x800c6b64 + idx*8:
 *   [0] uvclut 0x7c104800 wh 0x001f00c9  -> uv(0,0x48) 201x31  (top cap)
 *   [1] uvclut 0x7c106700 wh 0x000800c9  -> uv(0,0x67) 201x8   (frame row)
 *   [2] uvclut 0x7c106f00 wh 0x000600c9  -> uv(0,0x6f) 201x6   (box gap band)
 *   [3] uvclut 0x7c107d00 wh 0x000a00c9  -> uv(0,0x7d) 201x10  (bottom cap)
 * clut 0x7c10 = VRAM (256,496) = TEX.TIM CLUT-block row 16 for all entries. */
static void exam_elem(emit_t *e, int idx, int x, int y)
{
    static const uint8_t ev[4] = { 0x48, 0x67, 0x6f, 0x7d };
    static const uint8_t eh[4] = { 0x1f, 0x08, 0x06, 0x0a };
    sprt(e, RE15_INV_PAGE_EXAM4, RE15_INV_CLUT_TEXROW16,
         x, y, 0xc9, eh[idx], 0, ev[idx], 128, 128, 128, 0);
}

/* g7 element-0 label prim MEMORY (wave 3): the original writes the label uv into the
 * PERSISTENT prim buffer and SKIPS the update while 25c2 in {5,6} (addiu -5; sltiu 2
 * @0x80047d14-28) — the label keeps showing its LAST face through the anim/slide-out.
 * The port rebuilds the op list per frame, so the last computed face lives here.
 * 0xff = not yet computed (arena template default; unreachable in states 5/6 since the
 * command stage always runs state 4 first). */
static uint8_t s_g7_u = 0xff, s_g7_v = 0xff;
static void g7_label_reset(void) { s_g7_u = 0xff; s_g7_v = 0xff; }

/* ==================================================================================== */
/* ITEM BOX subscreen display list (shots/itembox_spec.md §6 — STATUS-SCREEN-STYLE      */
/* composition, [DESIGN, non-canonical layout]: the original has NO box screen           */
/* (itembox_verdict.md), so the panel placement here is invented; every ART/geometry     */
/* PIPELINE is the campaign's byte-true one (chrome g0 @0x80075108, ITEM-LIST panel g1,  */
/* cell table @0x80076274, cursor g8 templates, digit glyphs FUN_80048f28, name bank     */
/* FUN_80028c1c, text printer FUN_80028ec4, backdrop tiles FUN_80046a1c). Emitted as a   */
/* SEPARATE branch so the normal status-screen list stays bit-identical (idle pixel      */
/* regression).                                                                          */
/* ==================================================================================== */
#define BOX_BX  24    /* [DESIGN] box panel base x — mirrors the ITEM-LIST panel
                       * geometry (base 215) on the left half; cells at 28/68 */
#define BOX_BY  26    /* [DESIGN] same base y as the ITEM LIST (LIST_BY)      */

/* one box/inv icon cell via the ITEMTILE page (v = tile id; see the enum note) */
static void box_cell_icon(emit_t *e, int x, int y, uint8_t id, uint8_t kind)
{
    (void)kind;   /* boxed-wide (kind 3) draws its ITEMALL 40x30 tile in the ONE
                   * cell [DESIGN — RE2 draws a wide bright ROW (§2.5); the icon
                   * grid has no wide rows, the single kept slot mirrors quirk 6's
                   * "ONE slot" storage semantics]. Tile identity = ITEMALL[id]
                   * (report-C byte-proof; empty id 0 = tile 0 blank navy). */
    sprt(e, RE15_INV_PAGE_ITEMTILE, RE15_INV_CLUT_ST00_ROW0,
         x, y, 40, 30, 0, id, 128, 128, 128, 1);
}

/* qty digits for a BOX slot — the FUN_80048f28 glyph geometry verbatim (8x8 SPRT
 * code u=digit*8 v=0xf0 @0x80049214-28, x accumulator init 3 step 6 @0x80048f44/
 * @0x80049230, leading-zero suppression @0x800490d8-108, y = cell+20, clut =
 * DAT_800b2610[2 + prop8] @0x8004922c-4c with prop8 @0x80074DA8+id*12+8; the
 * id >= 0x22 digit gate `sltiu id,0x22` @0x80049124) at the box cell position. */
static void box_digits(emit_t *e, int cx, int cy, uint8_t id, uint8_t qty)
{
    int hund, tens, ones, sx, di;
    if (id == 0 || id >= 0x22) return;
    hund = qty / 100; tens = (qty % 100) / 10; ones = qty % 10;
    sx = 3;
    for (di = 2; di >= 0; di--) {
        int dv = (di == 2) ? hund : (di == 1) ? tens : ones;
        if (di == 2 && hund == 0) continue;
        if (di == 1 && tens == 0 && hund == 0) continue;
        sprt(e, RE15_INV_PAGE_TEX4,
             2 + bu8(PROP_TBL + (uint32_t)id * 12u + 8u),
             cx + sx, cy + 20, 8, 8, dv * 8, 0xf0, 128, 128, 128, 1);
        sx += 6;
    }
}

static int build_box_mode(const re15_inv_screen_t *st, re15_inv_op_t *ops, int max_ops)
{
    emit_t e; int i, clut_idx, count; uint32_t tmpl;
    int x, y, w, h, u, v;
    e.ops = ops; e.n = 0; e.max = max_ops;

    /* 0. hovered-item name caption (topmost; the campaign's byte-true name-bank
     * print FUN_80028c1c at (0x18,0xa8) — set per tick by re15_itembox.c). */
    if (st->name_item >= 0) emit_name(&e, st->name_item);

    /* 0b. the reject message slice (desc-bank entry 0 through the shared msg VM —
     * the RE1.5 cant-use infra standing in for RE2's box reject FUN_8002fe38). */
    if (st->msg_entry >= 0 && (st->msg_reveal > 0 || st->msg_arrow)) emit_msg(&e, st);

    /* 1. EXIT affordance [DESIGN]: the word "EXIT" in the byte-true FUN_80028ec4
     * glyph printer (codes = chr-0x24: E=0x21 X=0x34 I=0x25 T=0x30, end 0x07);
     * bright TEX row 0 when the EXIT row is selected, the dim row-6 palette
     * (0x30 flag — the FILE hidden-row palette) otherwise. ≙ RE2's EXIT row with
     * highlight 0x80/0x40 @§2.5. */
    {
        static const uint8_t k_exit[5] = { 0x21, 0x34, 0x25, 0x30, 0x07 };
        emit_text(&e, 0x90, 0xd8, k_exit, (st->box_side == 2) ? 0x00 : 0x30);
    }

    /* 2. page indicator "N/4" [DESIGN §6] in the FILE-footer digit codes
     * (digit = value+0xc, separator glyph 0x38 — the 0x800c7744 formatter). */
    {
        uint8_t buf[5];
        buf[0] = (uint8_t)(st->box_page + 1 + 0xc);
        buf[1] = 0x38;
        buf[2] = (uint8_t)(4 + 0xc);
        buf[3] = 0x07;
        emit_text(&e, BOX_BX + 30, 0xb6, buf, 0x20);
    }

    /* 3. chrome group 0 (FUN_80047648(0) — absolute, always drawn) */
    master_row(0, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x, y, w, h, u, v, 128, 128, 128, 0);
    }

    /* 4. the two panels: the g1 ITEM-LIST chrome at the BOX base (left) and at
     * the LIST base (right) — the same capacity-10 template fixups as the main
     * screen (case 1 @0x800477e8-0x8004796c). */
    master_row(1, &clut_idx, &count, &tmpl);
    {
        int side, n8;
        for (side = 0; side < 2; side++) {
            int bx = side ? LIST_BX : BOX_BX;
            n8 = 0;
            for (i = 0; i < count; i++) {
                tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
                if (CAPACITY == 10 && i >= 7) { y += 8 * n8; n8++; }
                if (CAPACITY == 10 && i == 1) { y += 56; v += 32; }
                sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + bx, y + BOX_BY, w, h, u, v,
                     128, 128, 128, 0);
            }
        }
    }

    /* 5. cursors (g8 pulsing-frame template, the wave-1 model): the INVENTORY
     * cursor is always drawn (RE2 keeps the pulsing inv cursor visible while the
     * box side is active, §2.4 tail FUN_80073350) + the BOX cell cursor when the
     * box side is active. */
    {
        int base8 = 0;
        uint8_t k8 = g_inv.slots[st->item_cursor].flags;
        if (k8 == 1) base8 = 8; else if (k8 == 2) base8 = 0x10;  /* @0x800476f8-714 */
        master_row(8, &clut_idx, &count, &tmpl);
        for (i = 0; i < count; i++) {
            tmpl_row(tmpl, base8 + i, &x, &y, &w, &h, &u, &v);
            sprt(&e, RE15_INV_PAGE_TEX4, clut_idx,
                 bs16(CELL_TBL + (uint32_t)st->item_cursor * 4u) + x + LIST_BX,
                 bs16(CELL_TBL + (uint32_t)st->item_cursor * 4u + 2u) + y + LIST_BY,
                 w, h, u, v, 128, 128, 128, 0);
        }
        if (st->box_side == 1) {
            master_row(8, &clut_idx, &count, &tmpl);
            for (i = 0; i < count; i++) {
                tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);   /* base 0 = 1-cell */
                sprt(&e, RE15_INV_PAGE_TEX4, clut_idx,
                     bs16(CELL_TBL + (uint32_t)st->box_cursor * 4u) + x + BOX_BX,
                     bs16(CELL_TBL + (uint32_t)st->box_cursor * 4u + 2u) + y + BOX_BY,
                     w, h, u, v, 128, 128, 128, 0);
            }
        }
    }

    /* 6. qty digits: inventory grid (the byte-true emit_digits path) + box cells */
    for (i = 0; i < 10; i++) emit_digits(&e, st, i, 0);
    for (i = 0; i < RE15_BOX_PAGE_SLOTS; i++) {
        const re15_inv_slot_t *s = &g_itembox.pages[st->box_page].slots[i];
        box_digits(&e,
                   bs16(CELL_TBL + (uint32_t)i * 4u) + BOX_BX,
                   bs16(CELL_TBL + (uint32_t)i * 4u + 2u) + BOX_BY,
                   s->id, s->qty);
    }

    /* 7. icon cells: inventory 0..9 (the composed ICON8 cache, byte-true art) +
     * box page cells 0..7 (ITEMTILE direct) + blank navy for cells 8/9 of the
     * box panel (the 10-cell panel art shows 5 rows; the box page has 4 [DESIGN]). */
    for (i = 0; i < 10 && i < CAPACITY; i++)
        sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
             bs16(CELL_TBL + (uint32_t)i * 4u) + LIST_BX,
             bs16(CELL_TBL + (uint32_t)i * 4u + 2u) + LIST_BY,
             40, 30, (int)bu16(ICON_UV_TBL + (uint32_t)i * 4u),
             (int)bu16(ICON_UV_TBL + (uint32_t)i * 4u + 2u), 128, 128, 128, 1);
    for (i = 0; i < 10; i++) {
        int cx = bs16(CELL_TBL + (uint32_t)i * 4u) + BOX_BX;
        int cy = bs16(CELL_TBL + (uint32_t)i * 4u + 2u) + BOX_BY;
        if (i < RE15_BOX_PAGE_SLOTS) {
            const re15_inv_slot_t *s = &g_itembox.pages[st->box_page].slots[i];
            box_cell_icon(&e, cx, cy, s->id, s->flags);
        } else {
            box_cell_icon(&e, cx, cy, 0, 0);   /* blank tile 0 */
        }
    }

    /* 8. backdrop: the 48 navy tiles (FUN_80046a1c L199-236 geometry — same as
     * the main screen sec 10; the ID card/ECG/ARMS panels are NOT drawn in box
     * mode [DESIGN: their screen area holds the box panel]). */
    {
        int col, row;
        for (col = 0; col < 8; col++)
            for (row = 0; row < 6; row++)
                sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_STPIC,
                     col * 40, 12 + row * 40, 40, 40, 0, 213, 128, 128, 128, 1);
    }
    return e.n;
}

int re15_inv_screen_build(const re15_inv_screen_t *st, re15_inv_op_t *ops, int max_ops)
{
    emit_t e; int i, clut_idx, count; uint32_t tmpl;
    int x, y, w, h, u, v;

    /* ITEM BOX subscreen: a dedicated list — the normal status-screen list below
     * stays bit-identical when box_mode == 0 (idle pixel regression). */
    if (st->box_mode) return build_box_mode(st, ops, max_ops);

    e.ops = ops; e.n = 0; e.max = max_ops;

    /* ---- 0. grid-mode ITEM NAME (wave 2): emitted FIRST = TOPMOST. The original's
     * glyphs go through the shared text-ring OT-slot chain @0x800b829c (depth class 1),
     * drawn over the screen prims (OPEN: the exact OT-slot depth ordering of the text
     * chain was not decoded — the name region (24,168)+ overlaps no screen prim, so the
     * relative order is not observable; spec Task D open question). name_item is set
     * per frame by the grid handler tail only (@0x800c659c-65d8); id 0 = empty string
     * (offset 0 -> first byte 0x07) draws nothing. */
    if (st->name_item >= 0) emit_name(&e, st->name_item);

    /* ---- 0b. msg-slice text (wave 3 cant-use = entry 0; wave 4 CHECK desc = entry id):
     * text-ring glyphs, same topmost text layer as the name print (OT depth class chain
     * @0x800b829c). Drawn while the msg VM is active (8520&0x80), typewritered to
     * msg_reveal tick-codes of the current page; + the page-wait blink arrow. The
     * name print never coexists (the grid tail doesn't run in ITEM states 5/9). */
    if (st->msg_entry >= 0 && (st->msg_reveal > 0 || st->msg_arrow)) emit_msg(&e, st);

    /* ---- 0c. FILE screen content (FILE wave, 25c1==2): the FUN_800c6ca0 runner draws
     * its own prims per state — state 1 list @0x800c6d70-8c (rows/title/icon/highlight),
     * state 3 reader @0x800c6f90-a4 (text 0x800c7600 at x=0x28 + arrows 0x800c7528),
     * states 4-7 page-turn @0x800c6fb0-d4 (text at the driver's s16 @0x800c78a6, NO
     * arrows). States 0/2 (slides) draw no FILE content — the main chain below keeps
     * running every frame (LAB_8004974c draws AFTER the sub-state dispatch), its panels
     * slid offscreen by the 25e0/25e6/25d8/25dc/25ea/25f2 deltas. The FILE prims go to
     * the near bucket @0x800aa6a8 (getter 0x800c74f0) / the text ring @0x800b829c —
     * both above the main-chain content (wave-2 topmost-text model). */
    if (st->substate == 2) {
        if (st->item_state == 1) {
            emit_file_list(&e, st);
        } else if (st->item_state == 3) {
            emit_file_reader(&e, st, 0x28);      /* a0=0x28 @0x800c6f94 */
            emit_file_arrows(&e, st);
        } else if (st->item_state >= 4 && st->item_state <= 7) {
            emit_file_reader(&e, st, st->file_text_x);  /* lh 78a6 @0x800c6fc8-d0 */
        }
    }

    /* ---- 1. FUN_80047648(0): chrome group 0, absolute xy, always drawn ---- */
    master_row(0, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x, y, w, h, u, v, 128, 128, 128, 0);
    }

    /* ---- 1b. WAVE 4: CHECK examine panel (DEBUG.BIN draw hook 0x800c6228, called from
     * the trampoline 0x800c6c58 AFTER groups 1-11 -> its prims sit ABOVE the sliding
     * panels/ECG in the shared OT bucket (0x800aa6d8 + parity<<12, getter 0x800c750c ==
     * the group tail @0x8004866c parity<<12) but BELOW group 0 (earlier AddPrim =
     * topmost). Emission = the AddPrim order of 0x800c69f0 at base (exam_x, exam_y):
     *   elem0 top cap @(X,Y); PHOTO 112x72 @(X+45,Y+43) (prim 0x800c6944-84: SPRT
     *   0x64808080, uv(0,0), clut 0x7a40=(0,489), wh 0x00480070, DR_MODE 0xe100009d =
     *   8bpp texpage (832,256)); gradient boxes @(X+4,Y+31) 193x95 and @(X+4,Y+132)
     *   193x50 (0x800c6a24-48); 19 frame rows @(X, Y+31+i*8) (loop 0x800c6a4c-6c,
     *   s2=0x13); elem2 @(X,Y+126); elem3 @(X,Y+182). */
    if (st->exam_visible) {
        int X = st->exam_x, Y = st->exam_y;
        exam_elem(&e, 0, X, Y);
        sprt(&e, RE15_INV_PAGE_PHOTO8, RE15_INV_CLUT_PHOTO,
             X + 45, Y + 43, 112, 72, 0, 0, 128, 128, 128, 0);
        gbox(&e, X + 4, Y + 31, 0xc1, 0x5f);
        gbox(&e, X + 4, Y + 132, 0xc1, 0x32);
        for (i = 0; i < 0x13; i++) exam_elem(&e, 1, X, Y + 31 + i * 8);
        exam_elem(&e, 2, X, Y + 126);
        exam_elem(&e, 3, X, Y + 182);
    }

    /* ---- 2. FUN_80048a44(cond, lines): 32 vertical ABE LineF2 ---- */
    {
        uint32_t wavetbl = bu32(ECG_PTR_TBL + (uint32_t)st->cond * 4u);  /* PSX addr */
        int br = bu8(ECG_COL_TBL + (uint32_t)st->cond * 4u);
        int bg = bu8(ECG_COL_TBL + (uint32_t)st->cond * 4u + 1u);
        int bb = bu8(ECG_COL_TBL + (uint32_t)st->cond * 4u + 2u);
        int mask = bu8(ECG_COL_TBL + (uint32_t)st->cond * 4u + 3u);
        for (i = 0; i < 32; i++) {
            /* visibility gate sltiu (sweep-i-0x20),0x4d @0x80048c14-1c (UNSIGNED) */
            uint32_t gate = (uint32_t)((int)st->ecg_sweep - i - 0x20);
            int idx, y0, y1, fade;
            re15_inv_op_t *o;
            if (gate >= 0x4d) continue;
            idx = (int)st->ecg_sweep - i;
            /* x = sweep-(i+1) + base_x - 0x12 @0x80048c34-44; y0 = wave[idx].y+base_y;
             * y1 = y0 + wave[idx].h @0x80048c48-94 (wave entry = {s16 y, s16 h});
             * base_y = 25e6 live (MAP slide +9/f @0x8004c0f4) */
            y0 = st->ecg_y + bs16(wavetbl + (uint32_t)idx * 4u);
            y1 = y0 + bs16(wavetbl + (uint32_t)idx * 4u + 2u);
            fade = (i * 8) & 0xff;                     /* sll a0,i,3 @0x80048b68 */
            if (e.n >= e.max) break;
            o = &e.ops[e.n++];
            o->kind = RE15_INV_OP_LINE; o->page = 0; o->clut = 0; o->abe = 1;
            o->x = (int16_t)(st->ecg_sweep - (i + 1) + st->cond_x - 0x12);  /* 25e4 live (CHECK slide) */
            o->y = (int16_t)y0; o->w = o->x; o->h = (int16_t)y1;
            /* per-channel fade only where the mask bit is set @0x80048b54-c04 */
            o->r = (uint8_t)((mask & 1) ? (br - fade) : br);
            o->g = (uint8_t)((mask & 2) ? (bg - fade) : bg);
            o->b = (uint8_t)((mask & 4) ? (bb - fade) : bb);
        }
        /* condition-change WIPE prims (DAT_800b25d4=1/2, FUN_80048a44 @0x80048cc0+;
         * heal-use feedback, wave 2). Mode 1 = 32 VERTICAL LineF2 sweeping right:
         * x = 25fe-i-1 + 25e4 - 0x12, y from 25e6+0x22 to 25e6+0x46, rgb
         * (0x10, ~(i*8), 0x10). Mode 2 = 32 HORIZONTAL lines sweeping up:
         * x from 25e4+0xc to 25e4+0x59, y = 25e6 + 25fc - i, rgb (0x10,0x10,i*8).
         * (FUN_80048a44.c:61-125; cursor advance in re15_inv_screen_ecg_tick.) */
        if (st->wipe_mode == 1) {
            for (i = 0; i < 32 && e.n < e.max; i++) {
                re15_inv_op_t *o = &e.ops[e.n++];
                o->kind = RE15_INV_OP_LINE; o->page = 0; o->clut = 0; o->abe = 1;
                o->x = (int16_t)((int)st->wipe_v - i - 1 + st->cond_x - 0x12);
                o->y = (int16_t)(st->ecg_y + 0x22);
                o->w = o->x; o->h = (int16_t)(st->ecg_y + 0x46);
                o->r = 0x10; o->g = (uint8_t)~(i * 8); o->b = 0x10;
            }
        } else if (st->wipe_mode == 2) {
            for (i = 0; i < 32 && e.n < e.max; i++) {
                re15_inv_op_t *o = &e.ops[e.n++];
                o->kind = RE15_INV_OP_LINE; o->page = 0; o->clut = 0; o->abe = 1;
                o->x = (int16_t)(st->cond_x + 0x0c);
                o->y = (int16_t)(st->ecg_y + (int)st->wipe_h - i);
                o->w = (int16_t)(st->cond_x + 0x59); o->h = o->y;
                o->r = 0x10; o->g = 0x10; o->b = (uint8_t)(i * 8);
            }
        }
    }

    /* ---- 3. FUN_800c6c58 (DEBUG.BIN @0x800c6c58): FUN_80047648(g) for g=1..11 ---- */
    /* g1 ITEM LIST — capacity-dependent layout, case 1 @0x800477e8-0x8004796c
     * (DISASM-verified; the decompile hides it): cap==10: prims>=7 y+=8*n (n counts
     * those prims), prim1 y+=56 v+=32; cap==8: prims>=10 y+=8*n, prim1 y+=32 v+=16. */
    master_row(1, &clut_idx, &count, &tmpl);
    {
        int n8 = 0;
        for (i = 0; i < count; i++) {
            tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
            if (CAPACITY == 10 && i >= 7) { y += 8 * n8; n8++; }
            if (CAPACITY == 10 && i == 1) { y += 56; v += 32; }
            sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + st->list_x, y + LIST_BY, w, h, u, v,
                 128, 128, 128, 0);   /* 25e0 live (MAP slide +15/f @0x8004c0e0) */
        }
    }
    /* g2 CONDITION word + pulse LED (case 2 @0x80047970-0x80047a80) */
    master_row(2, &clut_idx, &count, &tmpl);
    tmpl_row(tmpl, 0, &x, &y, &w, &h, &u, &v);
    /* prim0: v += cond*16, clut = DAT_800b2610[clut_idx + cond] @0x800479ec-a0c */
    sprt(&e, RE15_INV_PAGE_TEX4, clut_idx + st->cond, x + st->cond_x, y + st->ecg_y, w, h,
         u, v + st->cond * 16, 128, 128, 128, 0);
    /* prim1: LED, clut = DAT_800b261c (=idx 6), rgb = glow DAT_800b2602 @0x8004797c-9e8 */
    tmpl_row(tmpl, 1, &x, &y, &w, &h, &u, &v);
    sprt(&e, RE15_INV_PAGE_TEX4, 6, x + st->cond_x, y + st->ecg_y, w, h, u, v,
         st->ecg_glow, st->ecg_glow, st->ecg_glow, 0);
    /* g3 ARMS (case 3 @0x80047a84-af0): prims 4+ add the slide DAT_800b2608 to y;
     * base y = 25da live (MAP slide -7/f @0x8004c130) */
    master_row(3, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + st->arms_x,   /* 25d8 live (CHECK slide) */
             y + st->arms_y + (i >= 4 ? st->arms_slide : 0), w, h, u, v, 128, 128, 128, 0);
    }
    /* g4 ID header: case 4 @0x80047af4-b24 positions it but SKIPS AddPrim (j 0x80048680)
     * — not drawn. */
    /* g5 cursor highlights (case 5 @0x80047b28-c54): xy = button-table dx/dy + base ONLY
     * (template xy unused); prim0 clut = 261a (idx5) when highlight else 261c (idx6) */
    master_row(5, &clut_idx, &count, &tmpl);
    tmpl_row(tmpl, 0, &x, &y, &w, &h, &u, &v);
    sprt(&e, RE15_INV_PAGE_TEX4, (st->highlight == 1) ? 5 : 6,
         bs16(BUTTON_TBL + (uint32_t)st->tab * 12u) + TAB_BX,
         bs16(BUTTON_TBL + (uint32_t)st->tab * 12u + 2u) + st->tab_base_y,
         w, h, u, v, 128, 128, 128, 0);
    tmpl_row(tmpl, 1, &x, &y, &w, &h, &u, &v);
    sprt(&e, RE15_INV_PAGE_TEX4, 6,
         bs16(BUTTON_TBL + (uint32_t)st->action_dir * 12u) + ACT_BX,
         bs16(BUTTON_TBL + (uint32_t)st->action_dir * 12u + 2u) + st->act_base_y,
         w, h, u, v, 128, 128, 128, 0);
    /* g6 tab cluster (case 6 @0x80047c58-d04): rgb 0x80, dim 0x40 when highlight except
     * the selected tab; prim 9 hidden when tab==1, prim 10 hidden when tab==3 */
    master_row(6, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        int c = 0x80;
        if (st->highlight == 1 && i != st->tab) c = 0x40;
        if (i == 9 && st->tab == 1) continue;
        if (i == 10 && st->tab == 3) continue;
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + TAB_BX, y + st->tab_base_y, w, h, u, v,
             c, c, c, 0);
    }
    /* g7 action cluster (case 7 @0x80047d08-dd0): prim0 label swap — the uv UPDATE is
     * SKIPPED while item_state (25c2) is 5 or 6 (addiu -5; sltiu 2 @0x80047d14-28): the
     * persistent prim keeps its LAST face through the anim/slide (wave-3 freeze memory).
     * Update rule: cursor id<0x15 -> uv(0x10,0x90) EQUIP face (sltiu 0x15 @0x80047d54)
     * else (0x68,0xc0) USE face; THEN overridden if cursor-slot == equip-slot (SLOT
     * equality @0x80047d7c-d90) -> (0x10,0xa0) unequip face. Byte-true MISMATCH corner:
     * a second copy of the equipped weapon id shows the plain EQUIP label (slot compare)
     * but USE on it UNEQUIPS (the classifier's ID compare @0x8004aadc) — do not "fix". */
    master_row(7, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        if (i == 0) {
            if ((uint8_t)(st->item_state - 5) >= 2) {
                uint8_t cid = g_inv.slots[st->item_cursor].id;
                if (cid < 0x15) { u = 0x10; v = 0x90; }
                else            { u = 0x68; v = 0xc0; }
                if (st->item_cursor == st->equipped_slot) { u = 0x10; v = 0xa0; }
                s_g7_u = (uint8_t)u; s_g7_v = (uint8_t)v;    /* remember the written prim */
            } else if (s_g7_u != 0xff) {
                u = s_g7_u; v = s_g7_v;                      /* frozen: last written face */
            }
        }
        sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + ACT_BX, y + st->act_base_y, w, h, u, v,
             128, 128, 128, 0);
    }
    /* g8 grid cursor (case 8 @0x80047dd4-e7c) + g9 second cursor (case 9 @0x80047e80-fe0):
     * drawn only when highlight==1 (joined_r0x80047fd4); template row = base(kind)+prim
     * index; anchored at the cursor cell */
    if (st->highlight == 1) {
        int base8 = 0, base9 = 0;
        uint8_t k8 = g_inv.slots[st->item_cursor].flags;
        uint8_t k9 = g_inv.slots[st->second_cursor].flags;
        if (k8 == 1) base8 = 8;  else if (k8 == 2) base8 = 0x10; /* @0x800476f8-714 */
        if (k9 == 1) base9 = 2;  else if (k9 == 2) base9 = 4;    /* @0x80047718-770 */
        master_row(8, &clut_idx, &count, &tmpl);
        for (i = 0; i < count; i++) {
            tmpl_row(tmpl, base8 + i, &x, &y, &w, &h, &u, &v);
            sprt(&e, RE15_INV_PAGE_TEX4, clut_idx,
                 bs16(CELL_TBL + (uint32_t)st->item_cursor * 4u) + x + st->list_x,
                 bs16(CELL_TBL + (uint32_t)st->item_cursor * 4u + 2u) + y + LIST_BY,
                 w, h, u, v, 128, 128, 128, 0);
        }
        master_row(9, &clut_idx, &count, &tmpl);
        for (i = 0; i < count; i++) {
            /* wave 5: jitter regs DAT_800b25d0-d3 (combine result-anim pulse), added
             * SIGN-EXTENDED per prim (case 9 sll 24/sra 24 @0x80047e98-eb8: prim0 +=
             * (s8)25d0/(s8)25d1, prim1 += (s8)25d2/(s8)25d3 @0x80047f88-9c); 0 outside
             * the combine flow (init zeroing @0x8004646c-84 + walker terminal). */
            int jx = (i == 0) ? (int8_t)st->comb_d0 : (int8_t)st->comb_d2;
            int jy = (i == 0) ? (int8_t)st->comb_d1 : (int8_t)st->comb_d3;
            tmpl_row(tmpl, base9 + i, &x, &y, &w, &h, &u, &v);
            sprt(&e, RE15_INV_PAGE_TEX4, clut_idx,
                 bs16(CELL_TBL + (uint32_t)st->second_cursor * 4u) + x + st->list_x + jx,
                 bs16(CELL_TBL + (uint32_t)st->second_cursor * 4u + 2u) + y + LIST_BY + jy,
                 w, h, u, v, 128, 128, 128, 0);
        }
    }
    /* g10 ECG monitor bezel (case 10 @0x80047fe4-800c): tmpl + ECG base (blocker Q2:
     * the "portrait" label was wrong — this is TEX art uv(128,0) 112x72) */
    master_row(10, &clut_idx, &count, &tmpl);
    tmpl_row(tmpl, 0, &x, &y, &w, &h, &u, &v);
    sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + st->cond_x, y + st->ecg_y, w, h, u, v,
         128, 128, 128, 0);
    /* g11 map screws (case 0xb @0x80048010-38): AddPrim ONLY when the packed word
     * word(25c0)&0xffffff == 0x00010100 (beq -> draw tail 0x8004866c @0x8004802c, else
     * skip tail 0x80048684 @0x80048034). No per-frame xy/uv write — the build-time
     * arena values stay (FUN_800467a8 writes the RAW template @0x80046860-74, code
     * 0x64 opaque): 4 x 16x16 screws at (16,120)/(280,120)/(155,24)/(155,200),
     * uv (112,56/72/40/88), master row 11 @0x80074B14 {clut 4, count 4,
     * tmpl @0x80075630}. */
    if (st->substate == 1 && st->item_state == 1) {
        master_row(11, &clut_idx, &count, &tmpl);
        for (i = 0; i < count; i++) {
            tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
            sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x, y, w, h, u, v, 128, 128, 128, 0);
        }
    }

    /* ---- 4. qty digits: 10 grid slots + equipped echo (FUN_80049a5c @0x80049ae0-b74) */
    for (i = 0; i < 10; i++) emit_digits(&e, st, i, 0);
    if (st->equipped_slot != 0x80)
        emit_digits(&e, st, st->equipped_slot & 0x0f, 1);

    /* ---- 4b. MAP content (FUN_80049a5c draw gate @0x80049bb4-cc: lw word(25c0) &
     * 0xffffff == 0x00010100 -> jal FUN_800473f8 @0x80049bcc + AddPrim of the DR_MODE
     * packet @0x800b2650 (tpage 0x17 = the 4bpp MAP page (448,256)) @0x80049bf4).
     * FUN_800473f8 AddPrims the marker quad FIRST (@0x800475d8), then the count+2
     * static prims (2 fixed sprites + count room rects, loop @0x800475f8-61c) — this
     * AddPrim position (after the digit rows, before the icon cells @0x80049bfc+)
     * puts the map set BELOW digits/ECG/chrome and ABOVE icons/card/backdrop. */
    if (st->substate == 1 && st->item_state == 1) {
        /* marker: POLY_FT4 code 0x2e (@0x80047130-34), 8x8 around the per-frame centre
         * (+-4: builder @0x8004715c-71f0, per-frame rewrite @0x80047554-75c4), uv
         * (224,128)-(232,136) with its own tpage 0x1b = the TEX page (sh 0x1b
         * @0x8004714c) + clut DAT_800b261c = 0x7b90 = UI row 6 (@0x80047144-50);
         * rgb = the 2602 pulse (@0x80047540-6c). */
        sprt(&e, RE15_INV_PAGE_TEX4, 6,
             st->map_marker_x - 4, st->map_marker_y - 4, 8, 8, 224, 128,
             st->ecg_glow, st->ecg_glow, st->ecg_glow, 1);
        /* fixed sprite 1: SPRT code 0x66 at (30,30) 88x32 uv(0,0), clut 0x7d50
         * (@0x80047204-268: ori 0x1e/0x1e wh 0x58/0x20, sh t4 clut @0x80047250) */
        sprt(&e, RE15_INV_PAGE_MAP4, RE15_INV_CLUT_TEXROW21,
             0x1e, 0x1e, 0x58, 0x20, 0, 0, 128, 128, 128, 1);
        /* fixed sprite 2: (270,40) 32x48 uv(96,0) (@0x8004726c-2c0: ori 0x10e/0x28,
         * wh 0x20/0x30, u 0x60, sh t4 clut @0x800472c0) */
        sprt(&e, RE15_INV_PAGE_MAP4, RE15_INV_CLUT_TEXROW21,
             0x10e, 0x28, 0x20, 0x30, 0x60, 0, 128, 128, 128, 1);
        /* room rects: pair table @0x80076840[page] {count, list ptr} (@0x80047048-70),
         * stride-12 entries {x,y,w,h,u@+8,v@+10} (@0x8004731c-60), SPRT code 0x64|2
         * (@0x800472fc-318), clut 0x7d50 (@0x800473cc), sampling the MAP page.
         *
         * RE2-KARTENSYSTEM (Port-Erweiterung, Nutzer-Auftrag 2026-08-30): Zustands-
         * Einfaerbung nach dem RE2-Retail-MECHANISMUS (Zeichner FUN_8006e120
         * @0x8006e4c4-72c, RE2-Leon-EXE — analysis/nutzer_batch_2026-08-30b/
         * map-re2-system.md §3.1): unbesucht -> Prim wird NICHT gezeichnet (schwarz =
         * Screen-Hintergrund), besucht -> Basis-Palette (gruen), aktueller Raum ->
         * Basis+1-Highlight (addiu @0x8006e60c), STETIG — im Map-Tab blinkt nur der
         * Spieler-Pfeil (RGB 0x28..0x78 @0x8006e2a0; Raum-Blinken existiert nur in der
         * CHECK-Variante FUN_8006f1c4). Unser Stock-Marker pulsiert bereits (ecg_glow).
         * RE2 schaltet fuer die Zustaende CLUT-Zeilen um (0x1f2/0x1f5/0x1f6/…); deren
         * RGB-Inhalt liegt in RE2-CD-Datei 170 (lokal nicht vorhanden) — die Tint-WERTE
         * hier sind deshalb dokumentierte PORT-WAHL (rot/gruen nach Nutzer-Spezifikation),
         * NUR die Logik ist RE2-byte-belegt. Rects ohne Raum-Zuordnung
         * (re15_map_rooms.h) bleiben im Stock-Neutralton. */
        {
            int cnt = (int)mu16(0x80076840u + (uint32_t)st->map_page * 8u);
            uint32_t lp = mu32(0x80076844u + (uint32_t)st->map_page * 8u);
            for (i = 0; i < cnt; i++) {
                uint32_t a = lp + (uint32_t)i * 12u;
                int rs = re15_map_rect_state((unsigned)st->map_page, (unsigned)i);
                int cr = 128, cg = 128, cb = 128;           /* UNMAPPED: Stock */
                if (rs == RE15_MAP_RECT_UNVISITED) continue;    /* schwarz */
                if (rs == RE15_MAP_RECT_VISITED)      { cr = 40;  cg = 144; cb = 40; }
                else if (rs == RE15_MAP_RECT_CURRENT) { cr = 192; cg = 24;  cb = 24; }
                sprt(&e, RE15_INV_PAGE_MAP4, RE15_INV_CLUT_TEXROW21,
                     (int16_t)mu16(a), (int16_t)mu16(a + 2u),
                     (int16_t)mu16(a + 4u), (int16_t)mu16(a + 6u),
                     mu8(a + 8u), mu8(a + 10u), cr, cg, cb, 1);
            }
        }
    }

    /* ---- 5. icon cells 0..9 (FUN_80048704 @0x80048704: xy = cell table + list base
     * 25e0/25e2 LIVE (lhu @0x80048748/@0x8004876c), AddPrim while slot < capacity;
     * build-time geometry FUN_80046a1c: 40x30 code 0x66, uv = icon-uv table entry i,
     * clut = ST_00 row 0) ---- */
    for (i = 0; i < 10 && i < CAPACITY; i++)
        sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
             bs16(CELL_TBL + (uint32_t)i * 4u) + st->list_x,
             bs16(CELL_TBL + (uint32_t)i * 4u + 2u) + LIST_BY,
             40, 30, (int)bu16(ICON_UV_TBL + (uint32_t)i * 4u),
             (int)bu16(ICON_UV_TBL + (uint32_t)i * 4u + 2u), 128, 128, 128, 1);

    /* ---- 6. Standard-Arms box art: FUN_80049a5c decompile L45-50 (@0x80049c44-60
     * lbu 0x800b25cd -> prim rgb): pair10 rgb = DAT_800b25cd, x = 25d8+0x18,
     * y = 25da+2608+0x58; build uv (40,90) = icon-cache cell 10 = the static ST_00
     * diagonal knife (Q3). WAVE-6 FINDING-1 RESOLVED: there is NO grid/command dim of
     * this sprite — the prim (code 0x66, rgb 0x3e), CLUT (0,484, STP=0) and art are
     * byte-identical across idle/grid/cmd savestates, and 25cd's complete writer set
     * is {0x3e, 0x80, ±6 pulse} (@0x800495e8-618/@0x8004a624-34/@0x8004a8b0/
     * @0x8004ab14-30/@0x8004ad58-68/@0x8004ac90-a4/@0x8004b1fc, DEBUG.BIN
     * @0x800c6510-20). The apparent 649-px "dim" was modulation-rounding drift
     * between capture sessions — see mod5() in inv_render_pc.c. ---- */
    sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
         st->arms_x + 0x18, st->arms_y + st->arms_slide + 0x58, 40, 30, 40, 90,
         st->arms_rgb, st->arms_rgb, st->arms_rgb, 1);

    /* ---- 7. equipped-weapon icon in the equip box (FUN_80049a5c L51-92): uv from the
     * icon-uv table at the equipped cache cell, xy = (25dc,25de); wide weapons (kind
     * 1/2) draw two 40-wide halves at x/x+0x28 ---- */
    if (st->equipped_slot != 0x80) {
        int eq = st->equipped_slot & 0x0f;
        uint8_t kind = g_inv.slots[eq].flags;
        if (kind == 0) {
            sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
                 st->equip_x, st->equip_y, 40, 30,
                 (int)bu16(ICON_UV_TBL + (uint32_t)eq * 4u),
                 (int)bu16(ICON_UV_TBL + (uint32_t)eq * 4u + 2u), 128, 128, 128, 1);
        } else if (kind == 1) {   /* head slot: cells eq and eq+1 @L65-78 */
            sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
                 st->equip_x, st->equip_y, 40, 30,
                 (int)bu16(ICON_UV_TBL + (uint32_t)eq * 4u),
                 (int)bu16(ICON_UV_TBL + (uint32_t)eq * 4u + 2u), 128, 128, 128, 1);
            sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
                 st->equip_x + 0x28, st->equip_y, 40, 30,
                 (int)bu16(ICON_UV_TBL + (uint32_t)(eq + 1) * 4u),
                 (int)bu16(ICON_UV_TBL + (uint32_t)(eq + 1) * 4u + 2u), 128, 128, 128, 1);
        } else if (kind == 2) {   /* tail slot: cells eq-1 and eq @L79-92 */
            sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
                 st->equip_x - 0x28, st->equip_y, 40, 30,
                 (int)bu16(ICON_UV_TBL + (uint32_t)(eq - 1) * 4u),
                 (int)bu16(ICON_UV_TBL + (uint32_t)(eq - 1) * 4u + 2u), 128, 128, 128, 1);
            sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
                 st->equip_x, st->equip_y, 40, 30,
                 (int)bu16(ICON_UV_TBL + (uint32_t)eq * 4u),
                 (int)bu16(ICON_UV_TBL + (uint32_t)eq * 4u + 2u), 128, 128, 128, 1);
        }
    }

    /* ---- 8. equip-box 80x30 backdrop (pair13, FUN_80049a5c L94-97): ALWAYS drawn at
     * (25d8+5, 25da+0x20); build uv (0,120) = ST_00 static band rows 120-149 ---- */
    sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
         st->arms_x + 5, st->arms_y + 0x20, 80, 30, 0, 120, 128, 128, 128, 1);

    /* ---- 9. ID card (pair14, FUN_80049a5c L98-100 + FUN_80049a5c tail @0x8004a018-44):
     * (25f0,25f2)=(14,26), 128x63, uv (0,150) = STPIC pixel rows 0-62, STPIC clut
     * (blocker Q2). 25f2 LIVE (FILE wave: enter slide -8/f @0x800c6d60). ---- */
    sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_STPIC,
         st->idcard_x, st->idcard_y, 128, 63, 0, 150, 128, 128, 128, 1);

    /* ---- 10. 48 background tiles (FUN_80046a1c L199-236 + FUN_80049a5c L101-106):
     * 8 cols x 6 rows of 40x40 at (col*40, 12+row*40), uv (0,213) = STPIC rows 63-102
     * (the navy backdrop ships inside the STPIC file — blocker Q2) ---- */
    {
        int col, row;
        for (col = 0; col < 8; col++)
            for (row = 0; row < 6; row++)
                sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_STPIC,
                     col * 40, 12 + row * 40, 40, 40, 0, 213, 128, 128, 128, 1);
    }
    return e.n;
}
