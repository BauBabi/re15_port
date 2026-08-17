/**
 * @file re15_esp.c
 * @brief RE1.5 ESP effect-sprite section parser (Phase ESP-A).
 *
 * Byte-true port of the runtime ESP installer:
 *   FUN_80019354  — reads RDT+0x4C/0x50/0x54/0x58, guards (idh && *idh != -1), calls the two below
 *   FUN_8001945c  — the effect-ID + EFF pointer-table walk (id array forward, ptr table downward,
 *                   EFF start = entry+idh, EFF end = start + (lo16*2 + hi16 + 2) u32 words)
 *   FUN_800194f8  — the embedded-TIM table walk (downward, tim = base + *(--ptr))
 * Verified against shared_assets/PSX/STAGE1/ROOM1140.RDT (id header 05 07 FF..; EFF @0x11E8/0x13B8;
 * TIM @0x1A628/0x1CA68). See re15_esp.h for the format. Parse/index only — no rendering.
 */
#include "re15_esp.h"
#include "re15_scd.h"   /* g_re15_pauseflags + RE15_PAUSE_ACTION — Selbst-Gate @0x80019e40 */
#include <string.h>

extern uint8_t re15_engine_rand8(void);   /* the shared FUN_8001af20 draw (re15_damage.c) */

/* ===== Phase ESP-B: the active effect-sprite pool ====================================== */

/* The port's effect pool (the re15_ems-style C analog of DAT_800b2360/DAT_800b2368). Unlike
 * the original — where a slot is a pointer INTO the live SCD bytecode — the port owns the slot
 * storage and copies the fields in; behaviour is faithful (the walker reads the same fields). */
static re15_esp_slot_t s_esp_pool[RE15_ESP_MAX_SLOTS];

void re15_esp_pool_reset(void)
{
    /* 0x8004c730: `sw zero, DAT_800b2360` — the room/per-frame setup zeroes the count and the
     * pool is rebuilt by the active effects (SCD op_sce_espr / the weapon discharge). */
    memset(s_esp_pool, 0, sizeof(s_esp_pool));
}

int re15_esp_pool_count(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_MAX_SLOTS; i++) if (s_esp_pool[i].active) n++;
    return n;   /* live DAT_800b2360 analog */
}

re15_esp_slot_t *re15_esp_spawn(uint8_t type, int16_t x, int16_t y,
                                uint16_t w, uint16_t h, int16_t duration)
{
    /* 0x80040858: register a new slot + count++. The original indexes DAT_800b2368 by the SCD
     * operand byte and stores the bytecode pointer; the port claims the first free slot. */
    for (int i = 0; i < RE15_ESP_MAX_SLOTS; i++) {
        re15_esp_slot_t *e = &s_esp_pool[i];
        if (e->active) continue;
        memset(e, 0, sizeof(*e));
        e->active   = 1;
        e->type     = type;
        e->x = x; e->y = y; e->w = w; e->h = h;
        e->duration = duration;
        return e;
    }
    return NULL;   /* pool full (no original overflow path) */
}

int re15_esp_run(int16_t px, int16_t py, re15_esp_handler_fn fn)
{
    /* FUN_8004d5f0: walk the slot array; for each live slot AABB-test the cull point and, on a
     * hit, dispatch handler[type](slot+0x0A). Byte-true AABB: (u32)(px - x) <= w (unsigned). */
    int dispatched = 0;
    for (int i = 0; i < RE15_ESP_MAX_SLOTS; i++) {
        re15_esp_slot_t *e = &s_esp_pool[i];
        if (!e->active) continue;                         /* null slot (pbVar2 == 0) */
        uint32_t dx = (uint32_t)(int32_t)(px - e->x);     /* (uint)(param_1 - slot[2]) */
        uint32_t dy = (uint32_t)(int32_t)(py - e->y);
        if (dx <= (uint32_t)e->w && dy <= (uint32_t)e->h) {
            if (fn) fn(e, i);                             /* (*handler[type])(slot + 0x0A) */
            dispatched++;
        }
        /* faithful-line lifetime: a positive duration counts down; 0 = unmanaged (the original's
         * exact per-slot lifetime lives in the per-type handlers / per-frame re-registration). */
        if (e->duration > 0 && --e->duration == 0) e->active = 0;
    }
    return dispatched;
}

static uint32_t rd_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static int32_t rd_s32(const uint8_t *p) { return (int32_t)rd_u32(p); }

