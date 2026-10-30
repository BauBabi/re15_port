/* test_re2_gore80.c — RIEGEL: das 80%-Gore-Aequivalent im RE2-GEHIRN (Nutzer-Auftrag 2026-10-30, GESTALTUNG).
 *
 *   zerfetzen  Zeile 7 (Schrot), Spalte 1 (Rumpf nah) = Zelle 66FC: mit Krit (+0x93 & 0x40) und Muenze laeuft das
 *              RE1.5-Zerfetzen (0x80107ee0 @0x801080c8ff) mit RE2-Mitteln — Kopf Part 8 |= 0x4A (Felder des
 *              Magnum-Kopfplatzers @0x80109460-8C), Halsstumpf Part 0 |= 0x80, je Arm Oberarm duenn (Teilschicht
 *              0x400) + Unterarm splittert ODER Oberarm splittert + Unterarm weg, Hand immer weg; danach der normale
 *              Sturz 0x80108530. Ohne Muenze bleibt es 66FC (Halbieren), ohne Krit ebenso.
 *   umordnen   Die RE1.5-Spawn-Variante (RE1.5-Teilnummern) landet auf dem RE2-Slot desselben Glieds
 *              (re2_hybrid_perm: RE1.5 12 -> RE2 13, RE1.5 14 Kopf -> RE2 8).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_esp.h"
#include "re15_room.h"
#include "re15_teile_r15.h"
#include "re2_ems.h"

void re15_re2z_death_test(re15_actor_t *e, re15_actor_t *pl);
int  re15_re2z_last_death_handler(void);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

#define Z 3
static re15_actor_t *zombie(uint8_t bits)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl); pl->active = 1; pl->hp = 100; pl->x = 0; pl->z = -1500;
    re15_actor_t *e = &g_actors[Z];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x10; e->hp = -1; e->em_flag_id = 0xFF;
    e->state = 3; e->sub_state_1 = 7; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->re2z_hits1d2 = 1; e->hit_react = bits;
    for (int i = 0; i < 16; i++) { e->re2z_part_flags[i] = (i < 15) ? 1u : 0u; e->re2z_part_tint[i] = 0x00808080u; e->re2z_part_mesh[i] = (uint8_t)i; }
    re15_teile_reset(Z);
    re15_esp_fx_reset();
    return e;
}

static int arm_ok(const re15_actor_t *e, int oben)
{
    const re15_teil_t *t = re15_teile(Z);
    int duenn = t && (t[oben].w & 0x400) && t[oben].scale[0] == 0x5f4 && t[oben].scale[1] == 0xc18
                && (e->re2z_part_flags[oben + 1] & 0x4Au) == 0x4Au && e->re2z_part_tint[oben + 1] == 0x0010104Fu;
    int platzt = (e->re2z_part_flags[oben] & 0x4Au) == 0x4Au && e->re2z_part_flags[oben + 1] == 0u;
    return (duenn ^ platzt) && e->re2z_part_flags[oben + 2] == 0u;
}

static void test_zerfetzen(void)
{
    int zerfetzt = 0, halbiert = 0;
    for (uint32_t seed = 1; seed < 80; seed++) {
        re15_actor_t *e = zombie(0x41);
        re15_damage_seed_rng(seed);
        re15_re2z_death_test(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
        CHECK(e->zg_9f & 0x10, "Seed %u: Entscheidung nicht gelatcht", seed);
        if (e->zg_9f & 0x20) {
            zerfetzt++;
            CHECK(re15_re2z_last_death_handler() == 1, "Seed %u: zerfetzt, aber Zelle %d statt MAIN", seed, re15_re2z_last_death_handler());
            CHECK((e->re2z_part_flags[8] & 0x4Au) == 0x4Au && e->re2z_part_tint[8] == 0x00101040u, "Seed %u: Kopf nicht geplatzt", seed);
            CHECK(e->re2z_part_w9c[8] == 400 && e->re2z_part_w9a[8] == -100 && e->re2z_part_wa4[8] == -50, "Seed %u: Kopf-Driftfelder", seed);
            CHECK(e->re2z_part_flags[0] & 0x80u, "Seed %u: kein Halsstumpf", seed);
            CHECK(arm_ok(e, 9), "Seed %u: R-Arm (9/10/11) %04x %04x %04x", seed, e->re2z_part_flags[9], e->re2z_part_flags[10], e->re2z_part_flags[11]);
            CHECK(arm_ok(e, 12), "Seed %u: L-Arm (12/13/14) %04x %04x %04x", seed, e->re2z_part_flags[12], e->re2z_part_flags[13], e->re2z_part_flags[14]);
            /* naechster Tick: bleibt beim Sturz */
            re15_re2z_death_test(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
            CHECK(re15_re2z_last_death_handler() == 1, "Seed %u: Folgetick nicht MAIN", seed);
        } else {
            halbiert++;
            CHECK(re15_re2z_last_death_handler() == 4, "Seed %u: ohne Zerfetzen Zelle %d statt 66FC", seed, re15_re2z_last_death_handler());
            CHECK((e->re2z_part_flags[8] & 0x4Au) != 0x4Au || e->re2z_part_tint[8] != 0x00101040u, "Seed %u: Kopf ohne Zerfetzen geplatzt", seed);
        }
    }
    CHECK(zerfetzt > 0 && halbiert > 0, "beide Zweige: zerfetzt %d halbiert %d", zerfetzt, halbiert);
    /* ohne Krit nie */
    for (uint32_t seed = 1; seed < 30; seed++) {
        re15_actor_t *e = zombie(0x01);
        re15_damage_seed_rng(seed);
        re15_re2z_death_test(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
        CHECK(!(e->zg_9f & 0x20) && re15_re2z_last_death_handler() == 4, "Seed %u: ohne Krit zerfetzt", seed);
    }
    /* andere Zeile (Magnum 5, Spalte 1 = 92C4) unberuehrt */
    {
        re15_actor_t *e = zombie(0x41); e->sub_state_1 = 5;
        re15_re2z_death_test(e, &g_actors[RE15_ACTOR_SLOT_PLAYER]);
        CHECK(re15_re2z_last_death_handler() == 3 && !(e->zg_9f & 0x30), "Magnum: Zelle %d", re15_re2z_last_death_handler());
    }
}

static void test_umordnen(void)
{
    const int8_t *perm = NULL; int n = re2_hybrid_perm(0x10, &perm);
    CHECK(n == 15 && perm, "Permutation Zombie");
    if (!perm) return;
    re15_teile_reset(Z);
    re15_teil_t *t = re15_teile(Z);
    t[12].w |= 0x480; t[12].scale[0] = 0x5f4;      /* 0x80101058: RE1.5-Teil 12 (L-Unterarm) */
    t[14].col = 0x123456;                          /* RE1.5 Kopf */
    re15_teile_markieren(Z);
    re15_teile_umordnen(Z, perm, n);
    CHECK((t[13].w & 0x480) == 0x480 && t[13].scale[0] == 0x5f4, "RE1.5 12 -> RE2 13 (L-Unterarm)");
    CHECK(!(t[12].w & 0x400), "RE2 12 (L-Oberarm) unveraendert");
    CHECK(t[8].col == 0x123456, "RE1.5 14 -> RE2 8 (Kopf)");
}

int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    g_current_room_id = 0x1140u;
    re15_actor_init(); re15_enemy_reset();
    test_zerfetzen();
    test_umordnen();
    if (fails) { fprintf(stderr, "%d Fehler\n", fails); return 1; }
    fprintf(stderr, "OK\n");
    return 0;
}
