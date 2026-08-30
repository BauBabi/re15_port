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
    RE15_INV_OP_TILE = 2,   /* FILE wave: untextured TILE 0x62 under DR_MODE
                             * 0xe1000440 = ABR 2 SUBTRACTIVE (the FILE selection
                             * highlight, DEBUG.BIN 0x800c742c: packet words
                             * 0x04000000/0xe1000440/0x62202020) — rect (x,y)+(w,h),
                             * dst -= rgb per channel                          */
    RE15_INV_OP_GBOX = 3    /* wave 4: POLY_G4 navy gradient box (CHECK panel
                             * interior, DEBUG.BIN 0x800c6b84: prim code 0x38
                             * @0x800c6bd0 opaque gouraud quad behind DR_MODE
                             * 0xe1000260 @0x800c6bc4; rect = (x,y)+(w,h)).
                             * Corner colors are the byte-true constants
                             * @0x800c6bd0/6bdc/6be8/6bf4 (rasterizer-side).   */
};

enum {
    RE15_INV_PAGE_TEX4  = 0,  /* 4bpp page VRAM (704,256) = DATA/TEX.TIM cols 0-63hw   */
    RE15_INV_PAGE_ICON8 = 1,  /* 8bpp page VRAM (640,256) = composed icon cache + card */
    RE15_INV_PAGE_FONT4 = 2,  /* 4bpp message-font page = TEX.TIM texel cols 256-511
                               * (glyph rows at v=32.., font_atlas_psx.h region) — the
                               * tpage of the text-ring DR_MODE the name print inherits
                               * (FUN_80028c1c glyphs; wave 2)                          */
    RE15_INV_PAGE_EXAM4 = 3,  /* wave 4: 4bpp texpage 0x1d = VRAM (832,256) = TEX.TIM
                               * texel cols 512-767 (halfword cols 128+) — the CHECK
                               * panel chrome art (DR_MODE 0xe100001d @0x800c6af0;
                               * element uv/wh table @0x800c6b64)                       */
    RE15_INV_PAGE_PHOTO8 = 4, /* wave 4: 8bpp texpage 0x9d = VRAM (832,256) 112x72
                               * window — the ITPS photo the CHECK loader 0x800c6878
                               * uploads over the TEX.TIM region (prim @0x800c6944-84:
                               * SPRT uv(0,0) 112x72 clut 0x7a40=(0,489))               */
    RE15_INV_PAGE_MAP4  = 5,  /* MAP wave: 4bpp texpage 0x17 = VRAM (448,256) — the
                               * MAP0x.PIX floor-plan page. Upload rect (448,256,64,256)
                               * @0x8004c1a0-b0 (LoadImage from 0x801a8000); DR_MODE
                               * packet @0x800b2650 tpage 0x17 built by SetDrawMode in
                               * FUN_800460b8 @0x8004631c-38 (new-GPU path; GetGraphType
                               * @0x80068948 in {1,2} would select the old encoding 0x27),
                               * AddPrim'd at the draw gate @0x80049bf4.               */
    RE15_INV_PAGE_ITEMTILE = 6 /* ITEM BOX [DESIGN, non-canonical mechanism]: direct
                               * item-tile sampling for BOX cells — `v` = the tile id,
                               * `u` = 0; w<=40 samples ITEMALL.PIX tile[v] (the SAME
                               * byte-true 40x30 art the identity icon cache uses,
                               * report-C byte-proof), w==80 samples the wide-weapon
                               * 80x30 icon at ITPS block v*0x3000+0x21A0
                               * (FUN_800492b8 mode-1 source @0x8001e0b8-c8). CLUT =
                               * ST_00 row 0, like every icon cell. The box cells
                               * bypass the 10-cell VRAM cache because the original
                               * has no box VRAM region (itembox_verdict.md).        */
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
    RE15_INV_CLUT_TEXROW0 = 10,
    /* wave 4: 11 = TEX.TIM CLUT row 16 at VRAM (256,496) = clut id 0x7c10 — the CHECK
     * panel chrome palette (element table @0x800c6b64 clut halfword 0x7c10; TEX.TIM's
     * CLUT block is (256,480) 32x24, so row 496-480=16 ships in the same asset;
     * savestate-VRAM verified resident at (256,496) while the menu is open).
     * 12 = the ITPS photo CLUT (0,489) — DYNAMIC: uploaded only by the CHECK/pickup
     * TIM upload 0x800c0258 (LoadImage of the block's embedded crect); zero before
     * the first upload (savestate: row (0,489) all-0 in the idle screen). */
    RE15_INV_CLUT_TEXROW16 = 11,
    RE15_INV_CLUT_PHOTO = 12,
    /* MAP wave: 13 = TEX.TIM CLUT row 21 at VRAM (256,501) = clut id 0x7d50 —
     * GetClut(0x100,0x1f5) @0x80046fdc-fe8 (jal 0x8006b3d8 a0=0x100 a1=0x1f5),
     * the palette of BOTH map fixed sprites (sh t4 @0x80047250/0x800472c0) and
     * every room-rect SPRT (sh t4 @0x800473cc). */
    RE15_INV_CLUT_TEXROW21 = 13,
    /* FILE wave: the FUN_80028ec4 text palettes — clut = 0x7810 | (a3&0x30)<<3
     * (@0x80028f5c-64) = TEX.TIM CLUT rows 0/2/4/6 at VRAM (256,480/482/484/486):
     * a3=0 -> row 0 (= RE15_INV_CLUT_TEXROW0), 0x10 title -> row 2 (0x7890),
     * 0x20 reader footer -> row 4 (0x7910), 0x30 hidden-row underscores -> row 6
     * (0x7990). (The 05 inline-palette control adds (op&3)<<7|(op&4)<<4 —
     * @0x8002903c-5c — no 05 codes exist in the vendored FILE data, generator
     * census.) */
    RE15_INV_CLUT_TEXROW2 = 14,
    RE15_INV_CLUT_TEXROW4 = 15,
    RE15_INV_CLUT_TEXROW6 = 16
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

/* FILE wave: the reader pages are glyph-heavy (page 1 = ~250 SPRT_16 glyphs; the
 * row list = up to 9 x 21 underscore glyphs + names + the full main chain), so the
 * cap grew from 256. Arrays are static at every call site. */
#define RE15_INV_MAX_OPS 768

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
    /* ---- wave 3 (generalized in wave 4 to the full desc-bank msg slice) ---- */
    uint8_t msg_reveal;     /* typewriter reveal = number of TICK-consuming codes (0x00
                             * space + glyphs 0x0c..0xf7) of the current PAGE to draw.
                             * 0 = nothing revealed yet. Maintained by the menu msg
                             * mini-FSM (menu_common.c msg_vm_tick = FUN_80028134 slice). */
    /* ---- wave 4 (CHECK/examine — DEBUG.BIN FUN_800c6630, file==RAM verified) ---- */
    int16_t msg_entry;      /* desc-bank entry (= ITEM ID; FUN_80027e68 a1&0xc00==0x400:
                             * ptr = 0x800c50de + u16[bank + entry*2]). -1 = no message.
                             * Cant-use (wave 3) = entry 0 (@0x8004b2d8 a2=0); CHECK desc
                             * = cursor id (@0x800c6734 a2 from helper 0x800c6600).       */
    int16_t msg_x, msg_y;   /* DAT_800b8534/8536 (FUN_80027e68 a0 = y<<16|x): cant-use
                             * (0x18,0xa8) @0x8004b2d8; CHECK desc (0x13,0xa0) = a0
                             * 0x00a00013 @0x800c6728-2c.                                 */
    uint16_t msg_page_off;  /* display start = byte offset of the current PAGE within the
                             * entry (DAT_800b8528; reset past the 02 00 control on the
                             * page-wait confirm — VM state 2 @FUN_80028134 case 2)       */
    uint8_t msg_arrow;      /* 1 = draw the page-wait arrow this frame (VM state 2 blink
                             * (8525 & 0x30)!=0; glyph = debug-text char 0x2f = 8x8 font
                             * cell u=(0x2f-0x20)%32*8=120 v=0 @FUN_80029214, at
                             * (0x6d, msg_y+44) — FUN_800c69bc with 8542==0)              */
    uint16_t exam_visible;  /* [0x800c6220] examine panel/photo visible flag (sh 1
                             * @0x800c66e4, cleared @0x800c67d4)                          */
    int16_t exam_x;         /* [0x800c6222] panel base x: init -207 @0x800c66d4, +22/f to
                             * +13 (@0x800c6714 addiu 22; terminal cmp 0xd @0x800c6710),
                             * spin-out -22/f back to -207 (@0x800c67c8/67c4)             */
    int16_t exam_y;         /* [0x800c6224] panel base y = 26 (ori 0x1a @0x800c66d8)      */
    uint8_t exam_item;      /* the ITPS block id given to the loader 0x800c6878 (cursor
                             * item id, or 1 if cursor==0xA — helper 0x800c6600)          */
    uint8_t exam_upload_seq;/* increments per CHECK photo load (0x800c6878 = CD read
                             * id*6 sectors @0x1740 + TIM upload 0x800c0258) — the
                             * rasterizer re-uploads its photo VRAM window on change,
                             * honoring the block's EMBEDDED rects (the id-0x24 block
                             * ships prect (0,0)/crect (0,480) -> the (832,256) window
                             * and (0,489) clut keep their STALE content, byte-true)      */
    int16_t arms_x;         /* DAT_800b25d8 x (126 idle; CHECK slide -36/f @0x800c6698,
                             * back +36/f @0x800c6814)                                    */
    int16_t cond_x;         /* DAT_800b25e4 x (13 idle; -36/f @0x800c66a0)                */
    int16_t idcard_x;       /* DAT_800b25f0 x (14 idle; -36/f @0x800c66a4)                */
    /* ---- MAP wave (FUN_8004c058 4-state FSM + FUN_80046fd8/FUN_800473f8 draw) ---- */
    uint8_t substate;       /* DAT_800b25c1 mirror (menu_common s_substate) — the map
                             * draw gate is the packed-word check word(25c0)&0xffffff
                             * == 0x00010100, i.e. 25c0==0 && 25c1==1 && 25c2==1
                             * (@0x80049bb4-cc; g11 repeats it @0x8004801c-2c)          */
    int16_t list_x;         /* DAT_800b25e0: ITEM-LIST base x (215 idle; MAP slide
                             * +15/f @0x8004c0e0; live-read by g1/g8/g9/digits/icons —
                             * FUN_80048f28 @0x80049148 lhu 0(s6)=25e0, FUN_80048704
                             * @0x80048748)                                             */
    int16_t ecg_y;          /* DAT_800b25e6: ECG base y (82 idle; MAP slide +9/f
                             * @0x8004c0f4; consumed by the ECG trace/wipes/g2/g10)     */
    int16_t arms_y;         /* DAT_800b25da: ARMS base y (26 idle; MAP slide -7/f
                             * @0x8004c130; ARMS draw y = 25da + 2608 + 0x58
                             * @0x80049c90)                                             */
    uint8_t map_room;       /* DAT_800b260d: global room-slot 0..105 = marker scale-row
                             * index (per-stage inits @0x80074c0c; PERSISTENT — menu
                             * init never writes it, out-of-range rooms keep the stale
                             * previous value)                                          */
    uint8_t map_page;       /* DAT_800b260e: map page 0..13 = rect-list index + CD file
                             * id index (u16 @0x80074c4c[page]; loader 0x8004c328)      */
    int16_t map_marker_x;   /* the per-frame FUN_800473f8 marker centre (x lives only in
                             * t0 @0x800474f4-addu; y is stored to DAT_800b2606
                             * @0x80047528) — recomputed every gated frame BEFORE the
                             * quad AddPrim, so the builder's div-formula value
                             * (@0x80047010-14, stored to 2604/2606 @0x80047098/
                             * @0x80047124) is dead for display                         */
    int16_t map_marker_y;
    /* ---- FILE wave (DEBUG.BIN FUN_800c6ca0 @0x800c6ca0, dispatch @0x800c6fe8) ---- */
    int16_t idcard_y;       /* DAT_800b25f2: ID-card base y (26 idle, init FUN_800460b8;
                             * FILE enter slide -8/f @0x800c6d60, exit +8/f @0x800c6f58) */
    uint8_t file_sub;       /* [0x800c6c94] list sub-state: 0 = page level, 1 = row
                             * select (inner dispatch @0x800c7008; SQUARE sets 1
                             * @0x800c6eac-b4). Zeroed every enter-slide frame by the
                             * word store @0x800c6cec (covers 6c94..6c97).             */
    uint8_t file_page;      /* [0x800c6c95] list page 0..2 (LEFT/RIGHT wrap
                             * @0x800c6e2c-98)                                         */
    uint8_t file_row;       /* [0x800c6c96] row cursor 0..9 (UP/DOWN wrap
                             * @0x800c7098-70fc)                                       */
    uint8_t file_reader_page; /* [0x800c6c97] reader page 0..7 (7 = the one-past-last
                             * END position — SQUARE/RIGHT close there @0x800c715c-70/
                             * @0x800c71ac; the text drawer clamps to 6 @0x800c7628-34) */
    uint16_t file_anim_phase; /* u16 @0x800c78a4: page-turn phase 0..10 (driver
                             * 0x800c77bc: two 10-frame halves)                        */
    int16_t file_text_x;    /* s16 @0x800c78a6: reader text x during the turn (input
                             * seeds 0xc fwd / 0x44 bwd @0x800c71c8-d0/0x800c7254-5c;
                             * +-28/frame @0x800c77f0/77fc; mid-turn snap 320/-240
                             * @0x800c7870/0x800c785c; drawn via lh @0x800c6fcc)       */
    uint16_t file_bob_off;  /* u16 @0x800c75fe as DRAWN this frame (the arrow drawer
                             * 0x800c7528 draws with the pre-update offset, then
                             * updates the 60-frame bob @0x800c75ac-e4: counter 0x1e ->
                             * off 4, 0x3c -> reset; reader open zeroes both via the
                             * word store @0x800c7070)                                 */
    /* ---- ITEM BOX (Unterschirm, VOLLSTAENDIG nach RE2 — Nutzer-Auftrag 2026-08-30;
     * Ableitung analysis/itembox_re2/re2-box-transfer.md). Die Felder spiegeln die
     * FSM in re15_itembox.c auf RE2s Register: box_scroll = DAT_800d5c14 (Scroll-
     * Stand des 64-Platz-Rings; der gewaehlte Platz ist (scroll+2)&0x3f, der Cursor
     * steht FEST), box_side = Panel DAT_800d5bf1 / Zustand DAT_800d5bf2. */
    uint8_t box_mode;       /* 1 = der Box-Unterschirm ist offen (Menue-Substate 4) */
    uint8_t box_scroll;     /* Scroll-Stand 0..63 (RE2 DAT_800d5c14)                */
    uint8_t box_side;       /* 0 = Inventar-Seite, 1 = Box-Seite, 2 = EXIT-Zeile    */
    int8_t  box_pixoff;     /* Scroll-Pixelversatz waehrend der Animation (RE2
                             * DAT_800d5c15, +/-3 je Frame ueber 6 Frames)          */
    /* ---- wave 5 (EXCHANGE/combine) ---- */
    uint8_t comb_d0, comb_d1, comb_d2, comb_d3;
                            /* DAT_800b25d0-d3: the g9 second-cursor jitter pulse, written
                             * ONLY by the combine result-anim walker @0x8004b408 (grow
                             * steps 0-7: d0/d1 +1, d2/d3 -1 @0x8004b468-494; shrink steps
                             * 8-15 reversed @0x8004b4d8-b510; terminal zeroes @0x8004b544-
                             * b55c; menu-init zeroing @0x8004646c-84). Consumed by the g9
                             * prim build SIGN-EXTENDED (sll 24/sra 24 @0x80047e98-eb8):
                             * prim0 += (s8)d0/(s8)d1, prim1 += (s8)d2/(s8)d3.             */
} re15_inv_screen_t;

