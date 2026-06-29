/**
 * @file test_enemy_ai.c
 * @brief Unit-Tests fuer die byte-true Enemy-AI-FSM-Foundation (Phase 2).
 *
 * Testet re15_enemy_ai.c isoliert (kein game_step-Tick → kein 1170-Risiko):
 *   - INIT (FUN_8011d84c): state 0→1 (ACTIVE), Guards geleert, AI-Params (arc 0x2c8…),
 *     Hitbox verdrahtet; stationär (grid_id & 0x40) → state 4 (IDLE) / sub 6.
 *   - State-Wort-Helfer: 0x701 → state 1 / sub 7, 0x201 → 1/2, 0x1001 → 1/0x10.
 *   - TICK-Entry (FUN_8011d6d4): Gate (pause + grid_id&0x20), dist@+0x1d0 byte-true,
 *     Main-Dispatch (INIT läuft → state wird 1).
 *   - ACTIVE-Sub-Dispatch (FUN_8011d9f4): grid_id & 0xf.
 *   - Decision-Brain FUN_80101b64 / FUN_80101c7c / FUN_80101de4 (3-handler-confirmed):
 *     Timer→0x101, Attack→0x701, Approach→0x201, Wander(0x801, RNG&7|&3), 0x1001.
 */
#include "re15_enemy_ai.h"
#include "re15_actor.h"
#include "re15_damage.h"

#include <stdint.h>
#include <stdio.h>

/* Frischer Gegner in Slot 1: aktiv, Typ 0x47 (STAGE1-Zombie), Blick +Z (rot_y -1024). */
static re15_actor_t *fresh_enemy(uint8_t type)
{
    re15_actor_init();                 /* slot 0 = player, hp 100, am Ursprung */
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = type;
    e->state = RE15_AI_STATE_INIT;     /* frisch gespawnt = state 0 */
    e->rot_y = -1024;                  /* faces +Z (wie test_damage) */
    return e;
}

/* Spieler positionieren (für die Arc-Tests). */
static void put_player(int32_t x, int32_t z) { g_actors[0].x = x; g_actors[0].z = z; }

/* ----- INIT (FUN_8011d84c): state→1, Guards, AI-Params, Hitbox ----- */
static int test_ai_init(void)
{
    re15_actor_t *e = fresh_enemy(0x47);
    e->hit_react = 0xFF; e->anim_frac = 9; e->anim_frame = 42;

    re15_enemy_ai_init(1);

    if (e->state != RE15_AI_STATE_ACTIVE) { fprintf(stderr, "FAIL: init state→1, ist %d\n", e->state); return 1; }
    if (e->hit_react != 0)  { fprintf(stderr, "FAIL: init +0x93 guard geleert, ist 0x%02X\n", e->hit_react); return 1; }
    if (e->anim_frac != 0)  { fprintf(stderr, "FAIL: init +0x8f anim_frac=0, ist %d\n", e->anim_frac); return 1; }
    if (e->anim_frame != 0) { fprintf(stderr, "FAIL: init +0x95 frame=0, ist %d\n", e->anim_frame); return 1; }
    if (e->ai_arc != 0x2c8) { fprintf(stderr, "FAIL: init ai_arc=0x2c8, ist 0x%x\n", e->ai_arc); return 1; }
    if (e->ai_p5f8 != 0x40 || e->ai_p5fa != 0x30 || e->ai_p5fe != 0x138) {
        fprintf(stderr, "FAIL: init params 0x40/0x30/0x138, ist 0x%x/0x%x/0x%x\n",
                e->ai_p5f8, e->ai_p5fa, e->ai_p5fe); return 1; }
    /* Hitbox per Typ verdrahtet (0x47 = 450/1530). */
    if (e->hit_radius_min != 450 || e->hit_height != 1530) {
        fprintf(stderr, "FAIL: init Hitbox 450/1530, ist %u/%u\n", e->hit_radius_min, e->hit_height); return 1; }

    printf("PASS: test_ai_init\n");
    return 0;
}

