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
#include <string.h>

static uint32_t rd_u32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static int32_t rd_s32(const uint8_t *p) { return (int32_t)rd_u32(p); }

int re15_esp_parse(const uint8_t *raw, size_t size,
                   uint32_t idh_off, uint32_t ptr_end_off,
                   uint32_t tim_base_off, uint32_t tim_end_off,
                   re15_esp_t *out)
{
    if (!raw || !out) return -1;
    memset(out, 0, sizeof(*out));
    out->raw = raw;
    out->raw_size = size;

    /* FUN_80019354 guard: piVar4 != 0 && *piVar4 != -1 (idh present + first word not the
     * 0xFFFFFFFF "no effects" marker). idh_off must leave room for the 8-byte id header. */
    if (idh_off == 0 || (size_t)idh_off + 8 > size) return -2;
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
