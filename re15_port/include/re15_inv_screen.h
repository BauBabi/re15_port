/*
 * re15_inv_screen.h — byte-true RE1.5 status/inventory screen DISPLAY LIST (Wave 1
 * of the S1-3 inventory campaign; spec shots/inv_plan.md + shots/inv_wave1_blockers.md,
 * raw RE shots/inv_re_reports.md).
 *
 * The engine side (re15_inv_screen.c) reproduces the original per-frame prim build of
 * FUN_80049a5c @0x80049a5c (draw chain) + FUN_80047648 @0x80047648 (group animator) +
 * FUN_80048a44 @0x80048a44 (ECG renderer) + FUN_80048f28 @0x80048f28 (qty digits) as an
 * ordered list of platform-agnostic draw ops. ORDER = the original AddPrim order into the
 * single OT bucket @0x800aa6d8: EARLIEST op = TOPMOST on screen (skill re15-pc-render-order)
 * — a rasterizer must draw ops[n-1] .. ops[0].
 *
 * All geometry/uv/clut data comes verbatim from the embedded EXE blob (re15_inv_ui.h,
 * PSX.EXE [0x80074A8C,0x800762A0)): panel master @0x80074A8C, templates @0x80075108+,
 * buttons @0x80075390, cells @0x80076274, icon uv @0x80076244, ECG waves @0x80076214/
 * @0x8007622C, digit color classes @0x80074DA8+8.
 */
#ifndef RE15_INV_SCREEN_H
#define RE15_INV_SCREEN_H

#include <stdint.h>

enum {
    RE15_INV_OP_SPRT = 0,   /* textured sprite (SPRT)                          */
    RE15_INV_OP_LINE = 1,   /* untextured ABE LineF2, axis-aligned: endpoints
                             * (x,y) -> (w,h); vertical (x==w) = ECG trace +
                             * wipe mode 1, horizontal (y==h) = wipe mode 2    */
    RE15_INV_OP_TILE = 2    /* flat rect (reserved; not emitted in wave 1)     */
};

enum {
    RE15_INV_PAGE_TEX4  = 0,  /* 4bpp page VRAM (704,256) = DATA/TEX.TIM cols 0-63hw   */
    RE15_INV_PAGE_ICON8 = 1,  /* 8bpp page VRAM (640,256) = composed icon cache + card */
    RE15_INV_PAGE_FONT4 = 2   /* 4bpp message-font page = TEX.TIM texel cols 256-511
                               * (glyph rows at v=32.., font_atlas_psx.h region) — the
                               * tpage of the text-ring DR_MODE the name print inherits
                               * (FUN_80028c1c glyphs; wave 2)                          */
};

/* clut selector: 0..7 = DAT_800b2610[0..7] = ids 0x7a10..0x7bd0 = TEX.TIM CLUT rows
 * 8..15 at VRAM (256,488..495) (Q4 F); 8 = ST_00.TIM CLUT row 0 = VRAM (0,484) =
 * GetClut(0,0x1e4) id 0x7900 (grid icons/knife/blank, FUN_80046a1c); 9 = STPIC's own
 * CLUT = VRAM (0,485) = GetClut(0,0x1e5) id 0x7940 (ID card + screen background). */
enum {
    RE15_INV_CLUT_UI0 = 0,
    RE15_INV_CLUT_ST00_ROW0 = 8,
    RE15_INV_CLUT_STPIC = 9,
    /* 10 = TEX.TIM CLUT row 0 at VRAM (256,480) = clut id 0x7810 — the grid-mode item-NAME
     * glyph palette (FUN_80028c1c @0x80028cb8-c0: clut = ((flags&0x30)<<3)|0x7810, grid
     * flags=1 -> row 0). Wave 2. */
    RE15_INV_CLUT_TEXROW0 = 10
};

typedef struct {
    uint8_t kind;         /* RE15_INV_OP_* */
    uint8_t page;         /* RE15_INV_PAGE_* (SPRT only) */
    uint8_t clut;         /* clut selector (SPRT only) */
    uint8_t abe;          /* semi-trans bit (SPRT code 0x66 / LINE code 0x42) */
    int16_t x, y;         /* screen position (LINE: first endpoint) */
    int16_t w, h;         /* SPRT: size; LINE: x2,y2 (endpoints inclusive) */
    uint8_t u, v;         /* SPRT texture origin within the page */
    uint8_t r, g, b;      /* color/modulation (neutral = 128,128,128) */
} re15_inv_op_t;

#define RE15_INV_MAX_OPS 256

