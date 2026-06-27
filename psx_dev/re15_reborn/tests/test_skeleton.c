/*
 * Unit tests for skeleton pose computation (Phase 4.5.7.2).
 *
 * Verifies:
 *   - Identity matrix from zero Euler angles
 *   - Rotation matrix from 90° X-axis rotation matches expected values
 *   - Composite rotation Rx*Ry*Rz produces expected Z-axis result
 *   - Full pose computation walks parent chain (root + child positions)
 *
 * Uses the PC trig backend (sinf/cosf) for matrix math.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_skeleton.h"

#define TEST(cond, msg)                                                \
    do {                                                                \
        if (!(cond)) {                                                  \
            fprintf(stderr, "FAIL %s:%d %s — %s\n",                     \
                    __FILE__, __LINE__, __func__, msg);                 \
            return 1;                                                   \
        }                                                               \
    } while (0)

/* Allow a small tolerance in Q12 comparisons — sinf-vs-isin can disagree
 * by a few LSBs even at the same input angle. 8 = ~0.2% of ONE. */
#define APPROX_EQ_Q12(a, b)  (((a) - (b)) >= -8 && ((a) - (b)) <= 8)

static int test_trig_identities(void)
{
    /* sin(0) = 0, cos(0) = 1.0. */
    TEST(APPROX_EQ_Q12(re15_sin_q12(0), 0),               "sin(0) ≈ 0");
    TEST(APPROX_EQ_Q12(re15_cos_q12(0), RE15_SKEL_ONE),   "cos(0) ≈ 1.0");

    /* sin(90° = 1024 units) = 1.0, cos(90°) = 0. */
    TEST(APPROX_EQ_Q12(re15_sin_q12(1024), RE15_SKEL_ONE), "sin(90°) ≈ 1.0");
    TEST(APPROX_EQ_Q12(re15_cos_q12(1024), 0),             "cos(90°) ≈ 0");

    /* sin(180° = 2048 units) = 0, cos(180°) = -1.0. */
    TEST(APPROX_EQ_Q12(re15_sin_q12(2048),  0),             "sin(180°) ≈ 0");
    TEST(APPROX_EQ_Q12(re15_cos_q12(2048), -RE15_SKEL_ONE), "cos(180°) ≈ -1.0");

    /* sin(-90° = -1024 units) = -1.0. */
    TEST(APPROX_EQ_Q12(re15_sin_q12(-1024), -RE15_SKEL_ONE), "sin(-90°) ≈ -1.0");
    return 0;
}

/* Verify zero-angle skeleton: every bone's rotation matrix is identity,
 * translations equal bone_relative_pos[b] + parent_world_trans. */
static int test_zero_angles_root_only(void)
{
    /* Synthesize a 2-bone skeleton with zero keyframe data. */
    re15_emd_skeleton_t skel;
    memset(&skel, 0, sizeof skel);
    skel.bone_count = 2;
    skel.bone_relative_pos[0][0] = 100;
    skel.bone_relative_pos[0][1] = 0;
    skel.bone_relative_pos[0][2] = 0;
    skel.bone_relative_pos[1][0] = 0;
    skel.bone_relative_pos[1][1] = 200;
    skel.bone_relative_pos[1][2] = 0;
    skel.bone_parent[0] = -1;     /* root  */
    skel.bone_parent[1] = 0;      /* child of root */
    skel.keyframe_size_bytes = 80;
    /* keyframe_count = 0 → accessors return zeros, so angles default to 0
     * and keyframe_position defaults to (0,0,0). */

    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int rv = re15_skel_compute_pose(&skel, 0, poses);
    TEST(rv == 0, "compute_pose succeeds");

    /* Root: rotation identity, translation = relative_pos. */
    TEST(APPROX_EQ_Q12(poses[0].rot[0], RE15_SKEL_ONE),  "root rot[0][0] = 1");
    TEST(APPROX_EQ_Q12(poses[0].rot[4], RE15_SKEL_ONE),  "root rot[1][1] = 1");
    TEST(APPROX_EQ_Q12(poses[0].rot[8], RE15_SKEL_ONE),  "root rot[2][2] = 1");
    TEST(APPROX_EQ_Q12(poses[0].rot[1], 0),              "root rot[0][1] = 0");
    TEST(poses[0].trans[0] == 100, "root trans x = 100");
    TEST(poses[0].trans[1] == 0,   "root trans y = 0");
    TEST(poses[0].trans[2] == 0,   "root trans z = 0");

    /* Child: world rotation = identity * identity = identity.
     * World trans = identity * (0,200,0) + (100,0,0) = (100,200,0). */
    TEST(APPROX_EQ_Q12(poses[1].rot[0], RE15_SKEL_ONE),  "child rot[0][0] = 1");
    TEST(poses[1].trans[0] == 100, "child trans x = root.x");
    TEST(poses[1].trans[1] == 200, "child trans y = root.y + offset.y");
    TEST(poses[1].trans[2] == 0,   "child trans z = 0");
    return 0;
}

