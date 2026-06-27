/*
 * Unit tests for the TIM parser (Phase 4.3).
 *
 * Synthesizes minimal in-memory TIM blobs for each BPP variant and verifies
 * the parser extracts the right metadata + pointers. No file I/O.
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "re15_tim.h"

#define TEST(cond, msg)                                                \
    do {                                                                \
        if (!(cond)) {                                                  \
            fprintf(stderr, "FAIL %s:%d %s — %s\n",                     \
                    __FILE__, __LINE__, __func__, msg);                 \
            return 1;                                                   \
        }                                                               \
    } while (0)

/* Helpers to write little-endian u32/u16 into a buffer */
static void w32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
    p[2] = (uint8_t)((v >> 16) & 0xFF);
    p[3] = (uint8_t)((v >> 24) & 0xFF);
}
static void w16(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFF);
    p[1] = (uint8_t)((v >> 8) & 0xFF);
}

/* ---- Test 1: 8bpp + CLUT (the DOOR00 pattern) -------------------------- */
static int test_8bpp_with_clut(void)
{
    /* 8bpp, 16x4 image, 256-entry CLUT */
    uint8_t buf[8 + 12 + 512 + 12 + 64];
    memset(buf, 0, sizeof(buf));

    w32(buf + 0, 0x10);             /* magic */
    w32(buf + 4, 0x09);             /* flag = 8bpp + has_clut */

    /* CLUT header at +8 */
    w32(buf + 8,  12 + 512);        /* clut_size */
    w16(buf + 12, 0);               /* clut_x */
    w16(buf + 14, 480);             /* clut_y */
    w16(buf + 16, 256);             /* clut_width */
    w16(buf + 18, 1);               /* clut_height */
    /* CLUT entries at +20..+531 (some non-zero so we can verify pointer) */
    w16(buf + 20, 0x1234);
    w16(buf + 22 + 510, 0x5678);   /* last entry */

    /* Data header at +532 */
    int data_off = 8 + 12 + 512;    /* = 532 */
    w32(buf + data_off + 0, 12 + 64);     /* data_size: 12 hdr + 8*8 pixels */
    w16(buf + data_off + 4, 0);            /* data_x */
    w16(buf + data_off + 6, 0);            /* data_y */
    w16(buf + data_off + 8, 8);            /* data_width = 8 in 16-bit units = 16 pixels in 8bpp */
    w16(buf + data_off + 10, 4);           /* data_height = 4 */
    /* Pixel data: each byte = palette index */
    buf[data_off + 12 + 0] = 0x01;
    buf[data_off + 12 + 63] = 0xFE;

    re15_tim_t tim;
    int r = re15_tim_parse(buf, sizeof(buf), &tim);
    TEST(r == 0, "parse should succeed");
    TEST(tim.bpp == 8, "bpp should be 8");
    TEST(tim.has_clut == 1, "has_clut should be 1");
    TEST(tim.clut_entries == 256, "clut_entries should be 256");
    TEST(tim.width == 16, "width = 8 units * 2 = 16 pixels");
    TEST(tim.height == 4, "height = 4");
    TEST(tim.clut != NULL, "clut pointer should be non-null");
    TEST(tim.pixels != NULL, "pixels pointer should be non-null");
    TEST(tim.clut[0] == 0x1234, "first CLUT entry");
    TEST(((const uint8_t *)tim.pixels)[0] == 0x01, "first pixel byte");
    TEST(((const uint8_t *)tim.pixels)[63] == 0xFE, "last pixel byte");

    return 0;
}

/* ---- Test 2: 4bpp + CLUT (16-entry palette) ----------------------------- */
static int test_4bpp_with_clut(void)
{
    /* 4bpp, 16-entry CLUT (= 32 bytes data) */
    uint8_t buf[8 + 12 + 32 + 12 + 16];
    memset(buf, 0, sizeof(buf));

    w32(buf + 0, 0x10);
    w32(buf + 4, 0x08);              /* flag = 4bpp + has_clut */

    w32(buf + 8,  12 + 32);
    w16(buf + 12, 0); w16(buf + 14, 480);
    w16(buf + 16, 16); w16(buf + 18, 1);
    w16(buf + 20, 0xAAAA);

    int data_off = 8 + 12 + 32;
    w32(buf + data_off + 0, 12 + 16);
    w16(buf + data_off + 4, 0); w16(buf + data_off + 6, 0);
    w16(buf + data_off + 8, 4);            /* 4 units in 16-bit = 4*4=16 pixels wide */
    w16(buf + data_off + 10, 2);           /* height 2 */

    re15_tim_t tim;
    int r = re15_tim_parse(buf, sizeof(buf), &tim);
    TEST(r == 0, "parse 4bpp");
    TEST(tim.bpp == 4, "bpp = 4");
    TEST(tim.width == 16, "4bpp: 4 units * 4 = 16 pixels");
    TEST(tim.height == 2, "height 2");
    TEST(tim.clut_entries == 16, "16-entry palette");

    return 0;
}

/* ---- Test 3: 16bpp direct color (no CLUT) ------------------------------- */
static int test_16bpp_no_clut(void)
{
    uint8_t buf[8 + 12 + 32];
    memset(buf, 0, sizeof(buf));

    w32(buf + 0, 0x10);
    w32(buf + 4, 0x02);              /* flag = 16bpp, no CLUT */

    w32(buf + 8, 12 + 32);
    w16(buf + 12, 0); w16(buf + 14, 0);
    w16(buf + 16, 4); w16(buf + 18, 4);   /* 4 units wide × 4 high */

    re15_tim_t tim;
    int r = re15_tim_parse(buf, sizeof(buf), &tim);
    TEST(r == 0, "parse 16bpp");
    TEST(tim.bpp == 16, "bpp 16");
    TEST(tim.has_clut == 0, "no CLUT");
    TEST(tim.width == 4, "16bpp: 4 units = 4 pixels");
    TEST(tim.height == 4, "height 4");
    TEST(tim.clut == NULL, "clut should be NULL when has_clut=0");

    return 0;
}

/* ---- Test 4: bad magic should fail -------------------------------------- */
static int test_bad_magic_fails(void)
{
    uint8_t buf[64];
    memset(buf, 0xFF, sizeof(buf));
    w32(buf + 0, 0x42);   /* not 0x10 */
    re15_tim_t tim;
    int r = re15_tim_parse(buf, sizeof(buf), &tim);
    TEST(r == -1, "bad magic should return -1");
    return 0;
}

/* ---- Test 5: truncated input should fail -------------------------------- */
static int test_truncated_fails(void)
{
    uint8_t buf[4];
    w32(buf + 0, 0x10);
    re15_tim_t tim;
    int r = re15_tim_parse(buf, 4, &tim);
    TEST(r == -1, "truncated (< 8 bytes) should fail");
    return 0;
}

int main(void)
{
    int fails = 0;
    fails += test_8bpp_with_clut();
    fails += test_4bpp_with_clut();
    fails += test_16bpp_no_clut();
    fails += test_bad_magic_fails();
    fails += test_truncated_fails();

    if (fails == 0) {
        printf("test_tim: all 5 tests PASSED\n");
        return 0;
    } else {
        printf("test_tim: %d test(s) FAILED\n", fails);
        return 1;
    }
}