/* DEBUG.BIN description-bank entry 0 = "You can't use it here." — string ptr resolve
 * FUN_80027e68 (a1&0xc00==0x400): 0x800c50de + u16[0x800c50de + 0*2] (= off 0x90); glyph
 * codes re-read from shared_assets/PSX/BIN/DEBUG.BIN file offset 0x50de+0x90 this wave:
 * 35 4b 51 00 3f 3d 4a 3a 50 00 51 4f 41 00 45 50 00 44 41 4e 41 57, terminated by the
 * msg-VM end-control 01 00 (code 1 operand 0 -> VM state 5 = PRESS-WAIT). 22 glyphs.
 * (Wave 4: the whole 72-entry bank is vendored in gen/inv_desc_bank.inc; entry 0 == this.) */
#define RE15_INV_CANTUSE_GLYPHS 22
extern const uint8_t re15_inv_cantuse_text[RE15_INV_CANTUSE_GLYPHS];

/* Wave 4: desc-bank access (gen/inv_desc_bank.inc, DEBUG.BIN @0x800c50de). Returns the
 * entry's byte stream (codes as documented in the .inc header), or NULL if id is outside
 * 0..RE15_INV_DESC_NIDS-1. The stream is terminated by the 01 00 end control. */
const uint8_t *re15_inv_desc_entry(int id);
int re15_inv_desc_nids(void);

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

