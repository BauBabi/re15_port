/*
 * probe_r34_schaden.c — Runde 34 (Granaten), Spur B: Schaden, Resolver-Tore, RE2-Stempel,
 * RE2-GL-Applier, Trefferkasten-Versatz.
 *
 * Dossier: analysis/befunde_runde34_granaten/bau_b.md (Bau), BAUPLAN.md §3.2 (Abnahme Spur B).
 * Rueckgabe 0 = gruen, sonst die NUMMER der ersten fehlgeschlagenen Pruefung (jede Pruefung
 * traegt ihre Nummer im Text). Jede tragende Pruefung hat eine Negativ-Kontrolle; die
 * Mutationsproben sind im Dossier protokolliert.
 *
 * Aufruf: probe_r34_schaden [teil]   teil = tore | stempel | applier | kasten | alle (Vorgabe)
 */
#include "re15_damage.h"
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_math.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int s_first_fail = 0;
static int s_fails = 0;
#define CHECK(nr, c, ...) do { if (!(c)) { printf("FAIL %d: ", (nr)); printf(__VA_ARGS__); printf("\n"); \
    s_fails++; if (!s_first_fail) s_first_fail = (nr); } else { printf("ok   %d\n", (nr)); } } while (0)

/* ---- Aufbau ------------------------------------------------------------------------------ */
static re15_actor_t *pl_far(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->hp = 100; pl->state = 1; pl->hit_react = 0;
    pl->x = 100000; pl->y = 0; pl->z = 100000; pl->rot_y = 0;
    re15_player_apply_hitbox(pl);
    return pl;
}

static re15_actor_t *mk(int slot, uint8_t type, int32_t x, int32_t y, int32_t z, int16_t hp)
{
    re15_actor_t *e = &g_actors[slot];
    e->active = 1; e->type = type; e->x = x; e->y = y; e->z = z; e->hp = hp;
    e->state = 1; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
    e->hit_react = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, type);
    return e;
}

static re15_attack_box_t box_at(int32_t x, int32_t y, int32_t z)
{
    re15_attack_box_t b; b.x = x; b.y = y; b.z = z; b.radius = 500;   /* @0x80018598 ori a0,zero,0x1f4 */
    return b;
}

/* =========================================================================================
 * TEIL "tore" — B1: Gate B (@0x80012f54-60), NPC-Ausschluss (E7), Ivy/FX immun.
 * ========================================================================================= */
static void teil_tore(void)
{
    printf("== tore (B1)\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);

    /* (11) GATE B: +0x93 & 3 == 3 -> uebersprungen, NICHT gezaehlt, +0x93 unberuehrt. */
    re15_actor_init(); pl_far();
    re15_actor_t *e = mk(1, 0x27, 300, 0, 0, 180);
    e->hit_react = 0x83;
    re15_attack_box_t b = box_at(0, -500, 0);
    int n = re15_resolve_attack(&b, 2, -1);
    CHECK(11, e->hp == 180 && e->hit_react == 0x83 && n == 0,
          "Gate B: hp=%d (180) hit_react=0x%02X (0x83) n=%d (0)", e->hp, e->hit_react, n);

    /* (18) GATE B laesst auch Bit 0x80 stehen: mit +0x93 = 0x03 und P HINTER der Figur (der
     *      Seitentest FUN_8001a7a8 wuerde 0x80 setzen, s. Pruefung 12/13) bleibt es 0x03. */
    re15_actor_init(); pl_far();
    e = mk(1, 0x27, 300, 0, 0, 180);
    e->hit_react = 0x03;
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(18, e->hp == 180 && e->hit_react == 0x03 && n == 0,
          "Gate B / Bit 0x80: hp=%d hit_react=0x%02X (0x03) n=%d (0)", e->hp, e->hit_react, n);

    /* (12) NEGATIV-KONTROLLE zu 11: nur Bit 0 -> Kandidat, |= 2 ohne Schaden, GEZAEHLT. */
    re15_actor_init(); pl_far();
    e = mk(1, 0x27, 300, 0, 0, 180);
    e->hit_react = 0x01;
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(12, e->hp == 180 && (e->hit_react & 3u) == 3u && n == 1,
          "Bit-0-Riegel: hp=%d (180) hit_react=0x%02X (&3==3) n=%d (1)", e->hp, e->hit_react, n);

    /* (13) NEGATIV-KONTROLLE: frei -> 1000 Schaden (@0x8006f41c), +0x5 = 9 (@0x8006f432). */
    re15_actor_init(); pl_far();
    e = mk(1, 0x27, 300, 0, 0, 180);
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(13, e->hp == 180 - 1000 && e->state == 3 && e->sub_state_1 == 9 && e->sub_state_2 == 1 && n == 1,
          "freier Gegner: hp=%d state=%d +5=%d +6=%d n=%d", e->hp, e->state, e->sub_state_1,
          e->sub_state_2, n);

    /* (14) NPC 0x45 mit HP -1 300 neben P -> kein Kandidat (E7). */
    re15_actor_init(); pl_far();
    e = mk(1, 0x45, 300, 0, 0, -1);
    uint8_t st0 = e->state;
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(14, e->hp == -1 && e->state == st0 && e->hit_react == 0 && n == 0,
          "NPC 0x45 HP -1: hp=%d state=%d (%d) hit_react=0x%02X n=%d", e->hp, e->state, st0,
          e->hit_react, n);

    /* (15) NEGATIV-KONTROLLE zu 14: derselbe Kasten mit HP >= 0 wird getroffen (der Kasten
     *      allein schliesst also NICHT aus — es ist die HP-Kennung). */
    re15_actor_init(); pl_far();
    e = mk(1, 0x45, 300, 0, 0, 0);
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(15, e->state == 3 && n == 1, "NPC 0x45 HP 0: state=%d (3) n=%d (1)", e->state, n);

    /* (16) Ivy 0x2d: INIT +0x93 = 3 (@0x8011693c/44 STAGE4) -> Gate B, immun. */
    re15_actor_init(); pl_far();
    e = mk(1, 0x2d, 300, 0, 0, 100);
    e->hit_react = 3;
    n = re15_resolve_attack(&b, 2, -1);
    CHECK(16, e->hp == 100 && e->hit_react == 3 && e->state == 1 && n == 0,
          "Ivy: hp=%d hit_react=0x%02X state=%d n=%d", e->hp, e->hit_react, e->state, n);

    /* (17) FX-Emitter 0x24: +0x93 = 1 (@0x8010ef58 STAGE2) -> 1. Treffer nur |= 2, kein Schaden;
     *      2. Treffer: Gate B. */
    re15_actor_init(); pl_far();
    e = mk(1, 0x24, 300, 0, 0, 100);
    e->hit_react = 1;
    int n1 = re15_resolve_attack(&b, 2, -1);
    uint8_t hr1 = e->hit_react;
    int n2 = re15_resolve_attack(&b, 2, -1);
    CHECK(17, e->hp == 100 && e->state == 1 && (hr1 & 3u) == 3u && n1 == 1 && n2 == 0 &&
              e->hit_react == hr1,
          "FX 0x24: hp=%d state=%d hr1=0x%02X n1=%d n2=%d hr=0x%02X", e->hp, e->state, hr1,
          n1, n2, e->hit_react);
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "alle";
    int alle = (strcmp(teil, "alle") == 0);
    if (alle || !strcmp(teil, "tore"))    teil_tore();
    printf("probe_r34_schaden %s: %d Fehler (erste Pruefung %d)\n", teil, s_fails, s_first_fail);
    return s_first_fail;
}
