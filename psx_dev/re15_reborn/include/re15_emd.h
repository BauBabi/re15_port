/*
 * RE1.5 Rebuilt — EMD/PLD skeleton + animation parser
 * (Phase 4.5.7.1, 2026-05-18).
 *
 * EMD (Enemy Model Data) and PLD (Player Model Data) share the same
 * sub-container layout. The PLD's component files are:
 *
 *   .md1   — multi-mesh geometry (one mesh per bone)
 *   .tim   — texture
 *   .emr   — skeleton + keyframe poses
 *   .edd   — animation clip table + per-frame keyframe-index list
 *
 * This header covers the EDD + EMR formats. MD1 + TIM parsers exist
 * already (re15_md1.h / re15_tim.h).
 *
 *   EDD layout (1-second summary):
 *     +0   u16  clip_0_frame_count
 *     +2   u16  clip_table_size_bytes (= clip_count * 4)
 *     +4   u16  clip_1_frame_count
 *     +6   u16  reserved
 *     ... (clip_count-1 pairs of count+reserved)
 *     +clip_table_size_bytes: frames[] = packed u32 (12-bit keyframe index + flags)
 *
 *   EMR layout:
 *     +0   u16  bones_table_offset      (relative to EMR start)
 *     +2   u16  keyframes_offset        (relative to EMR start)
 *     +4   u16  bone_count              (15 for Leon)
 *     +6   u16  keyframe_size_bytes     (typ. 80 = 12 pos+speed + N angle bytes)
 *     +8   16  bone_relative_positions[bone_count] × 6 bytes (s16 x,y,z)
 *     +bones_table_offset: per-bone (u16 mesh_count, u16 child_list_offset)
 *     +keyframes_offset: per-keyframe (s16 px,py,pz, s16 vx,vy,vz, then
 *                       bit-packed Euler angles 12 bits each x/y/z per bone)
 *
 * Animation keyframes use 12-bit packed Euler angles. 4096 units = 360°
 * (the PSX RotMatrix convention).
 *
 * Reference: src/main/java/de/re15/extractors/emd/*.java (in-tree).
 */
#ifndef RE15_EMD_H
#define RE15_EMD_H

#include <stdint.h>
#include <stddef.h>
#include "re15_md1.h"   /* re15_md1_t — EMD container mesh section */
#include "re15_tim.h"   /* re15_tim_t — EMD container texture section */

#define RE15_EMD_MAX_BONES      32
#define RE15_EMD_MAX_CLIPS      64
#define RE15_EMD_MAX_FRAMES     1664   /* enemy MAIN anim banks are large (em10 bank1 = 1604
                                        * frame entries across 43 clips); the old 1024 cap made
                                        * the EDD parse return -6 → enemy fell back to Leon. Kept
                                        * just above 1604 — frames[] sits in EVERY anim struct so
                                        * the PSX _end must stay < 0x80200000 (2 MB). */
#define RE15_EMD_FULL_TURN      4096

/* Animation clip = a logical animation (e.g., "idle", "walk", "shoot"). */
typedef struct {
    int      first_frame;       /* index into frames[]                   */
    int      frame_count;       /* number of frames in this clip         */
} re15_emd_clip_t;

/* Parsed EDD. `frames[i]` references a keyframe pose in the EMR. */
typedef struct {
    int              clip_count;
    re15_emd_clip_t  clips[RE15_EMD_MAX_CLIPS];
    int              frame_count;
    uint32_t         frames[RE15_EMD_MAX_FRAMES];
    int              last_keyframe_index;   /* max(frames[i] & 0xFFF)   */
} re15_emd_animation_t;

/* Parsed EMR. */
typedef struct {
    int              bone_count;
    int16_t          bone_relative_pos[RE15_EMD_MAX_BONES][3]; /* s16 PSX units */
    int8_t           bone_parent[RE15_EMD_MAX_BONES];          /* -1 if root */
    int8_t           bone_mesh_index[RE15_EMD_MAX_BONES];      /* MD1 mesh idx this bone deforms; -1 if none */
    int              keyframe_size_bytes;
    int              keyframe_count;
    const uint8_t   *keyframe_data;        /* points into EMR buffer (caller-owned) */
    size_t           keyframe_data_size;
} re15_emd_skeleton_t;

/* Parse the EDD animation table.
 * Returns 0 on success, negative on error (truncated buffer / bad header). */
int re15_emd_parse_animation(const uint8_t *edd_data, size_t edd_size,
                             re15_emd_animation_t *out);

/* Parse the EMR skeleton + bone-relative positions + parent hierarchy.
 * The keyframe_data pointer is stored as a sub-pointer into the input
 * buffer — caller must keep emr_data alive while using out->keyframe_data.
 * Returns 0 on success, negative on error. */
int re15_emd_parse_skeleton(const uint8_t *emr_data, size_t emr_size,
                            re15_emd_skeleton_t *out);

