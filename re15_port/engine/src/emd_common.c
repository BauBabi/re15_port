/*
 * RE1.5 Rebuilt — EDD + EMR parsers (Phase 4.5.7.1, 2026-05-18).
 *
 * Pure C, target-agnostic. Ported from src/main/java/de/re15/extractors/emd
 * (AnimationParser.java + SkeletonParser.java + BitReader.java).
 *
 * Both files are sub-components of the PLD/EMD model container. Parsing
 * is structural — no math, no bone-transform composition; that's Phase
 * 4.5.7.2's job. Here we only:
 *
 *   - Pull clip + frame indices out of EDD.
 *   - Pull bone hierarchy + relative positions out of EMR.
 *   - Compute parent[] via breadth-first scan of the children lists.
 *   - Hold a pointer to the raw keyframe block for later 12-bit-packed
 *     Euler-angle decoding in re15_emd_get_keyframe_angles().
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif
#include "re15_emd.h"

/* ===== little-endian helpers ============================================ */

static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] <<  8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static int16_t read_s16_le(const uint8_t *p)
{
    return (int16_t)read_u16_le(p);
}

/* ===== EDD: animation clips + frame list ================================ */

int re15_emd_parse_animation(const uint8_t *edd_data, size_t edd_size,
                             re15_emd_animation_t *out)
{
    if (!edd_data || !out)    return -1;
    if (edd_size < 4)         return -2;

    memset(out, 0, sizeof(*out));

    int count0  = (int)read_u16_le(edd_data + 0);
    int offset0 = (int)read_u16_le(edd_data + 2);
    if (offset0 == 0)         return -3;

    /* Clip table size in bytes = offset0; per-clip header = 4 bytes
     * (u16 count, u16 reserved). */
    int clip_count = offset0 / 4;
    if (clip_count <= 0 || clip_count > RE15_EMD_MAX_CLIPS) return -4;

    out->clip_count = clip_count;
    out->clips[0].first_frame = 0;
    out->clips[0].frame_count = count0;

    int frame_count_total = count0;
    int next_offset       = count0;

    int cur = 4;   /* offset of clip[1] header */
    for (int i = 1; i < clip_count; i++) {
        if (cur + 2 > (int)edd_size) return -5;
        int count_i = (int)read_u16_le(edd_data + cur);
        cur += 4;   /* skip count + reserved */

        out->clips[i].first_frame = next_offset;
        out->clips[i].frame_count = count_i;
        frame_count_total += count_i;
        next_offset       += count_i;
    }

    if (frame_count_total > RE15_EMD_MAX_FRAMES) return -6;
    out->frame_count = frame_count_total;

    int frames_offset = offset0;
    if (frames_offset + frame_count_total * 4 > (int)edd_size) return -7;

    int last_key = 0;
    for (int i = 0; i < frame_count_total; i++) {
        uint32_t value = read_u32_le(edd_data + frames_offset + i * 4);
        out->frames[i] = value;
        int key_index = (int)(value & 0xFFFu);
        if (key_index > last_key) last_key = key_index;
    }
    out->last_keyframe_index = last_key + 1;
    return 0;
}

/* ===== EMR: skeleton + bone hierarchy =================================== */

/* Build parent[] by BFS over the children lists. PSX format stores
 * children per-bone, not parents — we invert the relationship. */
static void emr_assign_parents(int                bone_count,
                               const int          *child_counts,
                               const uint8_t      *child_base,
                               int                 child_base_max_offset,
                               const uint16_t     *child_offsets,
                               int8_t             *parents)
{
    /* Initialize parents to -1 (= unattached = root). */
    for (int i = 0; i < bone_count; i++) parents[i] = -1;

    /* For each bone, walk its children list and assign self as parent. */
    for (int b = 0; b < bone_count; b++) {
        int mesh_count = child_counts[b];
        if (mesh_count <= 0)              continue;

        int base = (int)child_offsets[b];
        if (base >= child_base_max_offset) continue;
        int available = child_base_max_offset - base;
        if (available > mesh_count)        available = mesh_count;

        for (int i = 0; i < available; i++) {
            int child = (int)child_base[base + i];
            if (child < 0 || child >= bone_count) continue;
            if (child == b)                       continue;
            /* First seen parent wins (preserves Java's BFS-first
             * assignment); skip if already assigned. */
            if (parents[child] == -1)             parents[child] = (int8_t)b;
        }
    }
}

