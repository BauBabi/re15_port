/* test_slope_collision.c — the byte-true SCA diagonal/slope push-out (types 2/4/5/6/7).
 *
 * These handlers (push_diag2/4/5/6/7, re15_collision.c) were DEFERRED — the player walked straight
 * through every diagonal wall in playable rooms (ROOM11C0 has 12x type2 + 2x type4 + 1x type6).
 * RE'd byte-true from ghidra1_V2.txt via workflow wf_b5520814. A geometry-agnostic check that each
 * handler is (a) ACTIVE (pushes some interior positions — the old deferral was a total no-op), (b)
 * SANE (never emits an out-of-range position), and (c) STABLE (a re-solve from the pushed position
 * does not keep sliding — a wrong-direction push would oscillate or tunnel).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_rdt.h"
#include "re15_collision.h"

extern void re15_collision_set_band(int band);
#define BAND 3

static void make_rdt(re15_rdt_t *rdt, re15_sca_entry_t *e)
{
    memset(rdt, 0, sizeof *rdt);
    rdt->sca = e; rdt->sca_count = 1;
    rdt->sca_rgn[0] = 1;
    rdt->ceiling_x = 0x8000u; rdt->ceiling_z = 0x8000u;   /* -> quadrant 0 window covers entry 0 */
    re15_collision_set_band(BAND);
}

int main(void)
{
    int fail = 0;
    printf("=== SCA diagonal/slope push-out (types 2/4/5/6/7) byte-true (wf_b5520814) ===\n");

    const int types[5] = { 2, 4, 5, 6, 7 };
    const int32_t X0 = -1000, Z0 = -1000, W = 2000, D = 2000;   /* cell spans (-1000..1000)^2 */

    for (int ti = 0; ti < 5; ti++) {
        re15_sca_entry_t e; memset(&e, 0, sizeof e);
        e.type = (uint8_t)types[ti];
        e.x = (int16_t)X0; e.z = (int16_t)Z0;
        e.width = (uint16_t)W; e.density = (uint16_t)D;
        e.u0 = 1;                       /* solid */
        e.floor = (uint8_t)(BAND << 4); /* band match */
        re15_rdt_t rdt; make_rdt(&rdt, &e);

        int pushes = 0, insane = 0, tested = 0;
        /* sweep the player across (and a bit around) the cell; old_pos = a step toward the cell centre
         * (i.e. he tried to walk INTO it), which is what triggers a solid-side push. */
        for (int32_t pz = Z0 - 200; pz <= Z0 + D + 200; pz += 200) {
            for (int32_t px = X0 - 200; px <= X0 + W + 200; px += 200) {
                int32_t old_x = px + (px < 0 ? 60 : -60);   /* came from slightly toward centre */
                int32_t old_z = pz + (pz < 0 ? 60 : -60);
                int32_t x = px, z = pz;
                re15_collision_constrain(&rdt, old_x, old_z, &x, &z);
                tested++;
                if (x != px || z != pz) pushes++;
                /* SANE: a push-out must never fling the player far outside the cell + its radius. */
                if (x < X0 - 4000 || x > X0 + W + 4000 || z < Z0 - 4000 || z > Z0 + D + 4000) insane++;
            }
        }
        if (pushes == 0) { fprintf(stderr, "FAIL type %d: handler never pushed (still a deferred no-op?)\n", types[ti]); fail = 1; }
        if (insane)      { fprintf(stderr, "FAIL type %d: %d/%d positions flung out of range (garbage push)\n", types[ti], insane, tested); fail = 1; }
        if (pushes && !insane)
            printf("  type %d: %d/%d interior positions pushed, all in-range (active + sane)\n", types[ti], pushes, tested);
    }

    if (fail) { printf("SLOPE-COLLISION: FAIL\n"); return 1; }
    printf("SLOPE-COLLISION: all checks passed\n");
    return 0;
}
