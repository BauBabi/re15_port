/**
 * @file re15_esp.h
 * @brief RE1.5 ESP effect-sprite section parser (Phase ESP-A) — byte-true port of the
 *        runtime ESP installer FUN_80019354 / FUN_8001945c / FUN_800194f8 (PSX.EXE).
 *
 * The ESP (effect-sprite) data lives in the room RDT, addressed by four directory slots
 * (the same RDT directory the port already parses for SCD/camera/etc):
 *   RDT+0x4C -> the effect-ID header (u8[8], 0xFF = unused-slot terminator)   [param_1/param_3]
 *   RDT+0x50 -> the EFF pointer table END  (read DOWNWARD, 4 bytes/entry)      [param_2]
 *   RDT+0x54 -> the embedded-TIM base                                          [FUN_800194f8 arg2]
 *   RDT+0x58 -> the TIM offset table END   (read DOWNWARD, 4 bytes/entry)      [FUN_800194f8 arg1]
 *
 * Per the loader FUN_8001945c (verified byte-true against ROOM1140.RDT, 2026-06-30):
 *   - the effect-ID array is read FORWARD (param_1++), the pointer table DOWNWARD (param_2--);
 *   - EFF body start  = ptr_table_entry + idh_off      (puVar3 = iVar2 + param_3);
 *   - EFF header word0 = { count_a = lo16, count_b = hi16 };
 *   - EFF body size   = (count_a*2 + count_b + 2) u32 words = count_a*8 + count_b*4 + 8 bytes
 *     (count_a 8-byte records + count_b 4-byte sprite-coord records + an 8-byte header).
 * The embedded TIMs (FUN_800194f8): tim[i] = tim_base + *(tim_end - 4*(i+1)) (pre-decrement,
 * downward); each is a standard PSX TIM blob (magic 0x00000010). VRAM packing + CLUT/TPage/UV
 * binding is the renderer's job (Phase ESP-C), NOT this parser.
 *
 * This is the parse/index layer only (the re15_ems.c analog for effects). The effect pool,
 * the per-frame tick, and the GPU draw are later ESP phases.
 */
#ifndef RE15_ESP_H
#define RE15_ESP_H

#include <stdint.h>
#include <stddef.h>

#define RE15_ESP_MAX_IDS 8

/** One resolved EFF body (per used effect id, in load order). */
typedef struct {
    uint8_t  effect_id;   /* the effect id from the RDT+0x4C header                       */
    uint32_t eff_start;   /* file offset of the EFF body (ptr-entry + idh_off)            */
    uint32_t eff_end;     /* file offset one past the EFF body                            */
    uint16_t count_a;     /* EFF word0 lo16 — number of 8-byte records                    */
    uint16_t count_b;     /* EFF word0 hi16 — number of 4-byte sprite-coord records       */
    uint32_t tim_off;     /* file offset of this id's embedded TIM (0 = none/unused)      */
} re15_esp_eff_t;

/** Parsed ESP section of one room RDT. */
typedef struct {
    int            id_count;                 /* used ids before the 0xFF terminator (0..8) */
    re15_esp_eff_t eff[RE15_ESP_MAX_IDS];    /* per used id, in load order                 */
    const uint8_t *raw;                      /* the RDT buffer (borrowed, not owned)       */
    size_t         raw_size;
} re15_esp_t;

/**
 * Parse the ESP section of an RDT byte-true (FUN_80019354 chain).
 * @param raw,size      the full RDT buffer.
 * @param idh_off       RDT+0x4C value: file offset of the effect-ID header.
 * @param ptr_end_off   RDT+0x50 value: file offset of the EFF pointer-table END.
 * @param tim_base_off  RDT+0x54 value: file offset of the embedded-TIM base.
 * @param tim_end_off   RDT+0x58 value: file offset of the TIM offset-table END.
 * @param out           filled on success.
 * @return 0 on success; -1 bad args; -2 no/empty ESP (idh null or first word == -1, the
 *         FUN_80019354 guard); -3 a resolved offset fell out of the buffer.
 */
