/* test_squareroot0.c — byte-true PsyQ BIOS SquareRoot0 replica (0x80065f60).
 *
 * The engine's ONLY square root is the BIOS table approximation SquareRoot0 —
 * it is NOT floor(sqrt). These known-answer pairs were computed directly from
 * the disassembled routine + its extracted 192-entry table @0x8007d984 and
 * cross-checked by an independent simulation:
 *   SquareRoot0(289)      = 16   (floor sqrt = 17)   ← underestimates by 1
 *   SquareRoot0(10000)    = 99   (floor sqrt = 100)
 *   SquareRoot0(16000000) = 3998 (floor sqrt = 4000) ← underestimates by 2
 * A naive exact-isqrt port diverges from these on ~92% of inputs, and that
 * delta reaches AI thresholds, the circle push, camera, and lighting.
 */
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "re15_math.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

int main(void)
{
    printf("=== byte-true SquareRoot0 replica (0x80065f60) ===\n");

    /* --- known-answer pairs verified against the PSX bytes --- */
    CHECK("SquareRoot0(0) == 0",           re15_squareroot0(0) == 0);
    CHECK("SquareRoot0(1) == 1",           re15_squareroot0(1) == 1);
    CHECK("SquareRoot0(2) == 1",           re15_squareroot0(2) == 1);
    CHECK("SquareRoot0(3) == 1",           re15_squareroot0(3) == 1);
    CHECK("SquareRoot0(4) == 2",           re15_squareroot0(4) == 2);
    CHECK("SquareRoot0(64) == 8",          re15_squareroot0(64) == 8);
    CHECK("SquareRoot0(100) == 10",        re15_squareroot0(100) == 10);
    CHECK("SquareRoot0(256) == 16",        re15_squareroot0(256) == 16);
    CHECK("SquareRoot0(289) == 16 (NOT 17)", re15_squareroot0(289) == 16);
    CHECK("SquareRoot0(1024) == 32",       re15_squareroot0(1024) == 32);
    CHECK("SquareRoot0(10000) == 99 (NOT 100)", re15_squareroot0(10000) == 99);
    CHECK("SquareRoot0(65536) == 256",     re15_squareroot0(65536) == 256);
    CHECK("SquareRoot0(16000000) == 3998 (NOT 4000)", re15_squareroot0(16000000) == 3998);

    /* --- it must genuinely DIFFER from exact floor(sqrt) (else the fix is moot) --- */
    {
        int diffs = 0, total = 0;
        for (uint32_t v = 300; v < 2000000; v += 37) {
            uint32_t s0 = re15_squareroot0(v);
            uint32_t ex = (uint32_t)floor(sqrt((double)v));
            total++;
            if (s0 != ex) diffs++;
            /* the BIOS routine never OVER-estimates floor(sqrt) */
            if (s0 > ex) { printf("  FAIL: SquareRoot0(%u)=%u > floor sqrt %u\n", v, s0, ex); g_fail = 1; break; }
        }
        printf("  info: %d/%d inputs differ from exact floor(sqrt) (%.1f%%)\n",
               diffs, total, 100.0 * diffs / total);
        CHECK("SquareRoot0 differs from exact on the vast majority", diffs > total * 4 / 5);
    }

    /* --- no out-of-bounds table index across the full reachable domain --- */
    {
        /* max squared distance for s16 deltas is < 2^31; sweep a wide net incl. powers of two. */
        int ok = 1;
        for (uint32_t e = 0; e < 31; e++) {
            uint32_t base = 1u << e;
            uint32_t probes[5] = { base, base + 1, base + base/2, base*2 - 1, base + 12345 };
            for (int k = 0; k < 5; k++) {
                uint32_t v = probes[k];
                if (v >= (1u << 31)) continue;
                uint32_t s0 = re15_squareroot0(v);
                uint32_t ex = (uint32_t)floor(sqrt((double)v));
                /* result must be plausible (within a small band below exact) */
                if (s0 > ex || (ex - s0) > (ex / 32 + 4)) { ok = 0;
                    printf("  FAIL: SquareRoot0(%u)=%u vs exact %u out of band\n", v, s0, ex); }
            }
        }
        CHECK("no OOB / all results in the expected band across 2^0..2^30", ok);
    }

    /* --- byte-true BIOS trig (rsin/rcos/ratan2), values verified vs the disassembled cores --- */
    printf("--- BIOS trig ---\n");
    CHECK("rsin(0) == 0",          re15_rsin(0) == 0);
    CHECK("rsin(0x100) == 1567",   re15_rsin(0x100) == 1567);
    CHECK("rsin(0x200) == 2896",   re15_rsin(0x200) == 2896);
    CHECK("rsin(0x400) == 4096",   re15_rsin(0x400) == 4096);
    CHECK("rsin(0x555) == 3548",   re15_rsin(0x555) == 3548);   /* BIOS reflects the exact quarter (game=3545) */
    CHECK("rsin(0x800) == 0",      re15_rsin(0x800) == 0);
    CHECK("rsin(0xC00) == -4096",  re15_rsin(0xC00) == -4096);
    CHECK("rsin(-0x100) == -1567", re15_rsin(-0x100) == -1567);
    CHECK("rcos(0) == 4096",       re15_rcos(0) == 4096);
    CHECK("rcos(0x100) == 3784",   re15_rcos(0x100) == 3784);   /* game biased cos = 3782 */
    CHECK("rcos(0x200) == 2896",   re15_rcos(0x200) == 2896);
    CHECK("rcos(0x400) == 0",      re15_rcos(0x400) == 0);
    CHECK("rcos(0x555) == -2046",  re15_rcos(0x555) == -2046);
    CHECK("rcos(0x800) == -4096",  re15_rcos(0x800) == -4096);
    /* ratan2(y,x): angle of (x,y) CCW from +X, 0..4095 */
    CHECK("ratan2(0,100) == 0 (+X)",      (re15_ratan2(0, 100) & 0xfff) == 0);
    CHECK("ratan2(100,0) == 0x400 (+Y)",  (re15_ratan2(100, 0) & 0xfff) == 0x400);
    CHECK("ratan2(0,-100) == 0x800 (-X)", (re15_ratan2(0, -100) & 0xfff) == 0x800);
    CHECK("ratan2(-100,0) == 0xC00 (-Y)", (re15_ratan2(-100, 0) & 0xfff) == 0xC00);
    CHECK("ratan2(100,100) == 0x200 (45)",(re15_ratan2(100, 100) & 0xfff) == 0x200);
    CHECK("ratan2(50,100) == 302",        (re15_ratan2(50, 100) & 0xfff) == 302);
    CHECK("ratan2(100,50) == 722",        (re15_ratan2(100, 50) & 0xfff) == 722);

    /* --- byte-true GTE RTPS division (UNR reciprocal), values from the psx-spx algorithm --- */
    printf("--- GTE division (UNR) ---\n");
    CHECK("gte_divide(208,300) == 45438",   re15_gte_divide(208, 300) == 45438);
    CHECK("gte_divide(208,600) == 22719",   re15_gte_divide(208, 600) == 22719);
    CHECK("gte_divide(208,1000) == 13632 (UNR, exact=13631)", re15_gte_divide(208, 1000) == 13632);
    CHECK("gte_divide(400,500) == 52429",   re15_gte_divide(400, 500) == 52429);
    CHECK("gte_divide(100,101) == 64887",   re15_gte_divide(100, 101) == 64887);
    CHECK("gte_divide overflow (H>=2*SZ3) saturates 0x1FFFF", re15_gte_divide(1000, 100) == 0x1FFFF);
    CHECK("gte_divide div-by-0 saturates 0x1FFFF", re15_gte_divide(208, 0) == 0x1FFFF);
    /* it must genuinely DIFFER from an exact divide (else the UNR replica is pointless) */
    {
        int diffs = 0, total = 0;
        for (uint32_t sz = 200; sz < 4000; sz += 7) {
            uint32_t unr = re15_gte_divide(208, sz);
            uint32_t exact = (208u * 0x10000u + sz / 2) / sz;
            if (unr <= 0x1FFFF) { total++; if (unr != exact) diffs++; }
        }
        printf("  info: UNR differs from exact on %d/%d (the hardware division inaccuracy)\n", diffs, total);
        CHECK("UNR division inaccuracy is present (differs from exact)", diffs > 0);
    }

    if (g_fail) { printf("MATH-PRIMITIVES: FAIL\n"); return 1; }
    printf("MATH-PRIMITIVES: all checks passed\n");
    return 0;
}