static int esp_parse_core(const uint8_t *raw, size_t size,
                          uint32_t idh_off, uint32_t ptr_end_off,
                          uint32_t tim_base_off, uint32_t tim_end_off,
                          re15_esp_t *out, int zero_ok)
{
    if (!raw || !out) return -1;
    memset(out, 0, sizeof(*out));
    out->raw = raw;
    out->raw_size = size;

    /* FUN_80019354 guard: piVar4 != 0 && *piVar4 != -1 (idh present + first word not the
     * 0xFFFFFFFF "no effects" marker). For the ROOM call idh_off==0 = "no ESP" (RDT+0x4C is
     * NULL); the GLOBAL file CORE00.ESP (FUN_8001923c) has its id header at genuine file
     * offset 0, so zero_ok lifts that rejection. */
    if ((idh_off == 0 && !zero_ok) || (size_t)idh_off + 8 > size) return -2;
    if (rd_u32(raw + idh_off) == 0xFFFFFFFFu) return -2;

    /* FUN_8001945c: id array forward from idh_off; pointer table DOWNWARD from ptr_end_off
     * (int* with pre-... actually post-read then -1 each iteration). EFF body = entry + idh. */
    int n = 0;
    for (int i = 0; i < RE15_ESP_MAX_IDS; i++) {
        uint8_t id = raw[idh_off + (uint32_t)i];
        if (id == 0xFF) break;                                   /* 0xFF = end of used ids */

        uint32_t ent_off = ptr_end_off - (uint32_t)i * 4;        /* downward, 4 bytes/entry */
        if ((size_t)ent_off + 4 > size) return -3;
        int32_t  entry   = rd_s32(raw + ent_off);
        uint32_t eff_start = (uint32_t)((int32_t)idh_off + entry);   /* puVar3 = iVar2 + param_3 */
        if ((size_t)eff_start + 4 > size) return -3;

        uint32_t w0 = rd_u32(raw + eff_start);
        uint16_t count_a = (uint16_t)(w0 & 0xFFFF);
        uint16_t count_b = (uint16_t)(w0 >> 16);
        /* size = (count_a*2 + count_b + 2) u32 words (the FUN_8001945c EFF_end formula). */
        uint32_t eff_end = eff_start + ((uint32_t)count_a * 2u + count_b + 2u) * 4u;
        if (eff_end > size) return -3;

        out->eff[n].effect_id = id;
        out->eff[n].eff_start = eff_start;
        out->eff[n].eff_end   = eff_end;
        out->eff[n].count_a   = count_a;
        out->eff[n].count_b   = count_b;
        out->eff[n].tim_off   = 0;
        n++;
    }
    out->id_count = n;

    /* FUN_800194f8: per used id, tim[i] = tim_base + *(tim_end - 4*(i+1)) (pre-decrement,
     * downward). 0xFFFFFFFF entry = unused (no TIM for that slot). */
    if (tim_base_off != 0 && tim_end_off != 0) {
        for (int i = 0; i < n; i++) {
            uint32_t e_off = tim_end_off - (uint32_t)(i + 1) * 4;
            if ((size_t)e_off + 4 > size) return -3;
            uint32_t e = rd_u32(raw + e_off);
            if (e == 0xFFFFFFFFu) continue;                      /* unused-ptr marker */
            uint32_t tim = tim_base_off + e;
            if ((size_t)tim + 4 > size) return -3;
            out->eff[i].tim_off = tim;
        }
    }
    return 0;
}

int re15_esp_parse(const uint8_t *raw, size_t size,
                   uint32_t idh_off, uint32_t ptr_end_off,
                   uint32_t tim_base_off, uint32_t tim_end_off,
                   re15_esp_t *out)
{
    return esp_parse_core(raw, size, idh_off, ptr_end_off, tim_base_off, tim_end_off, out, 0);
}

/* Parse the GLOBAL effect bank file CORE00.ESP (FUN_8001923c @0x8001923c): the id header is at
 * file offset 0 ({3,8,0,2,4}), the pointer table is read DOWNWARD from ptr_end = round_up4(size)-4
 * (the installer math @0x80019310-0x80019330), and there is NO embedded TIM (the global installer
 * calls only FUN_8001945c, never the TIM walker — the effects address VRAM-resident textures via
 * the EFF-header word1 page/clut). Yields effect-id 0 (the universal hit effects) + 2/3/4/8. */
int re15_esp_parse_global(const uint8_t *raw, size_t size, re15_esp_t *out)
{
    if (!raw || size < 8) return -1;
    uint32_t ptr_end = (uint32_t)(((size + 3u) & ~(size_t)3u) - 4u);
    return esp_parse_core(raw, size, 0, ptr_end, 0, 0, out, 1);
}

/* ===== the ROW BLOCK (the row-machine data — trace wf_a18487d9, adversarially verified) =====
 *
 * rowblk = body + 8 + count_a*8 + count_b*4 (= coord_end; loader @0x800194c0-e0 computes
 * ((count_a*2 + count_b + 2) << 2)). Layout (spawner FUN_80019700 @0x80019728-88):
 *   8 x u16 sub-offset table @rowblk;  base = rowblk + lhu(rowblk + (sub&7)*2) * 4;
 *   u16 STREAMS @base (+2 pad) — the number of SLOTS spawned per trigger (one slot per
 *   stream, assigned in REVERSE; outer loop @0x800199a8-ac);
 *   per stream: u16 nrows (+2 pad), then nrows x 40-byte ROWS (skip stride nrows*40+4
 *   @0x800198c0-e0). Note: the (sub>>3)*0x40 addend shifts the CLUT seed (slot+0x32), it is
 *   NOT a row-pointer addend (@0x8001987c-88).
 * Row fields (identity-copied to slot +0x00..0x27 at spawn @0x80019908-44 and by every row
 * advance FUN_800174e4): +0x00/+0x02 routine selectors A/B (the 48-entry table @0x80071d40),
 * +0x04/06 sprite w/h, +0x08/0a/0c ACCEL (the tick physics constant — file bytes, e.g. blood
 * (-2,8,0) @CORE00 0x94C), +0x0e param/flags, +0x10/12/14 initial VELOCITY, +0x16 param,
 * +0x18/1a/1c angvel, +0x1e param, +0x20/22/24 euler, +0x26 advance gate. */