/* Arm the heal condition-change WIPE per the per-heal-item table @0x80010f84 (see
 * wipe_mode above): [1]=0x23 / [4]=0x26 -> mode 2 (blue), [12]=0x2e -> none, all others
 * -> mode 1 (green). WAVE 3: called by the heal sub-FSM at c4==0 (@0x8004ae48-ae68 —
 * the wipe runs BEFORE the consume; the wave-2 arm-at-consume divergence is closed).
 * The ECG sweep reset DAT_800b2600=0x20 (@0x8004b038) belongs to the c4==2 exit tail
 * and is now written there by menu_common.c — NOT here. */
void re15_inv_screen_heal_wipe(uint8_t id);

/* Which ITEMALL.PIX tile the composed 8bpp icon page holds at cache cell 0..9:
 * tile = g_inv.slots[cell].id (identity map, Q3/report C byte-compare; empty = tile 0
 * blank). Cells 10/11 are ST_00-static (returns -1). Wide weapons (kind 1/2) are NOT
 * ITEMALL tiles — returns -2; the rasterizer composes the 80x30 icon from the item's
 * ITPS block +0x21A0 (FUN_800492b8 mode 1, pickup src @0x8001e0b8-c8; wave 2 closes
 * wave 1's OPEN). WAVE 5: a FROZEN cell (reload-full id change with no VRAM write —
 * see re15_inv_icon_freeze_tile) returns its frozen ITEMALL tile instead of the id. */