int re15_emd_parse_skeleton(const uint8_t *emr_data, size_t emr_size,
                            re15_emd_skeleton_t *out)
{
    if (!emr_data || !out)    return -1;
    if (emr_size < 8)         return -2;

    memset(out, 0, sizeof(*out));

    int bones_table_offset = (int)read_u16_le(emr_data + 0);
    int keyframes_offset   = (int)read_u16_le(emr_data + 2);
    int bone_count         = (int)read_u16_le(emr_data + 4);
    int keyframe_size      = (int)read_u16_le(emr_data + 6);

    if (bone_count <= 0 || bone_count > RE15_EMD_MAX_BONES) return -3;
    if (keyframe_size <= 12)                                 return -4;

    out->bone_count           = bone_count;
    out->keyframe_size_bytes  = keyframe_size;

    /* Bone relative positions table follows the header. */
    int pos_table = 8;
    if (pos_table + bone_count * 6 > (int)emr_size) return -5;
    for (int b = 0; b < bone_count; b++) {
        out->bone_relative_pos[b][0] = read_s16_le(emr_data + pos_table + b*6 + 0);
        out->bone_relative_pos[b][1] = read_s16_le(emr_data + pos_table + b*6 + 2);
        out->bone_relative_pos[b][2] = read_s16_le(emr_data + pos_table + b*6 + 4);
    }

    /* Per-bone header: u16 mesh_count, u16 child_list_offset. The
     * child_list_offset is RELATIVE to bones_table_offset and indexes
     * a flat byte array of child indices. */
    int bones_header_end = bones_table_offset + bone_count * 4;
    if (bones_header_end > (int)emr_size) return -6;

    int             child_counts [RE15_EMD_MAX_BONES] = {0};
    uint16_t        child_offsets[RE15_EMD_MAX_BONES] = {0};
    for (int b = 0; b < bone_count; b++) {
        int raw_off = bones_table_offset + b * 4;
        child_counts [b] = (int)     read_u16_le(emr_data + raw_off);
        child_offsets[b] = (uint16_t)read_u16_le(emr_data + raw_off + 2);
    }
    /* The child-list bytes live IMMEDIATELY after the bones-header
     * table, but offsets are relative to bones_table_offset itself
     * (per the Java parser). */
    int child_max_off = (int)emr_size - bones_table_offset;
    emr_assign_parents(bone_count, child_counts,
                       emr_data + bones_table_offset, child_max_off,
                       child_offsets, out->bone_parent);

    /* Es gibt KEINE Bone->Mesh-Permutationstabelle in der EMR. Beide Renderer
     * (pc/main.c:1717 `mi=bi`, psx/mesh_psx.c:838 `mesh_idx=b`) nutzen 1:1
     * mesh_idx == bone_idx. Das frühere bone_mesh_index[] re-las die EMR-Child-
     * Traversal-Liste (bones_table[0].child_offset) und wurde NIE konsumiert
     * (0 Leser im gesamten re15_port) — entfernt. [BYTE_TRUE_AUDIT #43] */
    for (int b = 0; b < bone_count; b++) out->bone_mesh_index[b] = (int8_t)b;

    /* Keyframe block. */
    if (keyframes_offset > (int)emr_size) return -7;
    out->keyframe_data      = emr_data + keyframes_offset;
    out->keyframe_data_size = emr_size - (size_t)keyframes_offset;
    out->keyframe_count     = (int)(out->keyframe_data_size / (size_t)keyframe_size);
    return 0;
}

/* ===== 12-bit packed Euler angle accessor =============================== */

/* Read `count` bits at `bit_offset` from a little-endian-packed
 * byte stream. Bit ordering matches BitReader.java: bit 0 of byte 0 =
 * LSB of the first value. */
