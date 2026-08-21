/* test_re2_dog_jaw_contact.c — PIN fuer den RE2-Hunde-KONTAKTTEST (FUN_80104DF0 Naehe-Gate).
 *
 * Quelle: info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN (RAW @0x80100000) + info/re2leon/PSX.EXE.
 * Disassemblieren:
 *   .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMD0G_MOD0.BIN
 *
 * BELEG (selbst disassembliert 2026-08-21):
 *   80104e10: lw    a1,408(s0)          ; a1 = *(self+0x198) = PART-POOL
 *   80104e24: addiu a1,a1,688           ; + 4*0xAC  -> PART 4
 *   80104e34: lh    v0,342(s1)          ; Ziel-HP
 *   80104e3c: slti  v0,v0,21
 *   80104e40: bne   v0,zero,0x80104e4c
 *   80104e44: addiu a2,zero,700         ; Delay-Slot: r = 700  (HP < 21)
 *   80104e48: addiu a2,zero,1000        ; sonst      r = 1000
 *   80104e70: addiu a0,s1,56            ; a0 = Ziel+0x38 (Welt-X; Z @+0x40)
 *   80104e74: jal   0x800157d4
 *   80104e78: addiu a1,a1,92            ; Delay-Slot: +0x5C = Welt-Translation des Parts
 *   80104e7c: beq   v0,zero,0x80104fcc  ; kein Kontakt
 *   FUN_800157D4 (RE2-EXE):
 *   800157e0/e4: lw v0,0(a0) / lw v1,0(a1)   -> dx  (Feld +0)
 *   800157f8/fc: lw v0,8(a0) / lw v1,8(a1)   -> dz  (Feld +8)
 *   80015814:    jal 0x8008d2f4 (GTE-sqrt)
 *   8001581c:    sltu v0,v0,a2               -> dist2D < r  (STRIKT)
 *
 * Der Port mass hier frueher die HUND-WURZEL statt Part 4. Folge (gemessen, ROOM1190,
 * probe_re2_dog_kill): sobald die Spieler-HP unter 21 fallen und der Radius auf 700 sinkt,
 * traf der Hund praktisch nie mehr -> HP fror auf dem letzten Biss-Wert ein und der toedliche
 * Griff (Sub 7) wurde nie erreicht. Dieser Test pinnt den Kiefer-Anker gegen genau das.
 *
 * Der Test braucht die ECHTE RE2-EM020-Bank (ohne Skelett faellt der Bone-Helfer auf die
 * Wurzel zurueck und der Test waere vacuous) -> ohne shared_assets/RE2/CDEMD0.EMS SKIP.
 */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re2_ems.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static re15_actor_t *PL, *DOG;
static uint8_t *g_ems = NULL; static long g_ems_sz = 0;

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* Hund mitten im FLUG (Sub 3 P1) — genau der Zustand, in dem 0x80101488 den Kontakt prueft.
 * speed_h = 0, damit move3d den Hund vor dem Kontakt-Test NICHT horizontal versetzt und die
 * gemessene Kiefer-Position exakt die des Tests ist. */
static void fresh_flight(int16_t player_hp)
{
    re15_actor_init();
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    PL  = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    DOG = &g_actors[1];
    memset(PL, 0, sizeof *PL);
    memset(DOG, 0, sizeof *DOG);
    PL->active = 1; PL->hp = player_hp; PL->floor = 0;
    DOG->active = 1; DOG->type = 0x20; DOG->hp = 90; DOG->floor = 0;
    DOG->state = 1; DOG->sub_state_1 = 3; DOG->sub_state_2 = 1; DOG->sub_state_3 = 0;
    DOG->x = 0; DOG->y = 0; DOG->z = 0; DOG->rot_y = 0; DOG->dog_floor_y = 0;
    DOG->speed_h = 0; DOG->re2d_vy146 = -400; DOG->re2d_air219 = 1;
    DOG->motion = 20; DOG->anim_frame = 4;
    DOG->re2d_bite21e = 0; DOG->re2d_nolatch22c = 0;
    /* frontal: 0x80015910(Hund,Spieler) == 0 (@0x80104EEC-F8) — haelt das Latch-Gate offen,
     * damit ein Treffer nicht am RICHTUNGS-Gate scheitert statt am Naehe-Gate */
    PL->rot_y = (int16_t)(((int)DOG->rot_y + 2048) & 0xfff);
    /* echte RE2-Bank (Skelett fuer Part 4) */
    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (eb && re2_ems_load_bank(g_ems, (size_t)g_ems_sz, 0x20, eb, NULL) == 0)
        { eb->buf = NULL; eb->ok = 1; }
}

