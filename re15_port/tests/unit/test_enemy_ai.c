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
#include "re15_emd.h"      /* re15_emd_skeleton_t (synthetic skeleton for the attack-point) */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

/* ----- FUN_8001a780: relative-facing octant ----- */
static int test_facing_aligned(void)
{
    re15_actor_t e = {0}, o = {0};
    e.rot_y = 0;
    o.rot_y = 0;                                   /* gleiche Richtung → 1 (Front-Hemisphäre) */
    if (re15_ai_facing_aligned(&e, &o) != 1) { fprintf(stderr, "FAIL: gleiche Richtung→1\n"); return 1; }
    o.rot_y = 2048;                                /* 180° → außerhalb → 0 */
    if (re15_ai_facing_aligned(&e, &o) != 0) { fprintf(stderr, "FAIL: 180°→0\n"); return 1; }
    printf("PASS: test_facing_aligned\n");
    return 0;
}

/* ----- FUN_80102058: engage-Decision (attack / grab / dead-grab / contact) ----- */
static int test_decide_engage(void)
{
    re15_actor_t *e;

    /* (a) Attack 0x701: contact 0, Spieler rechts (+X, off narrow arc), dist<2000,
     * Spieler mid-hit (hit_react=1) → Grab-Arm aus, lebt → kein dead-grab. */
    e = fresh_enemy(0x47);
    put_player(1000, 0);
    g_actors[0].hit_react = 1; g_actors[0].hp = 100;
    e->ai_contact = 0; e->ai_dist = 1000u; e->floor = 0; g_actors[0].floor = 0;
    re15_ai_decide_engage(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 7) { fprintf(stderr, "FAIL: engage attack→0x701, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    /* (b) Directional grab 0x401: Spieler VORNE (+Z: arc 0x2c8==0 → kein attack, arc 0x200==0),
     * nah (<0x4b0), nicht mid-hit, gleicher floor; Spieler blickt gleich (aligned=1) → 0x401. */
    e = fresh_enemy(0x47);                          /* e.rot_y = -1024 (faces +Z) */
    put_player(0, 1000);
    g_actors[0].hit_react = 0; g_actors[0].hp = 100; g_actors[0].rot_y = -1024;  /* aligned=1 */
    e->ai_contact = 0; e->ai_dist = 1000u; e->floor = 0; g_actors[0].floor = 0;
    re15_ai_decide_engage(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 4) { fprintf(stderr, "FAIL: grab(aligned)→0x401, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    /* (c) Face-to-face grab 0x301: Spieler blickt zum Gegner (aligned=0) → 0x301. */
    e = fresh_enemy(0x47);
    put_player(0, 1000);
    g_actors[0].hit_react = 0; g_actors[0].hp = 100; g_actors[0].rot_y = 1024;   /* faces -Z → aligned=0 */
    e->ai_contact = 0; e->ai_dist = 1000u; e->floor = 0; g_actors[0].floor = 0;
    re15_ai_decide_engage(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 3) { fprintf(stderr, "FAIL: grab(face)→0x301, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    /* (d) Grab gated by floor mismatch: anderer floor → kein Grab (bleibt attack 0x701). */
    e = fresh_enemy(0x47);
    put_player(0, 1000);
    g_actors[0].hit_react = 0; g_actors[0].hp = 100; g_actors[0].rot_y = -1024;
    e->ai_contact = 0; e->ai_dist = 1000u; e->floor = 0; g_actors[0].floor = 1;   /* mismatch */
    re15_ai_decide_engage(e, &g_actors[0]);
    if (e->sub_state_1 == 4 || e->sub_state_1 == 3) { fprintf(stderr, "FAIL: floor-mismatch darf nicht grabben, sub=%d\n", e->sub_state_1); return 1; }

    /* (e) Player-dead grab 0xc01: nah (<0x5dc), Spieler HP<0. */
    e = fresh_enemy(0x47);
    put_player(1000, 0);
    g_actors[0].hit_react = 1; g_actors[0].hp = -5;
    e->ai_contact = 0; e->ai_dist = 1000u; e->floor = 0; g_actors[0].floor = 0;
    re15_ai_decide_engage(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 0x0c) { fprintf(stderr, "FAIL: dead-grab→0xc01, ist %d/0x%x\n", e->state, e->sub_state_1); return 1; }

    /* (f) Contact reaction 0x901/0xa01: ai_contact&3 gesetzt + Kontakt von vorne (off<=0x3ff). */
    e = fresh_enemy(0x47);
    e->rot_y = 0;                                   /* dir=0, off=(0+0x200)&0xfff=0x200<=0x3ff */
    e->ai_contact = 0x02;                           /* &3!=0, &1==0 → (0+9)→0x901 */
    re15_ai_decide_engage(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 9) { fprintf(stderr, "FAIL: contact even→0x901, ist %d/%d\n", e->state, e->sub_state_1); return 1; }
    e = fresh_enemy(0x47);
    e->rot_y = 0; e->ai_contact = 0x03;             /* &1==1 → (1+9)→0xa01 */
    re15_ai_decide_engage(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 0x0a) { fprintf(stderr, "FAIL: contact odd→0xa01, ist %d/0x%x\n", e->state, e->sub_state_1); return 1; }

    printf("PASS: test_decide_engage\n");
    return 0;
}

/* ----- +0x5 Decision-Dispatch (FUN_8010168c → f840[+0x5]) ----- */
static int test_dispatch_decision(void)
{
    /* sub_state_1 = 2 → engage; Attack-Szenario → 0x701 (überschreibt +0x5 auf 7). */
    re15_actor_t *e = fresh_enemy(0x47);
    put_player(1000, 0);
    g_actors[0].hit_react = 1; g_actors[0].hp = 100;
    e->ai_contact = 0; e->ai_dist = 1000u; e->sub_state_1 = 2;
    re15_ai_dispatch_decision(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 7) { fprintf(stderr, "FAIL: dispatch[2]=engage→0x701, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    /* sub_state_1 = 0 → search+timer; Timer 0 + fern/vorne → 0x101. */
    e = fresh_enemy(0x47);
    put_player(0, 50000);
    e->ai_dist = 50000u; e->ai_flags = 0; e->ai_timer = 0; e->sub_state_1 = 0;
    re15_ai_dispatch_decision(e, &g_actors[0]);
    if (e->state != 1 || e->sub_state_1 != 1) { fprintf(stderr, "FAIL: dispatch[0]=search_timer→0x101, ist %d/%d\n", e->state, e->sub_state_1); return 1; }

    printf("PASS: test_dispatch_decision\n");
    return 0;
}

/* ----- System A: live EXE-leaf assess (FUN_8004f100, +0x5=0) ----- */
static int test_exe_assess(void)
{
    re15_actor_t *e;
    re15_enemy_ai_set_global_flag(0);

    /* (a) far (dist>=0x5dd) + Spieler vorne (im 0x4b0-Arc) + nicht getroffen → +0x5=1. */
    e = fresh_enemy(0x10);
    put_player(0, 50000);                          /* vorne (+Z): arc 0x4b0 == 0 */
    g_actors[0].hit_react = 0;
    e->ai_dist = 2000u;                            /* >= 0x5dd(1501) */
    re15_ai_exe_assess(e, &g_actors[0]);
    if (e->sub_state_1 != 1) { fprintf(stderr, "FAIL: assess far→+0x5=1, ist %d\n", e->sub_state_1); return 1; }

    /* (b) nah + Spieler HINTEN (außerhalb ±0x4b0) → +0x5=2 (turn to face). */
    e = fresh_enemy(0x10);
    put_player(0, -50000);                         /* hinten (-Z): arc 0x4b0 != 0 */
    g_actors[0].hit_react = 0;
    e->ai_dist = 1000u;                            /* < 0x5dd → kein +0x5=1 */
    re15_ai_exe_assess(e, &g_actors[0]);
    if (e->sub_state_1 != 2) { fprintf(stderr, "FAIL: assess outside-arc→+0x5=2, ist %d\n", e->sub_state_1); return 1; }

    /* (c) Spieler getroffen (hit_react!=0) → +0x5=6 (überschreibt). */
    e = fresh_enemy(0x10);
    put_player(0, 50000);
    g_actors[0].hit_react = 1;
    e->ai_dist = 2000u;                            /* würde +0x5=1 setzen, aber hit überschreibt */
    re15_ai_exe_assess(e, &g_actors[0]);
    if (e->sub_state_1 != 6) { fprintf(stderr, "FAIL: assess player-hit→+0x5=6, ist %d\n", e->sub_state_1); return 1; }

    /* (d) global flag&1 + type==0x4b → +0x5=6; type!=0x4b → NICHT. */
    e = fresh_enemy(0x4b);
    put_player(0, 50000); g_actors[0].hit_react = 0; e->ai_dist = 1000u;
    re15_enemy_ai_set_global_flag(1);
    re15_ai_exe_assess(e, &g_actors[0]);
    if (e->sub_state_1 != 6) { fprintf(stderr, "FAIL: assess flag+type0x4b→+0x5=6, ist %d\n", e->sub_state_1); return 1; }
    e = fresh_enemy(0x10);                         /* type 0x10: flag-branch tot */
    put_player(0, 50000); g_actors[0].hit_react = 0; e->ai_dist = 1000u;
    re15_ai_exe_assess(e, &g_actors[0]);
    if (e->sub_state_1 == 6) { fprintf(stderr, "FAIL: flag-branch darf bei type!=0x4b nicht feuern\n"); return 1; }
    re15_enemy_ai_set_global_flag(0);

    printf("PASS: test_exe_assess\n");
    return 0;
}

/* ----- System A: +0x5=1 search leaf (FUN_8004f3a4) ----- */
static int test_exe_search(void)
{
    re15_actor_t *e;
    re15_enemy_ai_set_global_flag(0);

    /* (a) very close (dist<0x1f4) → +0x5=3, +0x6=1. ai_target vorne (arc==0). */
    e = fresh_enemy(0x10);
    g_actors[0].hit_react = 0;
    e->ai_dist = 400u; e->ai_target_x = 0; e->ai_target_z = 5000;   /* target vorne */
    re15_ai_exe_search(e, &g_actors[0]);
    if (e->sub_state_1 != 3 || e->sub_state_2 != 1) { fprintf(stderr, "FAIL: search close→3/1, ist %d/%d\n", e->sub_state_1, e->sub_state_2); return 1; }

    /* (b) lost (dist>=0xbb9) → +0x5=5. */
    e = fresh_enemy(0x10);
    g_actors[0].hit_react = 0;
    e->ai_dist = 4000u; e->ai_target_x = 0; e->ai_target_z = 5000;
    re15_ai_exe_search(e, &g_actors[0]);
    if (e->sub_state_1 != 5) { fprintf(stderr, "FAIL: search lost→5, ist %d\n", e->sub_state_1); return 1; }

    /* (c) target außerhalb ±0x400-Arc → +0x5=3 (auch wenn dist mittel). */
    e = fresh_enemy(0x10);
    g_actors[0].hit_react = 0;
    e->ai_dist = 1000u; e->ai_target_x = 0; e->ai_target_z = -5000;  /* target HINTEN → arc!=0 */
    re15_ai_exe_search(e, &g_actors[0]);
    if (e->sub_state_1 != 3) { fprintf(stderr, "FAIL: search target-off-arc→3, ist %d\n", e->sub_state_1); return 1; }

    /* (d) player hit → +0x5=6 (überschreibt). */
    e = fresh_enemy(0x10);
    g_actors[0].hit_react = 1;
    e->ai_dist = 4000u; e->ai_target_z = 5000;
    re15_ai_exe_search(e, &g_actors[0]);
    if (e->sub_state_1 != 6) { fprintf(stderr, "FAIL: search hit→6, ist %d\n", e->sub_state_1); return 1; }

    printf("PASS: test_exe_search\n");
    return 0;
}

/* ----- System A: +0x5=2 turn-to-face leaf (FUN_8004f5e8) ----- */
static int test_exe_turn(void)
{
    re15_actor_t *e;
    re15_enemy_ai_set_global_flag(0);

    /* (a) player dead-ahead (±0x40) → +0x5=0 (zurück zu assess). */
    e = fresh_enemy(0x10);
    put_player(0, 5000);                          /* exakt vorne */
    g_actors[0].hit_react = 0; e->ai_dist = 1000u;
    re15_ai_exe_turn(e, &g_actors[0]);
    if (e->sub_state_1 != 0) { fprintf(stderr, "FAIL: turn dead-ahead→0, ist %d\n", e->sub_state_1); return 1; }

    /* (b) seitlich (arc!=0) + weit (dist>=0x7d1) → +0x5=1. */
    e = fresh_enemy(0x10);
    put_player(1000, 0);                          /* rechts → nicht dead-ahead */
    g_actors[0].hit_react = 0; e->ai_dist = 3000u;
    re15_ai_exe_turn(e, &g_actors[0]);
    if (e->sub_state_1 != 1 || e->sub_state_2 != 1) { fprintf(stderr, "FAIL: turn far→1/1, ist %d/%d\n", e->sub_state_1, e->sub_state_2); return 1; }

    /* (c) player hit → +0x5=6. */
    e = fresh_enemy(0x10);
    put_player(1000, 0);
    g_actors[0].hit_react = 1; e->ai_dist = 1000u;
    re15_ai_exe_turn(e, &g_actors[0]);
    if (e->sub_state_1 != 6) { fprintf(stderr, "FAIL: turn hit→6, ist %d\n", e->sub_state_1); return 1; }

    printf("PASS: test_exe_turn\n");
    return 0;
}

/* ----- System A: live FUN_8011da48 logic dispatch + tick→active chain ----- */
static int test_exe_dispatch_and_tick(void)
{
    re15_enemy_ai_set_global_flag(0);

    /* dispatch +0x5=2 (turn) routes to turn leaf: player dead-ahead → +0x5=0. */
    re15_actor_t *e = fresh_enemy(0x10);
    put_player(0, 5000);
    g_actors[0].hit_react = 0; e->ai_dist = 1000u; e->sub_state_1 = 2;
    re15_ai_exe_dispatch(e, &g_actors[0]);
    if (e->sub_state_1 != 0) { fprintf(stderr, "FAIL: dispatch[2]=turn→0, ist %d\n", e->sub_state_1); return 1; }

    /* +0x5>=3 (deferred movement leaf) → kein Decision-Write (unverändert). */
    e = fresh_enemy(0x10);
    e->sub_state_1 = 4; e->ai_dist = 1000u;
    re15_ai_exe_dispatch(e, &g_actors[0]);
    if (e->sub_state_1 != 4) { fprintf(stderr, "FAIL: dispatch[>=3] deferred, sub geändert auf %d\n", e->sub_state_1); return 1; }

    /* FULL LIVE CHAIN: tick → ACTIVE → exe_dispatch → assess. Gegner aktiv, sub(+0x9)=0,
     * +0x5=0; Spieler fern+vorne → assess setzt +0x5=1. */
    e = fresh_enemy(0x10);
    e->state = RE15_AI_STATE_ACTIVE; e->grid_id = 0; e->sub_state_1 = 0;
    e->x = 0; e->z = 0;
    put_player(0, 2000);                          /* dist 2000 (>=1501), vorne (arc 0x4b0==0) */
    g_actors[0].hit_react = 0;
    re15_enemy_ai_set_paused(0);
    int r = re15_enemy_ai_tick(1);
    if (r != 1)            { fprintf(stderr, "FAIL: live tick dispatch (1), war %d\n", r); return 1; }
    if (e->ai_dist != 2000){ fprintf(stderr, "FAIL: live tick dist=2000, ist %u\n", e->ai_dist); return 1; }
    if (e->sub_state_1 != 1) { fprintf(stderr, "FAIL: live tick→assess→+0x5=1, ist %d\n", e->sub_state_1); return 1; }

    printf("PASS: test_exe_dispatch_and_tick\n");
    return 0;
}

/* ----- Phase 7: full chain via re15_enemy_ai_step (spawn → FSM tick → lunge → HP fällt) ----- *
 * Komponiert alle byte-true Teile in-engine: ein gespawnter Gegner wird über re15_enemy_ai_step
 * getickt (INIT→ACTIVE, dist gecacht, Decision läuft); dann wird die Lunge ausgelöst (= das
 * deferred Anim-Keyframe-Event; atk_pt = Skeleton-Attack-Bone-Stand-in auf dem Spieler) und der
 * nächste Step feuert die Hitbox → Spieler-HP fällt. KEIN game_step-Eingriff. */
static int test_ai_step_chain(void)
{
    re15_actor_init();                         /* Spieler slot 0: hp 100, Hitbox 450/1530 @origin */
    re15_actor_t *pl = &g_actors[0];
    pl->x = 0; pl->z = 0; pl->hit_react = 0;
    re15_enemy_ai_set_paused(0);
    re15_enemy_ai_set_global_flag(0);

    int s = re15_actor_alloc(0x10);            /* Gegner spawnen */
    if (s < 1) { fprintf(stderr, "FAIL: actor_alloc Gegner\n"); return 1; }
    re15_actor_t *e = &g_actors[s];
    e->x = 0; e->z = 1200; e->rot_y = 1024;    /* 1200 vor dem Spieler, blickt -Z (zum Spieler) */
    e->state = RE15_AI_STATE_INIT;

    /* Frame 1: INIT läuft (state 0→1), dist gecacht. */
    re15_enemy_ai_step(s);
    if (e->state != RE15_AI_STATE_ACTIVE) { fprintf(stderr, "FAIL: chain INIT→ACTIVE, ist %d\n", e->state); return 1; }
    if (e->ai_dist != 1200) { fprintf(stderr, "FAIL: chain dist=1200, ist %u\n", e->ai_dist); return 1; }

    /* Frame 2: ACTIVE-Decision läuft (assess; bleibt aktiv). */
    re15_enemy_ai_step(s);
    if (e->state != RE15_AI_STATE_ACTIVE) { fprintf(stderr, "FAIL: chain bleibt ACTIVE, ist %d\n", e->state); return 1; }

    /* Lunge auslösen (deferred Anim-Keyframe-Event). Attack-Point jetzt über die REALE
     * faithful-line Skeleton-Abbildung (Phase 8.1, re15_enemy_update_attack_point) statt eines
     * manuellen Stand-ins: ein synthetisches 2-Bone-Skelett, dessen Attack-Bone lokal +X 1200
     * (= Modell-Vorne) reicht. Bei rot_y=1024 zeigt Modell-+X nach Welt-−Z, der Gegner steht bei
     * z=1200 → die Attack-Bone-Weltpos fällt auf den Spieler am Ursprung. Posed als QUERY. */
    re15_emd_skeleton_t skel; memset(&skel, 0, sizeof skel);
    skel.bone_count = 2;
    skel.bone_parent[0] = -1; skel.bone_parent[1] = 0;
    skel.bone_relative_pos[1][0] = 1200;          /* Modell-+X-Reichweite (Vorne) */
    skel.keyframe_size_bytes = 80;
    re15_enemy_update_attack_point(s, &skel, 0, 1);    /* attack_bone = 1 → atk_pt ≈ Spieler */
    if (e->atk_pt_x * e->atk_pt_x + e->atk_pt_z * e->atk_pt_z > 500 * 500) {
        fprintf(stderr, "FAIL: chain atk_pt (%d,%d) sollte am Spieler liegen (<500)\n",
                e->atk_pt_x, e->atk_pt_z); return 1; }
    re15_enemy_lunge_begin(s);
    int16_t hp_before = pl->hp;

    /* Frame 3: ai_step tickt die Lunge → Hitbox feuert → Spieler -10. */
    re15_enemy_ai_step(s);
    if (pl->hp != (int16_t)(hp_before - 10)) { fprintf(stderr, "FAIL: chain Lunge-Biss -10, HP %d→%d\n", hp_before, pl->hp); return 1; }
    if (pl->state != 2) { fprintf(stderr, "FAIL: chain Spieler hurt(2), ist %d\n", pl->state); return 1; }

    printf("PASS: test_ai_step_chain (spawn→tick→lunge→HP %d→%d)\n", hp_before, pl->hp);
    return 0;
}

/* ----- System (C): LIVE @0x8011f7b4 family — INIT (FUN_80100688) + active lunge-timer (FUN_80101224) ----- *
 * Re-rooted live zombie AI (the savestate-proven correction). INIT: state->ACTIVE, ai_timer=0x14.
 * Active: while attack-armed (ai_flags & 0x100) the windup timer ai_attack_timer counts down,
 * fires the (shared) lunge at == 0x12c (300), and recovers to state 3 / +0x5=0x15 at == 0. */
static int test_live_active_lunge(void)
{
    /* (a) INIT (FUN_80100688): state -> ACTIVE, ai_timer = 0x14. */
    re15_actor_t *e = fresh_enemy(0x10);
    e->state = RE15_AI_STATE_INIT;
    re15_enemy_ai_live_init(1);
    if (e->state != RE15_AI_STATE_ACTIVE) { fprintf(stderr, "FAIL: live init state->1, ist %d\n", e->state); return 1; }
    if (e->ai_timer != 0x14)              { fprintf(stderr, "FAIL: live init ai_timer=0x14, ist 0x%x\n", e->ai_timer); return 1; }

    /* (b) NOT armed, combat INACTIVE: the unarmed path runs the LIVE decision brain (sub 0 ->
     * @0x8011f80c[0]=FUN_8010168c -> @0x8011f840[+0x5] = re15_ai_dispatch_decision). Player in range
     * + off the front arc -> brain commits attack (0x701 -> +0x5=7), but with combat off the arm
     * stays inert (no +0x1d8|0x100, no windup seed). */
    re15_enemy_ai_set_combat_active(0);
    e = fresh_enemy(0x10);                 /* grid_id 0 (sub 0), +0x5=0 (search-timer brain entry) */
    put_player(1000, 0);                   /* right (+X): off narrow arc, dist 1000 < 2000 */
    e->ai_flags = 0; e->ai_dist = 1000; e->ai_attack_timer = 500;
    if (re15_enemy_ai_live_active(1) != 0) { fprintf(stderr, "FAIL: unarmed active muss 0 liefern\n"); return 1; }
    if (e->sub_state_1 != 7)              { fprintf(stderr, "FAIL: unarmed sub0 -> Brain committet Attack (+0x5=7), ist %d\n", e->sub_state_1); return 1; }
    if (e->ai_flags & 0x100)             { fprintf(stderr, "FAIL: combat-off darf nicht armen\n"); return 1; }
    if (e->ai_attack_timer != 500)       { fprintf(stderr, "FAIL: combat-off darf windup-timer nicht seeden, ist %d\n", e->ai_attack_timer); return 1; }

    /* (c) Armed: timer counts down; at == 0x12c (300) it fires the lunge (re15_enemy_lunge_begin
     * -> lunge_frames = 0x20) and returns 1. */
    e = fresh_enemy(0x10);
    e->ai_flags = 0x100; e->ai_attack_timer = 0x12d; e->lunge_frames = 0;
    int r = re15_enemy_ai_live_active(1);
    if (r != 1)                       { fprintf(stderr, "FAIL: armed @300 muss Lunge feuern (1), war %d\n", r); return 1; }
    if (e->ai_attack_timer != 0x12c)  { fprintf(stderr, "FAIL: timer 0x12d->0x12c, ist 0x%x\n", e->ai_attack_timer); return 1; }
    if (e->lunge_frames != 0x20)      { fprintf(stderr, "FAIL: lunge_begin muss lunge_frames=0x20 setzen, ist %d\n", e->lunge_frames); return 1; }

    /* A non-300 frame counts down without firing. */
    e = fresh_enemy(0x10);
    e->ai_flags = 0x100; e->ai_attack_timer = 0x200; e->lunge_frames = 0;
    if (re15_enemy_ai_live_active(1) != 0) { fprintf(stderr, "FAIL: timer!=300 darf nicht feuern\n"); return 1; }
    if (e->lunge_frames != 0)              { fprintf(stderr, "FAIL: kein Lunge ausserhalb 300\n"); return 1; }

    /* (d) At == 0: post-attack recovery (state 3 / +0x5=0x15 / motion 0xb; 0x1f if grid&0x80). */
    e = fresh_enemy(0x10);
    e->ai_flags = 0x100; e->ai_attack_timer = 1; e->grid_id = 0;
    re15_enemy_ai_live_active(1);
    if (e->state != 3 || e->sub_state_1 != 0x15) { fprintf(stderr, "FAIL: timer0->recovery 0x1503, ist state %d sub %d\n", e->state, e->sub_state_1); return 1; }
    if (e->motion != 0x0b)                       { fprintf(stderr, "FAIL: recovery motion 0xb, ist %d\n", e->motion); return 1; }
    e = fresh_enemy(0x10);
    e->ai_flags = 0x100; e->ai_attack_timer = 1; e->grid_id = 0x80;
    re15_enemy_ai_live_active(1);
    if (e->motion != 0x1f) { fprintf(stderr, "FAIL: grid&0x80 recovery motion 0x1f, ist %d\n", e->motion); return 1; }

    printf("PASS: test_live_active_lunge\n");
    return 0;
}

/* ----- LIVE decision -> ARM (FUN_8010ab2c): the brain commits +0x5=7 with combat ACTIVE -> the
 * lunge windup is armed (ai_flags|0x100, ai_attack_timer seeded rand8()+rand8()+600 in [600,1110],
 * live attack arc ai_arc=0x390). With combat inactive the commit happens but the arm stays inert. */
static int test_live_decision_arm(void)
{
    re15_enemy_ai_set_combat_active(1);
    re15_damage_seed_rng(0x13572468u);
    re15_actor_t *e = fresh_enemy(0x10);   /* sub 0, +0x5=0 (search-timer brain entry) */
    put_player(1000, 0);                    /* in range + off arc -> brain commits */
    e->ai_flags = 0; e->ai_dist = 1000; e->ai_attack_timer = 0;

    re15_enemy_ai_live_active(1);           /* unarmed -> brain commits +0x5=7 -> arm */

    if (e->sub_state_1 != 7)    { fprintf(stderr, "FAIL: arm: Brain muss committen (+0x5=7), ist %d\n", e->sub_state_1); return 1; }
    if (!(e->ai_flags & 0x100)) { fprintf(stderr, "FAIL: arm: combat-on muss +0x1d8|0x100 setzen\n"); return 1; }
    if (e->ai_arc != 0x390)     { fprintf(stderr, "FAIL: arm: live attack arc 0x390, ist 0x%x\n", e->ai_arc); return 1; }
    if (e->ai_attack_timer < 600 || e->ai_attack_timer > 1110) {
        fprintf(stderr, "FAIL: arm: +0x1da seed in [600,1110], ist %d\n", e->ai_attack_timer); return 1; }

    /* Combat OFF: same commit, arm inert. */
    re15_enemy_ai_set_combat_active(0);
    e = fresh_enemy(0x10);
    put_player(1000, 0);
    e->ai_flags = 0; e->ai_dist = 1000; e->ai_attack_timer = 0;
    re15_enemy_ai_live_active(1);
    if (e->ai_flags & 0x100) { fprintf(stderr, "FAIL: arm: combat-off darf nicht armen\n"); return 1; }

    printf("PASS: test_live_decision_arm\n");
    return 0;
}

/* ----- LIVE full chain via re15_enemy_ai_live_step (FUN_80100424 tick + shared lunge) ----- *
 * The correct-family analog of test_ai_step_chain: a live 0x10 zombie is ticked through the
 * @0x8011f7b4 dispatcher (INIT->ACTIVE, dist cached); then armed; the windup timer fires the
 * lunge at == 300; the shared lunge slice fires the hitbox -> player HP falls. KEIN game_step. */
static int test_live_step_chain(void)
{
    re15_actor_init();
    re15_actor_t *pl = &g_actors[0];
    pl->x = 0; pl->z = 0; pl->hit_react = 0;
    re15_enemy_ai_set_paused(0);

    int s = re15_actor_alloc(0x10);
    if (s < 1) { fprintf(stderr, "FAIL: actor_alloc live zombie\n"); return 1; }
    re15_actor_t *e = &g_actors[s];
    e->x = 0; e->z = 1200; e->rot_y = 1024; e->state = RE15_AI_STATE_INIT;

    /* Frame 1: live tick runs the live INIT (state 0->1), dist cached @+0x1d0, ai_timer=0x14. */
    re15_enemy_ai_live_step(s);
    if (e->state != RE15_AI_STATE_ACTIVE) { fprintf(stderr, "FAIL: live chain INIT->ACTIVE, ist %d\n", e->state); return 1; }
    if (e->ai_dist != 1200)               { fprintf(stderr, "FAIL: live chain dist=1200, ist %u\n", e->ai_dist); return 1; }
    if (e->ai_timer != 0x14)              { fprintf(stderr, "FAIL: live chain ai_timer=0x14, ist 0x%x\n", e->ai_timer); return 1; }

    /* Arm the attack + seed the windup so the next active frame hits 300 = fires the lunge.
     * atk_pt set directly to the player (the skeleton mapping is covered by test_attack_point_mapping). */
    e->ai_flags |= 0x100;
    e->ai_attack_timer = 0x12d;          /* -> 0x12c (300) next frame */
    e->atk_pt_x = 0; e->atk_pt_y = 0; e->atk_pt_z = 0;
    int16_t hp_before = pl->hp;

    /* Frame 2: active counts 0x12d->0x12c (300) -> lunge_begin (window 0x20); then the lunge slice
     * fires the shared hitbox this same frame -> player -10 + hurt(2), bite-once resets the window. */
    re15_enemy_ai_live_step(s);
    if (pl->hp != (int16_t)(hp_before - 10)) { fprintf(stderr, "FAIL: live chain lunge bite -10, HP %d->%d\n", hp_before, pl->hp); return 1; }
    if (pl->state != 2)                      { fprintf(stderr, "FAIL: live chain player hurt(2), ist %d\n", pl->state); return 1; }

    printf("PASS: test_live_step_chain (live spawn->tick->arm->lunge@300->HP %d->%d)\n", hp_before, pl->hp);
    return 0;
}

int main(void)
{
    int failures = 0;
    printf("=== Enemy-AI FSM Unit Tests (Phase 2-7, dispatch + brain + live leaves + lunge chain) ===\n\n");

    failures += test_ai_init();
    failures += test_ai_init_stationary();
    failures += test_state_word();
    failures += test_ai_tick();
    failures += test_ai_active_dispatch();
    failures += test_decide_search_timer();
    failures += test_decide_approach();
    failures += test_decide_search();
    failures += test_decide_wander_path();
    failures += test_facing_aligned();
    failures += test_decide_engage();
    failures += test_dispatch_decision();
    failures += test_exe_assess();
    failures += test_exe_search();
    failures += test_exe_turn();
    failures += test_exe_dispatch_and_tick();
    failures += test_live_active_lunge();
    failures += test_live_decision_arm();
    failures += test_live_step_chain();
    failures += test_ai_step_chain();

    if (failures == 0) printf("\nALL ENEMY-AI TESTS PASSED\n");
    else               fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    return failures;
}