static uint32_t bit_read(const uint8_t *data, size_t data_size,
                         int bit_offset, int count)
{
    uint32_t value = 0;
    for (int i = 0; i < count; i++) {
        int byte_idx  = (bit_offset + i) >> 3;
        int inner_idx = (bit_offset + i) & 7;
        if (byte_idx >= (int)data_size) break;
        uint32_t bit = (uint32_t)((data[byte_idx] >> inner_idx) & 1);
        value |= (bit << i);
    }
    return value;
}

int re15_emd_get_keyframe_angles(const re15_emd_skeleton_t *skel,
                                  int keyframe_index, int bone_index,
                                  int16_t *out_x, int16_t *out_y,
                                  int16_t *out_z)
{
    if (out_x) *out_x = 0;
    if (out_y) *out_y = 0;
    if (out_z) *out_z = 0;
    if (!skel || !skel->keyframe_data)               return 0;
    if (keyframe_index < 0 || keyframe_index >= skel->keyframe_count) return 0;
    if (bone_index < 0 || bone_index >= skel->bone_count)             return 0;

    /* ✅ CANONICAL — DO NOT "FIX" (BO-round 2026-05-29, deep-RE confirmed).
     * `bit_offset = bone_index * 36` (all 15 bones packed from keyframe byte
     * +12, bone 0 first, 36 bits/bone, stride 0xac) IS exactly what PSX
     * FUN_8001f664 does (ghidra1_V2.txt:98547; a2 = keyframe_ptr+12). This is
     * NOT a hack — the 2026-05-29 hack audit FALSELY flagged it; the deeper RE
     * proved the current decode is correct.
     *
     * THE AO8 TRAP — explicitly forbidden: the reverted AO8 attempt used
     * bit_offset=(bone_index-1)*36 for bones 1+, which reads bone 1 from bone
     * 0's bits (off-by-one into the stream) → corrupts every non-zero bone.
     * It is WRONG; do not reapply it.
     *
     * Keyframe bytes 0..5 = bone 0's three rotation angles as full s16, used
     * ONLY for the GTE inter-frame interpolation history buffer (not RotMatrix
     * input). Bytes 6..11 = speed vector, unread here; root translation is
     * SCD-speed-driven (Speed_set/Add_speed), so zeroing kf translation is
     * also canonical. The Z-axis barrel-roll is a SEPARATE renderer issue
     * (RotMatrix YXZ Euler order / coord-system / world-yaw composition at
     * skeleton_common.c root path) — NOT a keyframe-layout bug. */
    int frame_base = keyframe_index * skel->keyframe_size_bytes;
    int angles_byte_base = frame_base + 12;
    int angles_byte_size = skel->keyframe_size_bytes - 12;
    if (angles_byte_size <= 0) return 0;

    const uint8_t *angles_ptr = skel->keyframe_data + angles_byte_base;
    if (angles_byte_base + angles_byte_size > (int)skel->keyframe_data_size) return 0;

    int bit_offset = bone_index * 36;
    if (bit_offset + 36 > angles_byte_size * 8) return 0;

    int16_t x = (int16_t)bit_read(angles_ptr, (size_t)angles_byte_size, bit_offset +  0, 12);
    int16_t y = (int16_t)bit_read(angles_ptr, (size_t)angles_byte_size, bit_offset + 12, 12);
    int16_t z = (int16_t)bit_read(angles_ptr, (size_t)angles_byte_size, bit_offset + 24, 12);
    /* H32 Rank 2 fix (2026-05-24): 12-bit Euler angles are two's-complement
     * signed per lib_rbj/RE2 convention. bit_read returns unsigned 0..4095;
     * values 2048..4095 represent negative angles -2048..-1. Without this
     * sign-extension, every bone with a "negative" rotation rotates +180°
     * to +359° in the wrong direction → bones go wild → Q12 matrix chain
     * saturates → triangles cover huge screen areas → affine UV stretches
     * a few texels across many pixels → user-visible "multi-colored cubes"
     * on Leon's mesh during cinematic motion clips.
     *
     * Why NPCs render correctly: per debug.log Elliot stays at motion=0
     * keyframe 0 (T-pose) where most angles are 0/near-0, so the missing
     * sign extension rarely matters for him. */
    if (x & 0x800) x = (int16_t)(x | 0xF000);
    if (y & 0x800) y = (int16_t)(y | 0xF000);
    if (z & 0x800) z = (int16_t)(z | 0xF000);
    /* BO-round 2026-05-29 (hack audit): removed a stale per-bone clip-20 diag
     * fprintf in this hot path (dev-only, zero engine impact). */
    if (out_x) *out_x = x;
    if (out_y) *out_y = y;
    if (out_z) *out_z = z;
    return 1;
}

