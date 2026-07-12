/* test_catan.c — byte-true PsyQ BIOS catan (0x800658fc) + the walker heading atan2 (FUN_8001a6d4).
 *
 * The enemy/stair heading builds its bearing from the game's OWN 12-iteration CORDIC arctan `catan`,
 * NOT a float or downsampled LUT. The port previously used a port-generated ATAN256 LUT that diverged
 * from catan on 302/360 directions by up to 6 Q12 units (~0.53°). This pins the byte-true catan
 * rounding (e.g. catan(0)=1, catan(4096)=511 not 512) and the exact bearings it produces — including
 * the game's own off-by-one (+X yaw is 0x3ff, not the clean 0x400).
 */
#include <stdio.h>
#include <stdint.h>
#include "re15_math.h"     /* re15_catan */
#include "re15_actor.h"    /* re15_atan2_q12 (the public walker bearing) */

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

int main(void)
{
    printf("=== byte-true catan CORDIC + walker bearing (0x800658fc / FUN_8001a6d4) ===\n");

    /* ---- (1) catan known values — the game's exact CORDIC rounding (NOT ideal atan) ---- */
    CHECK("catan(0) = 1 (game off-by-one, not 0)",      re15_catan(0)      == 1);
    CHECK("catan(4096) = 511 (atan(1)=45deg, not 512)", re15_catan(4096)   == 511);
    CHECK("catan(-4096) = -511",                        re15_catan(-4096)  == -511);
    CHECK("catan(2048) = 301 (atan(0.5)~26.57deg)",     re15_catan(2048)   == 301);
    /* monotone + odd-ish symmetry */
    CHECK("catan is monotone at 1024<2048<4096",
          re15_catan(1024) < re15_catan(2048) && re15_catan(2048) < re15_catan(4096));

    /* ---- (2) the walker bearing (yaw, 0 = +Z) — byte-true FUN_8001a6d4 + the +Z convention ---- */
    CHECK("+Z  (dz>0,dx=0) -> yaw 0x000",  (re15_atan2_q12( 100,    0) & 0xfff) == 0x000);
    CHECK("-Z  (dz<0,dx=0) -> yaw 0x800",  (re15_atan2_q12(-100,    0) & 0xfff) == 0x800);
    CHECK("+X  (dz=0,dx>0) -> yaw 0x3ff (catan(0)=1 off-by-one, NOT 0x400)",
          (re15_atan2_q12(   0,  100) & 0xfff) == 0x3ff);
    CHECK("-X  (dz=0,dx<0) -> yaw 0xbff",  (re15_atan2_q12(   0, -100) & 0xfff) == 0xbff);
    CHECK("+X+Z (45deg)    -> yaw 0x201",  (re15_atan2_q12( 100,  100) & 0xfff) == 0x201);
    CHECK("-X+Z            -> yaw 0xdff",  (re15_atan2_q12( 100, -100) & 0xfff) == 0xdff);

    /* ---- (3) the DIVERGENCE from the old ATAN256 LUT: +X is 0x3ff, not the LUT's clean 0x400 ---- */
    CHECK("byte-true +X (0x3ff) != the old ATAN256 LUT value (0x400)",
          (re15_atan2_q12(0, 100) & 0xfff) != 0x400);

    if (g_fail) { printf("CATAN: FAIL\n"); return 1; }
    printf("CATAN: all checks passed\n");
    return 0;
}
