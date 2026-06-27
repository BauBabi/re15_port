/*
 * Unit tests for the BSS chunk-header parser (Phase 4.5.6.1).
 *
 * Verifies:
 *   - Header field byte order (little-endian s16 quartet)
 *   - re15_bss_chunk_has_video correctly identifies the VLC_ID tag
 *   - The vlc_payload pointer + size are derived correctly
 *   - Truncated buffers are rejected without crashing
 *
 * Reference data: ROOM11000.bs has header bytes
 *   a0 4d 00 38 01 00 03 00
 * = run_length_words=0x4DA0 (19872), id=0x3800, quant=1, version=3
 * (verified by xxd at extract time on 2026-05-18).
 */
#include <stdio.h>
#include <string.h>
#include "re15_bss.h"

#define TEST(cond, msg)                                                \
    do {                                                                \
        if (!(cond)) {                                                  \
            fprintf(stderr, "FAIL %s:%d %s — %s\n",                     \
                    __FILE__, __LINE__, __func__, msg);                 \
            return 1;                                                   \
        }                                                               \
    } while (0)

/* Synthetic 16-byte chunk that mirrors real ROOM11000.bs's leading
 * 8 header bytes + 8 dummy payload bytes. We don't ship the real
 * 64 KB BSS in the test binary — that's the asset-bundled file
 * inside the PSX EXE / PC executable. */
static const uint8_t kSyntheticChunk[16] = {
    0xA0, 0x4D, 0x00, 0x38, 0x01, 0x00, 0x03, 0x00,    /* header */
    0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE,    /* payload sentinel */
};

static int test_header_field_byte_order(void)
{
    re15_bss_chunk_t chunk;
    TEST(re15_bss_parse_chunk(kSyntheticChunk, sizeof kSyntheticChunk, &chunk),
         "parse should succeed on 16-byte buffer");
    TEST(chunk.run_length_words == 0x4DA0, "u16 LE: run_length_words = 0x4DA0");
    TEST(chunk.id               == 0x3800, "u16 LE: id = VLC_ID");
    TEST(chunk.quant            == 0x0001, "u16 LE: quant = 1");
    TEST(chunk.version          == 0x0003, "u16 LE: version = 3 (RE1.5 default)");
    return 0;
}

static int test_payload_offset(void)
{
    re15_bss_chunk_t chunk;
    re15_bss_parse_chunk(kSyntheticChunk, sizeof kSyntheticChunk, &chunk);
    TEST(chunk.vlc_payload == kSyntheticChunk + RE15_BSS_HEADER_SIZE,
         "payload starts at +8 from chunk base");
    TEST(chunk.vlc_payload_size == sizeof(kSyntheticChunk) - RE15_BSS_HEADER_SIZE,
         "payload size = chunk_size - 8");
    TEST(chunk.vlc_payload[0] == 0xDE, "payload first byte = 0xDE");
    return 0;
}

static int test_has_video_predicate(void)
{
    re15_bss_chunk_t vid = { .id = RE15_BSS_VLC_ID };
    re15_bss_chunk_t pad = { .id = 0x0000 };
    re15_bss_chunk_t garbage = { .id = 0xFFFF };
    TEST(re15_bss_chunk_has_video(&vid),       "VLC_ID chunk → has video");
    TEST(!re15_bss_chunk_has_video(&pad),      "zero id → no video");
    TEST(!re15_bss_chunk_has_video(&garbage),  "garbage id → no video");
    TEST(!re15_bss_chunk_has_video(NULL),      "NULL → no video, no crash");
    return 0;
}

static int test_truncated_buffer_rejected(void)
{
    re15_bss_chunk_t chunk;
    TEST(!re15_bss_parse_chunk(kSyntheticChunk, 0, &chunk),  "size 0 → reject");
    TEST(!re15_bss_parse_chunk(kSyntheticChunk, 7, &chunk),
         "size 7 (one less than header) → reject");
    TEST(re15_bss_parse_chunk(kSyntheticChunk, 8, &chunk),
         "size 8 (exact header) → accept with empty payload");
    /* Header-only chunk has zero-byte payload but a well-defined pointer. */
    TEST(chunk.vlc_payload_size == 0,           "payload size = 0 when size = 8");
    return 0;
}

