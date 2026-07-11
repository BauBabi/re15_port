/* fade_common.c — the byte-true screen-fade channel engine (FUN_800217b0 / FUN_800216ec /
 * FUN_80021880 / FUN_80021764 / FUN_8002178c; the 0x44-stride array @DAT_800b5458).
 * Trace wf_2c73ab52, every load-bearing instruction self-verified 2026-07-11 — see re15_fade.h
 * for the mechanism summary + addresses. The render layer consumes the tick outputs (out_* /
 * drawn / abr) as full-screen overlay draws in the byte-true OT position. */
#include <string.h>
#include "re15_fade.h"

re15_fade_ch_t g_fade_ch[RE15_FADE_CHANNELS];

/* FUN_800217b0: config. Writes step/abr/bucket + the 0x00/0xff rgb masks from the 3-bit flags
 * (bit2=R @0x800217f4-800, bit1=G @0x8002180c-18, bit0=B @0x80021824-30) and rebuilds the two
 * DR_MODEs (tpage = abr<<5 — modeled by keeping `abr` for the render). Does NOT touch level. */
void re15_fade_config(int ch, int abr, int rgb_mask, int16_t step, int ot_bucket)
{
    if (ch < 0 || ch >= RE15_FADE_CHANNELS) return;
    re15_fade_ch_t *c = &g_fade_ch[ch];
    c->step      = step;
    c->abr       = (uint8_t)(abr & 3);
    c->ot_bucket = (uint8_t)ot_bucket;
    c->mask_r    = (rgb_mask & 4) ? 0xFF : 0x00;
    c->mask_g    = (rgb_mask & 2) ? 0xFF : 0x00;
    c->mask_b    = (rgb_mask & 1) ? 0xFF : 0x00;
}

/* FUN_800216ec: kick. step!=0 -> level = (step<=0) ? 0x7fff : 0 (the ramp start; value IGNORED,
 * @0x80021710-20). step==0 -> level = value (static overlay, delay slot @0x80021728). The
 * optional RECT override (+0x14/+0x18 tile writes) is always NULL from the SCD path — the tile
 * stays the boot-built full-screen 320x240. */
void re15_fade_kick(int ch, uint16_t value)
{
    if (ch < 0 || ch >= RE15_FADE_CHANNELS) return;
    re15_fade_ch_t *c = &g_fade_ch[ch];
    if (c->step != 0) c->level = (c->step <= 0) ? 0x7FFF : 0;
    else              c->level = value;
}

/* FUN_80021764: kill — level = 0xffff (bit15 set = idle + done-poll true, @0x80021780). */
void re15_fade_kill(int ch)
{
    if (ch < 0 || ch >= RE15_FADE_CHANNELS) return;
    g_fade_ch[ch].level = 0xFFFF;
}

/* FUN_8002178c: done poll — the sign bit of level. */
int re15_fade_done(int ch)
{
    if (ch < 0 || ch >= RE15_FADE_CHANNELS) return 1;
    return (g_fade_ch[ch].level & 0x8000u) ? 1 : 0;
}

/* FUN_80021880: the per-frame tick (once per rendered frame, after the game tick, before
 * present). Inactive channels (bit15) are skipped WITHOUT integrating (@0x800218c8-cc).
 * brightness = level>>7 (@0x800218d0), tile rgb = brightness & masks (@0x800218e4-918,
 * written from the level BEFORE integration), then level += step with natural u16 wrap
 * (@0x80021928-2c) — crossing 0x8000 (ramp-up) or underflowing (ramp-down) sets bit15 and
 * stops the channel next frame. */
void re15_fade_tick(void)
{
    for (int ch = 0; ch < RE15_FADE_CHANNELS; ch++) {
        re15_fade_ch_t *c = &g_fade_ch[ch];
        if (c->level & 0x8000u) { c->drawn = 0; continue; }
        uint8_t br = (uint8_t)(c->level >> 7);
        c->out_r = (uint8_t)(br & c->mask_r);
        c->out_g = (uint8_t)(br & c->mask_g);
        c->out_b = (uint8_t)(br & c->mask_b);
        c->drawn = 1;
        c->level = (uint16_t)(c->level + (uint16_t)c->step);
    }
}

/* CINEMATIC LETTERBOX counter (FUN_80021a0c, trace wf_bba41002 — every instruction verified):
 *   up:   (DAT_800aca3c & 0x10) && cnt < 0xF0  -> cnt += 0x10   (lbu/sltiu 0xf0 @0x80021a40-54)
 *   down: !(bit)               && cnt >= 0x10  -> cnt -= 0x10   (sltiu 0x10 @0x80021a68; the
 *         decrement is `addiu +0xF0` with u8 wrap @0x80021a7c-80 — same result)
 * 15 frames for a full open/close. The counter is an ENGINE global (persists across rooms;
 * the room-load chain clears the DIRECTION bit instead — FUN_800396fc masks aca3c's low 16
 * bits — so the bars ramp shut behind the load blank). */
uint8_t g_letterbox_level = 0;

void re15_letterbox_tick(int cine_request)
{
    if (cine_request) {
        if (g_letterbox_level < 0xF0) g_letterbox_level = (uint8_t)(g_letterbox_level + 0x10);
    } else {
        if (g_letterbox_level >= 0x10) g_letterbox_level = (uint8_t)(g_letterbox_level - 0x10);
    }
}

/* Boot park (LAB_80021138 @0x80021238-40 per channel): FUN_800217b0(ch|0x100, 0x100, 3, 0)
 * = ABR1 additive, step 0x100, mask 3 (G+B), bucket 0 — then FUN_80021764 (level=0xffff).
 * The defaults are never seen; every real user reconfigures first. NOT re-run on room load
 * (the channels are engine globals that persist across rooms). */
void re15_fade_init(void)
{
    memset(g_fade_ch, 0, sizeof g_fade_ch);
    for (int ch = 0; ch < RE15_FADE_CHANNELS; ch++) {
        re15_fade_config(ch, 1, 3, 0x100, 0);
        re15_fade_kill(ch);
    }
}
