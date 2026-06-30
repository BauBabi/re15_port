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

/* ===== Phase ESP-C: the op-0x3a effect PARTICLE pool ====================================
 *
 * The model_inst-pool effect sprites (PSX DAT_800a73b8, spawned by op 0x3a -> FUN_80019700,
 * ticked by FUN_80019e20). The op selects a bank by effect-id (pc[2]) + sub-index (pc[3]),
 * the local offset (pc[8/10/12]) is added to the owner-transform position, and the anim
 * records cycle per the byte-true timing in FUN_80019e20 (per-record duration = anim.param
 * low byte; 0xff = loop back to anim.desc low byte; 0/0 = end -> despawn). This is the port's
 * particle pool; the GPU draw (project + textri quad w/ the coord cell) is the platform side. */

#define RE15_ESP_FX_MAX 16

typedef struct {
    uint8_t  active;
    uint8_t  effect_id;    /* op-0x3a pc[2] (RDT effect id, 0x05/0x07 in ROOM1140) */
    uint8_t  sub_index;    /* op-0x3a pc[3] */
    int8_t   eff_idx;      /* resolved index into re15_esp_t.eff[] (-1 = unresolved bank) */
    int16_t  frame;        /* current anim-record index */
    int16_t  timer;        /* frames until next advance (the byte-true 0x6d frame timer) */
    int32_t  x, y, z;      /* world position = owner-transform + local offset (Q12) */
    int16_t  param;        /* op-0x3a pc[14] */
} re15_esp_fx_t;

void           re15_esp_fx_reset(void);
int            re15_esp_fx_count(void);
/** Spawn an op-0x3a effect particle. `bank` = the room's parsed ESP (resolves effect_id ->
 *  eff_idx for anim cycling; may be NULL -> eff_idx=-1, anim disabled). Returns the slot or NULL. */
re15_esp_fx_t *re15_esp_fx_spawn(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub_index,
                                 int32_t x, int32_t y, int32_t z, int16_t param);
/** Per-frame anim advance (byte-true FUN_80019e20 frame timer); despawns ended particles. */
void           re15_esp_fx_tick(const re15_esp_t *bank);
/** Read slot `i` (for the draw/tests); returns NULL if inactive/out-of-range. */
const re15_esp_fx_t *re15_esp_fx_get(int i);

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