/* Screen state — mirrors the original globals it is built from (all RE1.5-EXE). */
typedef struct {
    uint8_t tab;            /* DAT_800b25bc: 0=ITEM 1=MAP 2=EXIT 3=FILE (tab cursor)   */
    uint8_t item_cursor;    /* DAT_800b25bd: grid cursor 0..9                          */
    uint8_t second_cursor;  /* DAT_800b25be: combine cursor (mirrors bd in grid mode)  */
    uint8_t equipped_slot;  /* DAT_800b25c8 (0x80 = nothing equipped)                  */
    uint8_t highlight;      /* DAT_800b25ca: 1 = ITEM grid active (dim tabs + cursors) */
    uint8_t cond;           /* FUN_8004ed6c result 0..3 (0=Fine 1=Caution 2=Danger
                             * 3=Poison; thresholds @0x800112b4/b5 = hp>=80 / hp>=20)  */
    uint8_t item_state;     /* DAT_800b25c2 (ITEM-mode FSM state; gates the g7 label)  */
    uint8_t action_dir;     /* DAT_800b25d6: action-cluster cursor 0..3                */
    uint8_t ecg_sweep;      /* DAT_800b2600: 0x20..0x80, +1/frame (FUN_80048a44)       */
    uint8_t ecg_glow;       /* DAT_800b2602: pulse LED brightness, +-4/frame           */
    uint8_t ecg_phase;      /* DAT_800b2603: 0..0x3f pulse phase counter               */
    uint8_t arms_rgb;       /* DAT_800b25cd: Standard-Arms sprite rgb (0x3e idle,
                             * 0x80 while nothing equipped — LAB_80049524 @0x800495e8) */
    int16_t arms_slide;     /* DAT_800b2608: ARMS prims 4+ y slide (0 idle; -55 when
                             * nothing equipped — LAB_80049524 @0x800495fc)            */
    int16_t tab_base_y;     /* DAT_800b25ea: tab cluster y (0xa6 idle; slides to 0x108
                             * = offscreen in ITEM mode, FUN_8004a0cc state 0)         */
    int16_t act_base_y;     /* DAT_800b25ee: action cluster y (0x108 staged offscreen) */
    int16_t equip_x, equip_y; /* DAT_800b25dc/25de: equip-box icon position, set by
                             * kind of the equipped slot: kind0=(0x96,0x3a),
                             * 1=(0x82,0x3a), 2=(0xaa,0x3a) (LAB_80049524 @0x80049620) */
    /* ---- wave 2 ---- */
    uint8_t wipe_mode;      /* DAT_800b25d4: ECG condition-change wipe. 0=off; 1=GREEN sweep
                             * (heal FSM @0x8004ae8c); 2=BLUE vertical wipe (@0x8004ae78).
                             * Trigger table @0x80010f84 (ids 0x22..0x2e): [1]=0x23 / [4]=0x26
                             * -> 2; [12]=0x2e -> none; others -> 1. Cleared by the renderer. */
    uint8_t wipe_v;         /* DAT_800b25fe: mode-1 sweep cursor 0x20..0x80, +3/frame
                             * (FUN_80048a44.c:61-125; 32 frames)                          */
    uint8_t wipe_h;         /* DAT_800b25fc: mode-2 line cursor 0x6a..0x22, -2/frame
                             * (36 frames)                                                 */
    int16_t name_item;      /* grid-frame ITEM-NAME print: -1 = FUN_80028c1c NOT called this
                             * frame; else the a3 id passed by the grid tail @0x800c65a8-d8
                             * (cursor==0xA -> 1; empty slot -> 0 = empty string, no draw)  */
} re15_inv_screen_t;

/* The live screen state (engine-owned). */
extern re15_inv_screen_t g_inv_screen;

/* Reset the state to the byte-true menu-open values (FUN_800460b8 @0x800460b8:
 * bc/bd/be=0, ca=0, 2600=0x20, 2602=0, 2603=0; LAB_80049524 @0x800495e8-0x8004969c:
 * 25cd/2608 + 25dc/25de from the equipped slot). Call on the menu-open edge. */
void re15_inv_screen_open(void);

/* Advance the free-running ECG registers exactly like the head of FUN_80048a44
 * @0x80048a44 (sweep wrap >0x7f -> 0x20 @0x80048ab4-ac0; glow +-4 on the 0x40-period
 * phase @0x80048ac8-b34). Call once per frame while the menu is open, BEFORE build. */
void re15_inv_screen_ecg_tick(void);

/* The status-screen condition classifier FUN_8004ed6c @0x8004ed6c: poison flag
 * (u16 @0x800acaec bit 1) -> 3; else hp >= 80 (@0x800112b4) -> 0 Fine;
 * hp >= 20 (@0x800112b5) -> 1 Caution; else 2 Danger. */
int re15_inv_screen_condition(int hp, int poisoned);

/* Re-derive the equip-panel registers from the CURRENT equipped slot (the LAB_80049524
 * @0x800495e8-0x800496dc rules: 25cd/2608 by equipped-or-not; 25dc/25de by the slot's
 * kind byte). Called by open() and by the wave-2 USE equip bridge after DAT_800b25c8
 * changes (the byte-true swap-anim FSM that maintains these live is WAVE 3). */
void re15_inv_screen_sync_equip(void);

/* Arm the heal-use ECG feedback (wave 2): the ECG sweep reset DAT_800b2600=0x20
 * (@0x8004b038, heal sub-step c4==2 common) + the condition-change wipe per the
 * per-heal-item table @0x80010f84 (see wipe_mode above). Called by the item-USE FSM
 * at heal apply/consume. (Byte-true ORDER divergence, WAVE 3: the original runs the
 * wipe BEFORE the consume — c4 0->1 wait ->2; the port arms it AT consume.) */
void re15_inv_screen_heal_wipe(uint8_t id);

/* Which ITEMALL.PIX tile the composed 8bpp icon page holds at cache cell 0..9:
 * tile = g_inv.slots[cell].id (identity map, Q3/report C byte-compare; empty = tile 0
 * blank). Cells 10/11 are ST_00-static (returns -1). Wide weapons (kind 1/2) are NOT
 * ITEMALL tiles — returns -2; the rasterizer composes the 80x30 icon from the item's
 * ITPS block +0x21A0 (FUN_800492b8 mode 1, pickup src @0x8001e0b8-c8; wave 2 closes
 * wave 1's OPEN). */
int re15_inv_screen_cache_tile(int cell);

/* Build one frame's display list from `st` + g_inv. Returns the op count.
 * ops[0] = TOPMOST (original AddPrim order); rasterize in reverse. */
int re15_inv_screen_build(const re15_inv_screen_t *st, re15_inv_op_t *ops, int max_ops);

#endif /* RE15_INV_SCREEN_H */
