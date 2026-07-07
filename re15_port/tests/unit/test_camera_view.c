/* test_camera_view.c — the byte-true RE1.5 camera LookAt (audit #3).
 *
 * re15_camera_build_view was rewritten from a float sqrt + float matrix + Q12_ROUND(+0.5)
 * approximation to the original's exact INTEGER LookAt (FUN_80053ca4 / setupCameraLookAtMatrix):
 * SquareRoot0 + truncating integer division for the normalized sin/cos, Rx(pitch)*Ry(yaw) via the
 * GTE MulMatrix, translation via ApplyMatrixLV. This test checks rot[9]+trans[3] against an
 * independent transliteration of RE_15_Quellcode_V2/FUN_80053ca4.c — using a FULL 3x3 matrix
 * multiply in the reference (vs the port's sparse single-product shortcut) so the shortcut is
 * verified, not assumed.
 */
#include <stdio.h>
#include <stdint.h>
#include "re15_camera.h"

/* copy of camera_common.c's cam_isqrt (== the port's SquareRoot0 stand-in) so the comparison
 * isolates the matrix construction, not the sqrt. */
static uint32_t isq(uint32_t x)
{
    uint32_t r = 0, b = 1UL << 30;
    while (b > x) b >>= 2;
    while (b) { if (x >= r + b) { x -= r + b; r = (r >> 1) + b; } else r >>= 1; b >>= 2; }
    return r;
}

/* independent transliteration of FUN_80053ca4.c (byte-true reference). */
static int ref_build(const re15_camera_cut_t *c, int32_t rot[9], int32_t trans[3])
{
    int32_t dx = c->target_x - c->pos_x, dy = c->target_y - c->pos_y, dz = c->target_z - c->pos_z;
    int32_t dist = (int32_t)isq((uint32_t)((int64_t)dx*dx + (int64_t)dy*dy + (int64_t)dz*dz));
    if (dist == 0) return -2;
    int32_t horiz = (int32_t)isq((uint32_t)((int64_t)dx*dx + (int64_t)dz*dz));
    int16_t sp = (int16_t)(((int64_t)(-dy) * 4096) / dist);      /* sVar1 */
    int16_t cp = (int16_t)(((int64_t)horiz * 4096) / dist);      /* uVar7 */
    int32_t Rx[9] = { 4096,0,0,  0,cp,sp,  0,-sp,cp };
    int32_t V[9];
    if (horiz != 0) {
        int16_t sy = (int16_t)(((int64_t)dx * 4096) / horiz);    /* uVar7' */
        int16_t cy = (int16_t)(((int64_t)dz * 4096) / horiz);    /* uVar8  */
        int32_t Ry[9] = { cy,0,-sy,  0,4096,0,  sy,0,cy };
        for (int i = 0; i < 3; i++)                              /* V = Rx * Ry, GTE MAC (sum then >>12) */
            for (int j = 0; j < 3; j++)
                V[i*3+j] = (int32_t)(((int64_t)Rx[i*3+0]*Ry[0*3+j]
                                    + (int64_t)Rx[i*3+1]*Ry[1*3+j]
                                    + (int64_t)Rx[i*3+2]*Ry[2*3+j]) >> 12);
    } else {
        for (int i = 0; i < 9; i++) V[i] = Rx[i];
    }
    for (int i = 0; i < 9; i++) rot[i] = V[i];
    int64_t npx = -(int64_t)c->pos_x, npy = -(int64_t)c->pos_y, npz = -(int64_t)c->pos_z;
    for (int i = 0; i < 3; i++)
        trans[i] = (int32_t)(((int64_t)V[i*3+0]*npx + (int64_t)V[i*3+1]*npy + (int64_t)V[i*3+2]*npz) >> 12);
    return 0;
}

int main(void)
{
    int fail = 0, cases = 0;
    printf("=== byte-true camera LookAt vs RE_15_Quellcode_V2/FUN_80053ca4.c (audit #3) ===\n");

    /* a spread of camera positions + look-at targets (world units), incl. pitched + off-axis */
    static const int32_t coord[] = { -30000, -12000, -3000, -800, 0, 700, 2500, 9000, 24000 };
    int n = (int)(sizeof coord / sizeof coord[0]);
    for (int pi = 0; pi < n; pi += 2) {
        for (int ti = 1; ti < n; ti += 2) {
            re15_camera_cut_t cut = {0};
            cut.pos_x = coord[pi];       cut.pos_y = coord[(pi+3)%n]; cut.pos_z = coord[(pi+5)%n];
            cut.target_x = coord[ti];    cut.target_y = coord[(ti+2)%n]; cut.target_z = coord[(ti+6)%n];
            cut.fov = 26684;
            int32_t rref[9], tref[3];
            int rr = ref_build(&cut, rref, tref);
            re15_camera_view_t v = {0};
            int pr = re15_camera_build_view(&cut, &v);
            if (rr != 0) { if (pr >= 0) { fprintf(stderr, "FAIL: degenerate case not rejected\n"); fail = 1; } continue; }
            if (pr != 0) { fprintf(stderr, "FAIL: port rejected a valid cut (pos %d,%d,%d)\n", cut.pos_x, cut.pos_y, cut.pos_z); fail = 1; continue; }
            cases++;
            for (int e = 0; e < 9; e++) if (v.rot[e] != rref[e]) {
                if (fail < 6) fprintf(stderr, "FAIL: rot[%d] port=%d ref=%d (pos %d,%d,%d -> %d,%d,%d)\n",
                    e, v.rot[e], rref[e], cut.pos_x,cut.pos_y,cut.pos_z, cut.target_x,cut.target_y,cut.target_z);
                fail = 1;
            }
            for (int e = 0; e < 3; e++) if (v.trans[e] != tref[e]) {
                if (fail < 6) fprintf(stderr, "FAIL: trans[%d] port=%d ref=%d\n", e, v.trans[e], tref[e]);
                fail = 1;
            }
        }
    }
    if (fail) { printf("CAMERA-VIEW: FAIL\n"); return 1; }
    printf("  port == byte-true decompile (rot[9]+trans[3]) across %d camera cuts\n", cases);
    printf("CAMERA-VIEW: all checks passed\n");
    return 0;
}
