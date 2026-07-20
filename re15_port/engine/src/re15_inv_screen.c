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
#include "font_width.h"          /* per-glyph advance u8 @0x800c4416 (DEBUG.BIN, vendored) */
#include "gen/inv_name_bank.inc" /* item-name bank @0x800c495c/4a28 + digraph pairs @0x800c4438 */
#include "gen/inv_desc_bank.inc" /* item-desc bank @0x800c50de (wave 4; entry idx = item id) */

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
            /* @0x80049138-180: cell table + list base, y+20 */
            x = bs16(CELL_TBL + (uint32_t)slot * 4u) + LIST_BX + sx;
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

int re15_inv_screen_build(const re15_inv_screen_t *st, re15_inv_op_t *ops, int max_ops)
{
    emit_t e; int i, clut_idx, count; uint32_t tmpl;
    int x, y, w, h, u, v;
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
             * y1 = y0 + wave[idx].h @0x80048c48-94 (wave entry = {s16 y, s16 h}) */
            y0 = ECG_BY + bs16(wavetbl + (uint32_t)idx * 4u);
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
                o->y = (int16_t)(ECG_BY + 0x22);
                o->w = o->x; o->h = (int16_t)(ECG_BY + 0x46);
                o->r = 0x10; o->g = (uint8_t)~(i * 8); o->b = 0x10;
            }
        } else if (st->wipe_mode == 2) {
            for (i = 0; i < 32 && e.n < e.max; i++) {
                re15_inv_op_t *o = &e.ops[e.n++];
                o->kind = RE15_INV_OP_LINE; o->page = 0; o->clut = 0; o->abe = 1;
                o->x = (int16_t)(st->cond_x + 0x0c);
                o->y = (int16_t)(ECG_BY + (int)st->wipe_h - i);
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
            sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + LIST_BX, y + LIST_BY, w, h, u, v,
                 128, 128, 128, 0);
        }
    }
    /* g2 CONDITION word + pulse LED (case 2 @0x80047970-0x80047a80) */
    master_row(2, &clut_idx, &count, &tmpl);
    tmpl_row(tmpl, 0, &x, &y, &w, &h, &u, &v);
    /* prim0: v += cond*16, clut = DAT_800b2610[clut_idx + cond] @0x800479ec-a0c */
    sprt(&e, RE15_INV_PAGE_TEX4, clut_idx + st->cond, x + st->cond_x, y + ECG_BY, w, h,
         u, v + st->cond * 16, 128, 128, 128, 0);
    /* prim1: LED, clut = DAT_800b261c (=idx 6), rgb = glow DAT_800b2602 @0x8004797c-9e8 */
    tmpl_row(tmpl, 1, &x, &y, &w, &h, &u, &v);
    sprt(&e, RE15_INV_PAGE_TEX4, 6, x + st->cond_x, y + ECG_BY, w, h, u, v,
         st->ecg_glow, st->ecg_glow, st->ecg_glow, 0);
    /* g3 ARMS (case 3 @0x80047a84-af0): prims 4+ add the slide DAT_800b2608 to y */
    master_row(3, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + st->arms_x,   /* 25d8 live (CHECK slide) */
             y + ARMS_BY + (i >= 4 ? st->arms_slide : 0), w, h, u, v, 128, 128, 128, 0);
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
                 bs16(CELL_TBL + (uint32_t)st->item_cursor * 4u) + x + LIST_BX,
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
                 bs16(CELL_TBL + (uint32_t)st->second_cursor * 4u) + x + LIST_BX + jx,
                 bs16(CELL_TBL + (uint32_t)st->second_cursor * 4u + 2u) + y + LIST_BY + jy,
                 w, h, u, v, 128, 128, 128, 0);
        }
    }
    /* g10 ECG monitor bezel (case 10 @0x80047fe4-800c): tmpl + ECG base (blocker Q2:
     * the "portrait" label was wrong — this is TEX art uv(128,0) 112x72) */
    master_row(10, &clut_idx, &count, &tmpl);
    tmpl_row(tmpl, 0, &x, &y, &w, &h, &u, &v);
    sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + st->cond_x, y + ECG_BY, w, h, u, v,
         128, 128, 128, 0);
    /* g11 screws (case 0xb @0x80048010-38): drawn only when DAT_800b25c0 word ==
     * 0x010100 (MAP mode) — never in wave 1. */

    /* ---- 4. qty digits: 10 grid slots + equipped echo (FUN_80049a5c @0x80049ae0-b74) */
    for (i = 0; i < 10; i++) emit_digits(&e, st, i, 0);
    if (st->equipped_slot != 0x80)
        emit_digits(&e, st, st->equipped_slot & 0x0f, 1);

    /* ---- 5. icon cells 0..9 (FUN_80048704 @0x80048704: xy = cell table + list base,
     * AddPrim while slot < capacity; build-time geometry FUN_80046a1c: 40x30 code 0x66,
     * uv = icon-uv table entry i, clut = ST_00 row 0) ---- */
    for (i = 0; i < 10 && i < CAPACITY; i++)
        sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
             bs16(CELL_TBL + (uint32_t)i * 4u) + LIST_BX,
             bs16(CELL_TBL + (uint32_t)i * 4u + 2u) + LIST_BY,
             40, 30, (int)bu16(ICON_UV_TBL + (uint32_t)i * 4u),
             (int)bu16(ICON_UV_TBL + (uint32_t)i * 4u + 2u), 128, 128, 128, 1);

    /* ---- 6. Standard-Arms box art: FUN_80049a5c @0x80049a90-... (decompile L45-50):
     * pair10 rgb = DAT_800b25cd, x = 25d8+0x18, y = 25da+2608+0x58; build uv (40,90)
     * = icon-cache cell 10 = the static ST_00 diagonal knife (Q3) ---- */
    sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_ST00_ROW0,
         st->arms_x + 0x18, ARMS_BY + st->arms_slide + 0x58, 40, 30, 40, 90,
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
         st->arms_x + 5, ARMS_BY + 0x20, 80, 30, 0, 120, 128, 128, 128, 1);

    /* ---- 9. ID card (pair14, FUN_80049a5c L98-100 + FUN_80049a5c tail @0x8004a018-44):
     * (25f0,25f2)=(14,26), 128x63, uv (0,150) = STPIC pixel rows 0-62, STPIC clut
     * (blocker Q2) ---- */
    sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_STPIC,
         st->idcard_x, ID_BY, 128, 63, 0, 150, 128, 128, 128, 1);

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
