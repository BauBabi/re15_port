/*
 * RE1.5 Rebuilt — Sprite priority (BG foreground occlusion) parser.
 *
 * PSX RE1.5 stores per-cut foreground-occluder sprite metadata in the
 * RDT, referenced by `re15_camera_cut_t.pri_offset` (RDT-relative byte
 * offset). The data structure:
 *
 *   Section (header 4B):
 *     u16 groupCount
 *     u16 maskCountDeclared
 *   Group (8B):
 *     u16 maskCount      (sprites in this group)
 *     u16 baseRaw        (likely TPage / texture bank)
 *     s16 destX, destY   (group screen anchor)
 *   Mask (8B base, 12B if rectangular):
 *     u8  srcX, srcY     (sample position in BG image)
 *     u8  dstX, dstY     (screen draw position, low byte)
 *     u16 depth          (Z priority value — 0=front, high=back)
 *     u8  unknown
 *     u8  sizeByte       (0 = rectangular [+u16 w, u16 h], else square)
 *
 * NULL section: pri_offset bytes = `FF FF FF FF` (zero groups + zero masks
 * treated as terminator by parser).
 *
 * Spec source: src/main/java/de/re15/extractors/SpritePriParser.java
 * (Java parser used by RDT extractor; bit-perfect format).
 */
#ifndef RE15_PRI_H
#define RE15_PRI_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Per-cut mask capacity. The ORIGINAL has NO fixed cap: FUN_80039270 carves the
 * per-room buffers from the bump arena using RDT header byte[7] as the element
 * count (`@0x8003928c: lbu v0,7(a0)`; flags hdr7*4, two prim buffers hdr7*0x20
 * each; arena head 0x800ac77c advanced by hdr7*68 `@0x800392bc`), and the parse
 * loop in FUN_800392d4 is bounded only by the DATA (outer over groupCount
 * `@0x8003954c`, inner over group[i].count `@0x8003950c`) — no compare against
 * any constant. Across all 206 real RDTs, hdr7 == max-over-cuts(sum of group
 * counts) with 0 mismatches; the game-wide maximum is 105 (ROOM3000/ROOM3001
 * cut 3, hdr byte[7] = 0x69 = 105, cut-3 pri header @0xdcc = `08 00 69 00`).
 * The old value 64 was a fabricated limit that silently dropped real masks in
 * 26 of 240 rooms (e.g. ROOM1090 cut 8 lost 3, two of them depth-1 = the
 * always-in-front bucket; ROOM3000 cut 3 lost 41 of 105). */
#define RE15_PRI_MAX_MASKS_PER_CUT 105

typedef struct {
    uint16_t srcX, srcY;       /* sample position in BG image (256-wide TIM) */
    uint16_t dstX, dstY;       /* screen draw position (low byte * group offset) */
    uint16_t depth;            /* Z priority */
    uint16_t width, height;    /* rect dims (square = sizeByte; else u16+u16) */
} re15_pri_mask_t;

typedef struct {
    int             mask_count;   /* masks BUILT = sum over groups of group.count */
    int             draw_count;   /* masks DRAWN = declared count & 0xFF (see below) */
    re15_pri_mask_t masks[RE15_PRI_MAX_MASKS_PER_CUT];
} re15_pri_cut_t;

/* Shared sprite.pri depth model — the SINGLE source of truth for BOTH ports.
 * Every number below is read out of the ORIGINAL PSX.EXE (wf_beffdbe5, 8 drawers
 * cross-checked + adversarially re-verified); the earlier "PSX-verified depth*2 /
 * RE2 FUN_8002d718 otz>>3" derivation was an RE2 analogy, NOT RE1.5, and is void.
 *
 * ONE ordering table is shared by masks and characters: OT-B = 0x800AA6D8 +
 * frameflip*0x1000, 1024 u32 entries (ClearOTagR n=0x400 @0x80020c6c, DrawOTag
 * &ot[1023] @0x800215d0 → reverse-linked: HIGHER index draws EARLIER = farther,
 * index 0 is frontmost).
 *
 *   MASK OT word index = depth (×1).  FUN_80039590 @0x80039650-64:
 *       80039650: lh   a0,2(s3)     ; the authored depth halfword
 *       80039658: sll  a0,a0,2      ; *4 BYTES = *1 WORD
 *       8003965c: addu a0,a0,s6     ; s6 = 0x800AA6D8 (+frameflip @0x800395e4)
 *       80039660: jal  0x8006b538   ; OT link
 *     The depth is copied verbatim out of the RDT record and never shifted
 *     (@0x800393f0: lhu v0,-6(s2) ; @0x800393f8: sh v0,0(t1)).
 *
 *   CHARACTER poly OT word index = otz>>4.  FUN_800254a0 @0x80025648-64 (and
 *   identically in all 8 mesh drawers: 0x8002303c, 0x8002321c, 0x80023670,
 *   0x800239ac, 0x80023e10, 0x80024294, 0x800258a8):
 *       80025648: stotz t4          ; GTE OTZ after AVSZ3 (@0x80025638)
 *       80025654: sra  v0,v1,6      ; NEAR GATE: otz < 64 …
 *       80025658: beq  v0,zero,…    ; … drops the poly entirely
 *       8002565c: sra  v1,v1,4      ; <<< the shift: index = otz>>4
 *       80025660: sll  v1,v1,2      ; *4 = byte offset
 *     No bias, no 0xffc mask, no upper clamp between the OTZ read and the link.
 *
 *   otz = AVSZ3 = ZSF3*(SZ1+SZ2+SZ3)>>12 with ZSF3=341 (@0x80066c70/74:
 *   `li t0,0x155 ; ctc2 t0,$29`) → for uniform depth vz: otz = (1023*vz)>>12.
 *   Quads use AVSZ4 with ZSF4=256 (@0x80066c7c/80) → otz = vz>>2.
 *
 *   A mask occludes a character poly iff it draws LATER, i.e. depth < otz>>4:
 *       tris : depth < (1023*vz)>>16  → vz ≥ (depth+1) * 65536/1023 = ×64.0625
 *       quads: depth < vz>>6          → vz ≥ (depth+1) * 64        (exactly)
 *   The PC has no GTE/OT; its painter's-sort key is the raw view-space Z, so the
 *   PC threshold camera-Z = depth*64. (K=32 — from the void otz>>3 analogy — was
 *   measured wrong against the ROOM1170 helipad lamp and reverted in e848b677.) */
#define RE15_PRI_MASK_OT_BUCKET(depth) (depth)              /* PSX OT word index @0x80039658 */
#define RE15_PRI_CHAR_OT_BUCKET(otz)   ((otz) >> 4)         /* PSX OT word index @0x8002565c */
#define RE15_PRI_OTZ_NEAR_DROP         64                   /* otz<64 drops the poly @0x80025654 */
static inline int re15_pri_mask_camera_z(int depth) {       /* PC painter's sort key = vz */
    return depth * 64;                /* depth < otz>>4 = (1023*vz)>>16 (ZSF3=341) */
}

/* Parse a sprite priority section at `data + offset` into out. Returns
 * number of masks parsed (0 if NULL section / parse failure). */
int re15_pri_parse_section(const uint8_t *data, size_t data_size,
                           uint32_t offset, re15_pri_cut_t *out);

#ifdef __cplusplus
}
#endif

#endif /* RE15_PRI_H */
