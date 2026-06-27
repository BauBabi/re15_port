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
 * PRIMARY truth (byte-true, save-state-verified on PSX): the mask's OT bucket is
 * `depth*2`. The original authors `depth` in its otz>>4 OT; our mesh buckets actors
 * via otz>>3 (render.c:21), so the authored value ×2 lands in our scale. The PSX
 * links the mask at ot[depth*2] directly — RE15_PRI_MASK_OT_BUCKET.
 *
 * The PC has NO GTE / no OT. Its actor painter's-sort key is the RAW view-space Z
 * (avg_z, in world units / thousands). So the PC needs the mask's camera-Z.
 *
 * BYTE-EXACT (2026-06-13, RE'd from the GTE: ZSF3 read straight out of InitGeom in our
 * own ELF — no PSX run, no guessing):
 *   - otz = gte_avsz3 = ZSF3*(SZ1+SZ2+SZ3) >> 12, with SZ = vz (the view-space Z; SZ3 =
 *     MAC3, H from SetGeomScreen only scales screen X/Y, NOT SZ). InitGeom sets ZSF3=341
 *     (0x155) [mipsel-objdump re15_reborn.elf <InitGeom>: `li a0,341; ctc2 a0,$29`]. So
 *     otz = 341*3*vz>>12 = 1023*vz>>12 ≈ vz/4.
 *   - Actor OT word index = otz>>3 (mesh_psx.c otz_to_bucket = RE2 FUN_8002d718 ((otz>>1)
 *     &0xffc)/4). Mask OT word index = depth*2 (RE15_PRI_MASK_OT_BUCKET, PSX-verified).
 *   - Mask occludes (more front, lower index) when depth*2 < otz>>3 = 1023*vz>>15, i.e.
 *     vz > depth*65536/1023 = depth*64.06. So the PC threshold camera-Z = depth*64. The PC
 *     merge-walk draws the mask OVER a tri when mask_camZ < tri_vz → identical to PSX.
 *   This pins the constant the two prior guesses got wrong: K=16 (round-1, assumed otz=vz =
 *   ZSF3≈1365) was TOO SMALL → "lamp on top of Leon when he's in FRONT"; K=218 (round-2,
 *   assumed ZSF3=H/3=100) was TOO LARGE → "Leon on top of lamp when BEHIND". The real
 *   ZSF3=341 gives K=64. (Verified vs measured PC values: cut masks depth 117 → threshold
 *   ~7496; Leon vz 2666..5599 < that → in FRONT, not occluded — matches the visual.) */
#define RE15_PRI_MASK_OT_BUCKET(depth) ((depth) * 2)        /* PSX OT word index */
static inline int re15_pri_mask_camera_z(int depth) {       /* PC painter's sort key = vz */
    return depth * 64;                /* vz > depth*64 ⟺ depth*2 < 1023*vz>>15 (ZSF3=341) */
}

/* Parse a sprite priority section at `data + offset` into out. Returns
 * number of masks parsed (0 if NULL section / parse failure). */
int re15_pri_parse_section(const uint8_t *data, size_t data_size,
                           uint32_t offset, re15_pri_cut_t *out);

#ifdef __cplusplus
}
#endif

#endif /* RE15_PRI_H */