int re15_esp_parse(const uint8_t *raw, size_t size,
                   uint32_t idh_off, uint32_t ptr_end_off,
                   uint32_t tim_base_off, uint32_t tim_end_off,
                   re15_esp_t *out);

/* ===== Phase ESP-C: the EFF clip records (the data the effect renderer draws) =============
 *
 * Inside each EFF body (verified byte-true against ROOM1140.RDT, 2026-06-30) the layout is:
 *     [+0]  u32  word0  = { count_a = lo16, count_b = hi16 }
 *     [+4]  u16  clut/U-base (0x7840 in ROOM1140), u16 V-base (0)        — the 8-byte header
 *     [+8]  count_a x 8-byte ANIM records  { u16 desc, u16 param, u32 rsv }
 *     [..]  count_b x 4-byte COORD records { u8 u, v, w, h }             — sprite cells in the TIM
 * The op-0x3a spawn (FUN_80019700) walks these per (category, sub-index); the model_inst tick
 * (FUN_80019e20) cycles the anim records, each `desc` low-byte selecting a coord cell. See
 * C3_RENDER_DESIGN.md §2b/§2c for the full draw path. These accessors expose the records
 * byte-true (read straight from esp->raw — no copy held). */

/** One 8-byte EFF anim record (count_a of them, @eff_start+8). `desc` high byte = sprite group,
 *  low byte = frame index (steps of 4 in ROOM1140 eff05); the last record's desc==0 = terminator. */
typedef struct { uint16_t desc; uint16_t param; uint32_t rsv; } re15_esp_anim_t;

/** One 4-byte EFF sprite-coord record (count_b of them, @eff_start+8+count_a*8): a sprite cell. */
typedef struct { uint8_t u, v, w, h; } re15_esp_coord_t;

/** Read anim record `i` (0..count_a-1) of effect `eff_idx` (0..id_count-1). 0 = ok, -1 = bad index. */
int re15_esp_anim (const re15_esp_t *esp, int eff_idx, int i, re15_esp_anim_t  *out);
/** Read sprite-coord record `i` (0..count_b-1) of effect `eff_idx`. 0 = ok, -1 = bad index. */
int re15_esp_coord(const re15_esp_t *esp, int eff_idx, int i, re15_esp_coord_t *out);

/** Resolve effect-id (RDT header value, e.g. 0x05) to its eff[] index, or -1 if not present. */
int re15_esp_find_id(const re15_esp_t *esp, uint8_t effect_id);

/** Parse the GLOBAL effect bank file CORE00.ESP (FUN_8001923c): id header @file offset 0, ptr
 *  table downward from round_up4(size)-4, no embedded TIM. Holds effect-ids {3,8,0,2,4} incl the
 *  universal hit effects (effect-id 0). 0 = ok, <0 = error (see re15_esp_parse). */
int re15_esp_parse_global(const uint8_t *raw, size_t size, re15_esp_t *out);

/** Bind the parsed GLOBAL effect bank (CORE00.ESP), set once at game-init. re15_esp_fx_spawn
 *  resolves an effect-id against the room bank first, then this global bank (effect-0 etc.). */
void              re15_esp_set_global_bank(const re15_esp_t *bank);
const re15_esp_t *re15_esp_global_bank(void);

/** Number of distinct sprite sheets the GLOBAL bank needs (= its 5 used effect ids). */
#define RE15_ESP_GLOBAL_SHEETS 5

