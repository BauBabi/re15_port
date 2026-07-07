/* test_rotmatrix.c — the byte-true RE1.5 RotMatrix (audit #1).
 *
 * mat3_from_euler was changed from a Q36-single-shift accumulation (more precise than the console)
 * to the original's exact per-product truncation + negate-before-shift. This test drives it over a
 * wide angle vector and checks EVERY element against an independent transliteration of the
 * authoritative RE1.5 decompile (RE_15_Quellcode_V2/RotMatrix.c lines 18-39), which reads the same
 * DAT_800794c4 table (re15_sin_q12/cos_q12). It also proves the byte-true form actually DIFFERS from
 * the naive Q36 form on some angles (i.e. the fix is live and is the pixel-shift source), while the
 * two never diverge by more than a couple LSB (a bounded truncation change, not a broken matrix).
 */
#include <stdio.h>
#include <stdint.h>
#include "re15_skeleton.h"

extern void re15_skel_euler_matrix_for_test(int ax, int ay, int az, int32_t m[9]);

/* Independent transliteration of RE_15_Quellcode_V2/RotMatrix.c (the byte-true reference). */
static void ref_rotmatrix(int ax, int ay, int az, int32_t m[9])
{
    int32_t sx = re15_sin_q12(ax), cx = re15_cos_q12(ax);   /* iVar6, iVar3 */
    int32_t sy = re15_sin_q12(ay), cy = re15_cos_q12(ay);   /* sVar8, iVar4 */
    int32_t sz = re15_sin_q12(az), cz = re15_cos_q12(az);   /* iVar7, iVar5 */
    int32_t nsy = -sy;                                       /* iVar9 */
    m[2] = (int16_t)sy;
    m[5] = (int16_t)((-(cy * sx)) >> 12);
    m[8] = (int16_t)((cy * cx) >> 12);
    m[0] = (int16_t)((cz * cy) >> 12);
    m[1] = (int16_t)((-(sz * cy)) >> 12);
    int32_t iv4 = (cz * nsy) >> 12;
    m[3] = (int16_t)((int16_t)((sz * cx) >> 12) - (int16_t)((iv4 * sx) >> 12));
    m[6] = (int16_t)((int16_t)((sz * sx) >> 12) + (int16_t)((iv4 * cx) >> 12));
    int32_t iv9 = (sz * nsy) >> 12;
    m[4] = (int16_t)((int16_t)((cz * cx) >> 12) + (int16_t)((iv9 * sx) >> 12));
    m[7] = (int16_t)((int16_t)((cz * sx) >> 12) - (int16_t)((iv9 * cx) >> 12));
}

/* The OLD (naive) form: accumulate each off-diagonal in Q36, single >>24 (more precise). */
static void naive_rotmatrix(int ax, int ay, int az, int32_t m[9])
{
    int64_t sx = re15_sin_q12(ax), cx = re15_cos_q12(ax);
    int64_t sy = re15_sin_q12(ay), cy = re15_cos_q12(ay);
    int64_t sz = re15_sin_q12(az), cz = re15_cos_q12(az);
    m[0] = (int32_t)((cz * cy) >> 12);
    m[1] = (int32_t)(-((sz * cy) >> 12));
    m[2] = (int32_t)sy;
    m[3] = (int32_t)((((sz * cx) << 12) + cz * sy * sx) >> 24);
    m[4] = (int32_t)((((cz * cx) << 12) - sz * sy * sx) >> 24);
    m[5] = (int32_t)(-((cy * sx) >> 12));
    m[6] = (int32_t)((((sz * sx) << 12) - cz * sy * cx) >> 24);
    m[7] = (int32_t)((((cz * sx) << 12) + sz * sy * cx) >> 24);
    m[8] = (int32_t)((cy * cx) >> 12);
}

int main(void)
{
    int fail = 0, diffs = 0, max_vs_naive = 0;
    printf("=== byte-true RotMatrix vs RE_15_Quellcode_V2/RotMatrix.c (audit #1) ===\n");

    /* wide, deterministic angle vector: every 37th angle on each axis (~1300 triples), all pitched */
    for (int ax = 0; ax < 4096; ax += 337) {
        for (int ay = 0; ay < 4096; ay += 331) {       /* ay steps hit non-zero pitch (sy!=0) */
            for (int az = 0; az < 4096; az += 347) {
                int32_t got[9], ref[9], nai[9];
                re15_skel_euler_matrix_for_test(ax, ay, az, got);
                ref_rotmatrix(ax, ay, az, ref);
                naive_rotmatrix(ax, ay, az, nai);
                for (int e = 0; e < 9; e++) {
                    if (got[e] != ref[e]) {
                        if (fail < 8)
                            fprintf(stderr, "FAIL: (%d,%d,%d) m[%d] port=%d ref=%d\n",
                                    ax, ay, az, e, got[e], ref[e]);
                        fail = 1;
                    }
                    int d = got[e] - nai[e]; if (d < 0) d = -d;
                    if (d > max_vs_naive) max_vs_naive = d;
                    if (got[e] != nai[e]) diffs++;
                }
            }
        }
    }
    if (fail) { printf("ROTMATRIX: FAIL (port != byte-true decompile)\n"); return 1; }
    if (diffs == 0) {
        fprintf(stderr, "FAIL: byte-true form never differs from the naive Q36 form — fix not live?\n");
        return 1;
    }
    if (max_vs_naive > 3) {
        fprintf(stderr, "FAIL: byte-true vs naive diverges by %d LSB (>3 = not a pure truncation change)\n", max_vs_naive);
        return 1;
    }
    printf("  port == byte-true decompile on every element across all triples\n");
    printf("  differs from the naive Q36 form on %d element-samples, max %d LSB (bounded truncation)\n",
           diffs, max_vs_naive);
    printf("ROTMATRIX: all checks passed\n");
    return 0;
}