#define ESP_ROW_BYTES 40

static const uint8_t *esp_rowblk(const re15_esp_t *esp, int eff_idx, uint32_t *out_len)
{
    if (!esp || !esp->raw || eff_idx < 0 || eff_idx >= esp->id_count) return NULL;
    const re15_esp_eff_t *e = &esp->eff[eff_idx];
    /* NOTE: e->eff_end = eff_start + (count_a*2 + count_b + 2)*4 = exactly the ROW BLOCK START
     * (the loader formula @0x800194c0-e0 — the parser's "body end" EXCLUDES the rows). The row
     * block extends from there to the next body; bound the accessors by the file size (every
     * read below is length-checked, and the shipped in-file counts terminate the walk). */
    uint32_t off = e->eff_end;
    if (off + 16u > esp->raw_size) return NULL;
    if (out_len) *out_len = (uint32_t)esp->raw_size - off;
    return esp->raw + off;
}

int re15_esp_row_streams(const re15_esp_t *esp, int eff_idx, int sub)
{
    uint32_t len = 0;
    const uint8_t *blk = esp_rowblk(esp, eff_idx, &len);
    if (!blk) return -1;
    uint32_t sub_off = (uint32_t)(blk[(sub & 7) * 2] | (blk[(sub & 7) * 2 + 1] << 8));
    uint32_t base    = sub_off * 4u;
    if (base + 4u > len) return -1;
    return (int)(blk[base] | (blk[base + 1] << 8));
}

const uint8_t *re15_esp_row_stream(const re15_esp_t *esp, int eff_idx, int sub,
                                   int stream, int *out_nrows)
{
    uint32_t len = 0;
    const uint8_t *blk = esp_rowblk(esp, eff_idx, &len);
    if (out_nrows) *out_nrows = 0;
    if (!blk) return NULL;
    uint32_t sub_off = (uint32_t)(blk[(sub & 7) * 2] | (blk[(sub & 7) * 2 + 1] << 8));
    uint32_t base    = sub_off * 4u;
    if (base + 4u > len) return NULL;
    int streams = (int)(blk[base] | (blk[base + 1] << 8));
    if (stream < 0 || stream >= streams) return NULL;
    uint32_t p = base + 4u;                              /* first stream header */
    for (int s = 0; s < stream; s++) {
        if (p + 4u > len) return NULL;
        uint32_t nr = (uint32_t)(blk[p] | (blk[p + 1] << 8));
        p += 4u + nr * ESP_ROW_BYTES;                    /* skip stride nrows*40+4 (@0x800198c0-e0) */
    }
    if (p + 4u > len) return NULL;
    uint32_t nr = (uint32_t)(blk[p] | (blk[p + 1] << 8));
    if (p + 4u + nr * ESP_ROW_BYTES > len) return NULL;
    if (out_nrows) *out_nrows = (int)nr;
    return blk + p + 4u;
}

/* ===== Phase ESP-C: EFF clip record accessors ========================================== */

int re15_esp_anim(const re15_esp_t *esp, int eff_idx, int i, re15_esp_anim_t *out)
{
    if (!esp || !esp->raw || !out || eff_idx < 0 || eff_idx >= esp->id_count) return -1;
    const re15_esp_eff_t *e = &esp->eff[eff_idx];
    if (i < 0 || i >= (int)e->count_a) return -1;
    /* anim records start after the 8-byte EFF header (word0 + clut/U/V), 8 bytes each. */
    uint32_t off = e->eff_start + 8u + (uint32_t)i * 8u;
    if ((size_t)off + 8 > esp->raw_size) return -1;
    const uint8_t *p = esp->raw + off;
    out->desc  = (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
    out->param = (uint16_t)((uint16_t)p[2] | ((uint16_t)p[3] << 8));
    out->rsv   = rd_u32(p + 4);
    return 0;
}

int re15_esp_coord(const re15_esp_t *esp, int eff_idx, int i, re15_esp_coord_t *out)
{
    if (!esp || !esp->raw || !out || eff_idx < 0 || eff_idx >= esp->id_count) return -1;
    const re15_esp_eff_t *e = &esp->eff[eff_idx];
    if (i < 0 || i >= (int)e->count_b) return -1;
    /* coord records follow the count_a 8-byte anim records (after the 8-byte header). */
    uint32_t off = e->eff_start + 8u + (uint32_t)e->count_a * 8u + (uint32_t)i * 4u;
    if ((size_t)off + 4 > esp->raw_size) return -1;
    const uint8_t *p = esp->raw + off;
    out->u = p[0]; out->v = p[1]; out->w = p[2]; out->h = p[3];
    return 0;
}

int re15_esp_find_id(const re15_esp_t *esp, uint8_t effect_id)
{
    if (!esp) return -1;
    for (int i = 0; i < esp->id_count; i++)
        if (esp->eff[i].effect_id == effect_id) return i;
    return -1;
}

/* ===== Phase ESP-C: op-0x3a effect particle pool ======================================= */

static re15_esp_fx_t s_esp_fx[RE15_ESP_FX_MAX];
static const re15_esp_t *s_room_bank = NULL;

void              re15_esp_set_room_bank(const re15_esp_t *bank) { s_room_bank = bank; }
const re15_esp_t *re15_esp_room_bank(void)                       { return s_room_bank; }

static const re15_esp_t *s_global_bank = NULL;   /* CORE00.ESP (effect-ids 0/2/3/4/8) */
void              re15_esp_set_global_bank(const re15_esp_t *bank) { s_global_bank = bank; }
const re15_esp_t *re15_esp_global_bank(void)                       { return s_global_bank; }

void re15_esp_fx_reset(void) { memset(s_esp_fx, 0, sizeof(s_esp_fx)); }

int re15_esp_fx_count(void)
{
    int n = 0;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) if (s_esp_fx[i].active) n++;
    return n;
}