/** GLOBAL-Bank (CORE00.ESP) Effekt-Id -> Sheet-Index 0..RE15_ESP_GLOBAL_SHEETS-1, sonst -1.
 *
 *  Die Bank traegt KEINE eingebettete TIM: jeder ihrer EFF-Header nennt in word1 (u32 @eff_start+4)
 *  ein `(tpage<<16)|clut`-Paar, das auf eine beim Boot hochgeladene VRAM-Seite zeigt. Nur ZWEI
 *  Seiten, je zweimal bzw. dreimal per CLUT umgefaerbt (Datei-Offsets in CORE00.ESP):
 *
 *    Idx 0  id 0x00 @0x824  word1 0x001f7951  tpage->VRAM(960,256)  clut->VRAM(272,485)  Blut
 *    Idx 1  id 0x02 @0xf00  word1 0x001f7a51  tpage->VRAM(960,256)  clut->VRAM(272,489)  Muendungsfeuer
 *    Idx 2  id 0x03 @0x008  word1 0x001e7811  tpage->VRAM(896,256)  clut->VRAM(272,480)  Rauch
 *    Idx 3  id 0x04 @0x1728 word1 0x001f7ad1  tpage->VRAM(960,256)  clut->VRAM(272,491)  Huelsen
 *    Idx 4  id 0x08 @0x628  word1 0x001e7911  tpage->VRAM(896,256)  clut->VRAM(272,484)  FEUER
 *
 *  Der PC-Port hat kein VRAM-Modell, sondern einen TIM-Slot je Sheet; diese Abbildung ist die
 *  EINZIGE Quelle dafuer, welcher Slot zu welcher Effekt-Id gehoert (platform/pc/main.c bindet
 *  k_global_fx_slot[] in genau dieser Reihenfolge). */
int re15_esp_global_sheet_index(uint8_t effect_id);

/* ===== the ROW BLOCK (the row-machine data — trace wf_a18487d9, adversarially verified) =====
 * rowblk = EFF body + 8 + count_a*8 + count_b*4 (= coord_end). Layout (FUN_80019700
 * @0x80019728-88): 8 x u16 sub-offset table; base = rowblk + off*4; u16 STREAMS @base = the
 * number of SLOTS spawned per trigger; per stream: u16 nrows (+2 pad) + nrows x 40-byte rows.
 * Row fields (identity-copied to slot +0x00..0x27): +0x00/02 routine selectors A/B (48-entry
 * table @0x80071d40), +0x04/06 w/h, +0x08/0a/0c ACCEL, +0x0e param/flags, +0x10/12/14 initial
 * VELOCITY, +0x16 param, +0x18/1a/1c angvel, +0x1e param, +0x20/22/24 euler, +0x26 gate. */
int re15_esp_row_streams(const re15_esp_t *esp, int eff_idx, int sub);   /* streams or <0 */
const uint8_t *re15_esp_row_stream(const re15_esp_t *esp, int eff_idx, int sub,
                                   int stream, int *out_nrows);          /* rows base or NULL */

/* ===== Phase ESP-C: the op-0x3a effect PARTICLE pool ====================================
 *
 * The model_inst-pool effect sprites (PSX DAT_800a73b8, spawned by op 0x3a -> FUN_80019700,
 * ticked by FUN_80019e20). The op selects a bank by effect-id (pc[2]) + sub-index (pc[3]),
 * the local offset (pc[8/10/12]) is added to the owner-transform position, and the anim
 * records cycle per the byte-true timing in FUN_80019e20 (per-record duration = anim.param
 * low byte; 0xff = loop back to anim.desc low byte; 0/0 = end -> despawn). This is the port's
 * particle pool; the GPU draw (project + textri quad w/ the coord cell) is the platform side. */

#define RE15_ESP_FX_MAX 96   /* byte-true: FUN_80019700 spawn scan caps at 0x60=96 (sltiu @0x8001978c),
                              * pool @0x800a73b8 stride 0x84 spans 96 slots (0x3180/0x84). Was 16 → the
                              * port silently dropped op-0x3a effects / blood past the 16th live particle
                              * (audit wf_8cc15b53). s_esp_fx[] scales linearly; spawn/tick loop 0..MAX. */