/* ----- INIT stationär: grid_id & 0x40 → state 4 (IDLE), sub 6 ----- */
static int test_ai_init_stationary(void)
{
    re15_actor_t *e = fresh_enemy(0x47);
    e->grid_id = RE15_AI_GRID_STATIONARY;     /* 0x40 — Briefing-Zombie */

    re15_enemy_ai_init(1);

    if (e->state != RE15_AI_STATE_IDLE) { fprintf(stderr, "FAIL: stationär state→4, ist %d\n", e->state); return 1; }
    if (e->sub_state_1 != 6)            { fprintf(stderr, "FAIL: stationär sub→6, ist %d\n", e->sub_state_1); return 1; }
    printf("PASS: test_ai_init_stationary\n");
    return 0;
}

/* ----- State-Wort-Helfer: 32-bit-Wort → Byte-Felder ----- */
static int test_state_word(void)
{
    re15_actor_t e = {0};
    re15_ai_set_state_word(&e, 0x701);
    if (e.state != 1 || e.sub_state_1 != 7) { fprintf(stderr, "FAIL: 0x701→1/7, ist %d/%d\n", e.state, e.sub_state_1); return 1; }
    re15_ai_set_state_word(&e, 0x201);
    if (e.state != 1 || e.sub_state_1 != 2) { fprintf(stderr, "FAIL: 0x201→1/2, ist %d/%d\n", e.state, e.sub_state_1); return 1; }
    re15_ai_set_state_word(&e, 0x1001);
    if (e.state != 1 || e.sub_state_1 != 0x10) { fprintf(stderr, "FAIL: 0x1001→1/0x10, ist %d/0x%x\n", e.state, e.sub_state_1); return 1; }
    re15_ai_set_state_word(&e, 0x101);
    if (e.state != 1 || e.sub_state_1 != 1) { fprintf(stderr, "FAIL: 0x101→1/1, ist %d/%d\n", e.state, e.sub_state_1); return 1; }
    printf("PASS: test_state_word\n");
    return 0;
}

/* ----- TICK-Entry (FUN_8011d6d4): Gate + dist-Cache + Main-Dispatch ----- */
static int test_ai_tick(void)
{
    /* Normal: dist gecacht (byte-true), INIT dispatcht → state 1. */
    re15_actor_t *e = fresh_enemy(0x47);
    put_player(1200, 0);                       /* dx=1200, dz=0 → dist 1200 */
    e->x = 0; e->z = 0;
    re15_enemy_ai_set_paused(0);
    int r = re15_enemy_ai_tick(1);
    if (r != 1)            { fprintf(stderr, "FAIL: tick sollte dispatchen (1), war %d\n", r); return 1; }
    if (e->ai_dist != 1200){ fprintf(stderr, "FAIL: tick dist@+0x1d0=1200, ist %u\n", e->ai_dist); return 1; }
    if (e->state != RE15_AI_STATE_ACTIVE) { fprintf(stderr, "FAIL: tick INIT→state1, ist %d\n", e->state); return 1; }

    /* Per-Entity-Skip (grid_id & 0x20): Gate greift → kein Dispatch, kein dist-Update. */
    re15_actor_t *e2 = fresh_enemy(0x47);
    e2->grid_id = RE15_AI_GRID_SKIP;           /* 0x20 */
    e2->ai_dist = 0xDEAD;
    int r2 = re15_enemy_ai_tick(1);
    if (r2 != 0)             { fprintf(stderr, "FAIL: skip-gate muss 0 liefern, war %d\n", r2); return 1; }
    if (e2->ai_dist != 0xDEAD){ fprintf(stderr, "FAIL: skip-gate darf dist nicht updaten\n"); return 1; }

    /* Engine-Pause (DAT_800aca40 & 0x20000000): Gate greift global. */
    re15_actor_t *e3 = fresh_enemy(0x47);
    e3->ai_dist = 0xBEEF;
    re15_enemy_ai_set_paused(1);
    int r3 = re15_enemy_ai_tick(1);
    re15_enemy_ai_set_paused(0);
    if (r3 != 0)             { fprintf(stderr, "FAIL: pause-gate muss 0 liefern, war %d\n", r3); return 1; }
    if (e3->ai_dist != 0xBEEF){ fprintf(stderr, "FAIL: pause-gate darf dist nicht updaten\n"); return 1; }

    printf("PASS: test_ai_tick\n");
    return 0;
}

