/*
 * Unit tests for the EDD + EMR parsers (Phase 4.5.7.1).
 *
 * Real-asset integration test: parses bundled PL00.edd + PL00.emr (Leon
 * player model from RE1.5). Verifies clip + bone counts, then samples
 * the first keyframe's bone angles for non-zero data sanity.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_emd.h"

#define TEST(cond, msg)                                                \
    do {                                                                \
        if (!(cond)) {                                                  \
            fprintf(stderr, "FAIL %s:%d %s — %s\n",                     \
                    __FILE__, __LINE__, __func__, msg);                 \
            return 1;                                                   \
        }                                                               \
    } while (0)

#ifndef RE15_TEST_EDD_PATH
# error "Test must be built with RE15_TEST_EDD_PATH defined"
#endif
#ifndef RE15_TEST_EMR_PATH
# error "Test must be built with RE15_TEST_EMR_PATH defined"
#endif

static uint8_t *read_file(const char *path, long *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *) malloc((size_t)sz);
    if (buf) fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (out_size) *out_size = sz;
    return buf;
}

static int test_parse_pl00_edd(void)
{
    long sz;
    uint8_t *buf = read_file(RE15_TEST_EDD_PATH, &sz);
    TEST(buf != NULL, "read PL00.edd");
    TEST(sz == 3160, "PL00.edd is 3160 bytes (known fixture)");

    re15_emd_animation_t anim;
    int rv = re15_emd_parse_animation(buf, (size_t)sz, &anim);
    TEST(rv == 0, "EDD parser succeeds");

    /* PL00 has 24 animation clips per Leon's RE1.5 build. */
    TEST(anim.clip_count == 24,            "PL00 has 24 clips");
    TEST(anim.clips[0].first_frame == 0,   "clip 0 starts at frame 0");
    TEST(anim.clips[0].frame_count == 34,  "clip 0 has 34 frames");
    TEST(anim.frame_count > 0,             "frame list non-empty");
    TEST(anim.last_keyframe_index > 0,     "last_keyframe_index computed");

    free(buf);
    return 0;
}

static int test_parse_pl00_emr(void)
{
    long sz;
    uint8_t *buf = read_file(RE15_TEST_EMR_PATH, &sz);
    TEST(buf != NULL, "read PL00.emr");
    TEST(sz == 57136, "PL00.emr is 57136 bytes (known fixture)");

    re15_emd_skeleton_t skel;
    int rv = re15_emd_parse_skeleton(buf, (size_t)sz, &skel);
    TEST(rv == 0, "EMR parser succeeds");

    /* Leon's PL00.emr has 15 bones, 80-byte keyframes. */
    TEST(skel.bone_count == 15,            "PL00 has 15 bones");
    TEST(skel.keyframe_size_bytes == 80,   "keyframe = 80 bytes (12 pos+speed + 68 angles)");
    TEST(skel.keyframe_count > 0,          "non-zero keyframe count");

    /* At least one bone must have a parent (skeleton must be hierarchical). */
    int non_root_bones = 0;
    int root_bones = 0;
    for (int i = 0; i < skel.bone_count; i++) {
        if (skel.bone_parent[i] >= 0) non_root_bones++;
        else                          root_bones++;
    }
    TEST(non_root_bones > 0, "at least one non-root bone");
    TEST(root_bones >= 1,    "at least one root bone");

    /* First keyframe should have non-trivial angles for at least one bone. */
    int16_t total_x = 0, total_y = 0, total_z = 0;
    for (int b = 0; b < skel.bone_count; b++) {
        int16_t ax, ay, az;
        re15_emd_get_keyframe_angles(&skel, 0, b, &ax, &ay, &az);
        total_x += ax;
        total_y += ay;
        total_z += az;
    }
    TEST((total_x | total_y | total_z) != 0,
         "first-keyframe Euler angles are not all zero (animation has motion)");

    /* Frame 0 position should also read. */
    int16_t px, py, pz;
    int rv2 = re15_emd_get_keyframe_position(&skel, 0, &px, &py, &pz);
    TEST(rv2 == 1, "keyframe-position accessor succeeds for frame 0");

    free(buf);
    return 0;
}

static int test_parse_rejects_bad(void)
{
    re15_emd_animation_t anim;
    re15_emd_skeleton_t  skel;
    TEST(re15_emd_parse_animation(NULL, 100, &anim)   < 0, "NULL edd rejected");
    TEST(re15_emd_parse_animation((const uint8_t *)"x", 3, &anim) < 0, "tiny edd rejected");
    TEST(re15_emd_parse_skeleton (NULL, 100, &skel)   < 0, "NULL emr rejected");
    uint8_t bad[16] = {0};   /* all zeros: bonesOffset=0, boneCount=0 */
    TEST(re15_emd_parse_skeleton(bad, sizeof bad, &skel) < 0, "zero bones rejected");
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_parse_pl00_edd();
    fails += test_parse_pl00_emr();
    fails += test_parse_rejects_bad();

    if (fails == 0) {
        printf("test_emd: all 3 tests PASSED\n");
        return 0;
    }
    printf("test_emd: %d test(s) FAILED\n", fails);
    return 1;
}
