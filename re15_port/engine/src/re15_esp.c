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
#include "re15_actor.h" /* g_actors — Follow-Anker (Flags-Bit 0x04, @0x80019f44-f94) */
#include "re15_damage.h"   /* re15_resolve_attack = FUN_80012d60 (Routine 31, Runde 34 A5) */
#include "re15_skeleton.h" /* re15_sin_q12/re15_cos_q12 = Tabelle 0x800794c4 (RotMatrix-Zwilling) */
#include "re15_engine.h"   /* g_engine.frame_count — nur RE15_GRANATE_LOG */
#include "re15_esp_brocken.h" /* Runde 35 Spur D: Routinen B 36/37 (esp_brocken.c) */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>        /* getenv — RE15_GRANATE_LOG (Diagnose, kein Verhalten) */

extern uint8_t re15_engine_rand8(void);   /* the shared FUN_8001af20 draw (re15_damage.c) */
/* Runde 34 A3: das Wort 0x800acaec (Spieler +0x98) = Zielbits 0x8000/0x4000/0x2000 | Status-
 * Unterbits; im Port getrennt gefuehrt (player_common.c s_aim_elev + actor.status_flags) und dort
 * wieder zusammengesetzt. */
extern uint16_t re15_player_acaec(void);

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

/* GLOBAL-Bank Effekt-Id -> Sheet-Index. Reihenfolge = die der Slot-Tabelle in main.c; die
 * Zuordnung selbst steht (mit den word1-Belegen je Id) im Header-Kommentar. Die Ids sind genau
 * die 5 der Datei (CORE00.ESP[0..4] = `03 08 00 02 04`, danach 0xFF) — ein anderer Wert kann aus
 * dieser Bank nicht kommen. */
static const uint8_t s_global_sheet_id[RE15_ESP_GLOBAL_SHEETS] = { 0x00, 0x02, 0x03, 0x04, 0x08 };

int re15_esp_global_sheet_index(uint8_t effect_id)
{
    for (int i = 0; i < RE15_ESP_GLOBAL_SHEETS; i++)
        if (s_global_sheet_id[i] == effect_id) return i;
    return -1;
}

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
        /* ANIM-START wie die beiden Spawner (Integration Runde 34 W7, selbst disassembliert):
         *   FUN_80019700  8001989c lbu v1,10(t5)        Satz[0].Byte2 (Dauer; t5+8 = Satztabelle)
         *                 800198a0 ori v0,zero,0x1 / 800198a4 sb v0,110(t0)   +0x6e := 1
         *                 800198bc sb v1,109(t0)        +0x6d := Satz[0].Byte2
         *   FUN_800199d4  80019b70 lbu v1,10(t5) / 80019b74-78 ori 1, sb 110 / 80019b90 sb v1,109
         * Vorher: frame 0 / timer 0 (der erste Anim-Schritt sprang auf Satz 1 mit Dauer von Satz 1).
         * In allen 506 Effekten der Auslieferung (501 Raum-Effekte, 5 CORE00) ist Dauer(Satz 0) ==
         * Dauer(Satz 1) (gemessen, integration.md W7) -> dieselbe Bildfolge; anders nur, wenn eine
         * Routine im Spawn-Takt den Satzindex OHNE Zeitgeber setzt (Routine 41 `sb v0,110(v1)`
         * @0x80018f60) oder ein Platz vor seinem ersten Takt gezeichnet wird (Original: Satz 1). */
        f->frame     = 1;
        {
            re15_esp_anim_t a0;
            f->timer = (f->eff_idx >= 0 && re15_esp_anim(f->bank, f->eff_idx, 0, &a0) == 0)
                       ? (int16_t)(a0.param & 0xff) : 0;
        }
        f->x = x; f->y = y; f->z = z;
        f->param     = param;
        f->follow_slot = -1;  /* kein Eltern-Anker (Flags-Bit 0x04 wirkt nur mit Slot) */
        return f;
    }
    return NULL;   /* pool full */
}

re15_esp_fx_t *re15_esp_fx_spawn(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub_index,
                                 int32_t x, int32_t y, int32_t z, int16_t param)
{
    return re15_esp_fx_spawn_ex(bank, effect_id, sub_index, 0x1000, x, y, z, param);
}

/* ===== ROOM1090 "aussen fehlt Feuer" — der FEUER-Emitter (Entity-Typ 0x26) ================
 *
 * Die sieben Entities, die ROOM1090s sub00 setzt, sind die BRENNENDEN TRUEMMER auf dem
 * Hinterhof — nicht "Spinnen". Beleg-Kette (alles selbst gedumpt/disassembliert):
 *
 *  1) ROOM1090.RDT sub00 @Datei 0x2214: 7x Sce_em_set (Opcode 0x44, 20 B)
 *       44 00 26 00 | 44 01 26 01 | 44 02 26 02 | 44 03 26 04
 *       44 04 26 03 | 44 05 26 03 | 44 06 26 04
 *     = Typ 0x26, Spawn-Byte pc[3] = 0,1,2,4,3,3,4 -> entity+0x9 (grid_id). Alle sieben
 *     auf Y=-1800 (Bodenhoehe), X 1374..3104 / Z -3498..+2040 = eine Reihe am Boden.
 *     Typ 0x26 kommt in KEINEM anderen der 240 RDTs vor.
 *  2) EXE-Dispatch 0x80072bac[0x26] = 0x80116288 (STAGE1-Registrierung @0x8011e8f4).
 *  3) ROOM1090.RDT+0x4C (Datei 0x11010) laedt die Effekt-Ids `05 07 09 10`; die eingebettete
 *     TIM von Id 0x10 (RDT+0x54 0x2EEB8 + Eintrag 0x64C0 = Datei 0x35378, 4bpp 256x144)
 *     ist ein FLAMMEN-Sheet (8 grosse Frames + zwei 3x3-Kachelsaeulen); Id 0x09
 *     (Datei 0x32338, 256x96) sind die FUNKEN. ROOM109.BSS traegt 16 Cuts = 8 Kamera-
 *     winkel x 2 Beleuchtungen, Frames 8..15 = dieselben Winkel im orangen Feuerschein.
 *  4) Die Zustands-Handler des Roots spawnen genau diese beiden Effekte:
 *       INIT  @0x801166c4-e8: `andi v0,0x80; bne -> skip` (Gate grid&0x80), dann
 *                             `lui a0,0x903; ori a0,a0,0x1800` = a0 0x09031800,
 *                             a1 = lh +0x6a, a2 = *(+0x188)+0x40, a3 = @0x80121248,
 *                             `jal 0x80019700`  => Id 0x09, sub 3, scale 0x1800.
 *       FLAMME FUN_80116d00: `lbu v1,9(ent); andi v1,0x7f; sltiu v0,v1,0x5;
 *                             beq v0,zero,<skip>` @0x80116d10-20, dann Sprungtabelle
 *                             @0x80100364[variant]:
 *                               [0]=0x80116d44 `lui v1,0x803`  -> Id 0x08 (CORE00-Bank)
 *                               [1]=0x80116d5c `lui v1,0x1003` -> Id 0x10 (Raum-Bank)  @0x80116d6c
 *                               [2]=0x80116d5c -> 0x10   [3]=0x80116d44 -> 0x08
 *                               [4]=0x80116d5c -> 0x10
 *                             a0 = (lbu +0x1d0) << 8 | v1   (@0x80116d70/d7c/d80)
 *                             a1 = lh +0x6a, a2 = lw +0x188 + 64, a3 = @0x80121248 (Null-SVECTOR)
 *                             `jal 0x80019700` @0x80116d84
 *                          => Id 0x08/0x10, sub 3, scale16 = entity[0x1D0] << 8.
 *     entity[0x1D0] wird VOR dem Aufruf im Delay-Slot gesetzt (`sb v0,464(a0)` @0x801167a8
 *     bzw. @0x801168c0) — Behavior A (Variante 0..2) 0x28, Behavior B (3..4) 0x2C
 *     (@0x80116784 / @0x8011689c) -> scale16 0x2800 bzw. 0x2C00.
 *  5) Voll-Slice ueber ALLE 702 `jal`-Stellen zu 0x80019700 / 0x800199d4 / 0x80019d50 /
 *     0x80019ca8 in PSX.EXE + STAGE1..5.BIN: cat 0x10 taucht NUR in 0x80116bb4-0x80116d88
 *     auf (dieser Emitter). Es gibt keinen zweiten Feuer-Spawner im Spiel.
 *
 * Die beiden Funktionen unten sind die byte-treue Uebersetzung von 4) — sie geben NULL
 * zurueck, wo das Original gar nicht spawnt (Gate grid&0x80 bzw. variant >= 5). */

/* Sprungtabelle @0x80100364 (STAGE1.BIN, 5 Eintraege, Index = entity[+0x9] & 0x7f):
 * die Zieladresse bestimmt nur, welches `lui v1,...` laeuft -> die Effekt-Id. */
static const uint8_t s_type26_flame_id[5] = { 0x08, 0x10, 0x10, 0x08, 0x10 };

int re15_esp_type26_flame_id(uint8_t grid_id)
{
    uint8_t v = (uint8_t)(grid_id & 0x7f);        /* `andi v1,v0,0x7f`  @0x80116d18 */
    if (v >= 5) return -1;                        /* `sltiu v0,v1,0x5` + beq -> kein Spawn @0x80116d1c-20 */
    return (int)s_type26_flame_id[v];
}

static void esp_fx_row_load(re15_esp_fx_t *f, int idx);                 /* unten definiert */
static void esp_fx_seed_header(re15_esp_fx_t *f, const re15_esp_t *rb, int ei, uint8_t sub);

/* Ein Row-VM-Slot fuer GENAU EINEN Stream (Stream 0) — die Feuer-Effekte (sub 3) tragen
 * je einen Stream mit 2 Rows (Raum-RDT @0x1161c/@0x11858, CORE00 @0x7cc: row0 selA=17,
 * row1 selA=18). Fallback ohne Row-Block: der alte spawn_ex-Pfad (defensiv). */