int re15_emd_get_keyframe_position(const re15_emd_skeleton_t *skel,
                                    int keyframe_index,
                                    int16_t *out_x, int16_t *out_y,
                                    int16_t *out_z)
{
    if (out_x) *out_x = 0;
    if (out_y) *out_y = 0;
    if (out_z) *out_z = 0;
    if (!skel || !skel->keyframe_data) return 0;
    if (keyframe_index < 0 || keyframe_index >= skel->keyframe_count) return 0;

    int frame_base = keyframe_index * skel->keyframe_size_bytes;
    if (frame_base + 6 > (int)skel->keyframe_data_size) return 0;

    if (out_x) *out_x = read_s16_le(skel->keyframe_data + frame_base + 0);
    if (out_y) *out_y = read_s16_le(skel->keyframe_data + frame_base + 2);
    if (out_z) *out_z = read_s16_le(skel->keyframe_data + frame_base + 4);
    return 1;
}

/* Raw packed FRAME ENTRY (frames[clip.first_frame + frame_in_clip]) — the 12-bit
 * keyframe index (bits 0-11) PLUS the upper-bit flags. The walk/run handlers
 * (LAB_80030af0/d28) read this exact dword and test bit 0x4000 = foot-plant +
 * 0x1000 = which-foot to gate the footstep SE. Returns 0 if out of range. */
uint32_t re15_emd_get_frame_entry(const re15_emd_animation_t *anim,
                                  int clip, int frame_in_clip)
{
    if (!anim || clip < 0 || clip >= anim->clip_count) return 0;
    if (frame_in_clip < 0 || frame_in_clip >= anim->clips[clip].frame_count) return 0;
    int idx = anim->clips[clip].first_frame + frame_in_clip;
    if (idx < 0 || idx >= anim->frame_count) return 0;
    return anim->frames[idx];
}

/* Phase 4.5.13-RE2 I3 (2026-05-21): keyframe SPEED field reader.
 *
 * Keyframe layout (G1/H3 verified): bytes 0..5 = pos_xyz s16 LE, bytes
 * 6..11 = SPEED_xyz s16 LE. The speed field drives engine-side root
 * translation in RE1.5 — clip 15 of rbj for ROOM1170 has speed ramping
 * from 22→111 per frame (integrated disp = 1428 units over 20 frames),
 * which is exactly the walking-to-the-helicopter motion the user
 * expects from sub02. Our engine never read this field, so Plc_motion
 * showed only the cinematic bone pose without translation = "floating".
 *
 * Per H2's PSX.EXE integrator trace (FUN_8001F3BC): the engine reads
 * these 3 s16 values, GTE-rotates by actor's yaw matrix, and ADDS to
 * actor.pos every tick the clip is active. We mirror that here. */
int re15_emd_get_keyframe_speed(const re15_emd_skeleton_t *skel,
                                 int keyframe_index,
                                 int16_t *out_sx, int16_t *out_sy,
                                 int16_t *out_sz)
{
    if (out_sx) *out_sx = 0;
    if (out_sy) *out_sy = 0;
    if (out_sz) *out_sz = 0;
    if (!skel || !skel->keyframe_data) return 0;
    if (keyframe_index < 0 || keyframe_index >= skel->keyframe_count) return 0;

    int frame_base = keyframe_index * skel->keyframe_size_bytes;
    if (frame_base + 12 > (int)skel->keyframe_data_size) return 0;

    if (out_sx) *out_sx = read_s16_le(skel->keyframe_data + frame_base + 6);
    if (out_sy) *out_sy = read_s16_le(skel->keyframe_data + frame_base + 8);
    if (out_sz) *out_sz = read_s16_le(skel->keyframe_data + frame_base + 10);
    return 1;
}