/* Read 12-bit-packed Euler angles for one bone in one keyframe.
 * Outputs are in PSX trig units (0..4095 = 360°). When the requested
 * keyframe/bone is out of range, outputs are set to 0 and the function
 * returns 0; on success returns 1. */
int re15_emd_get_keyframe_angles(const re15_emd_skeleton_t *skel,
                                  int keyframe_index, int bone_index,
                                  int16_t *out_x, int16_t *out_y,
                                  int16_t *out_z);

/* Read the per-keyframe position vector (px, py, pz) in s16 PSX units. */
int re15_emd_get_keyframe_position(const re15_emd_skeleton_t *skel,
                                    int keyframe_index,
                                    int16_t *out_x, int16_t *out_y,
                                    int16_t *out_z);

/* Raw packed FRAME ENTRY for clip[clip], frame_in_clip (frames[first_frame+f]):
 * bits 0-11 = keyframe index, upper bits = FLAGS. Byte-true to the walk/run
 * handlers LAB_80030af0/d28: bit 0x4000 = FOOT-PLANT (footstep-SE trigger),
 * bit 0x1000 = which-foot. Returns 0 if out of range. */
uint32_t re15_emd_get_frame_entry(const re15_emd_animation_t *anim,
                                  int clip, int frame_in_clip);

/* Phase 4.5.13-RE2 I3: read the per-keyframe SPEED vector (bytes 6..11
 * of each keyframe, s16 LE). Speed drives engine-side root translation:
 * each tick the engine reads the current keyframe's speed and adds
 * (yaw-rotated) to the actor's world position. rbj cinematic clips
 * (e.g. clip 15 for ROOM1170 sub02 walk) use speed for locomotion;
 * the pos field at bytes 0..5 is typically zero. */
int re15_emd_get_keyframe_speed(const re15_emd_skeleton_t *skel,
                                 int keyframe_index,
                                 int16_t *out_sx, int16_t *out_sy,
                                 int16_t *out_sz);

/* Phase 4.5.13-B2: parse a room-local animation.rbj file (RDT-embedded
 * cinematic anim container). RBJ is multi-record; we extract record 1
 * (Leon's cinematic keyframes) and overlay it on the supplied base
 * skeleton (PL00.emr's bone hierarchy). The result is a skeleton with
 * base bone_rel_pos + parents + mesh map, but the rbj's keyframe data
 * and clip table.
 *
 * Layout (per phase4_5_13_rbj_byte_layout memory):
 *   0x00 u32 total_length
 *   0x04 u32 record_count (= 2 for ROOM1170)
 *   0x08 u32 ?
 *   0x0C u32 ?
 *   ...  record 0 data
 *   ...  record 1 EMR header + keyframes + EDD
 *   trailer @ total_length: [?, R0_EDD, R1_EMR_prefix, R1_EDD]
 *   R1_EMR header proper = R1_EMR_prefix + 4
 *   R1 keyframes = R1_EMR + 8 (after 8-byte EMR header)
 *
 * Returns 0 on success, negative on error.
 * `out_skel` is filled with base_skel's hierarchy + rbj's keyframes.
 * `out_anim` is filled with rbj's EDD clip table.
 * `rbj_data` MUST stay alive — out_skel->keyframe_data points into it. */
int re15_emd_parse_rbj(const uint8_t *rbj_data, size_t rbj_size,
                       const re15_emd_skeleton_t *base_skel,
                       re15_emd_skeleton_t *out_skel,
                       re15_emd_animation_t *out_anim);

/* As above but selects RBJ RECORD `record_index` (0 = idle/player overlay; 1 = the per-entity
 * override the RBJ header bitmask names — ROOM1150 RECORD 1 = the prone wounded-lying Irons). */
int re15_emd_parse_rbj_record(const uint8_t *rbj_data, size_t rbj_size, int record_index,
                       const re15_emd_skeleton_t *base_skel,
                       re15_emd_skeleton_t *out_skel,
                       re15_emd_animation_t *out_anim);

/* Parse a FULL EMD/PLD container (an em<NN>.emd or PLD file as a single blob) and
 * extract + parse its PRIMARY mesh + skeleton + animation + texture. Layout (Java
 * EmdFileParser): begin=u32@0 (offset to the section-offset directory), count=u32@4,
 * then `count` u32 section offsets at `begin`. count==4 → PLD [EDD,EMR,MD1,TIM];
 * else (em<NN>) → EDD=offsets[1], EMR=offsets[2], MD1=offsets[7], TIM=offsets[8]
 * (the 1/3/5 + 2/4/6 alternates are extra anim/skel sets, unused). Any out param may
 * be NULL. Returns 0 if mesh+skeleton+animation all parsed (texture optional), else
 * negative. Lets the engine CD-load any enemy/NPC model without an external split. */
int re15_emd_parse_container(const uint8_t *emd_data, size_t emd_size,
                             re15_md1_t           *out_md1,
                             re15_emd_skeleton_t  *out_skel,
                             re15_emd_animation_t *out_anim,
                             re15_tim_t           *out_tim);

#endif /* RE15_EMD_H */