const re15_esp_fx_t *re15_esp_fx_get(int i)
{
    if (i < 0 || i >= RE15_ESP_FX_MAX || !s_esp_fx[i].active) return NULL;
    return &s_esp_fx[i];
}

int re15_esp_fx_visible(const re15_esp_fx_t *f)
{
    if (!f) return 0;
    if (!f->rows_base) return 1;          /* legacy fx: no flags model -> always drawn */
    return (f->flags & 0x02) ? 1 : 0;     /* row-VM: byte-true visible bit (slot+0x6c bit1) */
}

re15_esp_fx_t *re15_esp_fx_spawn_ex(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub_index,
                                    uint16_t scale16,
                                    int32_t x, int32_t y, int32_t z, int16_t param)
{
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        re15_esp_fx_t *f = &s_esp_fx[i];
        if (f->active) continue;
        memset(f, 0, sizeof(*f));
        f->active    = 1;
        f->effect_id = effect_id;
        f->sub_index = sub_index;
        f->scale16   = scale16 ? scale16 : 0x1000;   /* spawn packed-arg low16 (Q12; @entry+0x72) */
        f->eff_idx   = (int8_t)re15_esp_find_id(bank, effect_id);   /* the ROOM bank first */
        f->bank      = (f->eff_idx >= 0) ? bank : NULL;
        if (f->eff_idx < 0) {                                       /* fall back to the GLOBAL bank */
            const re15_esp_t *gb = re15_esp_global_bank();          /* CORE00.ESP (effect-0 hit fx, …) */
            int gi = re15_esp_find_id(gb, effect_id);
            if (gi >= 0) { f->eff_idx = (int8_t)gi; f->bank = gb; }
        }
        f->frame     = 0;
        f->timer     = 0;     /* 0 -> the next tick advances to frame 0's duration (FUN_80019e20) */
        f->x = x; f->y = y; f->z = z;
        f->param     = param;
        return f;
    }
    return NULL;   /* pool full */
}

re15_esp_fx_t *re15_esp_fx_spawn(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub_index,
                                 int32_t x, int32_t y, int32_t z, int16_t param)
{
    return re15_esp_fx_spawn_ex(bank, effect_id, sub_index, 0x1000, x, y, z, param);
}

/* ===== the ROW VM (stage 2, blood subset — trace wf_a18487d9) ==============================
 * Row helpers: identity-copy row `idx` into the slot (the spawner row-0 copy @0x80019908-44
 * and the advance FUN_800174e4 both do the same 40-byte copy) — accel/velocity/selectors are
 * RE-SEEDED on every advance (each row = one ballistic phase). */
static uint16_t row_u16(const uint8_t *r, int off) { return (uint16_t)(r[off] | (r[off+1] << 8)); }

static void esp_fx_row_load(re15_esp_fx_t *f, int idx)
{
    if (!f->rows_base || idx < 0 || idx >= f->row_count) return;
    const uint8_t *r = f->rows_base + idx * ESP_ROW_BYTES;
    memcpy(f->row, r, ESP_ROW_BYTES);
    f->accel_x = (int16_t)row_u16(r, 0x08);   /* +0x08/0a/0c */
    f->accel_y = (int16_t)row_u16(r, 0x0a);
    f->accel_z = (int16_t)row_u16(r, 0x0c);
    f->drift_x = (int16_t)row_u16(r, 0x10);   /* +0x10/12/14 */
    f->drift_y = (int16_t)row_u16(r, 0x12);
    f->drift_z = (int16_t)row_u16(r, 0x14);
}

/* FUN_800174e4: cursor++ (slot+0x6f), copy the next row (no terminator — the data ends on a
 * gate-less noop row). */
static void esp_fx_row_advance(re15_esp_fx_t *f)
{
    if (!f->rows_base) return;
    if (f->row_cursor + 1 >= f->row_count) return;   /* past the list: hold (data never does this) */
    f->row_cursor++;
    esp_fx_row_load(f, f->row_cursor);
}

