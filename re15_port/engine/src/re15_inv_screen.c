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
    re15_inv_screen_sync_equip();
}

void re15_inv_screen_heal_wipe(uint8_t id)
{
    /* ECG sweep restart DAT_800b2600 = 0x20 @0x8004b038 (heal sub-step c4==2 common). */
    g_inv_screen.ecg_sweep = 0x20;
    /* Wipe dispatch @0x8004ae24-ae68 via table @0x80010f84, index id-0x22 (sltiu 0xd —
     * ids outside 0x22..0x2e, e.g. 0x2f NUT, get no wipe): [1]=0x23 Antidote / [4]=0x26
     * Blue -> 25d4=2 (@0x8004ae78); [12]=0x2e G+R+B -> none (jumps to the c4++ tail,
     * byte-true prototype inconsistency); [0],[2],[3],[5..11] -> 25d4=1 (@0x8004ae8c). */
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

int re15_inv_screen_cache_tile(int cell)
{
    if (cell < 0 || cell > 9) return -1;      /* cells 10/11 = ST_00 static art (Q3) */
    {
        uint8_t id = g_inv.slots[cell].id;
        uint8_t kind = g_inv.slots[cell].flags;
        if (kind == 1 || kind == 2) return -2; /* wide weapon: 80x30 from ITPS blk+0x21A0
                                                * (FUN_800492b8 mode1) — OPEN in wave 1 */
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

    /* ---- 1. FUN_80047648(0): chrome group 0, absolute xy, always drawn ---- */
    master_row(0, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x, y, w, h, u, v, 128, 128, 128, 0);
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
            o->x = (int16_t)(st->ecg_sweep - (i + 1) + ECG_BX - 0x12);
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
                o->x = (int16_t)((int)st->wipe_v - i - 1 + ECG_BX - 0x12);
                o->y = (int16_t)(ECG_BY + 0x22);
                o->w = o->x; o->h = (int16_t)(ECG_BY + 0x46);
                o->r = 0x10; o->g = (uint8_t)~(i * 8); o->b = 0x10;
            }
        } else if (st->wipe_mode == 2) {
            for (i = 0; i < 32 && e.n < e.max; i++) {
                re15_inv_op_t *o = &e.ops[e.n++];
                o->kind = RE15_INV_OP_LINE; o->page = 0; o->clut = 0; o->abe = 1;
                o->x = (int16_t)(ECG_BX + 0x0c);
                o->y = (int16_t)(ECG_BY + (int)st->wipe_h - i);
                o->w = (int16_t)(ECG_BX + 0x59); o->h = o->y;
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
    sprt(&e, RE15_INV_PAGE_TEX4, clut_idx + st->cond, x + ECG_BX, y + ECG_BY, w, h,
         u, v + st->cond * 16, 128, 128, 128, 0);
    /* prim1: LED, clut = DAT_800b261c (=idx 6), rgb = glow DAT_800b2602 @0x8004797c-9e8 */
    tmpl_row(tmpl, 1, &x, &y, &w, &h, &u, &v);
    sprt(&e, RE15_INV_PAGE_TEX4, 6, x + ECG_BX, y + ECG_BY, w, h, u, v,
         st->ecg_glow, st->ecg_glow, st->ecg_glow, 0);
    /* g3 ARMS (case 3 @0x80047a84-af0): prims 4+ add the slide DAT_800b2608 to y */
    master_row(3, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + ARMS_BX,
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
    /* g7 action cluster (case 7 @0x80047d08-dd0): prim0 label swap when item_state not
     * 5/6: cursor id<0x15 -> uv(0x10,0x90) EQUIP else (0x68,0xc0) USE; cursor==equipped
     * -> (0x10,0xa0) */
    master_row(7, &clut_idx, &count, &tmpl);
    for (i = 0; i < count; i++) {
        tmpl_row(tmpl, i, &x, &y, &w, &h, &u, &v);
        if (i == 0 && (uint8_t)(st->item_state - 5) >= 2) {
            uint8_t cid = g_inv.slots[st->item_cursor].id;
            if (cid < 0x15) { u = 0x10; v = 0x90; }
            else            { u = 0x68; v = 0xc0; }
            if (st->item_cursor == st->equipped_slot) { u = 0x10; v = 0xa0; }
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
            /* jitter regs DAT_800b25d0-d3 = 0 outside the combine flow */
            tmpl_row(tmpl, base9 + i, &x, &y, &w, &h, &u, &v);
            sprt(&e, RE15_INV_PAGE_TEX4, clut_idx,
                 bs16(CELL_TBL + (uint32_t)st->second_cursor * 4u) + x + LIST_BX,
                 bs16(CELL_TBL + (uint32_t)st->second_cursor * 4u + 2u) + y + LIST_BY,
                 w, h, u, v, 128, 128, 128, 0);
        }
    }
    /* g10 ECG monitor bezel (case 10 @0x80047fe4-800c): tmpl + ECG base (blocker Q2:
     * the "portrait" label was wrong — this is TEX art uv(128,0) 112x72) */
    master_row(10, &clut_idx, &count, &tmpl);
    tmpl_row(tmpl, 0, &x, &y, &w, &h, &u, &v);
    sprt(&e, RE15_INV_PAGE_TEX4, clut_idx, x + ECG_BX, y + ECG_BY, w, h, u, v,
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
         ARMS_BX + 0x18, ARMS_BY + st->arms_slide + 0x58, 40, 30, 40, 90,
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
         ARMS_BX + 5, ARMS_BY + 0x20, 80, 30, 0, 120, 128, 128, 128, 1);

    /* ---- 9. ID card (pair14, FUN_80049a5c L98-100 + FUN_80049a5c tail @0x8004a018-44):
     * (25f0,25f2)=(14,26), 128x63, uv (0,150) = STPIC pixel rows 0-62, STPIC clut
     * (blocker Q2) ---- */
    sprt(&e, RE15_INV_PAGE_ICON8, RE15_INV_CLUT_STPIC,
         ID_BX, ID_BY, 128, 63, 0, 150, 128, 128, 128, 1);

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