typedef struct {
    uint8_t  active;
    uint8_t  effect_id;    /* op-0x3a pc[2] (RDT effect id, 0x05/0x07 in ROOM1140) */
    uint8_t  sub_index;    /* op-0x3a pc[3] */
    int8_t   eff_idx;      /* resolved index into re15_esp_t.eff[] (-1 = unresolved bank) */
    int16_t  frame;        /* current anim-record index */
    int16_t  timer;        /* frames until next advance (the byte-true 0x6d frame timer) */
    int32_t  x, y, z;      /* world position = owner-transform + local offset (Q12) */
    int16_t  param;        /* op-0x3a pc[14] */
    uint16_t scale16;      /* spawn packed-arg low16 (Q12 size; slot+0x72 in the pool @0x800a73b8) —
                            * muzzle 0x800, smoke 0xc00, shell 0x1000, gore 0x2000/0x2800 */
    const re15_esp_t *bank;/* the bank that resolved effect_id (room or GLOBAL CORE00.ESP); NULL = unresolved */
    /* ===== byte-true physics (RE15_ESP_ROWMACHINE.md; tick @0x8001a2f0, routine 11 @0x80017718;
     * LIVE-confirmed mzd_stage1_hit_effect.sav). A SPLATTER child particle: gravity accel.y=8,
     * RNG-spread initial drift, integrated position offset xlat, floor bounce (routine 12). Draw
     * adds xlat to the anchor. phys==0 = a normal (non-splatter) fx = the existing cell-cycle. */
    uint8_t  phys;         /* 1 = run the drift/gravity integration (slot+0x6c bit5==0) */
    int16_t  accel_x, accel_y, accel_z;   /* slot+0x08/0a/0c (ROW constants — trace wf_a18487d9)  */
    int16_t  drift_x, drift_y, drift_z;   /* slot+0x10/12/14 (velocity; row-seeded)               */
    int32_t  xlat_x, xlat_y, xlat_z;      /* slot+0x34/38/3c s32 (accumulated fall offset)         */
    int32_t  floor_y;      /* the bounce plane (routine 12 room_coll → collapsed to a floor clamp) */
    /* ===== the ROW VM (stage 2, blood subset — trace wf_a18487d9): the active 40-byte row copy
     * (identity copy @0x80019908-44 / FUN_800174e4), the stream row list, and the flags byte
     * (+0x6c: bit0 active, bit1 visible, bit4 ABE, bit5 freeze-physics, bit6 freeze-frame).
     * routineA (row u16 +0x00) dispatches per tick; supported subset: 0 noop, 3 countdown-then-
     * flags, 4 two-phase freeze-stagger, 5 anim-set. A row ADVANCE re-copies the next row =
     * re-seeds accel/velocity mid-flight (the multi-phase ballistics). rows_base==NULL = the
     * legacy fx (no VM). */
    const uint8_t *rows_base;   /* stream rows (points into the bank's raw ESP; borrowed) */
    uint8_t  row_count;         /* rows in this stream */
    uint8_t  row_cursor;        /* slot+0x6f */
    uint8_t  row[40];           /* the active row copy (slot+0x00..0x27) */
    uint8_t  flags;             /* slot+0x6c */
    /* ===== byte-true render words (ROOM11E0 Strom-Effekt, 2026-08-08) =====
     * slot+0x30 TPAGE / slot+0x32 CLUT. Seeded at spawn from the EFF header
     * (FUN_80019700: +0x32 = hdr16[2] + ((sub&0xff)>>3)*0x40, +0x30 = hdr16[3]);
     * routines 8 (@0x80017608-20) and 10 (@0x800176d8-fc) OR row[0x16] into TPAGE
     * and add row[0x1e]<<6 to CLUT. The draw FUN_800534c4 emits TPAGE<<16 into
     * POLY_FT4 word5 — its bits 5-6 = PSX ABR semi-transparency mode (0=50/50,
     * 1=B+F additiv, 2=B-F, 3=B+F/4); ABE itself = flags bit4 (prim code |= 2,
     * FUN_800534c4 `>>3 & 2`). On PSX the page bits are runtime-patched by
     * FUN_800194f8 (GetTPage of the VRAM slot); on PC the page = the bound TIM
     * slot, so only the ABR bits are consumed by the draw. */
    uint16_t tpage;             /* slot+0x30 */
    uint16_t clut;              /* slot+0x32 */
} re15_esp_fx_t;

void           re15_esp_fx_reset(void);
int            re15_esp_fx_count(void);
/** Spawn an op-0x3a effect particle. `bank` = the room's parsed ESP (resolves effect_id ->
 *  eff_idx for anim cycling; may be NULL -> eff_idx=-1, anim disabled). Returns the slot or NULL. */