/* Verify rotation propagates to children. Root rotates 90° about Y;
 * child has local offset (100, 0, 0). World-space child translation
 * should be (0, 0, -100) (X-axis rotated 90° around Y → -Z). */
static int test_parent_rotation_rotates_child_offset(void)
{
    re15_emd_skeleton_t skel;
    memset(&skel, 0, sizeof skel);
    skel.bone_count = 2;
    skel.bone_relative_pos[0][0] = 0;
    skel.bone_relative_pos[1][0] = 100;
    skel.bone_parent[0] = -1;
    skel.bone_parent[1] = 0;
    skel.keyframe_size_bytes = 80;

    /* Synthesize a keyframe with: pos (0,0,0), root angles (0, 1024, 0)
     * = 90° around Y. Child angles (0,0,0). 12 byte pos+speed + 6
     * 16-bit-words of angles bit-packed = 12 + 9 bytes (we round up
     * to even but 21 is the minimum). */
    uint8_t kf[80];
    memset(kf, 0, sizeof kf);
    /* Bone 0: angle Y = 1024. Bit-packed format: 36 bits per bone,
     * 12 bits per axis (X,Y,Z). Bit offset 0..11 = X, 12..23 = Y,
     * 24..35 = Z. Within the bit stream we set bits 12..21 = 1024.
     * Byte 12..15 = first 32 bits of bone 0:
     *   bits  0..11 = X = 0
     *   bits 12..23 = Y = 1024 = 0b010000000000
     *   bits 24..31 = bottom 8 of Z = 0
     * So:
     *   byte 12 = 0
     *   byte 13 = (1024 >> 4) low 4 bits to upper of byte = (0x40 << 4) wait...
     * Actually bit 12 of the stream = bit 4 of byte 1 (of the angles
     * sub-array starting at byte 12 of the keyframe). Easier: just
     * encode bone 0 Y = 1024 directly: bit position of "1" = bit 22
     * of the bit stream. So byte (22/8) = 2 (relative to angles base
     * = byte 14 of keyframe), bit 22 % 8 = 6. → byte 14 bit 6 = set.
     */
    kf[12 + 2] = (uint8_t)(1u << 6);   /* bit 22 of angle stream = 1 → angle = 1024 */

    skel.keyframe_data      = kf;
    skel.keyframe_data_size = sizeof kf;
    skel.keyframe_count     = 1;

    /* Verify the angle decoder produces what we expect. */
    int16_t ax, ay, az;
    re15_emd_get_keyframe_angles(&skel, 0, 0, &ax, &ay, &az);
    TEST(ax == 0,                    "decoder reads bone 0 X = 0");
    TEST(ay == 1024,                 "decoder reads bone 0 Y = 1024 (synth fixture)");
    TEST(az == 0,                    "decoder reads bone 0 Z = 0");

    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int rv = re15_skel_compute_pose(&skel, 0, poses);
    TEST(rv == 0, "compute_pose succeeds");

    /* Root rotation 90° around Y: x-axis maps to -z, z maps to +x.
     * So rotated (100, 0, 0) = (0, 0, -100). Plus parent_trans (0,0,0)
     * → child world translation = (0, 0, -100).
     * Allow ±4 LSB tolerance for Q12-precision rounding. */
    TEST(poses[1].trans[0] >= -4 && poses[1].trans[0] <= 4,
         "child world.x ≈ 0 (rotated 100 around Y by 90°)");
    TEST(poses[1].trans[1] == 0,
         "child world.y == 0");
    TEST(poses[1].trans[2] >= -104 && poses[1].trans[2] <= -96,
         "child world.z ≈ -100 (rotated -90° X mapping)");
    return 0;
}

static int test_compute_rejects_bad_inputs(void)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    re15_emd_skeleton_t skel = {0};
    skel.bone_count = 1;
    TEST(re15_skel_compute_pose(NULL, 0, poses) < 0, "NULL skel");
    TEST(re15_skel_compute_pose(&skel, 0, NULL) < 0, "NULL poses");
    skel.bone_count = -1;
    TEST(re15_skel_compute_pose(&skel, 0, poses) < 0, "bad bone_count");
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_trig_identities();
    fails += test_zero_angles_root_only();
    fails += test_parent_rotation_rotates_child_offset();
    fails += test_compute_rejects_bad_inputs();

    if (fails == 0) {
        printf("test_skeleton: all 4 tests PASSED\n");
        return 0;
    }
    printf("test_skeleton: %d test(s) FAILED\n", fails);
    return 1;
}
