/* test_xa_decode.c — byte-true CD-XA ADPCM decode (FE-3 opening-movie audio).
 *
 * Regression guard for re15_xa_decode_sector (re15_xa.c). The full-movie decode
 * was verified BIT-EXACT against an independent reference decoder over all of
 * CAPCOM.STR (663,264 stereo frames). This test pins the decode MATH on a
 * synthetic sound-group so a future edit that breaks the indexing, the filter/
 * shift split, the (·+32)>>6 predictor recurrence, the int16 saturation, or the
 * 4-bit-stereo L/R nibble map fails loudly.
 *
 * Group 0 is crafted so:
 *   block0 L: param 0x00 (shift 0, filter 0), nibble 1 every sample -> constant
 *             1<<12 = 4096 (filter 0 has no predictor term).
 *   block0 R: high nibble 0 -> silent (proves L/R separation).
 *   block1 L: param 0x20 (shift 0, filter 2 = K0 115 / K1 -52), nibble 1 every
 *             sample -> the predictor recurrence: 8128, 15373, 25115, then
 *             saturates at +32767 (proves the predictor + sat16 clamp).
 * Expected values come from the verified reference decoder (history starts 0).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_xa.h"

int main(void)
{
    int fail = 0;
    printf("=== CD-XA ADPCM decode (FE-3 movie audio) ===\n");

    uint8_t adpcm[2304];
    memset(adpcm, 0, sizeof adpcm);
    /* group 0 header params: blk0 L = 0x00 (shift0/filt0), blk1 L = 0x20 (shift0/filt2). */
    adpcm[6] = 0x20;
    /* data words: block0 low nibble = 1 (L), block1 low nibble = 1 (L); highs 0. */
    for (int j = 0; j < 28; j++) {
        adpcm[16 + 0 + j * 4] = 0x01;   /* block 0 */
        adpcm[16 + 1 + j * 4] = 0x01;   /* block 1 */
    }

    int16_t dst[2016 * 2];
    int hist[4] = { 0, 0, 0, 0 };
    re15_xa_decode_sector(adpcm, dst, hist);

    /* block0 L (filter 0): frames 0..27 all == 4096. */
    if (dst[0] != 4096 || dst[2] != 4096 || dst[27 * 2] != 4096) {
        fprintf(stderr, "FAIL(1): filter-0 block0 L must be 4096, got %d/%d/%d\n",
                dst[0], dst[2], dst[27 * 2]); fail = 1;
    }
    /* block0 R silent (L/R separation). */
    if (dst[1] != 0 || dst[27 * 2 + 1] != 0) {
        fprintf(stderr, "FAIL(2): block0 R must be silent, got %d/%d\n", dst[1], dst[27 * 2 + 1]);
        fail = 1;
    }
    /* block1 L (filter 2 predictor) CONTINUES block0 L's history (old=older=4096):
     * 8128, 15373, 25115, then saturates at 32767 — proves the predictor recurrence,
     * the within-block history carry, and the int16 saturation clamp all at once. */
    int b1[4] = { dst[28 * 2], dst[29 * 2], dst[30 * 2], dst[31 * 2] };
    if (b1[0] != 8128 || b1[1] != 15373 || b1[2] != 25115 || b1[3] != 32767) {
        fprintf(stderr, "FAIL(3): filter-2 predictor/sat wrong: %d %d %d %d (want 8128 15373 25115 32767)\n",
                b1[0], b1[1], b1[2], b1[3]); fail = 1;
    }
    if (!fail)
        printf("  filter0 const, L/R split, filter2 predictor+sat, history carry: OK\n");

    if (fail) { printf("XA-DECODE: FAIL\n"); return 1; }
    printf("XA-DECODE: all checks passed\n");
    return 0;
}
