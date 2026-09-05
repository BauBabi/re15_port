/* test_re2z_spawn_seed.c — PIN (S4-Fix 2026-09-05): Freeze-Fenster-Posen-Seed des RE2-Flavors.
 *
 * Nutzer-Report: "Wenn ich den Raum wechsle (1050->1030), starten die Zombies erst alle
 * LIEGEND und stehen dann ploetzlich mit einem Clip."
 * Ursache (diag_zombie_lying_spawn.md, verify_lying_spawn.md): der byte-true RE1.5-Spawn-Seed
 * (Clip 0x27=39 fuer behavior 0x0D @0x80100F20-54) liegt ausserhalb der 31-Clip-RE2-Bank;
 * der Renderer-Modulo (anim_select_common.c:75) machte daraus 39%31=8 = Boden-Aufsteher
 * (Frame 0 flach). Fix: re15_re2z_spawn_pose_seed saet den Clip, den re2z_init committen
 * wird (Original-INIT-Seeds: Clip-Wort 1 @0x801009B8/D4, Liege 22/23 @0x801009D8-0x80100A94,
 * Fresser 18 @0x80100AD0-DC, Kriecher 23 @0x80100AF8-FC; RE2-Sce_em_set-Seed Clip 0
 * @0x800576A4 wird nie gerendert, weil der Original-INIT im Lade-Frame laeuft).
 *
 * PIN: die per-behavior-Tabelle (Verify-Korrektur: NICHT pauschal Clip 0 — das haette
 * Fresser/Lieger im Fade aufrecht gestellt). Plus RE1.5-Regressionswache: der RE1.5-Seed
 * in scd_vm bleibt unangetastet (Gate = re15_ai_re2_for_type). */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy.h"      /* re15_re2z_re15_pose_anim (0x0E-Sitz-Import-Gate) */

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static re15_actor_t A;

static int seed_for(uint8_t behavior)
{
    memset(&A, 0, sizeof A);
    A.type = 0x16;
    re15_re2z_spawn_pose_seed(&A, behavior);
    return (int)A.motion;
}

int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    /* 0x0D (1030/1040/1200-Massen) + DEFAULT -> Clip-Wort 1 (@0x801009B8/D4) */
    CHECK(seed_for(0x0d) == 1,  "0x0D -> 1, ist %d", seed_for(0x0d));
    CHECK(seed_for(0x00) == 1,  "0x00 -> 1, ist %d", seed_for(0x00));
    CHECK(seed_for(0x02) == 1,  "0x02 -> 1, ist %d", seed_for(0x02));

    /* Fresser sel 6 -> 18 (@0x80100AD0-DC); 1140/10E0 tragen 0x86 */
    CHECK(seed_for(0x06) == 18, "0x06 -> 18, ist %d", seed_for(0x06));
    CHECK(seed_for(0x86) == 18, "0x86 -> 18, ist %d", seed_for(0x86));

    /* Liege-Familien: Seite A (RE1.5 {4,7,9} = Clip-0x12-Familie) -> 23; Seite B
     * ({5,8,0xa} = 0x13-Familie) -> 22 (+0x21A|=4, dieselbe Wahl wie EXEC[7] P0
     * @0x801037D0-E4). 1020/1070/1100/1140 tragen 0x87/0x88. */
    CHECK(seed_for(0x87) == 23, "0x87 -> 23 (Seite A), ist %d", seed_for(0x87));
    CHECK(seed_for(0x84) == 23, "0x84 -> 23 (Seite A), ist %d", seed_for(0x84));
    CHECK(seed_for(0x88) == 22, "0x88 -> 22 (Seite B), ist %d", seed_for(0x88));
    CHECK((A.re2z_flags21a & 0x4u) != 0, "0x88 setzt die Seite +0x21A|=4");
    CHECK(seed_for(0x85) == 22, "0x85 -> 22 (Seite B), ist %d", seed_for(0x85));
    CHECK(seed_for(0x08) == 22, "0x08 (ungegatet, wie Port-INIT) -> 22, ist %d", seed_for(0x08));

    /* Kriecher-Deskriptor 0x81/0x83 (1010/1220/3010) -> 23 (@0x80100AF8-FC) */
    CHECK(seed_for(0x81) == 23, "0x81 -> 23, ist %d", seed_for(0x81));
    CHECK(seed_for(0x83) == 23, "0x83 -> 23, ist %d", seed_for(0x83));

    /* 10D0-Sitzer 0x0E: ohne registrierte RE1.5-Pose-Bank -> Fallback Seite B, Clip 22 */
    if (re15_re2z_re15_pose_anim() == NULL) {
        CHECK(seed_for(0x0e) == 22, "0x0E (Fallback ohne Pose-Bank) -> 22, ist %d", seed_for(0x0e));
        CHECK((A.re2z_flags21a & 0x4u) != 0, "0x0E-Fallback setzt Seite 0x4");
    } else {
        CHECK(seed_for(0x0e) == 0x2a && A.re2z_re15_pose == 1,
              "0x0E -> RE1.5-Sitz 0x2A + re2z_re15_pose, ist %d/%d",
              seed_for(0x0e), (int)A.re2z_re15_pose);
    }

    /* Alle Seeds muessen IN der 31-Clip-RE2-Bank liegen (ausser dem 0x2A-Sitz-Import,
     * der ueber die RE1.5-Pose-Bank rendert) — der Kern des Fixes. */
    static const uint8_t behs[] = {0x00,0x02,0x06,0x0d,0x81,0x83,0x84,0x85,0x86,0x87,0x88,0x08};
    for (unsigned i = 0; i < sizeof behs; i++) {
        int m = seed_for(behs[i]);
        CHECK(m >= 0 && m < 31, "Seed fuer 0x%02x muss < 31 sein, ist %d", behs[i], m);
    }

    printf(fails ? "test_re2z_spawn_seed: %d FAIL(s)\n" : "test_re2z_spawn_seed: OK\n", fails);
    return fails ? 1 : 0;
}