int re15_inv_screen_cache_tile(int cell);

/* ---- WAVE 5: the icon-cache VRAM page as an EVENT-DRIVEN per-cell art state --------
 * The original icon page (VRAM (640,256)+, cells 0-9) is written ONLY by
 *   FUN_800492b8 (upload at grant/spawn/EXCHANGE-mix), FUN_80049390 (cell-to-cell
 *   MoveImage: RECT from a0=src cell, dest xy from a1=dst cell @0x80049448/0x80049460 ->
 *   jal 0x80068d50 @0x8004945c) and FUN_8004947c (blank: MoveImage from the static
 *   blank RECT {0x2a8,0x15a,0x14,0x1e} = (680,346) 40x30 @0x80049498-b4 to cell a0).
 * The slot ops keep it coherent: compaction FUN_8004dadc copies cell s0 -> s0-1 per
 * shifted slot (@0x8004db30) + blanks the last (@0x8004dbec); the pickup insert
 * FUN_8004dc4c copies 8 cells on the wide-weapon front shift (@0x8004dc98-de3c) and
 * uploads the new icon (@0x8004df08). The port models this as a per-cell override on
 * top of the identity recomposition: 0 = identity (ITEMALL[slot id] — every grant
 * uploads exactly that), 1..14 = MIXITEM.PIX tile pic (EXCHANGE executor uploads
 * (pic-1)*1200 @0x8004e224-44/e5f4-610/e700-720/e810-830/e87c-898), 0x80|tile =
 * frozen ITEMALL tile (reload-full changes the id byte with NO icon call in
 * 0x8004e34c-e3e0 / 0x8004e458-e4ec -> the cell keeps its old art; only the GLOCK 18
 * pair 06->04 @0x80074c98 actually diverges). */