static re15_esp_fx_t *esp_fx_spawn_rows_one(const re15_esp_t *bank, uint8_t effect_id,
                                            uint8_t sub, uint16_t scale16,
                                            int32_t x, int32_t y, int32_t z, int16_t param)
{
    const re15_esp_t *rb = bank;
    int ei = re15_esp_find_id(rb, effect_id);
    if (ei < 0) { rb = re15_esp_global_bank(); ei = re15_esp_find_id(rb, effect_id); }
    int nrows = 0;
    const uint8_t *rows = (ei >= 0) ? re15_esp_row_stream(rb, ei, sub, 0, &nrows) : NULL;
    re15_esp_fx_t *f = re15_esp_fx_spawn_ex(bank, effect_id, sub, scale16, x, y, z, param);
    if (!f) return NULL;
    if (rows && nrows > 0) {
        f->phys = 1; f->flags = 0x03;
        esp_fx_seed_header(f, rb, ei, sub);           /* CLUT/TPAGE-Seed (FUN_80019700) */
        f->rows_base = rows; f->row_count = (uint8_t)(nrows > 255 ? 255 : nrows);
        f->row_cursor = 0;
        esp_fx_row_load(f, 0);
        f->xlat_x = f->xlat_y = f->xlat_z = 0;
        f->floor_y = y;
    }
    return f;
}

re15_esp_fx_t *re15_esp_type26_flame(const re15_esp_t *bank, uint8_t grid_id, uint8_t phase,
                                     int32_t x, int32_t y, int32_t z, int16_t yaw,
                                     int follow_slot)
{
    int id = re15_esp_type26_flame_id(grid_id);
    if (id < 0) return NULL;
    /* a0 = (entity[0x1D0] << 8) | (id << 24) | (3 << 16): cat = a0>>24, sub = (a0>>16)&0xff,
     * scale16 = a0 & 0xffff (Spawner FUN_80019700 @0x80019704-1c). Row-VM-Pfad (2026-08-29,
     * analysis/nutzer_batch_2026-08-29/1090-feuer-letzte-kamera.md): sub-3-Stream = Routine 17
     * (Flags 0x17 = aktiv|sichtbar|FOLLOW|ABE + TPAGE|=0x20 ABR1 ADDITIV) -> Routine 18
     * (defW/defH-Oszillator). a2 des Originals = *(entity+0x188)+0x40 = die Part-Weltmatrix
     * des Emitters -> Port: follow_slot. */
    re15_esp_fx_t *f = esp_fx_spawn_rows_one(bank, (uint8_t)id, 3,
                                             (uint16_t)((uint16_t)phase << 8), x, y, z, yaw);
    if (f) f->follow_slot = (int8_t)follow_slot;
    return f;
}

