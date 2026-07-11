/* re15_fade.h — the byte-true SCREEN-FADE channel engine (trace wf_2c73ab52, self-verified).
 *
 * PSX: a 4-channel, 0x44-stride array @DAT_800b5458 of full-screen semi-transparent TILE prims.
 *   FUN_800217b0 (SCD opcode 0x56 @0x80042a58) = CONFIG: ch|abr<<8, step s16, rgb mask, OT bucket
 *     (the SCD handler hardwires bucket 7 — `ori a3,7` @0x80042a70).
 *   FUN_800216ec (SCD opcode 0x57 @0x80042ab4) = KICK: step==0 -> level = value (static overlay,
 *     brightness = level>>7); step!=0 -> value IGNORED, level auto-starts at 0 (step>0) or 0x7fff
 *     (step<=0) — the ramp start (`slti/subu/andi 0x7fff` @0x80021710-20).
 *   FUN_80021880 (per frame, after the game tick, before present) = TICK: skip when level bit15
 *     set (`sll 16; bltz` @0x800218c8); brightness = level>>7 (`sra 23` @0x800218d0); tile rgb =
 *     brightness & mask bytes (masks are 0x00/0xff only); THEN level += step with natural u16
 *     wrap (@0x80021928-2c) — the wrap flipping bit15 IS the stop condition (no clamp/target).
 *   FUN_80021764 = KILL (level=0xffff); FUN_8002178c = DONE poll ((s16)level < 0).
 * Compositing: channels 0-2 target the 8-entry TOP OT (drawn LAST = over 3D+backdrop; bucket 7 =
 * bottom of that layer -> UNDER the letterbox @bucket 4 and the UI); channel 3 targets the
 * 16-entry BACKGROUND OT (drawn FIRST = behind the scene). ABR: 0=B/2+F/2, 1=B+F (additive),
 * 2=B-F (subtractive darken), 3=B+F/4.
 * Shipped SCD usage (ROOM11F0/11F1 @0x174a, ROOM3080 @0x81e): always `56 00 02 07 00 00` (ch0,
 * subtractive white, step 0) + static `57 00 <level>` pulses = flickering-lights ambience.
 * The channels are ENGINE globals: boot-parked once, NOT cleared on room load. */
#ifndef RE15_FADE_H
#define RE15_FADE_H

#include <stdint.h>

typedef struct {
    uint16_t level;       /* +0x00 — bit15 set = channel inactive/done          */
    int16_t  step;        /* +0x02 — signed per-frame delta (0 = static)        */
    uint8_t  abr;         /* +0x04 — semi-trans mode 0..3                       */
    uint8_t  mask_r;      /* +0x05 — 0x00 or 0xff (config rgb flag bit2)        */
    uint8_t  mask_g;      /* +0x06 — (bit1)                                     */
    uint8_t  mask_b;      /* +0x07 — (bit0)                                     */
    uint8_t  ot_bucket;   /* +0x08 — bucket inside the target OT (SCD: 7)       */
    /* tick outputs (the tile rgb written this frame; valid when drawn=1) */
    uint8_t  out_r, out_g, out_b;
    uint8_t  drawn;       /* 1 = the channel emitted its overlay this frame     */
} re15_fade_ch_t;

#define RE15_FADE_CHANNELS 4
extern re15_fade_ch_t g_fade_ch[RE15_FADE_CHANNELS];

void re15_fade_init(void);    /* boot park (LAB_80021138): config(ch, abr1, mask3, step 0x100,
                               * bucket 0) + kill -> all channels idle/invisible */

/* CINEMATIC LETTERBOX counter (byte-true FUN_80021a0c, trace wf_bba41002 CONFIRMED): ONE u8
 * counter DAT_800b5568 ramped ±0x10 per rendered frame within [0x00, 0xF0] — direction = a LIVE
 * per-frame test of DAT_800aca3c bit 0x10 (@0x80021a24) = flag bank 1 idx 27 (SCD `Set(1,0x1b,op)`,
 * 247 shipped sites). The counter IS the bars' RGB (two static POLY_F4s (0,0,320,24) +
 * (0,216,320,240), code 0x2A semi-trans, DR_MODE ABR=2 SUBTRACTIVE -> pixel = bg - counter:
 * the strips darken smoothly to black over 15 frames). Bars emit only when counter != 0 (and
 * the PSX also suppresses during the room-load display blank, DAT_800aca38 bit 0x4000). */
extern uint8_t g_letterbox_level;
void re15_letterbox_tick(int cine_request);   /* once per rendered frame (PSX @0x80020f34) */
void re15_fade_config(int ch, int abr, int rgb_mask, int16_t step, int ot_bucket); /* FUN_800217b0 */
void re15_fade_kick(int ch, uint16_t value);                                       /* FUN_800216ec */
void re15_fade_kill(int ch);                                                       /* FUN_80021764 */
int  re15_fade_done(int ch);                                                       /* FUN_8002178c */
void re15_fade_tick(void);                                                         /* FUN_80021880 */

#endif /* RE15_FADE_H */