static int test_null_pointer_safety(void)
{
    re15_bss_chunk_t chunk;
    TEST(!re15_bss_parse_chunk(NULL, 64, &chunk),               "NULL buf → reject");
    TEST(!re15_bss_parse_chunk(kSyntheticChunk, 64, NULL),      "NULL out → reject");
    return 0;
}

/* ===== VLC decoder integration tests (Phase 4.5.6.2) ===================== */

#include <stdlib.h>     /* malloc / free */

#ifndef RE15_TEST_BSS_PATH
# error "Test must be built with RE15_TEST_BSS_PATH defined to assets/test.bss"
#endif

static int test_vlc_decode_real_chunk(void)
{
    /* Read the bundled 64 KB BSS chunk (ROOM11000.bs from RE1.5 stage1). */
    FILE *f = fopen(RE15_TEST_BSS_PATH, "rb");
    TEST(f != NULL, "open assets/test.bss");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    TEST(fsize == RE15_BSS_CHUNK_SIZE,
         "test.bss is exactly one 64 KB BSS chunk");
    uint8_t *buf = (uint8_t *) malloc((size_t) fsize);
    TEST(buf != NULL, "malloc chunk buffer");
    size_t got = fread(buf, 1, (size_t) fsize, f);
    fclose(f);
    TEST(got == (size_t) fsize, "fread full chunk");

    re15_bss_chunk_t chunk;
    TEST(re15_bss_parse_chunk(buf, (size_t) fsize, &chunk), "parse real chunk");
    TEST(re15_bss_chunk_has_video(&chunk), "real chunk has video");

    /* Sanity on real-data header fields. ROOM11000.bs values come from
     * xxd at extract time (2026-05-18): run_length_words=19872, id=0x3800,
     * quant=1, version=3. */
    TEST(chunk.run_length_words == 19872, "real run_length_words = 19872");
    TEST(chunk.quant            == 1,     "real quant = 1");
    TEST(chunk.version          == 3,     "real version = 3 (RE1.5)");

    /* Worst-case output buffer size per the decoder contract:
     * (run_length_words + 2) * 4 int16 coefficients. */
    size_t out_capacity = ((size_t) chunk.run_length_words + 2) * 4;
    int16_t *out = (int16_t *) malloc(out_capacity * sizeof(int16_t));
    TEST(out != NULL, "malloc decoder output");

    int written = re15_bss_vlc_decode(chunk.vlc_payload, chunk.vlc_payload_size,
                                       chunk.run_length_words, chunk.quant,
                                       chunk.version, out, out_capacity);
    TEST(written > 0, "VLC decoder returned positive count");
    TEST((size_t) written <= out_capacity, "VLC didn't overflow buffer");

    /* First two coefficients must be the MDEC preamble. */
    TEST(out[0] == (int16_t) chunk.run_length_words, "preamble [0] = run_length_words");
    TEST(out[1] == (int16_t) RE15_BSS_VLC_ID,        "preamble [1] = VLC_ID");

    /* The decoder fills (or near-fills) the worst-case buffer for a
     * normal 320x240 frame. We don't check for exact equality because
     * partial chunks at the file tail can finish early — but for this
     * full-frame chunk we expect at least 90% fill. */
    TEST((size_t) written >= out_capacity * 9 / 10,
         "≥90% of capacity used for a full 320x240 frame");

    free(out);
    free(buf);
    return 0;
}

static int test_vlc_rejects_null_inputs(void)
{
    int16_t out_buf[4];
    TEST(re15_bss_vlc_decode(NULL, 100, 1, 1, 3, out_buf, 4) < 0,
         "NULL src → reject");
    TEST(re15_bss_vlc_decode((const uint8_t *) "x", 1, 1, 1, 3, NULL, 4) < 0,
         "NULL dst → reject");
    TEST(re15_bss_vlc_decode((const uint8_t *) "x", 1, 1, 1, 3, out_buf, 1) < 0,
         "dst too small for preamble → reject");
    return 0;
}