void re15_inv_icon_mix_upload(int cell, int pic);   /* FUN_800492b8(cell,0,MIX+(pic-1)*1200) */
void re15_inv_icon_copy(int src, int dst);          /* FUN_80049390(a0=src, a1=dst)          */
void re15_inv_icon_blank(int cell);                 /* FUN_8004947c(cell)                    */
void re15_inv_icon_freeze_tile(int cell, int tile); /* no-repaint id change (reload-full)    */
void re15_inv_icon_reset(void);                     /* inventory (re)init                    */
int  re15_inv_screen_cache_mix_pic(int cell);       /* 0 = none, else MIXITEM pic 1..14      */

/* ---- MAP wave ---------------------------------------------------------------------- */
/* Per-stage MAP entry init (dispatch table @0x80074c0c = {0x8004b568, 0x8004b8a0,
 * 0x8004b9d4, 0x8004bc9c, 0x8004bdd4, 0x8004bf70}, called at MAP entry @0x8004997c-98).
 * Writes the PERSISTENT globals 260d (room-slot) / 260e (page) per the decoded room
 * jump tables; out-of-range rooms RETURN WITHOUT WRITING (stale-previous, byte-true).
 * stage = 0-based (lh DAT_800b0fe0), room = 0-based index (lh DAT_800b0fe2). */