re15_esp_fx_t *re15_esp_fx_spawn_ex(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub_index,
                                    uint16_t scale16,
                                    int32_t x, int32_t y, int32_t z, int16_t param);
re15_esp_fx_t *re15_esp_fx_spawn(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub_index,
                                 int32_t x, int32_t y, int32_t z, int16_t param);
/* ===== ROOM1090 FEUER (Entity-Typ 0x26) — byte-true Spawn-Parameter ======================
 * Die 7 Typ-0x26-Entities aus ROOM1090 sub00 (`44 xx 26 vv`, Datei 0x2214..) sind die
 * brennenden Truemmer im Hinterhof. Root 0x80116288 (EXE-Dispatch 0x80072bac[0x26]).
 *  - FLAMME  FUN_80116d00: Effekt-Id ueber Sprungtabelle @0x80100364[grid&0x7f]
 *            (0x08/0x10/0x10/0x08/0x10; `lui v1,0x1003` @0x80116d6c), sub 3,
 *            scale16 = entity[0x1D0]<<8 (Budget 0x28/0x2C @0x80116784/0x8011689c),
 *            `jal 0x80019700` @0x80116d84.  Id 0x10 = die Flammen-TIM der Raum-Bank
 *            (RDT+0x4C ids `05 07 09 10`; TIM Datei 0x35378, 4bpp 256x144).
 *  - FUNKEN  INIT @0x801166c4-e8: a0 = 0x09031800 (Id 0x09, sub 3, scale 0x1800),
 *            gegated auf `!(grid & 0x80)`.
 * Beide geben NULL zurueck, wo das Original nicht spawnt (grid&0x80 bzw. variant >= 5). */
int            re15_esp_type26_flame_id(uint8_t grid_id);   /* 0x08/0x10, -1 = kein Spawn */
re15_esp_fx_t *re15_esp_type26_flame(const re15_esp_t *bank, uint8_t grid_id, uint8_t phase,
                                     int32_t x, int32_t y, int32_t z, int16_t yaw);
re15_esp_fx_t *re15_esp_type26_emerge(const re15_esp_t *bank, uint8_t grid_id,
                                      int32_t x, int32_t y, int32_t z, int16_t yaw);
/** Spawn `n` byte-true SPLATTER child particles at (x,y,z) with floor at floor_y — the
 *  parent→routine-2→routine-11 blood/gore chain collapsed: each child = effect_id sprite +
 *  gravity accel.y=8 + routine-11 RNG drift (drift.x -= rand&0xa, drift.y -= rand&0x14,
 *  drift.z += rand&0x14) + floor bounce. (RE15_ESP_ROWMACHINE.md; LIVE-confirmed.) */
void re15_esp_fx_splatter(const re15_esp_t *bank, uint8_t effect_id, int n,
                          int32_t x, int32_t y, int32_t z, int32_t floor_y);
/** Spawn the (effect_id, sub) row streams as ROW-VM slots (one per stream). Returns the count.
 *  The SHELL (id 4 sub 0) chains R16 2-tick freeze -> R11 RNG spread -> B=12 floor bounce.
 *  `param` = the op/parent param word (slot+0x2e in FUN_80019700; op 0x3A pc[14..15],
 *  routine-8 child = parent param @0x80017614). Seeds slot CLUT/TPAGE from the EFF header. */
int re15_esp_fx_spawn_rows(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub,
                           uint16_t scale16, int32_t x, int32_t y, int32_t z, int32_t floor_y,
                           int16_t param);
/** Platform SE hook: the shell-clink (FUN_80045024(0x01020001) = ARMS record 2). */
extern void (*re15_esp_shell_clink_hook)(void);
/** Platform SE hook: the gunshot BANG (routine 9, ARMS record 0, muzzle tick 2). */
extern void (*re15_esp_bang_hook)(void);
/** Per-frame anim advance (byte-true FUN_80019e20 frame timer); despawns ended particles. */
void           re15_esp_fx_tick(const re15_esp_t *bank);
/** Read slot `i` (for the draw/tests); returns NULL if inactive/out-of-range. */
const re15_esp_fx_t *re15_esp_fx_get(int i);