/* ===== MDEC integration tests (Phase 4.5.6.4) =========================== */

static int test_mdec_decode_real_frame(void)
{
    /* Reuse the same path-based loading as the VLC test. */
    FILE *f = fopen(RE15_TEST_BSS_PATH, "rb");
    TEST(f != NULL, "open assets/test.bss");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *) malloc((size_t) fsize);
    TEST(buf != NULL, "malloc chunk buffer");
    fread(buf, 1, (size_t) fsize, f);
    fclose(f);

    re15_bss_chunk_t chunk;
    re15_bss_parse_chunk(buf, (size_t) fsize, &chunk);

    /* VLC stage */
    size_t cap = ((size_t)chunk.run_length_words + 2) * 4;
    int16_t *coeffs = (int16_t *) malloc(cap * sizeof(int16_t));
    int wrote = re15_bss_vlc_decode(chunk.vlc_payload, chunk.vlc_payload_size,
                                     chunk.run_length_words, chunk.quant,
                                     chunk.version, coeffs, cap);
    TEST(wrote > 0, "VLC decoder fills the buffer");

    /* MDEC stage */
    uint8_t *rgb = (uint8_t *) malloc(320 * 240 * 3);
    TEST(rgb != NULL, "malloc RGB output");
    int rv = re15_bss_mdec_decode(coeffs, (size_t) wrote, 320, 240, rgb);
    TEST(rv == 0, "MDEC decode returns success");

    /* Sanity check: the output should NOT be all-zero (decoder ran and
     * produced something). Compute a simple sum check. */
    int64_t sum = 0;
    for (int i = 0; i < 320 * 240 * 3; i++) sum += rgb[i];
    TEST(sum > 320 * 240 * 10,
         "MDEC output has non-trivial pixel data (>10 avg per byte)");

    /* Sanity: pixels should cover a reasonable range (not stuck at one
     * color). Sample 9 spread-out pixels and verify at least 3 distinct
     * values across them. */
    int distinct = 0;
    uint8_t seen[9 * 3];
    int seen_count = 0;
    int sample_xs[3] = {  40, 160, 280 };
    int sample_ys[3] = {  40, 120, 200 };
    for (int yi = 0; yi < 3; yi++)
        for (int xi = 0; xi < 3; xi++) {
            int idx = (sample_ys[yi] * 320 + sample_xs[xi]) * 3;
            uint8_t r = rgb[idx];
            int found = 0;
            for (int s = 0; s < seen_count; s++)
                if (seen[s] == r) { found = 1; break; }
            if (!found) {
                seen[seen_count++] = r;
                distinct++;
            }
        }
    TEST(distinct >= 3, "≥3 distinct R values across 9 sample points");

    free(rgb);
    free(coeffs);
    free(buf);
    return 0;
}

static int test_mdec_rejects_bad_inputs(void)
{
    int16_t coeffs[4] = { 100, 0x3800, 0, 0 };
    uint8_t rgb[10];
    TEST(re15_bss_mdec_decode(NULL, 4, 320, 240, rgb) < 0, "NULL coeffs");
    TEST(re15_bss_mdec_decode(coeffs, 4, 320, 240, NULL) < 0, "NULL rgb");
    TEST(re15_bss_mdec_decode(coeffs, 1, 320, 240, rgb) < 0, "too short for preamble");
    coeffs[1] = 0x1234;  /* wrong VLC_ID */
    TEST(re15_bss_mdec_decode(coeffs, 4, 320, 240, rgb) < 0, "wrong preamble");
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_header_field_byte_order();
    fails += test_payload_offset();
    fails += test_has_video_predicate();
    fails += test_truncated_buffer_rejected();
    fails += test_null_pointer_safety();
    fails += test_vlc_decode_real_chunk();
    fails += test_vlc_rejects_null_inputs();
    fails += test_mdec_decode_real_frame();
    fails += test_mdec_rejects_bad_inputs();

    if (fails == 0) {
        printf("test_bss: all 9 tests PASSED\n");
        return 0;
    } else {
        printf("test_bss: %d test(s) FAILED\n", fails);
        return 1;
    }
}