/* The loop-1 routineA dispatch (FUN_80019e20 @0x80019e84-9c) — the BLOOD subset:
 *   0  noop (@0x80017248 jr-ra)
 *   3  countdown row[0x16]; at 0: flags := row[0x0e], advance if row[0x26] (@0x80017348-...)
 *   4  two-phase freeze: flags := row[0x0e] (e.g. 0x61 = bit5 physics-freeze), countdown
 *      row[0x16]; at 0: flags := row[0x1e] (release), advance if row[0x26] (@0x800173a4-...)
 *   5  anim-record index := row[0x0e], advance if row[0x26] (@0x80017430-...)
 * Unsupported selectors (the muzzle/shell/room chains — stage 3) act as noop here; the
 * physics/anim layers still run them faithfully enough for the ballistic droplets. */
static void esp_fx_dispatch(re15_esp_fx_t *f)
{
    if (!f->rows_base) return;
    uint16_t A = row_u16(f->row, 0x00);
    switch (A) {
        case 0: break;
        case 3: {
            uint16_t cnt = row_u16(f->row, 0x16);
            if (cnt != 0) { cnt--; f->row[0x16] = (uint8_t)cnt; f->row[0x17] = (uint8_t)(cnt >> 8); }
            else {
                f->flags = f->row[0x0e];
                if (row_u16(f->row, 0x26)) esp_fx_row_advance(f);
                else { f->row[0] = 0; f->row[1] = 0; }   /* hold as noop */
            }
            break;
        }
        case 4: {
            f->flags = f->row[0x0e];                     /* phase-1 flags (0x61 = frozen) */
            uint16_t cnt = row_u16(f->row, 0x16);
            if (cnt != 0) { cnt--; f->row[0x16] = (uint8_t)cnt; f->row[0x17] = (uint8_t)(cnt >> 8); }
            else {
                f->flags = f->row[0x1e];                 /* phase-2 flags (release, 0x03) */
                if (row_u16(f->row, 0x26)) esp_fx_row_advance(f);
                else { f->row[0] = 0; f->row[1] = 0; }
            }
            break;
        }
        case 5: {
            f->frame = (int16_t)((f->row[0x0e]) - 1);    /* anim idx := row[0x0e]; -1: the tick's
                                                          * frame++ lands ON it */
            f->timer = 0;
            if (row_u16(f->row, 0x26)) esp_fx_row_advance(f);
            else { f->row[0] = 0; f->row[1] = 0; }
            break;
        }
        case 16: {  /* @0x80017b80: 2-phase freeze — flags := row[0x0e] (0x63 = bit5+bit6 frozen);
                     * countdown row[0x16]; at 0: flags := row[0x1e], anim := row[0x26],
                     * advance UNCONDITIONAL. The shell's 2-tick eject hold. */
            uint16_t cnt = row_u16(f->row, 0x16);
            if (cnt != 0) {
                f->flags = f->row[0x0e];
                cnt--; f->row[0x16] = (uint8_t)cnt; f->row[0x17] = (uint8_t)(cnt >> 8);
            } else {
                f->flags = f->row[0x1e];
                f->frame = (int16_t)(f->row[0x26] - 1); f->timer = 0;
                esp_fx_row_advance(f);
            }
            break;
        }
        case 11: {  /* @0x80017718: RNG velocity spread ON TOP of the row seed (the SHELL eject —
                     * NOT blood), then A := 0, B := 12 (the floor bounce). Runs once. */
            f->drift_x = (int16_t)(f->drift_x - (re15_engine_rand8() & 0x0a));
            f->drift_y = (int16_t)(f->drift_y - (re15_engine_rand8() & 0x14));
            f->drift_z = (int16_t)(f->drift_z + (re15_engine_rand8() & 0x14));
            f->row[0x00] = 0;    f->row[0x01] = 0;       /* A := 0 (@0x80017780) */
            f->row[0x02] = 0x0c; f->row[0x03] = 0;       /* B := 12 (@0x80017774) */
            break;
        }
        case 8: {   /* @0x800175ec: flags := row[0x0e] (0x93 = anchor-placement show); tpage |=
                     * row[0x16] (@0x80017608-20: lhu +0x30 / lhu +0x16 / or / sh +0x30 — the ABR
                     * bits, muzzle rows carry 0x20 = ABR1 additive); CHILD spawn cat 2 FIXED
                     * (lui 0x200 @0x80017624), sub = row[0x1e], scale = row[0x26], param = the
                     * PARENT's param (lh a1,0x2e(a3) @0x80017614) — the 0x02040bb8 secondary
                     * flash; advance UNCONDITIONAL. */
            f->flags = f->row[0x0e];
            f->tpage |= row_u16(f->row, 0x16);
            re15_esp_fx_spawn_rows(f->bank, 2, f->row[0x1e], row_u16(f->row, 0x26),
                                   f->x, f->y, f->z, f->floor_y, f->param);
            esp_fx_row_advance(f);
            break;
        }
        case 9: {   /* @0x80017654 (self-verified): the positional BANG — FUN_80045024(0x01000001,
                     * &world_pos) + noise latch 0x800b5358 := 1 + advance UNCONDITIONAL. */
            if (re15_esp_bang_hook) re15_esp_bang_hook();
            esp_fx_row_advance(f);
            break;
        }
        case 10: {  /* @0x800176b0 (selbst disassembliert, ROOM11E0 Strom-Effekt 2026-08-08):
                     *   sb  row[0x0e] -> flags+0x6c        (@0x800176c0/c8)
                     *   lhu +0x30 | lhu +0x16 -> sh +0x30  (@0x800176d8-ec: TPAGE |= row[0x16];
                     *                                       id17 row0 = 0x20 -> ABR1 ADDITIV)
                     *   lhu +0x1e << 6 + lhu +0x32 -> sh   (@0x800176e0-fc: CLUT += row[0x1e]*0x40)
                     *   sb  row[0x26] -> anim_idx +0x6e    (@0x800176f4/704)
                     *   jal 0x800174e4 (advance UNCONDITIONAL) */
            f->flags = f->row[0x0e];
            f->tpage |= row_u16(f->row, 0x16);
            f->clut   = (uint16_t)(f->clut + (row_u16(f->row, 0x1e) << 6));
            f->frame = (int16_t)(f->row[0x26] - 1); f->timer = 0;
            esp_fx_row_advance(f);
            break;
        }
        default: break;                                  /* stage-3c selectors: noop for now */
    }
}

