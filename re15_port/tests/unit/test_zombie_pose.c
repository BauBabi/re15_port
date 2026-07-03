/**
 * @file test_zombie_pose.c
 * @brief POSE ORACLE regression — the em10 loco-bank (bank0) AWARE-WALK pose must reproduce the
 *        live PSX pose (adversarial P1: engage zombie +0x94=2 +0x95=10 == bank0 clip2 frame9 kf97,
 *        0-delta over 15 bones; the pose pool @0x80157fe8 in mzd_stage1_engage_live.sav).
 *
 * The user-visible bug this guards: the walking zombie rendered UPRIGHT in the port while the
 * original is HUNCHED — the authored kf97 pose has the head bones barely above the pelvis and
 * displaced far horizontally (python FK ground truth, 2026-07-04):
 *   b8 chest (-37,-2385,-383)  b9 head (255,-1986,-533)  b10 (659,-1983,-643)
 *   b12 arm (515,-2058,342)    b13 arm (933,-2033,322)   root (0,-1823,0)
 * If re15_skel_compute_pose returns an upright figure (head ~directly above pelvis, arms down)
 * the loco-bank FK is broken (hierarchy/angle decode/root).
 */
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

int main(void)
{
    long sz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &sz);
    if (!ems) { fprintf(stderr, "FAIL: cannot open CDEMD0.EMS\n"); return 1; }
    size_t off = 0, len = 0;
    if (re15_ems_get_entry(ems, (size_t)sz, re15_ems_index_for_type(0x10), &off, &len) != 0) {
        fprintf(stderr, "FAIL: em10 not found in CDEMD0.EMS\n"); return 1; }

    static re15_emd_skeleton_t  skel;
    static re15_emd_animation_t anim;
    if (re15_emd_parse_loco_bank(ems + off, len, &skel, &anim) != 0) {
        fprintf(stderr, "FAIL: em10 loco-bank parse\n"); return 1; }

    int fail = 0;
    printf("=== em10 loco bank: %d clips, %d bones ===\n", anim.clip_count, skel.bone_count);
    if (anim.clip_count < 6 || skel.bone_count != 15) {
        fprintf(stderr, "FAIL: expected 6 clips / 15 bones, got %d/%d\n",
                anim.clip_count, skel.bone_count); fail = 1; }

    /* clip 2 frame 9 -> the live-verified keyframe 97 */
    uint32_t fe = re15_emd_get_frame_entry(&anim, 2, 9);
    int kf = (int)(fe & 0xFFFu);
    printf("clip2 frame9 -> kf %d (live PSX: 97)\n", kf);
    if (kf != 97) { fprintf(stderr, "FAIL: kf must be 97, ist %d\n", kf); fail = 1; }

    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    extern void *g_anim_pose_actor;
    g_anim_pose_actor = NULL;                       /* pure pose query, no crossfade */
    if (re15_skel_compute_pose(&skel, kf, poses) != 0) {
        fprintf(stderr, "FAIL: compute_pose\n"); return 1; }
    for (int b = 0; b < 15; b++)
        printf("  b%-2d (%6d,%6d,%6d)\n", b,
               (int)poses[b].trans[0], (int)poses[b].trans[1], (int)poses[b].trans[2]);

    /* THE HUNCH METRICS (python-FK ground truth vs the live PSX pose pool, both agree):
     *  - head bone 9 sits BARELY above the pelvis: |y9 - y0| < 400 (truth: 163)
     *  - head bone 9 is displaced FAR horizontally: |dx|+|dz| > 500 (truth: 255+533=788)
     *  - forearm bone 13 reaches far out: |x13-x0| > 600 (truth: 933)
     * An UPRIGHT render (the bug) puts the head ~600+ straight above the pelvis with small
     * horizontal offset and the arms near the body. Tolerances are wide (fixed-point trig). */
    int32_t px = poses[0].trans[0], py = poses[0].trans[1], pz = poses[0].trans[2];
    int32_t hdy = poses[9].trans[1] - py;
    int32_t hdx = poses[9].trans[0] - px, hdz = poses[9].trans[2] - pz;
    int32_t adx = poses[13].trans[0] - px;
    if (hdy < 0) hdy = -hdy; if (hdx < 0) hdx = -hdx; if (hdz < 0) hdz = -hdz; if (adx < 0) adx = -adx;
    printf("hunch metrics: head dy=%d (want <400), head |dx|+|dz|=%d (want >500), arm |dx|=%d (want >600)\n",
           (int)hdy, (int)(hdx + hdz), (int)adx);
    if (hdy >= 400) { fprintf(stderr, "FAIL: head sits too high above pelvis (upright render), dy=%d\n", (int)hdy); fail = 1; }
    if (hdx + hdz <= 500) { fprintf(stderr, "FAIL: head not displaced horizontally (no hunch), %d\n", (int)(hdx+hdz)); fail = 1; }
    if (adx <= 600) { fprintf(stderr, "FAIL: arm not reaching forward, |dx|=%d\n", (int)adx); fail = 1; }

    free(ems);
    if (fail) { fprintf(stderr, "\nZOMBIE POSE TEST FAILED\n"); return 1; }
    printf("\nPASS: em10 loco clip2/kf97 reproduces the live PSX hunched aware-walk pose\n");
    return 0;
}
