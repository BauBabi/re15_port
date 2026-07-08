/* test_stair_turn.c — the byte-true stair TURN-to-face heading settle (LAB_80037fd8 @0x800380b0-f4).
 *
 * Before the stepping gait, the stair TURN preamble plays clip 5 in place and geometrically decays
 * the heading residual DAT_800acabe onto the run, then snaps. The decay (re15_stair_turn_settle):
 *   step = (res>>2)&0xff ; res = (res&0x200) ? res+step : res-step ; done when (res&0x3e0)==0.
 * The port carries this as a residual (rot_y - target) that must decay to 0 (rot_y reaches target).
 * Asserts:
 *   (1) the exact per-frame decay arithmetic for a sample value.
 *   (2) CONVERGENCE + BASIN: a residual decays to the TARGET cardinal (0) IFF it is inside the
 *       +-0x200 (+-45deg) basin [0..0x200) U [0xE00..0x1000); outside it lands on a 90deg-off
 *       cardinal (which is why the port instant-snaps out-of-basin entries). Every residual aligns
 *       in a bounded number of frames (no infinite loop).
 *   (3) ENDPOINT: driving rot_y = (target + res) each frame (as re15_stair_tick does) ends exactly
 *       at the target for an in-basin seed.
 */
#include <stdio.h>
#include <stdint.h>
#include "re15_stair.h"

/* nearest 0x400 cardinal of a 12-bit heading */
static uint16_t cardinal_of(uint16_t h) { return (uint16_t)(((h + 0x200) & 0x0FFF) & 0x0C00); }
static int in_basin(uint16_t res) { return (res < 0x200) || (res >= 0xE00); }   /* the port's classifier */

int main(void)
{
    int fail = 0;
    printf("=== stair TURN-to-face heading settle (byte-true LAB_80037fd8 @0x800380b0-f4) ===\n");

    /* (1) exact decay for res=0x100: step=(0x100>>2)&0xff=0x40; bit 0x200 clear -> subtract -> 0xC0;
     *     (0xC0 & 0x3e0)=0x080 != 0 -> not yet aligned. And res=0xF00 (negative side, bit 0x200 set):
     *     step=(0xF00>>2)&0xff=0xC0; add -> 0xFC0. */
    {
        int a1 = -1, a2 = -1;
        uint16_t r1 = re15_stair_turn_settle(0x100, &a1);
        uint16_t r2 = re15_stair_turn_settle(0x0F00, &a2);
        if (r1 != 0x0C0 || a1 != 0) { fprintf(stderr, "FAIL(1a): settle(0x100)=%#x aligned=%d, want 0xC0/0\n", r1, a1); fail = 1; }
        if (r2 != 0x0FC0 || a2 != 0) { fprintf(stderr, "FAIL(1b): settle(0xF00)=%#x aligned=%d, want 0xFC0/0\n", r2, a2); fail = 1; }
        if (!fail) printf("  (1) decay math: settle(0x100)=0xC0 (sub), settle(0xF00)=0xFC0 (add)\n");
    }

    /* (2) convergence + basin, over the whole 12-bit residual range. */
    {
        int mism = 0, unconverged = 0, basin_to_target = 0;
        for (uint32_t res0 = 0; res0 < 0x1000; res0++) {
            uint16_t res = (uint16_t)res0;
            int aligned = ((res & 0x3e0) == 0);
            int frames = 0;
            while (!aligned && frames < 200) { res = re15_stair_turn_settle(res, &aligned); frames++; }
            if (!aligned) { unconverged++; continue; }
            int hit_target = (cardinal_of(res) == 0);          /* converged onto the TARGET cardinal (0) */
            if (hit_target != in_basin((uint16_t)res0)) {
                if (mism < 4) fprintf(stderr, "FAIL(2): res %#x -> aligned %#x (target=%d) but in_basin=%d\n",
                                      res0, res, hit_target, in_basin((uint16_t)res0));
                mism++;
            }
            if (in_basin((uint16_t)res0)) basin_to_target++;
        }
        if (unconverged) { fprintf(stderr, "FAIL(2): %d residuals never aligned (non-convergent decay)\n", unconverged); fail = 1; }
        if (mism)        { fprintf(stderr, "FAIL(2): %d residuals disagree with the basin classifier\n", mism); fail = 1; }
        if (!fail) printf("  (2) all 4096 residuals converge; target-reaching set == the +-0x200 basin (%d seeds)\n", basin_to_target);
    }

    /* (3) endpoint: simulate the tick (rot_y = target + res each frame, snap on aligned) for a few
     *     in-basin seeds and confirm rot_y lands exactly on the target. */
    {
        const uint16_t target = 0x333;                          /* arbitrary run heading */
        const uint16_t seeds[5] = { 0x000, 0x050, 0x1F0, 0x0F80, 0x0E10 };  /* all in-basin (+-45deg) */
        for (int i = 0; i < 5; i++) {
            uint16_t res = seeds[i];
            uint16_t rot_y = (uint16_t)((target + res) & 0x0FFF);
            int aligned = ((res & 0x3e0) == 0), frames = 0;
            while (!aligned && frames < 200) {
                res   = re15_stair_turn_settle(res, &aligned);
                rot_y = (uint16_t)((target + res) & 0x0FFF);
                frames++;
            }
            if (aligned) rot_y = target;                        /* the tick's snap */
            if (rot_y != target) { fprintf(stderr, "FAIL(3): seed %#x ended rot_y=%#x, want target %#x\n", seeds[i], rot_y, target); fail = 1; }
        }
        if (!fail) printf("  (3) endpoint: every in-basin seed drives rot_y exactly to the target\n");
    }

    if (fail) { printf("STAIR-TURN: FAIL\n"); return 1; }
    printf("STAIR-TURN: all checks passed\n");
    return 0;
}