/* Platform SE hook: the gunshot BANG (routine 9 = FUN_80045024(0x01000001) = ARMS record 0,
 * fired on the muzzle slot's SECOND tick — replaces the game_step s_bang_delay scheduler). */
void (*re15_esp_bang_hook)(void) = NULL;

/* The MAIN-loop routineB dispatch (@0x8001a2b4-d4). Supported: 12 = the floor bounce
 * (@0x8001779c), collapsed to the port's floor_y plane (the PSX probes room_coll per tick; the
 * plane loses only the ledge/wall Z-reflect branch — documented): airborne = nothing; FIRST flat
 * contact = clink SE + gate := 1 + snap + drift.x/=2, z/=2, y := -(y/3) (the 0x55555556 div-3
 * idiom @0x80017924); SECOND contact (gate set) = kill (@0x800178b0). The gate lives in the row
 * copy's +0x26 (REPURPOSED as runtime state — byte-true). */
static void esp_fx_dispatch_b(re15_esp_fx_t *f)
{
    if (!f->rows_base) return;
    if (row_u16(f->row, 0x02) != 12) return;
    if (f->y + f->xlat_y < f->floor_y) return;           /* airborne */
    if (row_u16(f->row, 0x26)) { f->active = 0; return; }/* 2nd flat contact -> despawn */
    f->row[0x26] = 1; f->row[0x27] = 0;                  /* bounce-once gate (@0x800178ec) */
    f->xlat_y  = f->floor_y - f->y;                      /* snap onto the floor (@0x8001791c) */
    f->drift_x = (int16_t)(f->drift_x / 2);
    f->drift_z = (int16_t)(f->drift_z / 2);
    f->drift_y = (int16_t)(-(f->drift_y / 3));
    if (re15_esp_shell_clink_hook) re15_esp_shell_clink_hook();   /* SE 0x01020001 (platform) */
}

/* Platform SE hook for the shell clink (FUN_80045024(0x01020001) = ARMS bank record 2; the
 * shotgun-shell 0x01090001 variant is a stage-3 refinement). NULL = silent (engine tests). */
void (*re15_esp_shell_clink_hook)(void) = NULL;

/* FUN_80019700 header seed (decompile lines 85-88): per spawned slot
 *   slot+0x32 (CLUT)  = EFF hdr u16 @+4  + ((sub & 0xff) >> 3) * 0x40
 *   slot+0x30 (TPAGE) = EFF hdr u16 @+6
 * On PSX the hdr words are runtime-patched by the TIM installer FUN_800194f8
 * (GetClut(0x120,y) / GetTPage(0,0,x,y) low byte) — for the ROOM bank the file
 * carries only the pre-seed. The draw consumes the ABR bits 5-6 (routines 8/10
 * OR row[0x16] in); the PAGE bits collapse to the per-id TIM slot binding on PC. */
static void esp_fx_seed_header(re15_esp_fx_t *f, const re15_esp_t *rb, int ei, uint8_t sub)
{
    if (!rb || !rb->raw || ei < 0 || ei >= rb->id_count) return;
    uint32_t hs = rb->eff[ei].eff_start;
    if ((size_t)hs + 8 > rb->raw_size) return;
    uint16_t hdr_clut  = (uint16_t)(rb->raw[hs + 4] | (rb->raw[hs + 5] << 8));
    uint16_t hdr_tpage = (uint16_t)(rb->raw[hs + 6] | (rb->raw[hs + 7] << 8));
    f->clut  = (uint16_t)(hdr_clut + (uint16_t)((sub & 0xff) >> 3) * 0x40);
    f->tpage = hdr_tpage;
}

/* Spawn the (effect_id, sub) row streams as ROW-VM slots — one slot per stream (the byte-true
 * spawner allocation; trace wf_a18487d9). `param` = the op/parent param word stored at slot+0x2e
 * (FUN_80019700 `*(u16*)(slot+0x2e) = param_2`). Returns the number of slots spawned. */
