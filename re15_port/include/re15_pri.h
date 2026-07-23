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

#define RE15_PRI_MAX_MASKS_PER_CUT 64

typedef struct {
    uint16_t srcX, srcY;       /* sample position in BG image (256-wide TIM) */
    uint16_t dstX, dstY;       /* screen draw position (low byte * group offset) */
    uint16_t depth;            /* Z priority */
    uint16_t width, height;    /* rect dims (square = sizeByte; else u16+u16) */
} re15_pri_mask_t;

typedef struct {
    int             mask_count;
    re15_pri_mask_t masks[RE15_PRI_MAX_MASKS_PER_CUT];
} re15_pri_cut_t;

/* Shared sprite.pri depth model — the SINGLE source of truth for BOTH ports.
 *
 * PRIMARY truth — READ STRAIGHT FROM THE ORIGINAL PSX.EXE (byte-true, 2026-07-23):
 * the mask's OT word index is `depth` (×1), NOT `depth*2`. The original's sprite.pri OT
 * insertion is FUN_80039590 @0x80039650-64:
 *     lh   a0,2(s3)     ; a0 = mask depth (runtime mask struct +2)
 *     sll  a0,a0,2      ; a0 = depth << 2 = depth*4 BYTES = depth WORDS (OT entries are u32)
 *     addu a0,a0,s6     ; + OT base; the frame's OT is 1024 words (depth 0..1023 fits exactly)
 *     jal  0x8006b538   ; link the mask primitive at ot[depth]
 * So the mask OT WORD INDEX = depth. (The old comment's "otz>>4 → ×2" reasoning was a wrong
 * guess; the disasm links at ot[depth] directly. depth*2 also OVERFLOWED the 1024-word OT for
 * any depth>511.)
 *
 * The PC has NO GTE / no OT. Its actor painter's-sort key is the RAW view-space Z
 * (avg_z, in world units / thousands). So the PC needs the mask's camera-Z.
 *
 * BYTE-EXACT crossover (all inputs verified in the ORIGINAL PSX.EXE):
 *   - otz = gte_avsz3 = ZSF3*(SZ1+SZ2+SZ3) >> 12, SZ = view-space z. The original sets
 *     ZSF3=341 (0x155) @0x80066c74: `li t0,0x155; ctc2 t0,$29` (GTE ctrl reg 29 = ZSF3;
 *     the next writes set ZSF4=256/$30 etc. = the InitGeom constants). So
 *     otz = 341*3*vz>>12 = 1023*vz>>12 ≈ vz/4.
 *   - Actor OT word index = otz>>3 (mesh_psx.c otz_to_bucket = RE2 FUN_8002d718 ((otz>>1)
 *     &0xffc)/4). Mask OT word index = depth (@0x80039654, above).
 *   - Mask occludes (more front, lower index) when depth < otz>>3 = 1023*vz>>15, i.e.
 *     vz > depth*32768/1023 = depth*32.03. So the PC threshold camera-Z = depth*32. The PC
 *     merge-walk draws the mask OVER a tri when mask_camZ < tri_vz → identical to PSX.
 *   This corrects the prior K=64 (which used the wrong depth*2 mask bucket): with K=64 the
 *   threshold was 2× too high, so a foreground fixture never occluded the player at typical
 *   gameplay depths (measured: ROOM1090 masks depth 175.. → K64 camZ 11200.. > Leon vz
 *   4715..8504 → NEVER occluded = "the lamps don't cover Leon behind them"). K=32 restores
 *   the byte-true crossover (earlier K=16/K=218 were guesses on wrong ZSF3/bucket). */
#define RE15_PRI_MASK_OT_BUCKET(depth) (depth)              /* PSX OT word index (@0x80039654) */
static inline int re15_pri_mask_camera_z(int depth) {       /* PC painter's sort key = vz */
    return depth * 32;                /* vz > depth*32 ⟺ depth < 1023*vz>>15 (ZSF3=341, mask@ot[depth]) */
}

/* Parse a sprite priority section at `data + offset` into out. Returns
 * number of masks parsed (0 if NULL section / parse failure). */
int re15_pri_parse_section(const uint8_t *data, size_t data_size,
                           uint32_t offset, re15_pri_cut_t *out);

#ifdef __cplusplus
}
#endif

#endif /* RE15_PRI_H */