/* === Phase 4.5.13-B2: room-local animation.rbj parser ==================== */

int re15_emd_parse_rbj_record(const uint8_t *rbj_data, size_t rbj_size, int record_index,
                       const re15_emd_skeleton_t *base_skel,
                       re15_emd_skeleton_t *out_skel,
                       re15_emd_animation_t *out_anim)
{
    if (!rbj_data || !out_skel || !out_anim || !base_skel) return -1;
    if (rbj_size < 0x20) return -2;

    /* Container header: u32 total_length, u32 record_count, u32 ?, u32 ?. */
    uint32_t total_length = read_u32_le(rbj_data + 0);
    uint32_t record_count = read_u32_le(rbj_data + 4);
    if (total_length == 0 || total_length > rbj_size) return -3;
    if (record_count == 0 || record_count > 8) return -4;

    /* Trailer at total_length holds per-record offset table. For 2 records
     * (ROOM1170 layout) the trailer is 16 bytes:
     *   [u32 R0_EMR_prefix, u32 R0_EDD_off, u32 R1_EMR_prefix, u32 R1_EDD_off]
     * (byte-verified: 0x8, 0x7274, 0x78FC, 0xD1C8). EMR header proper starts
     * at *_EMR_prefix + 4 (skipping a 4-byte record marker per lib_rbj's
     * `o_arm<8 && o_frm==0` fallback path).
     *
     * Phase BO (2026-05-29): use RECORD 0, not Record 1. PSX repoints the
     * cinematic actor's EDD_base (actor+0x16c) at R0 — verified canonical via
     * op_plc_motion @0x80041b90 + anim_select_frame @0x8001f314 (direct,
     * unchecked motion_id*4 index into the EDD; NO motion->clip table, NO
     * clamp). R0 has 26 clips (0..25); R1 has only 25 (0..24). sub02's
     * Cut_chg(4) + Plc_motion(0,25,0) demands clip 25 = the 82-frame
     * "hand-to-face" reaction (firstkf 284, present in R0's 366-keyframe pool;
     * ABSENT from R1). R0 is a byte-verified strict superset of R1 for clips
     * 0..24 (identical fc+firstkf), so no earlier gesture regresses. The old
     * R1 load made motion 25 fall off the end → the renderer's modulo/saturate
     * clamp invented clip 0 (T-pose) / clip 24 (a spin) — both wrong. */
    /* BO-round 2026-05-29 (hack audit): trailer size = record_count * 8 (two
     * u32 per record), not a hardcoded 16. We still read only Record 0's pair. */
    if (total_length + (size_t)record_count * 8 > rbj_size) return -5;
    /* Per-record trailer pair (2 u32 each): RECORD i at total_length + i*8. record_index 0 =
     * the idle/player overlay (ROOM1170 Leon+Elliot); RECORD 1 = the per-entity override the
     * RBJ header bitmask names (ROOM1150 binds RECORD 1 = the prone wounded-lying Irons bank
     * onto enemy entity 0 = em45 — byte-true FUN_8001b3f8). */
    if (record_index < 0 || (uint32_t)record_index >= record_count) return -5;
    uint32_t r0_emr_prefix = read_u32_le(rbj_data + total_length + (size_t)record_index * 8 + 0);
    uint32_t r0_edd_off    = read_u32_le(rbj_data + total_length + (size_t)record_index * 8 + 4);
    if (r0_emr_prefix == 0 || r0_emr_prefix + 8 >= rbj_size) return -6;
    if (r0_edd_off    == 0 || r0_edd_off    >= rbj_size) return -7;
    uint32_t r0_emr_off = r0_emr_prefix + 4;

    /* Record 0 EMR header (8 bytes): o_arm, o_frm, count, size. */
    uint16_t o_arm  = read_u16_le(rbj_data + r0_emr_off + 0);
    uint16_t o_frm  = read_u16_le(rbj_data + r0_emr_off + 2);
    uint16_t count  = read_u16_le(rbj_data + r0_emr_off + 4);
    uint16_t ksize  = read_u16_le(rbj_data + r0_emr_off + 6);
    (void)o_arm;
    if (count != base_skel->bone_count) return -8;
    if (ksize == 0 || ksize > 256) return -9;

    /* Keyframe data starts at EMR header + o_frm (typ. 8 = right after header). */
    size_t kf_off  = (size_t)r0_emr_off + (size_t)o_frm;
    if (kf_off > r0_edd_off) return -10;
    size_t kf_size = (size_t)r0_edd_off - kf_off;
    int kf_count   = (int)(kf_size / (size_t)ksize);
    if (kf_count <= 0) return -11;

    /* Phase 4.5.13-RE2 H-REVERT (2026-05-21): hybrid theory was wrong.
     * rbj is only 22720 bytes — too small to mirror PL00.EDD's 24-clip
     * byte-offset layout (which spans ~57000 bytes). PL00.EDD clip 15
     * byte_offset is ~42000, OOB in rbj → garbage / static.
     *
     * Revert to FULL-REPLACE: rbj has its OWN EDD (Record 0 = 26 cinematic
     * clips) and its OWN keyframe pool (366 keyframes). When loaded for the
     * cinematic actor, BOTH replace PL00. User confirmed this gives
     * "wave/pointing correctly" when sub02 fires Plc_motion(0, 15, 0).
     *
     * Trade-off: gameplay walks (PL00.EDD clip 10/12) are lost while
     * rbj is active. That's fine for the cinematic; once the cinematic
     * ends (Plc_ret), we'd un-overlay rbj (NOT yet implemented). */
    *out_skel = *base_skel;
    out_skel->keyframe_size_bytes = (int)ksize;
    out_skel->keyframe_count      = kf_count;
    out_skel->keyframe_data       = rbj_data + kf_off;
    out_skel->keyframe_data_size  = kf_size;

    /* Parse rbj's own EDD (Record 0 = 26-clip cinematic table). */
    if (out_anim) {
        size_t edd_remaining = rbj_size - r0_edd_off;
        return re15_emd_parse_animation(rbj_data + r0_edd_off, edd_remaining, out_anim);
    }
    return 0;
}