int re15_esp_fx_spawn_rows(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub,
                           uint16_t scale16, int32_t x, int32_t y, int32_t z, int32_t floor_y,
                           int16_t param)
{
    const re15_esp_t *rb = bank;
    int ei = re15_esp_find_id(rb, effect_id);
    if (ei < 0) { rb = re15_esp_global_bank(); ei = re15_esp_find_id(rb, effect_id); }
    int streams = (ei >= 0) ? re15_esp_row_streams(rb, ei, sub) : -1;
    int spawned = 0;
    for (int s = 0; s < streams; s++) {
        int nrows = 0;
        const uint8_t *rows = re15_esp_row_stream(rb, ei, sub, s, &nrows);
        if (!rows || nrows <= 0) continue;
        re15_esp_fx_t *f = re15_esp_fx_spawn_ex(bank, effect_id, sub, scale16, x, y, z, param);
        if (!f) break;
        f->phys = 1; f->flags = 0x03;
        esp_fx_seed_header(f, rb, ei, sub);   /* CLUT/TPAGE seed (FUN_80019700) */
        f->rows_base = rows; f->row_count = (uint8_t)(nrows > 255 ? 255 : nrows);
        f->row_cursor = 0;
        esp_fx_row_load(f, 0);
        f->xlat_x = f->xlat_y = f->xlat_z = 0;
        f->floor_y = floor_y;
        spawned++;
    }
    return spawned;
}

/* Byte-true blood/gore SPLATTER — CORRECTED per trace wf_a18487d9 (adversarially verified):
 * blood id 0 is a set of PURE BALLISTIC STREAMS with per-stream ROW CONSTANTS — one spawner call
 * allocates one slot per STREAM, each seeded with ITS row-0 accel/velocity straight from the ESP
 * file (sub 0 = 3 streams: accel (-2,8,0)/(-3,8,0)/(-3,8,0), drift (74,-70,0)/(69,-46,-16)/
 * (64,-56,18) — file bytes @CORE00 0x94C/0x9A0/0x9F4). There is NO RNG on blood: the old
 * `accel_x = rand&3-1` / routine-11 seeding was a mis-attribution — routine 11's RNG spread
 * belongs to the SHELL CASING (CORE00 id 4), not blood (the live -2/-3 accel mix = the per-stream
 * data, not randomness). Floor handling stays the collapsed plane clamp (the chunk-gore chain
 * id5->id7 routine 36/37 is the room-bank path, stage 2). `n` = TRIGGER count (the overlay gore
 * setup fires 0x2000 twice = 2 triggers x 3 streams = the 6 live slots). Falls back to the old
 * RNG spread only when the bank carries no row block (defensive). */
void re15_esp_fx_splatter(const re15_esp_t *bank, uint8_t effect_id, int n,
                          int32_t x, int32_t y, int32_t z, int32_t floor_y)
{
    /* resolve the row bank exactly like spawn_ex (room first, then the global CORE00) */
    const re15_esp_t *rb = bank;
    int ei = re15_esp_find_id(rb, effect_id);
    if (ei < 0) { rb = re15_esp_global_bank(); ei = re15_esp_find_id(rb, effect_id); }
    int streams = (ei >= 0) ? re15_esp_row_streams(rb, ei, 0) : -1;

    for (int k = 0; k < n; k++) {
        if (streams > 0) {
            for (int s = 0; s < streams; s++) {
                int nrows = 0;
                const uint8_t *rows = re15_esp_row_stream(rb, ei, 0, s, &nrows);
                if (!rows || nrows <= 0) continue;
                re15_esp_fx_t *f = re15_esp_fx_spawn_ex(bank, effect_id, 0, 0x1000, x, y, z, 0);
                if (!f) return;                 /* pool full */
                f->phys      = 1;
                f->flags     = 0x03;            /* spawner init (@0x800197b4-d0): active+visible */
                esp_fx_seed_header(f, rb, ei, 0);   /* CLUT/TPAGE seed (FUN_80019700) */
                f->rows_base = rows;
                f->row_count = (uint8_t)(nrows > 255 ? 255 : nrows);
                f->row_cursor = 0;
                esp_fx_row_load(f, 0);          /* row-0 copy: accel/velocity/selectors */
                f->xlat_x = f->xlat_y = f->xlat_z = 0;
                f->floor_y = floor_y;
            }
        } else {
            /* no row block resolvable (synthetic bank): keep the previous behaviour */
            re15_esp_fx_t *f = re15_esp_fx_spawn_ex(bank, effect_id, 0, 0x1000, x, y, z, 0);
            if (!f) return;
            f->phys    = 1;
            f->accel_x = (int16_t)((re15_engine_rand8() & 3) - 1);
            f->accel_y = 8;
            f->accel_z = 0;
            f->drift_x = (int16_t)(-(re15_engine_rand8() & 0x0a));
            f->drift_y = (int16_t)(-(re15_engine_rand8() & 0x14));
            f->drift_z = (int16_t)( (re15_engine_rand8() & 0x14));
            f->xlat_x = f->xlat_y = f->xlat_z = 0;
            f->floor_y = floor_y;
        }
    }
}

