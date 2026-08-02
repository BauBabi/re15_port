/* test_wound_accumulator.c — pins the blood-decal wound accumulator (FUN_80037edc @0x80037edc,
 * analysis/blood_decals.md BD-3/BD-5, adversarial CONFIRMED):
 *   akku += amount (@0x80037f04-08); sltiu 0x78 (@0x80037f14): sum < 120 -> nur akkumulieren;
 *   sonst akku=0 (@0x80037f24), level++ (@0x80037f2c), Clamp auf 2 (@0x80037f40/f50).
 * GROUND TRUTH (stage_saves/mzd_blood_decals_hp30.sav): 3x Hurt-Substate-0-Helper
 * (@0x8010a1cc: Panel 0 +10, Panel 5 +50, Panel 7 +50) ->
 *   Panel 0: level 0, akku 30 · Panels 5/7: level 1, akku 0 · Rest 0/0. */
#include <stdio.h>
#include "re15_damage.h"

static int fails = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("FAIL: %s\n", msg); fails++; } } while (0)

int main(void)
{
    re15_wound_reset();

    /* Ground-Truth-Sequenz: 3x Helper-Substate-0 */
    for (int i = 0; i < 3; i++) {
        re15_wound_add(0, 10);
        re15_wound_add(5, 50);
        re15_wound_add(7, 50);
    }
    CHECK(re15_wound_level(0) == 0, "Panel 0 bleibt Level 0 (3x10=30 < 120)");
    CHECK(re15_wound_level(5) == 1, "Panel 5 -> Level 1 (150 >= 120 beim 3. Treffer)");
    CHECK(re15_wound_level(7) == 1, "Panel 7 -> Level 1");
    for (int p = 1; p < 8; p++)
        if (p != 5 && p != 7)
            CHECK(re15_wound_level(p) == 0, "unbeteiligte Panels bleiben 0");

    /* Schwellen-Kante: sltiu 0x78 — Summe EXAKT 120 stempelt (120 ist nicht < 120). */
    re15_wound_reset();
    re15_wound_add(1, 70);
    CHECK(re15_wound_level(1) == 0, "70 < 120: nur Akku");
    re15_wound_add(1, 50);
    CHECK(re15_wound_level(1) == 1, "70+50 = 120: stempelt (sltiu-Kante)");

    /* Level-Clamp @0x80037f40/f50: dritter Stempel bleibt bei 2. */
    re15_wound_reset();
    for (int i = 0; i < 5; i++) re15_wound_add(2, 120);
    CHECK(re15_wound_level(2) == 2, "Level-Clamp auf 2");

    /* Reset (Builder FUN_80037c1c @0x80037c48/ce8): alles auf 0. */
    re15_wound_reset();
    for (int p = 0; p < 8; p++) CHECK(re15_wound_level(p) == 0, "Reset nullt alle Panels");

    if (fails == 0) { printf("test_wound_accumulator: OK\n"); return 0; }
    printf("test_wound_accumulator: %d FAILURES\n", fails);
    return 1;
}