void re15_inv_map_stage_init(int stage, int room);
uint8_t re15_inv_map_room(void);   /* DAT_800b260d (persistent; BSS initial 0) */
uint8_t re15_inv_map_page(void);   /* DAT_800b260e (persistent; BSS initial 0) */

/* The per-frame marker formula of FUN_800473f8 (raw-verified @0x8004741c-0x80047528;
 * this — NOT the builder FUN_80046fd8's (world+25000)/scale div @0x80047010-14 — is
 * what the screen shows, because it rewrites the quad before every AddPrim):
 *   t  = (s32)((world+32000)*10*scale) >> 20   (32-bit wrapping mult, arithmetic shift)
 *   mx = (t + 5)/10 + x_off                    (signed magic-/10, truncation toward 0)
 *   my = (-(t2 + 5))/10 + y_off                (z axis negated @0x800474b0)
 * scale row = re15_inv_map_blob @0x800768B0 + room_slot*8 {x_off,y_off,x_scale,z_scale}. */
void re15_inv_map_marker(int32_t world_x, int32_t world_z, uint8_t room_slot,
                         int16_t *mx, int16_t *my);

/* Build one frame's display list from `st` + g_inv. Returns the op count.
 * ops[0] = TOPMOST (original AddPrim order); rasterize in reverse. */
int re15_inv_screen_build(const re15_inv_screen_t *st, re15_inv_op_t *ops, int max_ops);

#endif /* RE15_INV_SCREEN_H */