static void jaw_pos(int32_t out[3]) { re15_enemy_bone_world_pos(DOG, 4, out); }

static int bite_once(void)
{
    int16_t before = PL->hp;
    DOG->ai_dist = 0;
    re15_re2dog_tick(1);
    return (int)(before - PL->hp);
}

int main(void)
{
    g_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &g_ems_sz);
    if (!g_ems) { printf("test_re2_dog_jaw_contact: SKIP (CDEMD0.EMS fehlt)\n"); return 0; }

    /* --- 0: das Rig muss Part 4 wirklich als vordersten Bone fuehren ---------------------- */
    {
        fresh_flight(50);
        int32_t jaw[3]; jaw_pos(jaw);
        int lead = jaw[0] - DOG->x;            /* rot_y = 0 -> Blickachse = +X */
        CHECK(lead > 400,
              "Part 4 muss im geladenen EM020-Rig deutlich VOR der Wurzel liegen (sonst ist der "
              "Test vacuous / die Bank fehlt) — Vorlauf=%d", lead);
    }

    /* --- 1: KIEFER im Radius, WURZEL ausserhalb -> Kontakt (die reparierte Regression) ---- */
    {
        fresh_flight(50);
        int32_t jaw[3]; jaw_pos(jaw);
        PL->x = jaw[0] + 900; PL->z = jaw[2];              /* Kiefer-Abstand 900 < 1000 */
        int64_t rdx = PL->x - DOG->x, rdz = PL->z - DOG->z;
        CHECK(rdx * rdx + rdz * rdz > 1000LL * 1000LL,
              "Aufbau: die WURZEL-Distanz muss > 1000 sein, sonst testet der Fall nichts");
        int drop = bite_once();
        CHECK(drop == 20,
              "Kiefer 900 < r=1000 -> Biss 20 HP (FUN_800157D4 auf Part 4 @0x80104E24/E74), "
              "drop=%d", drop);
        CHECK(DOG->re2d_bite21e != 0, "Kontakt muss +0x21E setzen (@0x80104F24/@0x80104F84)");
    }

    /* --- 2: NEGATIV — Wurzel im Radius, Kiefer ausserhalb -> KEIN Kontakt ----------------- */
    {
        fresh_flight(50);
        int32_t jaw[3]; jaw_pos(jaw);
        PL->x = DOG->x - 900; PL->z = DOG->z;              /* hinter dem Hund: Wurzel 900 < 1000 */
        int64_t jdx = PL->x - jaw[0], jdz = PL->z - jaw[2];
        CHECK(jdx * jdx + jdz * jdz > 1000LL * 1000LL,
              "Aufbau: die KIEFER-Distanz muss > 1000 sein");
        int drop = bite_once();
        CHECK(drop == 0,
              "Wurzel 900 aber Kiefer > 1000 -> KEIN Biss (der Test misst Part 4, nicht +0x38 "
              "des Hundes), drop=%d", drop);
        CHECK(DOG->re2d_bite21e == 0, "kein Kontakt -> +0x21E bleibt 0");
    }

    /* --- 3: Radius-Umschaltung 1000/700 am Ziel-HP-Schwellwert 21 (@0x80104E34-48) -------- */
    {
        fresh_flight(50);                                   /* HP 50 >= 21 -> r = 1000 */
        int32_t jaw[3]; jaw_pos(jaw);
        PL->x = jaw[0] + 800; PL->z = jaw[2];               /* 700 < 800 < 1000 */
        CHECK(bite_once() == 20, "HP>=21 -> r=1000: Kiefer 800 trifft (@0x80104E48)");
    }
    {
        fresh_flight(20);                                   /* HP 20 < 21 -> r = 700 */
        int32_t jaw[3]; jaw_pos(jaw);
        PL->x = jaw[0] + 800; PL->z = jaw[2];               /* 800 > 700 */
        int drop = bite_once();
        CHECK(drop == 0,
              "HP<21 -> r=700: Kiefer 800 trifft NICHT (slti 21 @0x80104E3C, 700 @0x80104E44), "
              "drop=%d", drop);
    }
    {
        fresh_flight(20);                                   /* HP 20 < 21, Kiefer 600 < 700 */
        int32_t jaw[3]; jaw_pos(jaw);
        PL->x = jaw[0] + 600; PL->z = jaw[2];
        int drop = bite_once();
        CHECK(drop == 20, "HP<21 -> r=700: Kiefer 600 trifft (drop=%d)", drop);
        /* 20-20 = 0 -> FUN_800401d4 gibt 0 zurueck (HP nicht < 0) -> Boden-Biss, kein Latch */
        CHECK(PL->hp == 0, "HP 20-20 = 0 (kein One-Save noetig), hp=%d", PL->hp);
        CHECK(DOG->re2d_bite21e == 1,
              "HP 0 ist nicht < 0 -> Rueckgabe 0 -> Boden-Biss +0x21E=1 (@0x80104F84), val=%d",
              DOG->re2d_bite21e);
    }

    /* --- 4: NEGATIV — abgewandtes Ziel latcht NICHT, sondern bekommt den Boden-Biss ------- */
    {
        fresh_flight(0);
        PL->rot_y = DOG->rot_y;                             /* 0x80015910 != 0 -> kein Latch */
        int32_t jaw[3]; jaw_pos(jaw);
        PL->x = jaw[0] + 600; PL->z = jaw[2];
        int drop = bite_once();
        CHECK(drop == 20, "abgewandtes Ziel wird trotzdem gebissen, drop=%d", drop);
        CHECK(DOG->re2d_bite21e == 1,
              "0x80015910 != 0 -> `bne v0,zero,0x80104F7C` @0x80104EF8: Boden-Biss statt Latch, "
              "val=%d", DOG->re2d_bite21e);
    }

    /* --- 5: der TOEDLICHE Biss aus HP 0 heraus latcht (die Kette, die der Report vermisste).
     * MUSS ZULETZT laufen: der Latch setzt den globalen Opfer-Pin, der `re15_player_is_grabbed()`
     * (@0x80104E5C-68) in jedem Folgefall den Kontakt sperren wuerde. */
    {
        fresh_flight(0);                                    /* HP 0 -> 0-20 = -20 < -14 -> ret 2 */
        int32_t jaw[3]; jaw_pos(jaw);
        PL->x = jaw[0] + 600; PL->z = jaw[2];
        int drop = bite_once();
        CHECK(drop == 20, "toedlicher Biss zieht ebenfalls 20 (@0x80104EB8), drop=%d", drop);
        CHECK(PL->hp == -20, "HP 0-20 = -20 (< -14 -> FUN_800401d4 ret 2), hp=%d", PL->hp);
        CHECK(DOG->re2d_bite21e == 2,
              "Rueckgabe 2 + frontales Ziel -> LATCH +0x21E=2 (sb 2,6(s3) @0x80104F24), val=%d",
              DOG->re2d_bite21e);
        CHECK(((PL->rot_y - DOG->rot_y) & 0xfff) == 2048,
              "Latch stellt den Spieler-Yaw auf Hund+2048 (@0x80104F40-50), diff=%d",
              (PL->rot_y - DOG->rot_y) & 0xfff);
    }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    if (fails == 0) printf("test_re2_dog_jaw_contact: OK\n");
    else            printf("test_re2_dog_jaw_contact: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