void re15_esp_fx_tick(const re15_esp_t *bank)
{
    /* ANIM-/FX-FREEZE (Bit 0x10000000) — SELBST-GATE, byte-true zum Prolog derselben
     * Original-Funktion, deren Rumpf dieser Treiber portiert (Fix-Runde Cluster 1, Fund 5;
     * selbst nachdisassembliert 2026-08-17, info/Re1.5/PSX.EXE):
     *   80019e20  addiu sp,sp,-32
     *   80019e28  lw   v0,-13760(v0)      v0 = g_pauseflags (0x800aca40)
     *   80019e2c  lui  v1,0x1000
     *   80019e3c  and  v0,v0,v1
     *   80019e40  bne  v0,zero,0x8001a4a4 -> 0x8001a4a4 ist der REINE Epilog
     *                                        (`lw ra,28(sp)` .. @0x8001a4b8 `jr ra`)
     * Der komplette Rumpf faellt also aus, solange Bit 0x10000000 steht — deshalb stehen im
     * Original bei offenem Examine-Text auch die Partikel und nicht nur die Entscheidungen.
     * Das Gate sitzt hier (nicht am Aufrufer), weil das Original es ebenfalls im Prolog der
     * Funktion selbst traegt; damit gilt es fuer PC- UND PSX-Loop gleichermassen.
     * (Die ANDERE Haelfte derselben Original-Funktion, die Keyframe-Integration, ist in
     *  game_step_common.c mit denselben Adressen gegatet.) */
    if (g_re15_pauseflags & RE15_PAUSE_ACTION) return;
    /* Byte-true FUN_80019e20 frame timer (L117-131): when the per-slot timer hits 0, advance
     * the anim-record index; the new record's param-low byte = its duration, 0xFF = loop back
     * to the record's desc-low byte, 0/0 (duration & loop-target both 0) = end -> despawn.
     * Each fx animates from ITS OWN resolved bank (room or global), set at spawn. */
    (void)bank;   /* per-fx bank now (f->bank); kept for call-site compat */
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        re15_esp_fx_t *f = &s_esp_fx[i];
        if (!f->active) continue;

        /* ROW-VM DISPATCH (stage 2 blood subset — FUN_80019e20 loop 1): run the row's routineA
         * BEFORE the physics, exactly like the PSX tick order. An advance re-seeds accel/velocity
         * = the multi-phase ballistics (blood st0 switches to (60,14,0) after its 7-tick count). */
        if (f->rows_base) {
            esp_fx_dispatch(f);                  /* loop-1 routineA */
            esp_fx_dispatch_b(f);                /* main-loop routineB (the shell bounce) */
            if (!f->active) continue;            /* B may despawn (2nd floor contact) */
        }

        /* PHYSICS (byte-exact tick @0x8001a2f0, gated flags bit5==0): the position offset xlat
         * (s32) integrates the drift velocity, which itself accelerates by gravity — LIVE-confirmed
         * against mzd_stage1_hit_effect.sav. Draw adds xlat to the anchor. The row-VM freeze bit
         * (flags bit5, e.g. the sub-2 stagger rows' 0x61) pauses the integration. */
        if (f->phys && !(f->rows_base && (f->flags & 0x20))) {
            f->xlat_x += f->drift_x;                 /* xlat[0x34] += drift[0x10] */
            f->xlat_y += f->drift_y;                 /* xlat[0x38] += drift[0x12] */
            f->xlat_z += f->drift_z;                 /* xlat[0x3c] += drift[0x14] */
            f->drift_x = (int16_t)(f->drift_x + f->accel_x);   /* drift += accel (gravity) */
            f->drift_y = (int16_t)(f->drift_y + f->accel_y);
            f->drift_z = (int16_t)(f->drift_z + f->accel_z);
            /* FLOOR BOUNCE (routine 12 room_coll @0x8001c6e8, collapsed to a plane clamp): when the
             * particle reaches the floor, clamp it there and damp-flip drift.y so it settles. */
            if (f->y + f->xlat_y >= f->floor_y) {
                f->xlat_y = f->floor_y - f->y;
                f->drift_y = (int16_t)(-f->drift_y / 2);       /* 50% restitution */
                f->drift_x = (int16_t)(f->drift_x * 3 / 4);    /* ground friction */
                f->drift_z = (int16_t)(f->drift_z * 3 / 4);
            }
        }

        if (f->timer == 0) {
            if (!(f->flags & 0x40)) f->frame++;   /* freeze-frame bit6 (@0x8001a3a8 lbu +0x6c; andi 0x40;
                                                   * bne skip): a frozen droplet HOLDS its frame — re-read
                                                   * the current record + re-arm the timer, do not advance
                                                   * (audit wf_8cc15b53). */
            re15_esp_anim_t a;
            if (f->eff_idx < 0 || re15_esp_anim(f->bank, f->eff_idx, f->frame, &a) != 0) {
                f->active = 0;            /* no bank / ran past the records */
                continue;
            }
            uint8_t dur  = (uint8_t)(a.param & 0xff);   /* pcVar6[2] */
            uint8_t loop = (uint8_t)(a.desc  & 0xff);   /* pcVar6[0] */
            if (dur == 0 && loop == 0) { f->active = 0; continue; }   /* terminator */
            if (dur == 0xff) {                                        /* loop marker */
                f->frame = loop;
                if (re15_esp_anim(f->bank, f->eff_idx, f->frame, &a) != 0) { f->active = 0; continue; }
                dur = (uint8_t)(a.param & 0xff);
            }
            f->timer = (int16_t)dur;
        }
        if (f->timer > 0) f->timer--;
    }
}