re15_esp_fx_t *re15_esp_type26_emerge(const re15_esp_t *bank, uint8_t grid_id,
                                      int32_t x, int32_t y, int32_t z, int16_t yaw,
                                      int follow_slot)
{
    if (grid_id & 0x80) return NULL;              /* `andi v0,v0,0x80; bne -> skip` @0x801166c4-cc */
    re15_esp_fx_t *f = esp_fx_spawn_rows_one(bank, 0x09, 3, 0x1800, x, y, z, yaw);  /* a0 = 0x09031800 */
    if (f) f->follow_slot = (int8_t)follow_slot;
    return f;
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
void re15_esp_fx_zeile_weiter(re15_esp_fx_t *f) { esp_fx_row_advance(f); }   /* Runde 35 Spur D (esp_brocken.c) */

/* The loop-1 routineA dispatch (FUN_80019e20 @0x80019e84-9c) — the BLOOD subset:
 *   0  noop (@0x80017248 jr-ra)
 *   3  countdown row[0x16]; at 0: flags := row[0x0e], advance if row[0x26] (@0x80017348-...)
 *   4  two-phase freeze: flags := row[0x0e] (e.g. 0x61 = bit5 physics-freeze), countdown
 *      row[0x16]; at 0: flags := row[0x1e] (release), advance if row[0x26] (@0x800173a4-...)
 *   5  anim-record index := row[0x0e], advance if row[0x26] (@0x80017430-...)
 * Unsupported selectors (the muzzle/shell/room chains — stage 3) act as noop here; the
 * physics/anim layers still run them faithfully enough for the ballistic droplets. */
/* SLOT FREIGEBEN — das Flags-Byte slot+0x6c IST im Original zugleich die Belegung.
 * Beleg (selbst disassembliert, info/Re1.5/PSX.EXE):
 *   Spawner-Suche FUN_80019700: 800197a8 lbu v0,108(t0) / 800197b0 beq v0,zero,0x800197d0
 *                               -> ein Platz mit Flags==0 wird neu vergeben (0x60 Plaetze,
 *                                  Schrittweite 132 @0x800197c4)
 *   Tick-Schleife 1 (Routine A): 80019e70 lbu v0,108(v1) / 80019e78 andi v0,v0,0x1 /
 *                                80019e7c beq v0,zero -> Dispatch uebersprungen
 *   Tick-Hauptschleife:          80019f44 lbu v1,108(a3) / 80019f4c andi v0,v1,0x1 /
 *                                80019f50 beq v0,zero,0x8001a480 -> ganzer Slot-Rumpf aus
 * Der Port fuehrt die Belegung getrennt in `active`; ein `flags = 0` allein liess den Platz
 * deshalb ewig im Dispatch stehen. */
static void esp_fx_kill(re15_esp_fx_t *f)
{
    f->flags  = 0;    /* byte-true: slot+0x6c := 0 */
    f->active = 0;    /* Port-Belegung nachziehen (im Original dasselbe Byte) */
}

/* ===== Runde 34 Spur A — Hilfen fuer die Granaten-Routinen 29/30/31 =========================
 * u16-Schreiber in die Zeilenkopie slot+0x00..0x27 (die Routinen schreiben mit `sh`). */
static void row_set16(re15_esp_fx_t *f, int off, uint16_t v)
{
    f->row[off] = (uint8_t)v; f->row[off + 1] = (uint8_t)(v >> 8);
}

/* KEIN BODEN fuer Granatenplaetze und ihre Kinder (E12 / A6): der Original-Tick hat keine
 * Klemme — Physik @0x8001a2fc-388 ist reines xlat += vel, vel += acc; den Boden kennt nur
 * Routine B (12 bzw. 29: `lh t1,42(t0)` / `blez t1` @0x80018330-38). Die Port-Sammelklemme
 * (re15_esp_fx_tick, "FLOOR BOUNCE") vergleicht y + xlat_y >= floor_y; mit INT32_MAX greift
 * sie fuer diese Plaetze nie. Die uebrigen Effekte behalten ihre Klemme (eigenes Thema). */
#define ESP_KEIN_BODEN INT32_MAX

/* RE15_GRANATE_LOG=<datei> — DIAGNOSE (kein Verhalten): je ESP-Tick eine Zeile je Granaten-
 * platz (granate_art != 0) plus die Ereignisse der Routinen 29/31 (SE-, Resolver-, Kind-,
 * Aufschlag-, Latch-Aufrufe). Datei statt stderr: die GUI-exe hat kein stderr. */
static unsigned s_gr_tick = 0;   /* zaehlt die ESP-Ticks, die das Pause-Gate passieren */
static FILE *esp_granate_log(void)
{
    static FILE *s_gl = NULL; static int s_init = 0;
    if (!s_init) { s_init = 1;
        const char *e = getenv("RE15_GRANATE_LOG");
        if (e && *e) s_gl = fopen(e, "w"); }
    return s_gl;
}

/* MESSSCHIENE fuer die Sonde (kein Verhalten): Resolver-Aufrufe aus Routine 31. */
static unsigned s_granate_resolver_calls = 0;
unsigned re15_esp_granate_resolver_calls(void) { return s_granate_resolver_calls; }

/* FUN_800199d4-Zwilling (Kind-Spawner mit Start-Flags 0x0a), unten definiert. */
static int esp_fx_spawn_kind(const re15_esp_t *bank, uint32_t code, int16_t gier,
                             const int32_t p[3]);
/* Zeilen-Spawn mit waehlbaren Start-Flags (0x03 = FUN_80019700, 0x0a = FUN_800199d4), unten
 * definiert. Routinen 8/15 spawnen ihre Kinder damit ueber den 0x0a-Weg (Gegenpruefung M-3). */
static int esp_fx_spawn_rows_flags(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub,
                                   uint16_t scale16, int32_t x, int32_t y, int32_t z,
                                   int32_t floor_y, int16_t param, uint8_t flags0);

/* Explosionspunkt P = (x, Welt-y - 500, z), s32 aus den s16-Feldern slot+0x28/2a/2c:
 * `lh v0,40(v1)` / `lh v0,42(v1)` / `addiu v0,v0,-500` / `lh v0,44(v1)` @0x80018594-bc
 * (Zuender 7), @0x80018610-38 (Zuender 2), @0x80018688-ac (Zuender 0). */
static void esp_granate_p(const re15_esp_fx_t *f, int32_t p[3])
{
    p[0] = (int32_t)f->wpos[0];
    p[1] = (int32_t)f->wpos[1] - 500;
    p[2] = (int32_t)f->wpos[2];
}

/* Granaten-Art (Resolver-Art a2, granate_art) -> RE2-Art-Byte des Aufschlags (V1d). EXPLIZITE
 * Tabelle, nie "Id - 9" (RE2 +0x1B := Id - 9 @0x8001f1a8-b8 im RE2-PSX.EXE: 1 = Brand/Op 48,
 * 2 = Saeure/Op 49; RE1.5 0x0A - 9 waere 1 = Brand = vertauscht, Saeure-GP §15):
 *   Art 3 (Item 0x0A Acid)       -> 2 (Saeure, Optab @0x8009D868[49] = 0x800215C8)
 *   Art 4 (Item 0x0B Incendiary) -> 1 (Brand,  Optab @0x8009D868[48] = 0x80020F3C) */
static int esp_granate_re2_art(uint8_t art)
{
    return (art == 3) ? 2 : (art == 4) ? 1 : 0;
}

/* FUN_8002b7e8-Zwilling (Integration Runde 34 W7; selbst disassembliert `dis 0x8002b7e8 44`):
 *   8002b7f0-f4 lbu s1,-13746(s1)         Zahl aktiver Gegner 0x800aca4e
 *   8002b80c-10 addiu s0,s0,-13268        Gegnerliste 0x800acc2c
 *   8002b824-30 lw v0,0(s0) / andi 0x1    Wort 0 Bit 0 (aktiv) -> s1--, FUN_8002b5d0(s0, P, r & 0xffff)
 *   8002b848    or s2,s2,v0               Treffer ODER-verknuepft (kein Abbruch, kein Schaden)
 *   8002b854    addiu s0,s0,500           Schritt 0x1F4, bis s1 aktive gezaehlt sind (@0x8002b84c-50)
 *   8002b858-6c FUN_8002b5d0(Spieler 0x800aca54, P, r), ODER
 *   8002b870    andi v0,s2,0xff           Rueckgabe
 * FUN_8002b5d0 = re15_hitbox_test (Kasten je Typ, re15_damage.c). */
static int esp_treffer_test(const int32_t p[3], int r)
{
    re15_attack_box_t box;
    box.x = p[0]; box.y = p[1]; box.z = p[2];
    box.radius = (uint16_t)r;
    int t = 0;
    for (int i = RE15_ACTOR_SLOT_PLAYER + 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *e = &g_actors[i];
        if (!e->active) continue;
        t |= re15_hitbox_test(e, &box);
    }
    t |= re15_hitbox_test(&g_actors[RE15_ACTOR_SLOT_PLAYER], &box);
    return t & 0xff;
}

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
        case 15: {  /* @0x80017ac8 (selbst disassembliert 2026-09-12): UNSICHTBAR
                     * WARTEN, dann KIND-EFFEKT am Slot spawnen. Traeger u.a. die
                     * Huelsen-Slots der Burst-Pistolen (id4 sub2) - ohne die
                     * Routine hing deren Slot ewig (dieselbe Klasse wie die
                     * Schrothuelse, Dossier effekte-haengen.md par.3).
                     *   80017adc  sb 0x65,slot+0x6c   ; aktiv+FOLLOW+Freezes,
                     *                                  ; OHNE sichtbar-Bit 0x02
                     *   80017af4  row[0x0e] != 0 -> row[0x0e]--, RETURN
                     *   80017b10-3c  Kind: Code = row[0x17]<<24 | row[0x16]<<16
                     *                | scale16 (slot+0x72), param = slot+0x2e,
                     *                Ort = Slot-Position (FUN_800199d4 =
                     *                Positions-Spawner; im Port derselbe Weg wie
                     *                der Routine-8-Kind-Spawn)
                     *   80017b4c-60  row[0x26] != 0 -> Advance (0x800174e4)
                     *   80017b6c     sonst Slot AUS (Flags 0)
                     * Runde 34 A NACHBESSERUNG (Gegenpruefung M-3): das Kind kommt aus
                     * FUN_800199d4 (`jal 0x800199d4` @0x80017b38; a1 = `lh a1,46(a3)`
                     * @0x80017b1c, a2 = `lw a2,116(a3)` @0x80017b20, a3 = slot+0x40
                     * @0x80017b24) -> Start-Flags 0x0a (`ori v0,zero,0xa` @0x80019a88 /
                     * `sb v0,108(t0)` @0x80019aa4), NICHT 0x03 (FUN_80019700 @0x800197b4):
                     * Routine A des Kindes laeuft genau einmal in der Kind-Init des Haupt-
                     * laufs (@0x80019ef4-f30), auch wenn es UNTER dem Eltern-Index landet.
                     * Lage (Anker = Eltern-Anker), Boden und Skala wie bisher. */
            f->flags = 0x65;
            {
                uint16_t cnt = row_u16(f->row, 0x0e);
                if (cnt != 0) {
                    cnt--;
                    f->row[0x0e] = (uint8_t)cnt; f->row[0x0f] = (uint8_t)(cnt >> 8);
                    break;
                }
            }
            esp_fx_spawn_rows_flags(f->bank, f->row[0x17], f->row[0x16], f->scale16,
                                    f->x, f->y, f->z, f->floor_y, f->param,
                                    0x0a);   /* FUN_800199d4 @0x80019a88 */
            if (row_u16(f->row, 0x26)) esp_fx_row_advance(f);
            else esp_fx_kill(f);   /* @0x80017b6c sb zero,108(v1) = Flags-Byte 0 = Platz FREI */
            break;
        }
        case 38: {  /* @0x800188b8: SCHROTHUELSEN-INIT (Ein-Schuss-Routine; Dispatch-
                     * Tabelle @0x80071d40[38]). Der Port kannte 38 nicht (default:
                     * Noop) - die Huelse blieb ewig auf Row 0 stehen: vel (0,0,0),
                     * Anim von id 4 loopt ohne Terminator => "die Kugeln fliegen auf
                     * der Stelle rum und bleiben da" (Nutzer 2026-09-12).
                     * Byte-true:
                     *   800188bc lbu DAT_800aca5d     ; angelegte Waffe
                     *   800188c4 bne v1,8             ; 8 = Remington M870
                     *   800188e8 sh  v0,0x16(v1)      ; Haltedauer := 0x17 (23) bzw. 3
                     *                                  ; (SPAS-12 faellt in den 3er-Zweig)
                     *   80018900 sb  row[0x0e],flags  ; := 0x67 (FOLLOW + Physik-/Anim-Freeze)
                     *   80018914 sh  16,row[0x00]     ; Selector := Routine 16
                     * KEIN Advance, KEIN Countdown in diesem Tick - ab dem naechsten
                     * uebernimmt Routine 16 (Hold, dann Release auf Anim 9 + Row 1 =
                     * Routine 11 mit RNG-Streuung und B=12-Bodenkill). */
            extern int re15_player_equipped_weapon(void);
            uint16_t halt = (re15_player_equipped_weapon() == 8) ? 0x17 : 3;
            f->row[0x16] = (uint8_t)halt; f->row[0x17] = (uint8_t)(halt >> 8);
            f->flags = f->row[0x0e];
            f->row[0x00] = 16; f->row[0x01] = 0;
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
                     * flash; advance UNCONDITIONAL.
                     * Runde 34 A NACHBESSERUNG (Gegenpruefung M-3): Spawner = FUN_800199d4
                     * (`jal 0x800199d4` @0x80017634; a2 = slot+0x4c `addiu a2,a3,76`
                     * @0x80017604, a3 = slot+0x40 `addiu a3,a3,64` @0x8001762c) -> Start-
                     * Flags 0x0a (`ori v0,zero,0xa` @0x80019a88), nicht 0x03: Routine A des
                     * Zweitblitzes laeuft einmal in der Kind-Init (@0x80019ef4-f30), auch auf
                     * einem Platz UNTER dem Eltern-Index. Lage/Boden wie bisher. */
            f->flags = f->row[0x0e];
            f->tpage |= row_u16(f->row, 0x16);
            esp_fx_spawn_rows_flags(f->bank, 2, f->row[0x1e], row_u16(f->row, 0x26),
                                    f->x, f->y, f->z, f->floor_y, f->param,
                                    0x0a);   /* FUN_800199d4 @0x80019a88 */
            esp_fx_row_advance(f);
            break;
        }
        case 9: {   /* @0x80017654 (self-verified): the positional BANG — FUN_80045024(0x01000001,
                     * &world_pos) + noise latch 0x800b5358 := 1 + advance UNCONDITIONAL. */
            if (re15_esp_bang_hook) re15_esp_bang_hook();
            /* Runde 34 A7: der Latch 0x800b5358 ist ein EIN-BILD-LICHT, kein Laerm (Wurf-Dossier
             * §6, Gegenpruefung K): nach dem SE `jal 0x80045024` @0x80017684 setzt Routine 9
             *   8001768c  ori  v0,zero,0x1
             *   80017690  lui  at,0x800b
             *   80017694  sb   v0,21336(at)        ; 0x800b5358 := 1
             * und erst danach `jal 0x800174e4` @0x80017698 (Vorschub). Leser/Loescher = Plattform
             * (Spur C3: @0x8001ce60 / @0x8001d1b4). */
            g_re15_licht_latch = 1;
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
        case 17: {  /* FUN_80017c00 (FEUER-Init, byte-exakt selbst nachdisassembliert 2026-08-29):
                     *   flags := row[0x0e]              @0x80017c10/18   (0x17 = aktiv|sichtbar|
                     *                                                     FOLLOW 0x04|ABE 0x10)
                     *   tpage |= row[0x16]              @0x80017c28-3c   (0x20 = ABR1 ADDITIV)
                     *   +0x6e := rng & 4                @0x80017c38-50   (Anim-Startframe 0/4)
                     *   advance -> Routine 18           @0x80017c4c
                     *   Oszillator-Seeds in die frische Row-KOPIE (sh, @0x80017c5c-78):
                     *   row[0x0e]:=1, row[0x16]:=10, row[0x1e]:=-20, row[0x26]:=100 */
            f->flags = f->row[0x0e];
            f->tpage |= row_u16(f->row, 0x16);
            f->frame = (int16_t)((int)(re15_engine_rand8() & 4u) - 1);   /* +0x6e (Konvention
                                                                          * wie Routine 10) */
            f->timer = 0;
            esp_fx_row_advance(f);
            f->row[0x0e] = 1;    f->row[0x0f] = 0;
            f->row[0x16] = 10;   f->row[0x17] = 0;
            f->row[0x1e] = 0xec; f->row[0x1f] = 0xff;    /* -20 (0xffec) */
            f->row[0x26] = 100;  f->row[0x27] = 0;
            break;
        }
        case 18: {  /* FUN_80017c8c (defW/defH-OSZILLATOR, byte-exakt selbst nachdisassembliert):
                     *   row[0x16] != 0:  row[0x16]--; row[0x04] += row[0x1e];
                     *                    row[0x06] += row[0x26]           @0x80017ca8-d4
                     *   row[0x16] == 0:  row[0x16] := 10 (@0x80017ca4/cdc);
                     *                    row[0x0e]/-[0x1e]/-[0x26] negieren @0x80017cd8-fc
                     * = das Groessen-Flackern der Flamme (Draw liest defW/defH aus der Row). */
            uint16_t cnt = row_u16(f->row, 0x16);
            if (cnt != 0) {
                cnt--; f->row[0x16] = (uint8_t)cnt; f->row[0x17] = (uint8_t)(cnt >> 8);
                uint16_t w = (uint16_t)(row_u16(f->row, 0x04) + row_u16(f->row, 0x1e));
                uint16_t h = (uint16_t)(row_u16(f->row, 0x06) + row_u16(f->row, 0x26));
                f->row[0x04] = (uint8_t)w; f->row[0x05] = (uint8_t)(w >> 8);
                f->row[0x06] = (uint8_t)h; f->row[0x07] = (uint8_t)(h >> 8);
            } else {
                f->row[0x16] = 10; f->row[0x17] = 0;
                int16_t e14 = (int16_t)-(int16_t)row_u16(f->row, 0x0e);
                int16_t e1e = (int16_t)-(int16_t)row_u16(f->row, 0x1e);
                int16_t e26 = (int16_t)-(int16_t)row_u16(f->row, 0x26);
                f->row[0x0e] = (uint8_t)e14; f->row[0x0f] = (uint8_t)((uint16_t)e14 >> 8);
                f->row[0x1e] = (uint8_t)e1e; f->row[0x1f] = (uint8_t)((uint16_t)e1e >> 8);
                f->row[0x26] = (uint8_t)e26; f->row[0x27] = (uint8_t)((uint16_t)e26 >> 8);
            }
            break;
        }
        case 30: {  /* ROUTINE 30 @0x8001843c-544 — WURF-INIT der Granate (Routine A der Zeile 0,
                     * CORE00.ESP @0x1AB8 `1e 00 ...` = A 30). Laeuft im Spawnbild in Schleife 1.
                     * Selbst disassembliert (re15_disasm.py dis 0x8001843c 68):
                     *   80018448 ori v0,zero,0x17 / 80018450 sb v0,110(v1)   +0x6e := 23 (Anim-Satz)
                     *   8001845c ori v0,zero,0x3  / 80018460 sb v0,108(v1)   +0x6c := 3  (Flags)
                     *   8001846c ori v0,zero,0x1d / 80018470 sh v0,2(v1)     +0x02 := 29 (Routine B)
                     *   80018478 sh zero,0(v1)                               +0x00 := 0  (Routine A)
                     *   80018474 ori v0,zero,0x2a / 8001847c sh v0,30(v1)    +0x1e := 42 (Zuender)
                     *   80018484 lhu a0,-13588(a0)                           a = u16 0x800acaec */
            f->frame = (int16_t)(0x17 - 1); f->timer = 0;   /* +0x6e := 0x17; Port-Konvention wie
                                                             * Routine 5/10: der Anim-Schritt des
                                                             * Hauptlaufs landet AUF Satz 23. Gleich-
                                                             * wertig, weil Satz 0 (Spawner-+0x6d,
                                                             * `lbu v1,10(t5)` @0x8001989c) und Satz 23
                                                             * (CORE00.ESP @0x17E8 `10 01 01 10`) je
                                                             * Dauer 1 tragen. */
            f->flags = 0x03;
            row_set16(f, 0x02, 29);
            row_set16(f, 0x00, 0);
            row_set16(f, 0x1e, 42);
            {
                uint16_t a = re15_player_acaec();
                if (a & 0x8000) {                  /* HOCH: andi 0x8000 @0x8001848c */
                    f->drift_x = 0x17c;            /* ori 0x17c / sh 16(v1) @0x80018494-98 = 380 */
                    f->drift_y = -110;             /* addiu -110 / sh 18(v1) @0x8001849c-a0 */
                    f->drift_z = 0x15;             /* ori 0x15 / sh 20(v1) @0x800184a4-a8 = 21 */
                    f->accel_x = -2;               /* addiu v0,zero,-2 @0x800184b0 (Delay von
                                                    * j @0x800184ac) -> sh v0,8(v1) @0x800184dc */
                } else if (a & 0x4000) {           /* MITTE: andi 0x4000 @0x800184b4 */
                    f->drift_x = 0x118;            /* ori 0x118 @0x800184bc = 280 */
                    f->drift_y = -50;              /* addiu -50 @0x800184c4 */
                    f->drift_z = 0x18;             /* ori 0x18 @0x800184cc = 24 */
                    f->accel_x = -1;               /* addiu v0,zero,-1 @0x800184d4 -> @0x800184dc */
                } else if (a & 0x2000) {           /* TIEF: andi 0x2000 @0x80018510 */
                    f->drift_x = 0x50;             /* ori 0x50 / sh 16(v1) @0x80018518-1c = 80 */
                    f->drift_z = 0x1;              /* ori 0x1 / sh 20(v1) @0x80018520-24 */
                    f->accel_x = -1;               /* addiu -1 / sh 8(v1) @0x80018528-2c */
                    f->drift_y = 0;                /* sh zero,18(v1) @0x80018534 */
                    row_set16(f, 0x26, 5);         /* ori 0x5 / sh v0,38(v1) @0x80018530/38 */
                }
                if (a & 0xC000) {
                    /* HOCH und MITTE: Abprall-Zaehler +0x26 = RNG(a) % 4 + 7 (@0x800184d8-0x8001850c).
                     * Das "RNG" FUN_8001af20 wertet NUR das Register a0 = a aus (@0x8001af30-4c:
                     * srl v1,a0,7 / andi v1,0xff / addu a0,a0,v1 / andi a0,0xff / Rueckgabe
                     * andi v0,a0,0xff); der geladene Zustand (lhu t1 @0x8001af28) wird nie gelesen.
                     * -> deterministisch aus dem Wort 0x800acaec (gesund 7, Gift-Bit 0x2 -> 9).
                     * Den Zustands-Schreiber `sw a0,0(v0)` @0x8001af48 (0x800ac774) fuehrt der
                     * Port nicht (re15_engine_rand8 ist ein Entropie-Ersatz, re15_damage.c:60-79)
                     * und zieht deshalb KEINEN Port-Zufallswert. %4 = C-Rest auf v >= 0
                     * (bgez @0x800184ec) = & 3; +7 `addiu v0,v0,7` @0x80018504, `sh v0,38(a0)`
                     * @0x8001850c. */
                    uint32_t v1 = ((uint32_t)a >> 7) & 0xffu;
                    uint32_t r  = ((uint32_t)a + v1) & 0xffu;
                    row_set16(f, 0x26, (uint16_t)((r & 3u) + 7u));
                }
            }
            break;
        }
        case 31: {  /* ROUTINE 31 @0x8001854c-6dc — ZUENDER und EXPLOSION (Routine A ab dem Liegen).
                     * Selbst disassembliert (re15_disasm.py dis 0x8001854c 104):
                     *   80018560 lhu v1,30(a1)          Zuender +0x1e
                     *   80018568 beq v1,zero,0x80018688 == 0 -> Ende (Platz frei, Rauch #2)
                     *   8001856c/70 ori v0,zero,0x7 / bne v1,v0,0x800185f4  != 7 -> weiter
                     *   -- Zuender 7: Latch @0x8001857c, Flags 0x61 @0x80018584, P, Resolver
                     *      @0x800185b8, Kind 0x03195000 @0x800185dc, SE 0x04080001 @0x800185ec
                     *   80018600-08 lhu v1,30(a1) / ori v0,zero,0x2 / bne  -- Zuender 2: Kinder
                     *      0x03195000 @0x80018640 + 0x030B5400 @0x80018660
                     *   8001867c-84 addiu v0,v0,-1 / sh v0,30(v1)   Zuender -= 1
                     * Art 2 (0x09) = RE1.5 byte-true. Art 3/4 (0x0A/0x0B) = E8: Resolver-Art 3/4,
                     * Flags 0x61, P wie oben, dann Aufschlag-Uebergabe an die RE2-FX-Maschine;
                     * KEINE HE-Inhalte (Kinder, SE 0x04080001, Licht-Latch) — Routine 31 liest weder
                     * +0x70/+0x71/+0x72 (Saeure-GP §6); die Zuender-Zeitstruktur bleibt. */
            uint16_t z   = row_u16(f->row, 0x1e);
            /* Original: a2 = 2 FEST (`ori a2,zero,0x2` @0x800185b4) fuer JEDEN Platz, der Routine 31
             * faehrt; granate_art 3/4 ist die Port-Zuordnung E3. Ein Platz ohne Art (0) ist damit
             * ein Original-Platz = Art 2 (HE-Inhalte). */
            uint8_t  art = f->granate_art ? f->granate_art : 2;
            int32_t  p[3];
            FILE    *gl  = esp_granate_log();
            if (z == 0) {
                /* @0x80018688-cc: P, dann `sb zero,108(a1)` @0x800186b0 (Platz FREI) VOR dem Kind
                 * 0x030B5800 (`lui a0,0x30b` @0x80018698 / `ori a0,a0,0x5800` @0x800186a8, `jal
                 * 0x800199d4` @0x800186c8) — das Kind darf den Granatenplatz selbst belegen
                 * (Wurf-GP §D). a1 = `lh a1,46(v0)` @0x800186c4 liest die Gier aus dem schon
                 * freien Platz (+0x2e bleibt stehen) -> Gier/Bank VOR dem Freigeben sichern.
                 * Kein Abzug: der Sprung @0x80018568 umgeht @0x80018668-84. */
                const re15_esp_t *kb = f->bank;
                int16_t gier = f->param;
                esp_granate_p(f, p);
                esp_fx_kill(f);
                if (gl) fprintf(gl, "T=%u EV frei art=%u\n", s_gr_tick, (unsigned)art);
                if (art == 2) esp_fx_spawn_kind(kb, 0x030B5800u, gier, p);
                break;
            }
            if (z == 7) {
                if (art == 2) {
                    g_re15_licht_latch = 1;      /* ori v0,zero,0x1 (Delay @0x80018574) /
                                                  * sb v0,21336(at) @0x8001857c */
                    if (gl) fprintf(gl, "T=%u EV latch\n", s_gr_tick);
                }
                f->flags = 0x61;                 /* ori v0,zero,0x61 / sb v0,108(a1) @0x80018580-84:
                                                  * aktiv|Physik-Stopp|Bild-Stopp, OHNE Bit 1 = unsichtbar */
                esp_granate_p(f, p);
                {
                    /* FUN_80012d60(a0 = 500, a1 = &P, a2 = Art): `ori a0,zero,0x1f4` @0x80018598,
                     * `addiu a1,sp,16` @0x800185a4, `ori a2,zero,0x2` @0x800185b4 (Art 3/4 =
                     * Port-Zuordnung E3: DAT_8006f418[3]/[4] = 1000 @0x8006f41e/20, DAT_8006f430[3]/[4]
                     * = 10/11 @0x8006f433/34, ohne Aufrufer im Original), `jal 0x80012d60` @0x800185b8.
                     * Kein Gegner ausgeschlossen: Gate A vergleicht Platz+0x74 (Anker = Spieler-
                     * Knochen) mit Gegner+0x188+0x40 (@0x80012f38-4c) -> attacker -1. */
                    re15_attack_box_t box;
                    box.x = p[0]; box.y = p[1]; box.z = p[2];
                    box.radius = 500;
                    int treffer = re15_resolve_attack(&box, art, -1);
                    s_granate_resolver_calls++;
                    if (gl) fprintf(gl, "T=%u EV resolver art=%u P=(%d,%d,%d) r=500 eingriffe=%d\n",
                                    s_gr_tick, (unsigned)art, (int)p[0], (int)p[1], (int)p[2], treffer);
                }
                if (art == 2) {
                    /* Kind 0x03195000 an P (`lui a0,0x319` / `ori a0,a0,0x5000` @0x800185c0-c4;
                     * a2 = Einheitsmatrix 0x80072d4c @0x800185d0-d4; a1 = `lh a1,46(v0)` @0x800185d8;
                     * a3 = &P @0x800185e0; `jal 0x800199d4` @0x800185dc), dann SE 0x04080001 an P
                     * (`lui a0,0x408` / `ori a0,a0,0x1` / `jal 0x80045024` @0x800185e4-ec, a1 = &P). */
                    esp_fx_spawn_kind(f->bank, 0x03195000u, f->param, p);
                    if (gl) fprintf(gl, "T=%u EV se code=%08x pos=(%d,%d,%d)\n", s_gr_tick,
                                    0x04080001u, (int)p[0], (int)p[1], (int)p[2]);
                    if (re15_esp_se_hook) re15_esp_se_hook(0x04080001u, p);
                } else {
                    /* E8: Aufschlag an der Granaten-WELTLAGE Q = slot+0x28/2a/2c (nicht P), Gier =
                     * slot+0x2e. re2_art per expliziter Tabelle (esp_granate_re2_art). */
                    int32_t q[3] = { (int32_t)f->wpos[0], (int32_t)f->wpos[1], (int32_t)f->wpos[2] };
                    int re2_art = esp_granate_re2_art(art);
                    if (gl) fprintf(gl, "T=%u EV aufschlag re2_art=%d q=(%d,%d,%d) gier=%d\n", s_gr_tick,
                                    re2_art, (int)q[0], (int)q[1], (int)q[2], (int)f->param);
                    /* Bezugsebene der RE2-Bodenflammen = Standhoehe des Werfers (granate_boden,
                     * PORT-ZUORDNUNG, re15_esp.h / re2_fx.c s_boden_basis). */
                    { extern void re2fx_boden_basis_setzen(int32_t y);
                      re2fx_boden_basis_setzen(f->granate_boden); }
                    if (re2_art && re15_esp_aufschlag_hook) re15_esp_aufschlag_hook(re2_art, q, f->param);
                }
            }
            if (z == 2 && art == 2) {
                /* Zuender 2 (@0x80018600-64): P neu (@0x80018610-38), Kind 0x03195000 (`lui a0,0x319`
                 * @0x8001860c, `ori a0,a0,0x5000` @0x80018614, jal @0x80018640) und Kind 0x030B5400
                 * (`lui a0,0x30b` / `ori a0,a0,0x5400` @0x80018648-4c, jal @0x80018660), beide mit
                 * a2 = s0 = 0x80072d4c (@0x80018620-34) und a1 = Gier +0x2e. */
                esp_granate_p(f, p);
                esp_fx_spawn_kind(f->bank, 0x03195000u, f->param, p);
                esp_fx_spawn_kind(f->bank, 0x030B5400u, f->param, p);
            }
            row_set16(f, 0x1e, (uint16_t)(z - 1u));   /* addiu v0,v0,-1 @0x8001867c /
                                                       * sh v0,30(v1) @0x80018684 (Delay-Slot) */
            break;
        }
        case 41: {  /* ROUTINE 41 @0x80018ef4 — Integration Runde 34 W7 (bau_c.md N1.1; selbst
                     * disassembliert `re15_disasm.py dis 0x80018ef4 41`). Traeger: Raum-Effekt 0x0b
                     * (Wasserstrahl ROOM2000/2001/20B0/20B1, Zeile 0 jedes der 6 Stroeme):
                     *   80018f04 lbu v1,14(v0) / 80018f0c sb v1,108(v0)   Flags := row[0x0e]
                     *   80018f1c lhu v0,22(v1) / 80018f24 beq -> 80018f40  row[0x16] == 0 ?
                     *   80018f2c-3c lhu / addiu -1 / sh v0,22(v1)          sonst row[0x16]-- , ENDE
                     *   80018f40 lbu v0,30(v1) / 80018f48 sb v0,108(v1)   Flags := row[0x1e]
                     *   80018f58 lbu v0,38(v1) / 80018f60 sb v0,110(v1)   +0x6e := row[0x26] (Delay-
                     *                                                     Slot VOR dem Vorschub)
                     *   80018f5c jal 0x800174e4                            Vorschub (Zeile 1 = R42)
                     *   80018f64 jal 0x8001af20                            "RNG" auf a0 = Rest des
                     *            Kopier-Laufs von FUN_800174e4 (`lw a0,8(a2)` @0x80017594 bzw.
                     *            `lwl/lwr a0,0xb/8(a2)` @0x80017548/4c im 2. 16-Byte-Block) = u32 der
                     *            NEUEN Zeile ab +0x18 (`jr ra` @0x800175d4 ohne weiteren a0-Schreiber);
                     *            FUN_8001af20: v = (a0 + ((a0>>7)&0xff)) & 0xff (@0x8001af30-4c)
                     *   80018f78 lhu v1,10(a0) / 80018f7c andi v0,v0,0x3 / 80018f80 addu /
                     *   80018f84 sh v1,10(a0)                              +0x0a (Beschl. y der
                     *                                                     neuen Zeile) += v & 3
                     * Der Satzindex +0x6e wird OHNE den Zeitgeber +0x6d gesetzt — der Port setzt
                     * frame roh (der Anim-Schritt liest ihn wie das Original @0x8001a398-47c).
                     * KEIN BODEN: der Takt hat keine Klemme (Physik @0x8001a2fc-388 = xlat += vel,
                     * vel += acc); einen Boden kennen nur die B-Routinen (12/29/...), Effekt 0x0b hat
                     * B = 0 -> die Port-Sammelklemme (floor_y = Spawn-y beim SCD-Weg) haelt den
                     * Strahl sonst auf Spawnhoehe fest (Muster A6/E12, ESP_KEIN_BODEN). */
            f->floor_y = ESP_KEIN_BODEN;
            f->flags = f->row[0x0e];
            uint16_t halt = row_u16(f->row, 0x16);
            if (halt != 0) {
                row_set16(f, 0x16, (uint16_t)(halt - 1u));
                break;
            }
            f->flags = f->row[0x1e];
            f->frame = (int16_t)f->row[0x26];
            esp_fx_row_advance(f);
            {
                const uint32_t w = (uint32_t)f->row[0x18] | ((uint32_t)f->row[0x19] << 8) |
                                   ((uint32_t)f->row[0x1a] << 16) | ((uint32_t)f->row[0x1b] << 24);
                const uint32_t v = (w + ((w >> 7) & 0xffu)) & 0xffu;
                const uint16_t ay = (uint16_t)(row_u16(f->row, 0x0a) + (v & 3u));
                row_set16(f, 0x0a, ay);
                f->accel_y = (int16_t)ay;           /* Port fuehrt +0x0a zusaetzlich als accel_y */
            }
            break;
        }
        case 42: {  /* ROUTINE 42 @0x80018f98 — Integration Runde 34 W7 (bau_c.md N1.1; selbst
                     * disassembliert `re15_disasm.py dis 0x80018f98 120`):
                     *   80018fa8-b8 lhu 14 / lhu 38 / sltu / beq -> 80019068   row[0x0e] < row[0x26] ?
                     *   80018fc0-cc lhu / addiu 1 / sh v0,14(a0)              row[0x0e]++
                     *   80018fc4 lbu v1,112(a0) / 80018fd0 ori 0xb / 80018fd4 bne -> Ende
                     *                                                         nur Effekt-Id 0x0b:
                     *   80018fdc-80019000 P = (lh +0x28, lh +0x2a, lh +0x2c) (s32 auf dem Stapel),
                     *   80018ffc jal 0x8002b7e8 mit a1 = 0x2d (Delay-Slot @0x80018fd8)
                     *   80019004-20 Treffer -> Flags |= 0x20 (Physik-Stopp); 80019024-30 sonst Flags := 0x13
                     *   80019040-4c lhu 14 / sltiu 0x10 / bne -> Ende           row[0x0e] >= 16:
                     *   80019054-64 lhu v0,6(v1) / addiu v0,v0,768 / sh v0,6(v1)   defH += 768
                     *   80019068-8001911c SCHLEIFE: 40 Byte Zeile 0 aus [+0x80] (Strom-Anfang)
                     *   8001912c sb zero,111(v0)                              Cursor +0x6f := 0
                     *   80019138 sh zero,22(v0)                               row[0x16] := 0 (kein Halten)
                     *   8001913c-44 sw zero,52/56/60(v0)                      xlat := 0
                     *   80019148-4c ori v0,zero,0x1 / sb v0,110(v1)           +0x6e := 1
                     *   8001915c-74 lbu 110 / lw 120 / sll 3 / addu / lbu v0,2(v0) / sb v0,109(v1)
                     *                                                         +0x6d := Satz[1].Byte2 */
            uint16_t zaehler = row_u16(f->row, 0x0e);
            if (zaehler < row_u16(f->row, 0x26)) {
                zaehler = (uint16_t)(zaehler + 1u);
                row_set16(f, 0x0e, zaehler);
                if (f->effect_id != 0x0b) break;
                const int32_t p[3] = { f->wpos[0], f->wpos[1], f->wpos[2] };
                if (esp_treffer_test(p, 0x2d)) f->flags |= 0x20;
                else                           f->flags = 0x13;
                if (zaehler >= 0x10u) row_set16(f, 0x06, (uint16_t)(row_u16(f->row, 0x06) + 768u));
                break;
            }
            /* SCHLEIFE: Zeile 0 neu (esp_fx_row_load laedt auch die Port-Felder accel/drift aus
             * +0x08..+0x14 — im Original liegen sie IN der kopierten Zeile). */
            esp_fx_row_load(f, 0);
            f->row_cursor = 0;
            row_set16(f, 0x16, 0);
            f->xlat_x = 0; f->xlat_y = 0; f->xlat_z = 0;
            f->frame = 1;
            {
                re15_esp_anim_t a;
                f->timer = (f->eff_idx >= 0 && re15_esp_anim(f->bank, f->eff_idx, 1, &a) == 0)
                           ? (int16_t)(a.param & 0xff) : 0;   /* Satz[1].Byte2 (Dauer) */
            }
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
static void esp_fx_dispatch_b_29(re15_esp_fx_t *f);   /* Runde 34 A4, unten */
static void esp_fx_dispatch_b(re15_esp_fx_t *f)
{
    if (!f->rows_base) return;
    if (re15_esp_brocken_b(f)) return;   /* Runde 35 Spur D: B 36/37 @0x800187c4/0x8001885c */
    if (row_u16(f->row, 0x02) == 29) { esp_fx_dispatch_b_29(f); return; }
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

/* ROUTINE 29 @0x80018320-434 — FLUG und ABPRALL der Granate (Routine B, je Bild im Hauptlauf
 * NACH der Weltlage). Selbst disassembliert (re15_disasm.py dis 0x80018320 72):
 *   80018330 lh   t1,42(t0)          t1 = Welt-y (slot+0x2a, in DIESEM Bild gerechnet)
 *   80018338 blez t1,0x8001842c      Welt-y <= 0 -> in der Luft, nichts
 *   80018340 lhu  v0,38(t0)          Zaehler +0x26
 *   80018348 bne  v0,zero,0x80018388 != 0 -> Abprall
 * -- Zaehler 0 = LIEGEN: SE 0x010A0001 (`lui a0,0x10a` / `ori a0,a0,0x1` / `jal 0x80045024`
 *    @0x80018350-58, a1 = sp+16 UNBESCHRIEBEN), Flags := 0x63 (@0x80018368-6c), A := 31
 *    (@0x80018378-7c), B := 0 (`sh zero,2(v1)` @0x80018384); KEINE y-Korrektur.
 * -- Abprall: vx -= trunc(vx/3) (0x55555556-Idiom @0x8001834c-cc, `sh a3,16(t0)`),
 *    Zaehler -= 1 (@0x800183d0-d4), xlat_y -= Welt-y (`lw v0,56(t0)` / `subu v0,v0,t1` /
 *    `sw v0,56(t0)` @0x800183c4/dc/e0), vy := -trunc(vy/3) (@0x800183e4-f8), SE
 *    0x010A0001 | (Zaehler_neu << 8) an der Eindringstelle (`lh` 40/42/44 @0x800183fc-414,
 *    `sll a0,a0,8` / `or` @0x80018420/28, `jal 0x80045024` @0x80018424). vz wird NIE gedaempft. */
static void esp_fx_dispatch_b_29(re15_esp_fx_t *f)
{
    /* t1 = Eindringtiefe unter die Bezugsebene. Original: Ebene y 0 (`lh t1,42(t0)` / `blez t1`
     * @0x80018330-38). granate_boden (re15_esp.h) = Standhoehe des Werfers, PORT-ZUORDNUNG fuer
     * Raeume mit Boden != 0; bei Boden 0 (granate_boden == 0) byte-gleich. */
    int32_t t1 = (int32_t)f->wpos[1] - f->granate_boden;
    if (t1 <= 0) return;
    FILE *gl = esp_granate_log();
    uint16_t n = row_u16(f->row, 0x26);
    if (n == 0) {
        /* E14 (Port-Wahl, gekennzeichnet): die Lage dieses SEs ist im Original Stapelrest
         * (sp+16..27 im Liegen-Pfad unbeschrieben, Wurf-GP §C); der Port gibt die Granatenlage. */
        int32_t pos[3] = { (int32_t)f->wpos[0], (int32_t)f->wpos[1], (int32_t)f->wpos[2] };
        if (gl) fprintf(gl, "T=%u EV se code=%08x pos=(%d,%d,%d) liegen\n", s_gr_tick,
                        0x010A0001u, (int)pos[0], (int)pos[1], (int)pos[2]);
        if (re15_esp_se_hook) re15_esp_se_hook(0x010A0001u, pos);
        f->flags = 0x63;
        row_set16(f, 0x00, 31);
        row_set16(f, 0x02, 0);
        return;
    }
    {
        int16_t vx = f->drift_x;
        f->drift_x = (int16_t)(vx - (int16_t)(vx / 3));      /* C-Division = auf 0 gerundet */
    }
    n = (uint16_t)(n - 1u);
    row_set16(f, 0x26, n);
    f->xlat_y -= (int32_t)t1;
    {
        int16_t vy = f->drift_y;
        f->drift_y = (int16_t)(-(int16_t)(vy / 3));
    }
    {
        /* Punkt: `lh v1,40(t0)` @0x800183d8, `lh v0,42(t0)` @0x80018400, `lh v0,44(t0)`
         * @0x8001840c — slot+0x2a ist noch die Eindring-Welt-y (erst der naechste Tick rechnet neu). */
        int32_t pos[3] = { (int32_t)f->wpos[0], (int32_t)f->wpos[1], (int32_t)f->wpos[2] };
        uint32_t code = 0x010A0001u | ((uint32_t)n << 8);   /* Byte1 wirkungslos (FUN_80045024
                                                             * liest es nie, Wurf-GP §I) */
        if (gl) fprintf(gl, "T=%u EV se code=%08x pos=(%d,%d,%d) abprall\n", s_gr_tick,
                        code, (int)pos[0], (int)pos[1], (int)pos[2]);
        if (re15_esp_se_hook) re15_esp_se_hook(code, pos);
    }
}

/* Runde 34 VERTRAG V1 (C0): nur die Definitionen — Belege je Symbol an der Deklaration in
 * include/re15_esp.h. In C0 setzt/liest/ruft sie niemand (keine Verhaltensaenderung).
 *   g_re15_licht_latch       = 0x800b5358 (Setzer @0x8001857c/@0x80017694, Leser @0x8001ce60,
 *                              Loeschung @0x8001d1b4)
 *   re15_esp_se_hook         = FUN_80045024-Analogon (Rufer @0x80018424/@0x80018358/@0x800185ec)
 *   re15_esp_aufschlag_hook  = Uebergabe an die RE2-FX-Maschine (E8; RE2-Art-Byte @0x8001f1a8-b8) */
uint8_t g_re15_licht_latch = 0;
void (*re15_esp_se_hook)(uint32_t code, const int32_t pos[3]) = NULL;
void (*re15_esp_aufschlag_hook)(int re2_art, const int32_t q[3], int16_t gier) = NULL;

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
/* Gemeinsamer Kern der Zeilen-Spawner (Runde 34: aus re15_esp_fx_spawn_rows herausgezogen,
 * Verhalten fuer spawn_rows unveraendert). flags0 = Start-Flags des Platzes:
 *   0x03 = FUN_80019700 (`ori v0,zero,0x3` @0x800197b4)
 *   0x0a = FUN_800199d4 (`ori v0,zero,0xa` @0x80019a88, einziger Unterschied beider Spawner,
 *          Wurf-GP §G) -> Bit 3: der Hauptlauf desselben Bilds macht Flags ^= 9 und ruft
 *          Routine A einmal (@0x80019ef4-f30).
 * *first (optional) = der erste gespawnte Platz. */
static int esp_fx_spawn_rows_core(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub,
                                  uint16_t scale16, int32_t x, int32_t y, int32_t z,
                                  int32_t floor_y, int16_t param, uint8_t flags0,
                                  re15_esp_fx_t **first, int *out_streams)
{
    const re15_esp_t *rb = bank;
    int ei = re15_esp_find_id(rb, effect_id);
    if (ei < 0) { rb = re15_esp_global_bank(); ei = re15_esp_find_id(rb, effect_id); }
    int streams = (ei >= 0) ? re15_esp_row_streams(rb, ei, sub) : -1;
    int spawned = 0;
    if (first) *first = NULL;
    if (out_streams) *out_streams = streams;
    for (int s = 0; s < streams; s++) {
        int nrows = 0;
        const uint8_t *rows = re15_esp_row_stream(rb, ei, sub, s, &nrows);
        if (!rows || nrows <= 0) continue;
        re15_esp_fx_t *f = re15_esp_fx_spawn_ex(bank, effect_id, sub, scale16, x, y, z, param);
        if (!f) break;
        f->phys = 1; f->flags = flags0;
        esp_fx_seed_header(f, rb, ei, sub);   /* CLUT/TPAGE seed (FUN_80019700) */
        f->rows_base = rows; f->row_count = (uint8_t)(nrows > 255 ? 255 : nrows);
        f->row_cursor = 0;
        esp_fx_row_load(f, 0);
        f->xlat_x = f->xlat_y = f->xlat_z = 0;
        f->floor_y = floor_y;
        if (first && !*first) *first = f;
        spawned++;
    }
    return spawned;
}

/* Zeilen-Spawn mit Mess-Log und waehlbaren Start-Flags (Runde 34 A NACHBESSERUNG M-3):
 *   flags0 0x03 = FUN_80019700 (`ori v0,zero,0x3` @0x800197b4) — Aufrufer ausserhalb des
 *                 ESP-Ticks (jal-Scan PSX.EXE: 0x8002c74c-0x8002c8fc, Gun-FSM 0x800336ec-
 *                 0x80033e88, 0x800348b0-0x80034bdc, 0x80038794/bc, 0x80041954, 0x80045710)
 *   flags0 0x0a = FUN_800199d4 (`ori v0,zero,0xa` @0x80019a88) — ALLE Kind-Spawns der ESP-
 *                 Routinen (jal @0x800172f8 R2, @0x80017634 R8, @0x80017b38 R15, @0x80017da0,
 *                 @0x80018054, @0x800185dc/640/660/6c8 R31, @0x800189c4-0x80018c68,
 *                 @0x800191e0). */
static int esp_fx_spawn_rows_flags(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub,
                                   uint16_t scale16, int32_t x, int32_t y, int32_t z,
                                   int32_t floor_y, int16_t param, uint8_t flags0)
{
    const re15_esp_t *rb = bank;
    int ei = re15_esp_find_id(rb, effect_id);
    if (ei < 0) { rb = re15_esp_global_bank(); ei = re15_esp_find_id(rb, effect_id); }
    int streams = (ei >= 0) ? re15_esp_row_streams(rb, ei, sub) : -1;
    {   /* Mess-Log (Debug-Harness) */
        extern FILE *re15_waffen_log(void);
        FILE *wl = re15_waffen_log();
        if (wl) fprintf(wl, "    SPAWN id=%u sub=%u scale=%#x streams=%d\n",
                        (unsigned)effect_id, (unsigned)sub, (unsigned)scale16, streams);
    }
    return esp_fx_spawn_rows_core(bank, effect_id, sub, scale16, x, y, z, floor_y, param,
                                  flags0, NULL, NULL);
}

int re15_esp_fx_spawn_rows(const re15_esp_t *bank, uint8_t effect_id, uint8_t sub,
                           uint16_t scale16, int32_t x, int32_t y, int32_t z, int32_t floor_y,
                           int16_t param)
{
    /* oeffentlicher Weg = FUN_80019700 (Start-Flags 0x03 @0x800197b4): Waffen-FSM, SCD-Op 0x3A,
     * Gegner-Blut. Kinder der ESP-Routinen gehen ueber esp_fx_spawn_rows_flags(.., 0x0a). */
    return esp_fx_spawn_rows_flags(bank, effect_id, sub, scale16, x, y, z, floor_y, param, 0x03);
}

/* FUN_800199d4-ZWILLING (Runde 34 A5): Kind-Effekt a0 = (Kategorie<<24)|(sub<<16)|Skala
 * (Decode `srl t8,a0,24` @0x800199fc, `andi t7,v0,0xff` @0x80019a04, `andi s1,a0,0xffff`
 * @0x800199e8), a1 = Gier -> +0x2e (`sh s2,46(t0)` @0x80019ab8), a2 = Einheitsmatrix 0x80072d4c
 * (T = 0), a3 = &P -> Versatz +0x40.. (@0x80019abc-dc) => Anker = P. Erster freier Platz ab 0
 * (`sltiu v0,t3,0x60` @0x80019a60, `lbu v0,108(t0)` / `beq` @0x80019a7c-84), Start-Flags 0x0a
 * (@0x80019a88 / `sb v0,108(t0)` @0x80019aa4). Kein Boden (Original-Tick ohne Klemme). */
static int esp_fx_spawn_kind(const re15_esp_t *bank, uint32_t code, int16_t gier,
                             const int32_t p[3])
{
    uint8_t  cat   = (uint8_t)(code >> 24);
    uint8_t  sub   = (uint8_t)((code >> 16) & 0xffu);
    uint16_t scale = (uint16_t)(code & 0xffffu);
    re15_esp_fx_t *k = NULL;
    int n = esp_fx_spawn_rows_core(bank, cat, sub, scale, p[0], p[1], p[2], ESP_KEIN_BODEN,
                                   gier, 0x0a, &k, NULL);
    FILE *gl = esp_granate_log();
    if (gl) fprintf(gl, "T=%u EV kind code=%08x n=%d slot=%d P=(%d,%d,%d) gier=%d\n", s_gr_tick,
                    code, n, k ? (int)(k - s_esp_fx) : -1, (int)p[0], (int)p[1], (int)p[2], (int)gier);
    return n;
}

/* FUN_80019700-ZWILLING fuer den Wurfkoerper 0x040D1000 (Runde 34 A8, Deklaration mit Belegen
 * in re15_esp.h). Effekt 4 sub 0x0D hat 1 Strom mit 2 Zeilen (CORE00.ESP @0x1AB0 `01 00 00 00
 * 02 00 00 00`, Zeile 0 @0x1AB8 = A 30, acc (0,10,0)) -> genau ein Platz. */
re15_esp_fx_t *re15_esp_granate_spawn(const re15_esp_t *bank, uint8_t art,
                                      int32_t x, int32_t y, int32_t z, int16_t gier)
{
    re15_esp_fx_t *g = NULL;
    int streams = 0;
    int n = esp_fx_spawn_rows_core(bank, 0x04, 0x0d, 0x1000, x, y, z, ESP_KEIN_BODEN, gier,
                                   0x03, &g, &streams);
    {   /* Mess-Log wie re15_esp_fx_spawn_rows (Debug-Harness, gleiches Zeilenformat) */
        extern FILE *re15_waffen_log(void);
        FILE *wl = re15_waffen_log();
        if (wl) fprintf(wl, "    SPAWN id=%u sub=%u scale=%#x streams=%d\n",
                        4u, 0x0du, 0x1000u, streams);
    }
    if (n > 0 && g) g->granate_art = art;
    {
        FILE *gl = esp_granate_log();
        if (gl) fprintf(gl, "F=%u SPAWN granate art=%u slot=%d anker=(%d,%d,%d) gier=%d%s\n",
                        (unsigned)g_engine.frame_count, (unsigned)art,
                        g ? (int)(g - s_esp_fx) : -1, (int)x, (int)y, (int)z, (int)gier,
                        g ? "" : " POOL-VOLL");
    }
    return (n > 0) ? g : NULL;
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

/* ===== Runde 34 A2 — WELTLAGE slot+0x28/2a/2c (V1a) =========================================
 *
 * RotMatrix-Zwilling FUN_80068098 (re15_disasm.py dis 0x80068098 110, Tabelle 0x800794c4 =
 * re15_sin_q12/re15_cos_q12, je Eintrag lo16 = sin, hi16 = cos). Winkel `lh` (s16):
 *   a >= 0: Index a & 0xfff (`bgez t7` + Delay `andi t9,t7,0xfff` @0x800680a0-a4)
 *   a <  0: Index (-a) & 0xfff, sin NEGIERT, cos wie gelesen (`subu t7,zero,t7` @0x800680a8,
 *           `subu t3,zero,t8` @0x800680d0). Die Tabelle ist NICHT punktsymmetrisch
 *           (gemessen: tab[4095].sin = 0, tab[4094].sin = -6 = -tab[1].sin), deshalb ist der
 *           Negativ-Zweig NICHT gleich "a & 0xfff" — eigener Zwilling statt mat3_from_euler.
 * Eintraege (sh, 16 Bit):
 *   m[0][2] = sy                         `sh t6,4(a1)`  @0x8006816c
 *   m[1][2] = (-(cy*sx)) >> 12           `sh t6,10(a1)` @0x80068180
 *   m[2][2] = (cy*cx) >> 12              `sh t6,16(a1)` @0x80068194 / @0x800681d4
 *   m[0][0] = (cz*cy) >> 12              `sh t6,0(a1)`  @0x8006820c
 *   m[0][1] = (-(sz*cy)) >> 12           `sh t7,2(a1)`  @0x8006822c
 *   A = (cz*(-sy)) >> 12;  B = (sz*(-sy)) >> 12        @0x80068228-38 / @0x800682a0-b0
 *   m[1][0] = ((sz*cx)>>12) - ((A*sx)>>12)   `sh t7,6(a1)`  @0x80068274
 *   m[2][0] = ((sz*sx)>>12) + ((A*cx)>>12)   `sh t6,12(a1)` @0x800682a4
 *   m[1][1] = ((cz*cx)>>12) + ((B*sx)>>12)   `sh t7,8(a1)`  @0x800682ec
 *   m[2][1] = ((cz*sx)>>12) - ((B*cx)>>12)   `sh t6,14(a1)` @0x80068318 */
static void esp_trig(int16_t a, int32_t *s, int32_t *c)
{
    if (a >= 0) {
        *s = re15_sin_q12(a & 0xfff);
        *c = re15_cos_q12(a & 0xfff);
    } else {
        int n = (-(int)a) & 0xfff;
        *s = -re15_sin_q12(n);
        *c = re15_cos_q12(n);
    }
}

static void esp_rotmatrix(int16_t rx, int16_t ry, int16_t rz, int16_t m[9])
{
    int32_t sx, cx, sy, cy, sz, cz;
    esp_trig(rx, &sx, &cx);
    esp_trig(ry, &sy, &cy);
    esp_trig(rz, &sz, &cz);
    int32_t nsy = -sy;
    int32_t A = (cz * nsy) >> 12;
    int32_t B = (sz * nsy) >> 12;
    m[2] = (int16_t)sy;
    m[5] = (int16_t)((-(cy * sx)) >> 12);
    m[8] = (int16_t)((cy * cx) >> 12);
    m[0] = (int16_t)((cz * cy) >> 12);
    m[1] = (int16_t)((-(sz * cy)) >> 12);
    m[3] = (int16_t)(((sz * cx) >> 12) - ((A * sx) >> 12));
    m[6] = (int16_t)(((sz * sx) >> 12) + ((A * cx) >> 12));
    m[4] = (int16_t)(((cz * cx) >> 12) + ((B * sx) >> 12));
    m[7] = (int16_t)(((cz * sx) >> 12) - ((B * cx) >> 12));
}

/* ApplyMatrix-Zwilling FUN_800661c0 (re15_disasm.py bytes 0x800661c0 160): ctc2 RT11..RT33
 * (0x48c80000..0x48cc2000), lwc2 VXY0/VZ0 (0xc8a00000/0xc8a10004 = SVECTOR, s16), MVMVA
 * 0x4a486012 (sf=1, mx=RT, v=V0, cv=keine), swc2 MAC1..3 (0xe8d90000..0xe8db0008) = 32-Bit-
 * Ergebnis (Summe >> 12, KEINE Saettigung). */
static void esp_applymatrix(const int16_t m[9], const int16_t v[3], int32_t r[3])
{
    for (int i = 0; i < 3; i++) {
        int64_t s = (int64_t)m[i * 3 + 0] * v[0] + (int64_t)m[i * 3 + 1] * v[1]
                  + (int64_t)m[i * 3 + 2] * v[2];
        r[i] = (int32_t)(s >> 12);
    }
}

/* Weltlage je Tick (Hauptlauf, vor Routine B). Selbst disassembliert (dis 0x80019e20 420):
 *  Flags & 0x80 == 0 (@0x80019fb4-c0 -> @0x8001a118):
 *    w = (euler.x, euler.y + GIER, euler.z): `lhu v0,32(a1)` / `lhu v0,34(a1)` + `lhu v1,46(a1)`
 *        / `addu` @0x8001a16c-84 / `lhu v0,36(a1)` @0x8001a190; RotMatrix `jal 0x80068098` @0x8001a1a0
 *    r  = ApplyMatrix(m, xlat lo16 (`lhu` 52/56/60 @0x8001a1b8-d4))  `jal 0x800661c0` @0x8001a1e4
 *    +0x28/2a/2c := r (`sh v0,40/42/44(a0)` @0x8001a1fc/10/20)
 *    r2 = ApplyMatrix(Anker.R (slot+0x4c), Versatz +0x40/44/48)       `jal 0x800661c0` @0x8001a248
 *    +0x28 += r2.x + Anker.T.x (+0x60) usw. (16-Bit-`addu`/`sh` @0x8001a258-2a4)
 *  => wpos = (s16)(Anker.R*Versatz + Anker.T + RotMatrix(euler + (0,Gier,0)) * xlat).
 *  Der Port fuehrt Anker.R*Versatz + Anker.T als x/y/z (beim Spawn eingerechnet; Follow
 *  ueberschreibt sie je Tick, s. (c)).
 *  Flags & 0x80 != 0 (@0x80019fc8-0x8001a114): Welt = Anker.R*(RotMatrix(euler)*xlat + Versatz)
 *    + Anker.T (RotMatrix OHNE Gier @0x8001a020, xlat dann im Ankerraum). Der Port hat keine
 *    Anker-Matrix (Muendung: Waffenknochen) -> dort bleibt die bisherige Lage x + xlat
 *    (BAUPLAN A2 / OFFEN O4, Zweig jetzt gelesen, Matrix fehlt). */
static void esp_fx_weltlage(re15_esp_fx_t *f)
{
    if (f->flags & 0x80) {
        f->wpos[0] = (int16_t)(f->x + f->xlat_x);
        f->wpos[1] = (int16_t)(f->y + f->xlat_y);
        f->wpos[2] = (int16_t)(f->z + f->xlat_z);
        return;
    }
    int16_t m[9];
    esp_rotmatrix((int16_t)row_u16(f->row, 0x20),
                  (int16_t)(uint16_t)(row_u16(f->row, 0x22) + (uint16_t)f->param),
                  (int16_t)row_u16(f->row, 0x24), m);
    int16_t v[3] = { (int16_t)f->xlat_x, (int16_t)f->xlat_y, (int16_t)f->xlat_z };
    int32_t r[3];
    esp_applymatrix(m, v, r);
    f->wpos[0] = (int16_t)(r[0] + f->x);
    f->wpos[1] = (int16_t)(r[1] + f->y);
    f->wpos[2] = (int16_t)(r[2] + f->z);
}

/* RE15_GRANATE_LOG: Zustandszeile je Granatenplatz (Diagnose). */
static void esp_granate_log_tick(void)
{
    FILE *gl = esp_granate_log();
    if (!gl) return;
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = &s_esp_fx[i];
        if (!f->active || !f->granate_art) continue;
        fprintf(gl, "T=%u F=%u slot=%d art=%u A=%u B=%u fl=%02x zuender=%u zaehler=%u "
                    "wpos=(%d,%d,%d) xlat=(%d,%d,%d) vel=(%d,%d,%d) acc=(%d,%d,%d) satz=%d gier=%d\n",
                s_gr_tick, (unsigned)g_engine.frame_count, i, (unsigned)f->granate_art,
                (unsigned)row_u16(f->row, 0x00), (unsigned)row_u16(f->row, 0x02),
                (unsigned)f->flags, (unsigned)row_u16(f->row, 0x1e), (unsigned)row_u16(f->row, 0x26),
                (int)f->wpos[0], (int)f->wpos[1], (int)f->wpos[2],
                (int)f->xlat_x, (int)f->xlat_y, (int)f->xlat_z,
                (int)f->drift_x, (int)f->drift_y, (int)f->drift_z,
                (int)f->accel_x, (int)f->accel_y, (int)f->accel_z,
                (int)f->frame, (int)f->param);
    }
    fflush(gl);
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
    s_gr_tick++;
    /* Byte-true FUN_80019e20 frame timer (L117-131): when the per-slot timer hits 0, advance
     * the anim-record index; the new record's param-low byte = its duration, 0xFF = loop back
     * to the record's desc-low byte, 0/0 (duration & loop-target both 0) = end -> despawn.
     * Each fx animates from ITS OWN resolved bank (room or global), set at spawn. */
    (void)bank;   /* per-fx bank now (f->bank); kept for call-site compat */

    /* ===== Runde 34 A2: ZWEI DURCHGAENGE wie das Original (re15_disasm.py dis 0x80019e20 420):
     * DURCHGANG 1 = Schleife 1 @0x80019e64-c4 ueber ALLE 96 Plaetze (s0 = 0x800a73b8, s2 = s0+12672):
     *   80019e70 lbu v0,108(v1) / 80019e78 andi v0,v0,0x1 / 80019e7c beq -> Flags-Bit 0 aus: kein A
     *   80019e84 lhu v0,0(v1) / sll 2 / Tabelle 0x80071d40 / 80019e9c jalr v0   (Routine A)
     * Bis Runde 33 liefen A und B je Platz verschraenkt (A_i, B_i, Physik_i, dann Platz i+1).
     * KIND-SPAWNS aus Routinen (Nachbesserung M-3, korrigiert): im Original gehen ALLE ueber
     * FUN_800199d4 (jal-Scan PSX.EXE + STAGE1..6: nur @0x800172f8/0x80017634 R8/0x80017b38 R15/
     * 0x80017da0/0x80018054/0x800185dc-0x800186c8 R31/0x800189c4-0x80018c68/0x800191e0) mit
     * Start-Flags 0x0a (`ori v0,zero,0xa` @0x80019a88): so ein Kind wird hier NIE dispatcht (Bit 0
     * frei) und bekommt Routine A genau einmal in der Kind-Init von Durchgang 2 — egal, ob es
     * unter oder ueber dem Eltern-Index landet. FUN_80019700 (Flags 3) hat im ESP-Tick keinen
     * Aufrufer; einen "Flags-3-Kind ohne A"-Fall gibt es im Original nicht (Port R8/R15 seit der
     * Nachbesserung ebenfalls 0x0a). */
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        re15_esp_fx_t *f = &s_esp_fx[i];
        if (!f->active || !f->rows_base) continue;   /* Altplaetze ohne Zeilen-VM: kein A */
        if (!(f->flags & 0x01)) continue;            /* @0x80019e78-7c (Bit 0 = aktiv); ein Kind
                                                      * mit Flags 0x0a wartet auf Durchgang 2 */
        esp_fx_dispatch(f);                          /* loop-1 routineA */
    }

    /* DURCHGANG 2 = Hauptlauf @0x80019ee0-0x8001a49c je Platz: (a) Kind-Init, (b) Lebend-Gate,
     * (c) Follow, (d) Weltlage, (e) Routine B, (f) Physik, (g) Anim. */
    for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        re15_esp_fx_t *f = &s_esp_fx[i];
        if (!f->active) continue;

        if (f->rows_base) {
            /* (a) KIND-INIT @0x80019ef4-f30: `andi v0,v1,0x8` / `beq` / `xori v0,v1,0x9` (Delay)
             * / `sb v0,108(a0)` @0x80019f00, dann Routine A EINMAL (`jalr v0` @0x80019f30). Traeger:
             * Plaetze aus FUN_800199d4 (Start-Flags 0x0a -> 0x03). Laeuft VOR dem Lebend-Gate —
             * 0x0a hat Bit 0 noch nicht. */
            if (f->flags & 0x08) {
                f->flags ^= 0x09;
                esp_fx_dispatch(f);
                if (!f->active) continue;
            }
            /* (b) LEBEND-GATE des Flags-Bytes (nur Row-VM-Plaetze - die tragen, wie im Original,
             * IMMER ein datengetriebenes Flags-Byte). Original: 80019e70 lbu v0,108(v1) /
             * 80019e78 andi v0,v0,0x1 / 80019e7c beq v0,zero -> Routine-A-Dispatch aus;
             * 80019f44-50 dasselbe fuer den ganzen Slot-Rumpf; Spawner 800197a8-b0 vergibt
             * genau so einen Platz neu. */
            if (!(f->flags & 0x01)) { f->active = 0; continue; }
        }

        /* (c) FOLLOW (Flags-Bit 0x04, 1090-Feuer): der Original-Slot-Tick kopiert jeden Frame
         * die Eltern-Part-Matrix aus slot+0x74 (`lbu flags @0x80019f44; andi 0x4; lw +0x74;
         * 8x lw/sw` @0x80019f68-f94). Port: Position des Anker-Aktors uebernehmen — die
         * Emitter fahren in ROOM1090 per Heim-Pin ~940 Einheiten hoch (@0x80116444-98),
         * und die Feuer reiten mit (Savestate-Beleg im Dossier). */
        if (f->follow_slot >= 0 && (f->flags & 0x04)) {
            const re15_actor_t *pa = &g_actors[f->follow_slot];
            if (pa->active) { f->x = pa->x; f->y = pa->y; f->z = pa->z; }
        }

        /* (d) WELTLAGE slot+0x28/2a/2c (V1a) — VOR Routine B, die sie liest (Routine 29). */
        esp_fx_weltlage(f);

        /* (e) ROUTINE B @0x8001a2b4-d4 (`lhu v0,2(v0)` / Tabelle / `jalr v0`). */
        if (f->rows_base) {
            esp_fx_dispatch_b(f);                /* main-loop routineB (Huelse 12, Granate 29) */
            if (!f->active) continue;            /* B may despawn (2nd floor contact) */
        }

        /* (f) PHYSICS (byte-exact tick @0x8001a2fc-388, gated flags bit5==0 — die Flags werden
         * NACH Routine B neu gelesen, `lbu v0,108(a2)` @0x8001a2e8): euler += Winkelgeschw.
         * (+0x20.. += +0x18.., @0x8001a2fc-330), dann xlat (s32) += vel (s16), DANACH vel += acc
         * (@0x8001a324-388). LIVE-confirmed against mzd_stage1_hit_effect.sav. The row-VM freeze
         * bit (flags bit5, e.g. the sub-2 stagger rows' 0x61) pauses the integration. */
        if (f->phys && !(f->rows_base && (f->flags & 0x20))) {
            if (f->rows_base) {
                /* euler +0x20/22/24 += +0x18/1a/1c (`lhu`/`addu`/`sh` @0x8001a2fc-330; 16 Bit) —
                 * nur Row-VM-Plaetze fuehren die Zeilenkopie. */
                for (int k = 0; k < 3; k++)
                    row_set16(f, 0x20 + 2 * k,
                              (uint16_t)(row_u16(f->row, 0x20 + 2 * k) + row_u16(f->row, 0x18 + 2 * k)));
            }
            f->xlat_x += f->drift_x;                 /* xlat[0x34] += drift[0x10] */
            f->xlat_y += f->drift_y;                 /* xlat[0x38] += drift[0x12] */
            f->xlat_z += f->drift_z;                 /* xlat[0x3c] += drift[0x14] */
            f->drift_x = (int16_t)(f->drift_x + f->accel_x);   /* drift += accel (gravity) */
            f->drift_y = (int16_t)(f->drift_y + f->accel_y);
            f->drift_z = (int16_t)(f->drift_z + f->accel_z);
            /* FLOOR BOUNCE (routine 12 room_coll @0x8001c6e8, collapsed to a plane clamp): when the
             * particle reaches the floor, clamp it there and damp-flip drift.y so it settles.
             * Granatenplaetze und ihre Kinder tragen floor_y = ESP_KEIN_BODEN (E12/A6). */
            if (f->y + f->xlat_y >= f->floor_y) {
                f->xlat_y = f->floor_y - f->y;
                f->drift_y = (int16_t)(-f->drift_y / 2);       /* 50% restitution */
                f->drift_x = (int16_t)(f->drift_x * 3 / 4);    /* ground friction */
                f->drift_z = (int16_t)(f->drift_z * 3 / 4);
            }
        }

        /* (g) ANIM @0x8001a38c-47c */
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
    esp_granate_log_tick();
}