/** Byte-true DRAW-VISIBLE gate (FUN_80053240.c:30: the sprite draws iff flags bit0 active AND
 *  bit1 visible). Row-VM particles carry the flags byte (slot+0x6c) the routines poke — a frozen/
 *  staggered droplet (routine-4 flags 0x61 / routine-15 0x65 = no bit1) is INVISIBLE until it is
 *  released (flags 0x03/0x13). Legacy fx (no row VM, flags==0) are always visible. */
int re15_esp_fx_visible(const re15_esp_fx_t *f);

/** Bind the room's parsed ESP as the active bank for op-0x3a spawns (set at room load, NULL on
 *  unload). op_sce_espr_on + the per-frame fx tick read it via re15_esp_room_bank(). */
void              re15_esp_set_room_bank(const re15_esp_t *bank);
const re15_esp_t *re15_esp_room_bank(void);

/* ===== Phase ESP-B: the active effect-sprite POOL (spawn + AABB-cull dispatch) ============
 *
 * Byte-true model of the runtime effect pool (PSX globals DAT_800b2360 = active count,
 * DAT_800b2368 = slot-pointer array). In the original a slot is a POINTER into the live SCD
 * bytecode (spawn @0x80040858 stores `DAT_800b2368[operand_byte1] = pc+2`, then count++), so
 * the slot fields overlay the SCD operand bytes:
 *     slot+0x00 u8  type   (index into the 8-entry handler table @0x80074c68)
 *     slot+0x02 s16 x      (screen-space)        slot+0x04 s16 y
 *     slot+0x06 u16 w      (AABB extent X)       slot+0x08 u16 h (AABB extent Y)
 *     slot+0x0A ...  per-type handler data
 * The per-frame walker FUN_8004d5f0(px,py) skips null slots, AABB-tests each
 * ( (u32)(px - x) <= w  &&  (u32)(py - y) <= h ), and on a hit calls handler[type](slot+0x0A).
 * The pool count is zeroed by the room/per-frame setup (0x8004c730) and rebuilt.
 *
 * The PC port copies the fields into a slot struct (it doesn't alias bytecode); behaviour is
 * faithful. The per-type HANDLERS (the actual tick/render/despawn) are Phase ESP-C — here the
 * walker dispatches to a caller-supplied callback so the pool can be tested standalone. The
 * exact per-slot lifetime (handler-managed vs per-frame re-registration) is ESP-C/savestate;
 * a faithful-line `duration` is provided meanwhile. */

#define RE15_ESP_MAX_SLOTS  32   /* pool cap (DAT_800b2368 capacity unconfirmed; flag) */
#define RE15_ESP_SLOT_DATA  16   /* per-type handler data window (slot+0x0A) */

typedef struct {
    uint8_t  active;
    uint8_t  type;                       /* slot+0x00 -> handler table index */
    int16_t  x, y;                       /* slot+0x02 / +0x04 (screen-space) */
    uint16_t w, h;                       /* slot+0x06 / +0x08 (AABB extents) */
    int16_t  duration;                   /* faithful-line lifetime (<=0 = despawn); 0 = unmanaged */
    uint8_t  data[RE15_ESP_SLOT_DATA];   /* slot+0x0A handler data */
} re15_esp_slot_t;

/** Per-slot dispatch callback (the port-side analog of handler_table[type], ESP-C). */
typedef void (*re15_esp_handler_fn)(re15_esp_slot_t *slot, int index);

void             re15_esp_pool_reset(void);                 /* 0x8004c730: count = 0, clear slots */
int              re15_esp_pool_count(void);                 /* live DAT_800b2360 analog */
re15_esp_slot_t *re15_esp_spawn(uint8_t type, int16_t x, int16_t y,
                                uint16_t w, uint16_t h, int16_t duration); /* 0x80040858 add */
/** FUN_8004d5f0: walk the pool, AABB-cull against (px,py), dispatch `fn` on each hit.
 *  Decrements `duration` (faithful-line) and despawns slots that reach 0. Returns # dispatched. */
int              re15_esp_run(int16_t px, int16_t py, re15_esp_handler_fn fn);

#endif /* RE15_ESP_H */