/* ----- ACTIVE-Sub-Dispatch (FUN_8011d9f4): grid_id & 0xf ----- */
static int test_ai_active_dispatch(void)
{
    re15_actor_t *e = fresh_enemy(0x47);
    e->grid_id = 0x40 | 0x07;                  /* stationary-Bit + Sub-Index 7 */
    int sub = re15_enemy_ai_active(1);
    if (sub != 7) { fprintf(stderr, "FAIL: active sub-index = grid_id&0xf = 7, war %d\n", sub); return 1; }
    printf("PASS: test_ai_active_dispatch\n");
    return 0;
}

/* ----- Decision FUN_80101b64 (search+timer): Timer/Attack/Approach ----- */
static int test_decide_search_timer(void)
{
    /* (a) Timer 0 → 0x101 (Spieler vorne+fern, kein attack/approach). */
    re15_actor_t *e = fresh_enemy(0x47);
    put_player(0, 50000);                      /* vorne (+Z): narrow==0, wide==0 */
    e->ai_dist = 50000u; e->ai_flags = 0; e->ai_timer = 0;
    re15_ai_decide_search_timer(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 1) { fprintf(stderr, "FAIL: timer0→0x101, ist %d/%d\n", e->state, e->sub_state_1); return 1; }
    if (e->ai_timer != -1) { fprintf(stderr, "FAIL: timer dekrementiert auf -1, ist %d\n", e->ai_timer); return 1; }

    /* (b) Attack: nah (<2000) + off narrow arc (+X) + KEIN approach-Gate → 0x701. */
    e = fresh_enemy(0x47);
    put_player(1000, 0);                       /* rechts (+X): narrow!=0, wide==0 */
    e->ai_dist = 1000u; e->ai_flags = 0; e->ai_timer = 100;
    re15_ai_decide_search_timer(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 7) { fprintf(stderr, "FAIL: attack→0x701, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    /* (c) Approach: in wide arc (+X) + Gate (0x10) → 0x201 (overridet 0x701). */
    e = fresh_enemy(0x47);
    put_player(1000, 0);
    e->ai_dist = 1000u; e->ai_flags = 0x10; e->ai_timer = 100;
    re15_ai_decide_search_timer(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 2) { fprintf(stderr, "FAIL: approach→0x201, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    printf("PASS: test_decide_search_timer\n");
    return 0;
}

/* ----- Decision FUN_80101c7c (approach-only): dist<4000-Gate ----- */
static int test_decide_approach(void)
{
    /* nah (<4000) + wide arc + Gate → 0x201. */
    re15_actor_t *e = fresh_enemy(0x47);
    put_player(1000, 0);
    e->ai_dist = 1000u; e->ai_flags = 0x10; e->state = 5; e->sub_state_1 = 5;
    re15_ai_decide_approach(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 2) { fprintf(stderr, "FAIL: approach nah→0x201, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    /* fern (>=4000) → keine Transition (state unverändert). */
    e = fresh_enemy(0x47);
    put_player(1000, 0);
    e->ai_dist = 5000u; e->ai_flags = 0x10; e->state = 5; e->sub_state_1 = 5;
    re15_ai_decide_approach(e, &g_actors[0]);
    if (e->state != 5) { fprintf(stderr, "FAIL: approach fern darf nicht transitionen, state=%d\n", e->state); return 1; }

    printf("PASS: test_decide_approach\n");
    return 0;
}

/* ----- Decision FUN_80101de4 (search, kein Timer, +0x1c4-Override) ----- */
static int test_decide_search(void)
{
    /* Attack (mask &3 für wander, aber hier nah → 0x701). */
    re15_actor_t *e = fresh_enemy(0x47);
    put_player(1000, 0);
    e->ai_dist = 1000u; e->ai_flags = 0; e->anim_flags = 0;
    re15_ai_decide_search(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 7) { fprintf(stderr, "FAIL: search attack→0x701, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    /* anim_flags & 0x1000 (entity+0x1c4) overridet alles → 0x1001. */
    e = fresh_enemy(0x47);
    put_player(1000, 0);
    e->ai_dist = 1000u; e->ai_flags = 0; e->anim_flags = 0x1000;
    re15_ai_decide_search(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 0x10) { fprintf(stderr, "FAIL: 0x1c4-flag→0x1001, ist %d/0x%x\n", e->state, e->sub_state_1); return 1; }

    printf("PASS: test_decide_search\n");
    return 0;
}

/* ----- Wander-Pfad (0x801): RNG&7==0 + dist>10000 + wide arc + Gate ----- *
 * RNG-Ziehung ist non-reproduzierbar-by-construction (gemeinsame Engine-RNG);
 * wie test_bleed_gate prüfen wir die STRUKTUR: Ergebnis ist 0x201 ODER 0x801,
 * und 0x801 tritt manchmal auf (nicht nie, nicht immer). */
static int test_decide_wander_path(void)
{
    re15_damage_seed_rng(0x2468ace0u);
    int wander = 0, approach = 0;
    for (int i = 0; i < 4000; i++) {
        re15_actor_t *e = fresh_enemy(0x47);
        put_player(1000, 0);                   /* wide arc (+X) */
        e->ai_dist = 50000u;                   /* >10000 → RNG-Pfad aktiv */
        e->ai_flags = 0x10;
        re15_ai_decide_search_timer(e, &g_actors[0]);
        if (e->state != 1) { fprintf(stderr, "FAIL: wander-pfad state muss 1 sein, ist %d\n", e->state); return 1; }
        if (e->sub_state_1 == 8)      wander++;
        else if (e->sub_state_1 == 2) approach++;
        else { fprintf(stderr, "FAIL: sub muss 2 oder 8 sein, ist %d\n", e->sub_state_1); return 1; }
    }
    if (wander == 0)   { fprintf(stderr, "FAIL: 0x801 (1/8) sollte manchmal auftreten, war 0/4000\n"); return 1; }
    if (approach == 0) { fprintf(stderr, "FAIL: 0x201 sollte der Normalfall sein, war 0/4000\n"); return 1; }
    /* ~1/8 erwartet; lockere Schranken. */
    if (wander < 200 || wander > 800) { fprintf(stderr, "FAIL: wander ~1/8 erwartet, war %d/4000\n", wander); return 1; }
    printf("PASS: test_decide_wander_path (%d/4000 ~ 1/8)\n", wander);
    return 0;
}

int main(void)
{
    int failures = 0;
    printf("=== Enemy-AI FSM Unit Tests (Phase 2, FUN_8011d6d4/d84c/d9f4 + 80101b64/c7c/de4) ===\n\n");

    failures += test_ai_init();
    failures += test_ai_init_stationary();
    failures += test_state_word();
    failures += test_ai_tick();
    failures += test_ai_active_dispatch();
    failures += test_decide_search_timer();
    failures += test_decide_approach();
    failures += test_decide_search();
    failures += test_decide_wander_path();

    if (failures == 0) printf("\nALL ENEMY-AI TESTS PASSED\n");
    else               fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    return failures;
}
