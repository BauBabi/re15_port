/*
 * Unit tests for the VAB parser + PSX ADPCM decoder (Phase 4.6.2).
 *
 * Verifies:
 *   - Header signature check, byte-order
 *   - Sample size table parsing (DOOR00 has 2 VAGs)
 *   - Sample offsets accumulate correctly
 *   - ADPCM decoder produces sane PCM (non-zero, in-range)
 *
 * Real-asset integration test: reads ../assets/test.vh and ../assets/test.vb
 * (DOOR00 VAB from RE1.5 stage1).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_vab.h"

#define TEST(cond, msg)                                                \
    do {                                                                \
        if (!(cond)) {                                                  \
            fprintf(stderr, "FAIL %s:%d %s — %s\n",                     \
                    __FILE__, __LINE__, __func__, msg);                 \
            return 1;                                                   \
        }                                                               \
    } while (0)

#ifndef RE15_TEST_VH_PATH
# error "Test must be built with RE15_TEST_VH_PATH defined"
#endif
#ifndef RE15_TEST_VB_PATH
# error "Test must be built with RE15_TEST_VB_PATH defined"
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

static int test_header_rejects_bad_signature(void)
{
    uint8_t bad[64];
    memset(bad, 0, sizeof bad);
    re15_vab_t vab;
    TEST(re15_vab_parse(bad, sizeof bad, &vab) == -2, "bad signature rejected");
    TEST(re15_vab_parse(bad, 16,         &vab) == -1, "size < 32 rejected");
    TEST(re15_vab_parse(NULL, 64,        &vab) == -4, "NULL data rejected");
    TEST(re15_vab_parse(bad, 64,         NULL) == -4, "NULL out rejected");
    return 0;
}

static int test_parse_real_door00_vh(void)
{
    long vh_size;
    uint8_t *vh = read_file(RE15_TEST_VH_PATH, &vh_size);
    TEST(vh != NULL,    "read test.vh from disk");
    TEST(vh_size == 3104, "DOOR00.vh is 3104 bytes (known fixture)");

    re15_vab_t vab;
    int rv = re15_vab_parse(vh, (size_t)vh_size, &vab);
    TEST(rv == 0, "parser succeeds");

    TEST(vab.form == RE15_VAB_SIGNATURE,  "signature = pBAV");
    TEST(vab.program_count == 1,          "DOOR00 has 1 program");
    TEST(vab.vag_count    == 2,           "DOOR00 has 2 samples");
    TEST(vab.samples[0].offset == 0,      "first sample at offset 0");
    TEST(vab.samples[0].size > 0,         "first sample non-zero size");
    TEST(vab.samples[1].offset == vab.samples[0].size,
         "second sample starts after first");
    TEST(vab.vb_total_bytes <= 18112,
         "total ≤ VB file size (18112)");

    free(vh);
    return 0;
}

static int test_decode_first_sample(void)
{
    long vh_size, vb_size;
    uint8_t *vh = read_file(RE15_TEST_VH_PATH, &vh_size);
    uint8_t *vb = read_file(RE15_TEST_VB_PATH, &vb_size);
    TEST(vh != NULL && vb != NULL, "load both VAB files");
    TEST(vb_size == 18112,           "DOOR00.vb is 18112 bytes");

    re15_vab_t vab;
    TEST(re15_vab_parse(vh, (size_t)vh_size, &vab) == 0, "parse vh");

    /* Decode the first sample to PCM. Each 16-byte ADPCM frame yields
     * 28 PCM samples. Capacity = (size / 16) * 28 + slack. */
    size_t worst_pcm = (vab.samples[0].size / 16) * 28 + 64;
    int16_t *pcm = (int16_t *) malloc(worst_pcm * sizeof(int16_t));
    TEST(pcm != NULL, "malloc PCM");

    int wrote = re15_vag_adpcm_decode(vb + vab.samples[0].offset,
                                       vab.samples[0].size,
                                       pcm, worst_pcm);
    TEST(wrote > 0, "ADPCM decode returns positive sample count");
    TEST((size_t)wrote == (vab.samples[0].size / 16) * 28,
         "decoded count matches expected (frames × 28)");

    /* PCM range sanity — has non-zero samples (sample is audible). */
    int has_nonzero = 0;
    int min_pcm = 32767, max_pcm = -32768;
    for (int i = 0; i < wrote; i++) {
        if (pcm[i] != 0) has_nonzero = 1;
        if (pcm[i] < min_pcm) min_pcm = pcm[i];
        if (pcm[i] > max_pcm) max_pcm = pcm[i];
    }
    TEST(has_nonzero, "decoded PCM has non-zero samples");
    TEST(max_pcm - min_pcm > 1000,
         "decoded PCM has meaningful dynamic range (>1000 LSB peak-to-peak)");

    free(pcm);
    free(vh);
    free(vb);
    return 0;
}

static int test_decode_rejects_bad_inputs(void)
{
    int16_t pcm[28];
    TEST(re15_vag_adpcm_decode(NULL, 16, pcm, 28) == 0, "NULL src returns 0");
    TEST(re15_vag_adpcm_decode((const uint8_t *)"x", 16, NULL, 28) == 0, "NULL dst returns 0");
    TEST(re15_vag_adpcm_decode((const uint8_t *)"xxxxxxxxxxxxxxx", 15, pcm, 28) == 0,
         "size < 16 (one frame) returns 0");
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_header_rejects_bad_signature();
    fails += test_parse_real_door00_vh();
    fails += test_decode_first_sample();
    fails += test_decode_rejects_bad_inputs();

    if (fails == 0) {
        printf("test_vab: all 4 tests PASSED\n");
        return 0;
    }
    printf("test_vab: %d test(s) FAILED\n", fails);
    return 1;
}