/* Back-compat wrapper: parse RECORD 0 (the idle/player overlay — ROOM1170 Leon+Elliot). */
int re15_emd_parse_rbj(const uint8_t *rbj_data, size_t rbj_size,
                       const re15_emd_skeleton_t *base_skel,
                       re15_emd_skeleton_t *out_skel,
                       re15_emd_animation_t *out_anim)
{
    return re15_emd_parse_rbj_record(rbj_data, rbj_size, 0, base_skel, out_skel, out_anim);
}

/* Full EMD/PLD container parser — see re15_emd.h. */
int re15_emd_parse_container(const uint8_t *emd, size_t emd_size,
                             re15_md1_t           *out_md1,
                             re15_emd_skeleton_t  *out_skel,
                             re15_emd_animation_t *out_anim,
                             re15_tim_t           *out_tim)
{
    if (!emd || emd_size < 12) return -1;
    uint32_t begin = read_u32_le(emd + 0);
    uint32_t count = read_u32_le(emd + 4);
    if (count < 4 || count > 64) return -1;
    if ((size_t)begin + (size_t)count * 4 > emd_size) return -1;

    uint32_t off_edd, off_emr, off_md1 = 0, off_tim = 0, off_kf_bank = 0;
    if (count == 4) {                                  /* PLD: [EDD,EMR,MD1,TIM] */
        off_edd = read_u32_le(emd + begin + 0 * 4);
        off_emr = read_u32_le(emd + begin + 1 * 4);
        off_md1 = read_u32_le(emd + begin + 2 * 4);
        off_tim = read_u32_le(emd + begin + 3 * 4);
    } else {
        /* em<NN>: up to THREE animation banks — (EDD,EMR) pairs at directory indices
         * (1,2), (3,4), (5,6) — then MD1=7, TIM=8 (matches the Java EmdFileParser:
         * animIndex=1+i*2, skelIndex=2+i*2). The enemy's MAIN animation bank (the one the
         * STAGE1.BIN AI indexes via entity+0x94 — e.g. zombie action 19/39, which only fit a
         * 40+ clip table) is NOT always bank 0: em10 bank0=6 clips but bank1=43; em45 bank0=24.
         * The main set is the bank with the MOST EDD clips; the smaller banks are secondary
         * (shared poses / death variants). Pick the largest-EDD bank and use ITS matching EMR
         * (the per-frame bone keyframes live in the same bank as their EDD). */
        int best_bank = 0, best_clips = -1;
        for (int b = 0; b < 3; b++) {
            int ei = 1 + b * 2;
            if (ei >= (int)count) break;
            uint32_t eoff = read_u32_le(emd + begin + ei * 4);
            if (!eoff || (size_t)eoff + 4 > emd_size) continue;
            int clips = (int)read_u16_le(emd + eoff + 2) / 4;     /* offset0/4 = clip count */
            if (clips > best_clips) { best_clips = clips; best_bank = b; }
        }
        off_edd = read_u32_le(emd + begin + (1 + best_bank * 2) * 4);
        /* The skeleton STRUCTURE (bind pose, bone hierarchy, mesh binding) is SHARED across
         * all banks — it lives in bank 0's EMR (dir[2], keyframes_offset≈176 = structure +
         * bank0 keyframes). The OTHER banks' EMRs (dir[4]/dir[6]) are keyframe POOLS ONLY
         * (keyframes_offset≈8 = no bone table); parsing them as a skeleton yields a garbage
         * bind pose → the scattered/broken mesh. So always take the structure from dir[2] and,
         * for a non-zero bank, re-point the keyframe pool to that bank's EMR below. */
        off_emr     = read_u32_le(emd + begin + 2 * 4);            /* shared skeleton struct */
        off_kf_bank = read_u32_le(emd + begin + (2 + best_bank * 2) * 4); /* this bank's keyframes */
        if (count > 7) off_md1 = read_u32_le(emd + begin + 7 * 4);
        if (count > 8) off_tim = read_u32_le(emd + begin + 8 * 4);
    }

    int ok_anim = 0, ok_skel = 0, ok_md1 = 0;
    if (out_anim && off_edd && off_edd < emd_size)
        ok_anim = (re15_emd_parse_animation(emd + off_edd, emd_size - off_edd, out_anim) == 0);
    if (out_skel && off_emr && off_emr < emd_size) {
        ok_skel = (re15_emd_parse_skeleton(emd + off_emr, emd_size - off_emr, out_skel) == 0);
        /* Re-point the keyframe pool to the SELECTED bank's EMR (the shared structure carries
         * only bank 0's keyframes). The bank EMR's own keyframes_offset (u16 @ +2) locates its
         * pool; keyframe_size is the same across banks. */
        if (ok_skel && off_kf_bank && off_kf_bank != off_emr && (size_t)off_kf_bank + 8 < emd_size) {
            int kf_off = (int)read_u16_le(emd + off_kf_bank + 2);
            const uint8_t *kf = emd + off_kf_bank + kf_off;
            if ((size_t)(off_kf_bank + kf_off) < emd_size && out_skel->keyframe_size_bytes > 0) {
                out_skel->keyframe_data      = kf;
                out_skel->keyframe_data_size = emd_size - (size_t)(off_kf_bank + kf_off);
                out_skel->keyframe_count     = (int)(out_skel->keyframe_data_size /
                                                     (size_t)out_skel->keyframe_size_bytes);
            }
        }
    }
    if (out_md1 && off_md1 && off_md1 < emd_size)
        ok_md1 = (re15_md1_parse(emd + off_md1, (int)(emd_size - off_md1), out_md1) == 0);
    if (out_tim && off_tim && off_tim < emd_size)
        re15_tim_parse(emd + off_tim, (int)(emd_size - off_tim), out_tim);   /* optional */

    return (ok_anim && ok_skel && ok_md1) ? 0 : -1;
}
