/**
 * @file test_damage.c
 * @brief Unit-Tests fuer den byte-true Player-Damage-Resolver (#13).
 *
 * Testet die portierte PLAYER-Branch von FUN_80012d60 (re15_damage.c) isoliert:
 *   - Zombie-Biss (attack_type 0) zieht byte-true 10 HP ab (DAT_8006f418[0]).
 *   - Hit-Once-Sperre (+0x93 bit0x1): genau 1 Treffer pro Attacke, bis re-armed.
 *   - Tod: signed HP < 0 -> state 3 + sub-states geleert (@80012ee8-efc).
 *   - Instakill-Klasse (type 2 = 1000 dmg) -> sofort state 3.
 *   - Bleed/Poison-Gate: type<2 setzt +0x98 bit0x2 (~1/4), type>=2 NIE.
 *   - Damage-Tabelle byte-true gegen ghidra1_V2.txt:223455-223478.
 *
 * Kein In-Game-Trigger noetig (Gegner-AI/Hitbox = deferred); der Resolver wird
 * direkt mit (attack_type, hitbox-origin) aufgerufen — wie FUN_80017fa4 ihn ruft.
 */
#include "re15_damage.h"
#include "re15_actor.h"     /* g_actors, re15_actor_init, RE15_ACTOR_SLOT_PLAYER */
#include "re15_skeleton.h"  /* re15_skel_bone_to_world, g_anim_pose_actor (Phase 8.1) */
#include "re15_emd.h"       /* re15_emd_skeleton_t (synthetic skeleton) */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Frischer Spieler: Slot 0 aktiv, hp=100, alle Flags 0 (re15_actor_init memset). */
static re15_actor_t *fresh_player(void)
{
    re15_actor_init();
    return &g_actors[RE15_ACTOR_SLOT_PLAYER];
}

/* ----- Test 1: Zombie-Biss = 10 HP, hurt-state, Treffer-Rueckgabe ----- */
static int test_bite_10_damage(void)
{
    re15_actor_t *p = fresh_player();
    int landed = re15_player_take_damage(p, 0, 0, 0);   /* attack_type 0 = bite */

    if (landed != 1)        { fprintf(stderr, "FAIL: bite sollte landen (1), war %d\n", landed); return 1; }
    if (p->hp != 90)        { fprintf(stderr, "FAIL: HP sollte 100-10=90 sein, ist %d\n", p->hp); return 1; }
    if (p->state != 2)      { fprintf(stderr, "FAIL: state sollte 2 (hurt) sein, ist %d\n", p->state); return 1; }
    if ((p->hit_react & 1) == 0) { fprintf(stderr, "FAIL: hit-once guard (+0x93 bit0) muss gesetzt sein\n"); return 1; }
    printf("PASS: test_bite_10_damage\n");
    return 0;
}

/* ----- Test 2: Hit-Once-Sperre — zweiter Treffer wird gedroppt, bis re-armed ----- */
static int test_hit_once_guard(void)
{
    re15_actor_t *p = fresh_player();
    re15_player_take_damage(p, 0, 0, 0);                 /* 100 -> 90, guard set */

    int landed2 = re15_player_take_damage(p, 0, 0, 0);   /* guard set -> no-op */
    if (landed2 != 0)  { fprintf(stderr, "FAIL: zweiter Treffer muss 0 liefern (Sperre), war %d\n", landed2); return 1; }
    if (p->hp != 90)   { fprintf(stderr, "FAIL: HP muss 90 bleiben (kein Doppelschaden), ist %d\n", p->hp); return 1; }

    re15_player_clear_hit_guard(p);                      /* re-arm (Attacke endet) */
    int landed3 = re15_player_take_damage(p, 0, 0, 0);   /* 90 -> 80 */
    if (landed3 != 1)  { fprintf(stderr, "FAIL: nach clear muss Treffer wieder landen, war %d\n", landed3); return 1; }
    if (p->hp != 80)   { fprintf(stderr, "FAIL: HP sollte 80 sein nach re-arm, ist %d\n", p->hp); return 1; }
    printf("PASS: test_hit_once_guard\n");
    return 0;
}

/* ----- Test 3: Tod bei signed HP < 0 -> state 3, sub-states geleert ----- */
static int test_death_on_negative_hp(void)
{
    re15_actor_t *p = fresh_player();
    p->hp = 5;                                           /* 5 - 10 = -5 < 0 */
    p->sub_state_1 = 9; p->sub_state_2 = 9;              /* Vorbelegung -> muss 0 werden */
    re15_player_take_damage(p, 0, 0, 0);

    if (p->hp != -5)         { fprintf(stderr, "FAIL: HP sollte -5 sein, ist %d\n", p->hp); return 1; }
    if (p->state != 3)       { fprintf(stderr, "FAIL: state sollte 3 (death) sein, ist %d\n", p->state); return 1; }
    if (p->sub_state_1 != 0) { fprintf(stderr, "FAIL: sub_state_1 muss bei death 0 sein, ist %d\n", p->sub_state_1); return 1; }
    if (p->sub_state_2 != 0) { fprintf(stderr, "FAIL: sub_state_2 muss bei death 0 sein, ist %d\n", p->sub_state_2); return 1; }
    printf("PASS: test_death_on_negative_hp\n");
    return 0;
}

/* ----- Test 4: Instakill-Klasse (type 2 = 1000) -> sofort Tod ----- */
static int test_instakill_type2(void)
{
    re15_actor_t *p = fresh_player();
    re15_player_take_damage(p, 2, 0, 0);                 /* 100 - 1000 = -900 */
    if (p->hp != -900) { fprintf(stderr, "FAIL: HP sollte -900 sein, ist %d\n", p->hp); return 1; }
    if (p->state != 3) { fprintf(stderr, "FAIL: state sollte 3 (death) sein, ist %d\n", p->state); return 1; }
    printf("PASS: test_instakill_type2\n");
    return 0;
}

/* ----- Test 5: Bleed/Poison-Gate — nur type<2 setzt +0x98 bit0x2 ----- */
static int test_bleed_gate(void)
{
    re15_damage_seed_rng(0x13571357u);                   /* deterministisch */

    /* type 5 (>=2): NIE Bleed, auch ueber viele Versuche. */
    for (int i = 0; i < 2000; i++) {
        re15_actor_t *p = fresh_player();
        re15_player_take_damage(p, 5, 0, 0);             /* 50 dmg, kein Bleed-Pfad */
        if (p->status_flags & 0x2) {
            fprintf(stderr, "FAIL: type>=2 darf NIE bluten (iter %d)\n", i); return 1;
        }
        if (p->state != 2) { fprintf(stderr, "FAIL: type5 von 100 -> hurt(2), ist %d\n", p->state); return 1; }
    }

    /* type 0 (<2): blutet manchmal (~1/4). */
    int bleeds = 0;
    for (int i = 0; i < 2000; i++) {
        re15_actor_t *p = fresh_player();
        re15_player_take_damage(p, 0, 0, 0);
        if (p->status_flags & 0x2) bleeds++;
    }
    if (bleeds == 0)    { fprintf(stderr, "FAIL: type<2 sollte manchmal bluten, war 0\n"); return 1; }
    if (bleeds >= 2000) { fprintf(stderr, "FAIL: type<2 blutet immer? (%d/2000)\n", bleeds); return 1; }
    /* ~1/4 erwartet; lockere Schranken gegen Brittleness. */
    if (bleeds < 200 || bleeds > 800) {
        fprintf(stderr, "FAIL: Bleed-Rate ~1/4 erwartet, war %d/2000\n", bleeds); return 1;
    }
    printf("PASS: test_bleed_gate (%d/2000 ~ 1/4)\n", bleeds);
    return 0;
}

/* ----- Test 6: Damage-Tabelle byte-true (DAT_8006f418) ----- */
static int test_damage_table_bytes(void)
{
    static const int16_t expect[11] = { 10,20,1000,1000,1000,50,100,200,300,1000,0 };
    for (int i = 0; i < 11; i++) {
        if (re15_damage_table[i] != expect[i]) {
            fprintf(stderr, "FAIL: dmg_table[%d]=%d, erwartet %d\n", i, re15_damage_table[i], expect[i]);
            return 1;
        }
    }
    static const uint8_t rexpect[11] = { 0x03,0x03,0x09,0x0A,0x0B,0x0E,0x0F,0x10,0x11,0x12,0x14 };
    for (int i = 0; i < 11; i++) {
        if (re15_react_table[i] != rexpect[i]) {
            fprintf(stderr, "FAIL: react_table[%d]=0x%02X, erwartet 0x%02X\n", i, re15_react_table[i], rexpect[i]);
            return 1;
        }
    }
    printf("PASS: test_damage_table_bytes\n");
    return 0;
}

/* ----- Enemy branch (re15_enemy_take_damage): hp/state/reaction-clip + hit-once ----- */
static int test_enemy_take_damage(void)
{
    re15_actor_init();
    re15_actor_t *e = &g_actors[1];          /* an enemy slot */
    e->active = 1; e->hp = 100; e->hit_react = 0; e->state = 0;

    /* attack_type 6 = dmg 100, react clip 0x0F: 100-100=0 (>=0 → hurt, not death). */
    int landed = re15_enemy_take_damage(e, 6);
    if (landed != 1)            { fprintf(stderr, "FAIL: enemy hit sollte landen, war %d\n", landed); return 1; }
    if (e->hp != 0)             { fprintf(stderr, "FAIL: enemy hp 100-100=0, ist %d\n", e->hp); return 1; }
    if (e->state != 2)          { fprintf(stderr, "FAIL: enemy state hurt(2), ist %d\n", e->state); return 1; }
    if (e->sub_state_1 != 0x0F) { fprintf(stderr, "FAIL: react_table[6]=0x0F, ist 0x%02X\n", e->sub_state_1); return 1; }
    if (e->sub_state_2 != 1)    { fprintf(stderr, "FAIL: enemy sub_state_2=1, ist %d\n", e->sub_state_2); return 1; }
    if ((e->hit_react & 1) == 0){ fprintf(stderr, "FAIL: enemy hit-once guard +0x93 bit0\n"); return 1; }

    /* hit-once: second hit dropped (returns 0, sets bit0x2, no re-damage). */
    int landed2 = re15_enemy_take_damage(e, 6);
    if (landed2 != 0)            { fprintf(stderr, "FAIL: 2. enemy hit muss 0 (Sperre), war %d\n", landed2); return 1; }
    if (e->hp != 0)              { fprintf(stderr, "FAIL: enemy hp muss 0 bleiben, ist %d\n", e->hp); return 1; }
    if ((e->hit_react & 0x2) == 0){ fprintf(stderr, "FAIL: re-hit muss bit0x2 setzen\n"); return 1; }

    /* lethal → death: fresh enemy hp 50, type 7 (dmg 200) → -150 < 0 → state 3. */
    re15_actor_t *e2 = &g_actors[2];
    e2->active = 1; e2->hp = 50; e2->hit_react = 0; e2->state = 0;
    re15_enemy_take_damage(e2, 7);
    if (e2->hp != -150)         { fprintf(stderr, "FAIL: enemy hp 50-200=-150, ist %d\n", e2->hp); return 1; }
    if (e2->state != 3)         { fprintf(stderr, "FAIL: enemy death state(3), ist %d\n", e2->state); return 1; }

    printf("PASS: test_enemy_take_damage\n");
    return 0;
}

/* ----- Hitbox-Geometrie (re15_hitbox_overlap = FUN_8002b5d0 circular) ----- *
 * Zentrum (1000,0,2000), Hitbox-Radius 100, Höhe 200; Attack-Radius 50 → R=150,
 * h = 50+200 = 250. Belegt: direkter Treffer, AABB-Reject, strikte Kreis-Kante
 * (dist<R), inklusive AABB-Kante (|d|<=R), Y-Range strikt (-h<dy<h). */
static int test_hitbox_overlap_circular(void)
{
    const int32_t cx = 1000, cy = 0, cz = 2000, rad = 100, hgt = 200, ar = 50; /* R=150 */
    int r;

    /* A: direkter Treffer (Attack im Zentrum) → 1 */
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1000,0,2000, ar);
    if (r != 1) { fprintf(stderr, "FAIL: hitbox A direkter Treffer, war %d\n", r); return 1; }

    /* B: innerhalb Radius (dist=100 < 150) → 1 */
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1100,0,2000, ar);
    if (r != 1) { fprintf(stderr, "FAIL: hitbox B innerhalb, war %d\n", r); return 1; }

    /* C: in AABB aber außerhalb Kreis (dx=dz=140, dist≈197 > 150) → 0 */
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1140,0,2140, ar);
    if (r != 0) { fprintf(stderr, "FAIL: hitbox C AABB-Ecke außerhalb Kreis, war %d\n", r); return 1; }

    /* D: AABB-Reject auf X (dx=200 > R=150) → 0 */
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1200,0,2000, ar);
    if (r != 0) { fprintf(stderr, "FAIL: hitbox D AABB-X-reject, war %d\n", r); return 1; }

    /* E: Kreis-Kante strikt — dx=149 (dist 149 < 150) → 1; dx=150 (dist 150) → 0 */
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1149,0,2000, ar);
    if (r != 1) { fprintf(stderr, "FAIL: hitbox E dist=149<150, war %d\n", r); return 1; }
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1150,0,2000, ar);
    if (r != 0) { fprintf(stderr, "FAIL: hitbox E dist=150 (==R) muss strikt 0, war %d\n", r); return 1; }

    /* F: Y-Range strikt — dy=249 (<250) → 1; dy=250 (==h) → 0 */
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1000,249,2000, ar);
    if (r != 1) { fprintf(stderr, "FAIL: hitbox F dy=249<250, war %d\n", r); return 1; }
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1000,250,2000, ar);
    if (r != 0) { fprintf(stderr, "FAIL: hitbox F dy=250 (==h) muss strikt 0, war %d\n", r); return 1; }
    r = re15_hitbox_overlap(cx,cy,cz, rad,hgt, 1000,-250,2000, ar);
    if (r != 0) { fprintf(stderr, "FAIL: hitbox F dy=-250 (==-h) muss strikt 0, war %d\n", r); return 1; }

    printf("PASS: test_hitbox_overlap_circular\n");
    return 0;
}

/* Hitbox-Helfer: einem Slot eine zirkuläre Hitbox + Position geben. */
static void set_hitbox(re15_actor_t *a, int32_t x, int32_t z, uint16_t rad)
{
    a->x = x; a->y = 0; a->z = z;
    a->hit_radius_min = a->hit_radius_max = rad;
    a->hit_height = 200;
    a->hit_offset_x = a->hit_offset_y = a->hit_offset_z = 0;
}

/* ----- resolve_attack (FUN_80012d60-Loop): Iteration + Gates + take_damage ----- *
 * Szenario: Attack-Box @(1000,0,0) r=100. Player @(1000) getroffen; Angreifer
 * (slot1) @(1000) IN Range aber self-excluded; Opfer (slot2) @(1050) getroffen;
 * Ferner Gegner (slot3) @(50000) außerhalb. Dann Hit-Once über 2. Resolve. */
static int test_resolve_attack(void)
{
    re15_actor_init();
    re15_actor_t *pl  = &g_actors[0];          /* Player, hp 100 (init) */
    set_hitbox(pl, 1000, 0, 100);

    re15_actor_t *atk = &g_actors[1];          /* Angreifer (self-excluded) */
    atk->active = 1; atk->type = 0x10; atk->hp = 100; atk->state = 0; atk->hit_react = 0;
    set_hitbox(atk, 1000, 0, 100);

    re15_actor_t *vic = &g_actors[2];          /* Opfer in Range */
    vic->active = 1; vic->type = 0x10; vic->hp = 100; vic->state = 0; vic->hit_react = 0;
    set_hitbox(vic, 1050, 0, 100);

    re15_actor_t *far = &g_actors[3];          /* außerhalb */
    far->active = 1; far->type = 0x10; far->hp = 100; far->state = 0; far->hit_react = 0;
    set_hitbox(far, 50000, 0, 100);

    re15_attack_box_t box = { 1000, 0, 0, 100 };   /* type 0 = 10 dmg, attacker slot 1 */
    int n = re15_resolve_attack(&box, 0, 1);

    /* Engagiert: Player (overlap) + Opfer = 2. Angreifer self-excluded, Ferner außerhalb. */
    if (n != 2)            { fprintf(stderr, "FAIL: resolve n sollte 2 sein, war %d\n", n); return 1; }
    if (pl->hp != 90)      { fprintf(stderr, "FAIL: Player hp 100-10=90, ist %d\n", pl->hp); return 1; }
    if (pl->state != 2)    { fprintf(stderr, "FAIL: Player state hurt(2), ist %d\n", pl->state); return 1; }
    if (atk->hp != 100)    { fprintf(stderr, "FAIL: Angreifer (self-excluded) muss 100 bleiben, ist %d\n", atk->hp); return 1; }
    if (atk->state != 0)   { fprintf(stderr, "FAIL: Angreifer state muss 0 bleiben, ist %d\n", atk->state); return 1; }
    if (vic->hp != 90)     { fprintf(stderr, "FAIL: Opfer hp 100-10=90, ist %d\n", vic->hp); return 1; }
    if (vic->state != 2)   { fprintf(stderr, "FAIL: Opfer state hurt(2), ist %d\n", vic->state); return 1; }
    if ((vic->hit_react & 1) == 0) { fprintf(stderr, "FAIL: Opfer hit-once guard +0x93 bit0\n"); return 1; }
    if (far->hp != 100)    { fprintf(stderr, "FAIL: Ferner Gegner muss 100 bleiben, ist %d\n", far->hp); return 1; }

    /* 2. Resolve im selben Attack-Window: Guards gesetzt → kein Re-Schaden. */
    int n2 = re15_resolve_attack(&box, 0, 1);
    if (n2 != 2)           { fprintf(stderr, "FAIL: 2. resolve n auch 2 (Engagement), war %d\n", n2); return 1; }
    if (pl->hp != 90)      { fprintf(stderr, "FAIL: Player hp muss 90 bleiben (Sperre), ist %d\n", pl->hp); return 1; }
    if (vic->hp != 90)     { fprintf(stderr, "FAIL: Opfer hp muss 90 bleiben (Sperre), ist %d\n", vic->hp); return 1; }
    if ((vic->hit_react & 0x2) == 0) { fprintf(stderr, "FAIL: Opfer Re-Hit muss bit0x2 setzen\n"); return 1; }

    printf("PASS: test_resolve_attack\n");
    return 0;
}

/* ----- re15_enemy_attack (FUN_80017fa4-Trigger): Lunge feuert resolve ----- *
 * Angreifer (slot1) mit Forward-Attack-Point; Player am Punkt mit Hitbox.
 * re15_enemy_attack(1) → Box (attack-point, r500, type0) → Player -10 HP.
 * Out-of-range Attack-Point → kein Schaden. */
static int test_enemy_attack(void)
{
    re15_actor_init();
    re15_actor_t *pl  = &g_actors[0];
    set_hitbox(pl, 1000, 0, 100);                       /* Player am Treffpunkt */
    re15_actor_t *atk = &g_actors[1];
    atk->active = 1; atk->type = 0x10; atk->hp = 100;
    atk->atk_pt_x = 1000; atk->atk_pt_y = 0; atk->atk_pt_z = 0;   /* Forward-Point auf Player */

    int n = re15_enemy_attack(1);                       /* Lunge */
    if (n < 1)          { fprintf(stderr, "FAIL: enemy_attack treffen (>=1), war %d\n", n); return 1; }
    if (pl->hp != 90)   { fprintf(stderr, "FAIL: enemy_attack Player 100-10=90, ist %d\n", pl->hp); return 1; }
    if (pl->state != 2) { fprintf(stderr, "FAIL: enemy_attack Player hurt(2), ist %d\n", pl->state); return 1; }

    re15_actor_init();                                  /* Out-of-range */
    re15_actor_t *pl2 = &g_actors[0];
    set_hitbox(pl2, 1000, 0, 100);
    re15_actor_t *atk2 = &g_actors[1];
    atk2->active = 1; atk2->type = 0x10; atk2->hp = 100;
    atk2->atk_pt_x = 30000; atk2->atk_pt_y = 0; atk2->atk_pt_z = 0;
    int n2 = re15_enemy_attack(1);
    if (n2 != 0)        { fprintf(stderr, "FAIL: out-of-range enemy_attack 0, war %d\n", n2); return 1; }
    if (pl2->hp != 100) { fprintf(stderr, "FAIL: out-of-range Player muss 100 bleiben, ist %d\n", pl2->hp); return 1; }

    printf("PASS: test_enemy_attack\n");
    return 0;
}

/* ----- Echte Hitbox-Werte (savestate/EXE-verifiziert) ----- *
 * Player @0x80073e94 = 450/1530 circular (PSX.EXE @0x64694). Zombie Typ 0x47 =
 * 450/1530, Offset (0,-1530,0). Unbekannter Typ → keine Hitbox. */
static int test_real_hitbox_values(void)
{
    re15_actor_t a = {0};
    re15_player_apply_hitbox(&a);
    if (a.hit_radius_min != 450 || a.hit_radius_max != 450 || a.hit_height != 1530) {
        fprintf(stderr, "FAIL: Player-Hitbox 450/450/1530, ist %u/%u/%u\n",
                a.hit_radius_min, a.hit_radius_max, a.hit_height); return 1; }

    re15_actor_t z = {0};
    re15_enemy_apply_hitbox(&z, 0x47);              /* STAGE1-Zombie */
    if (z.hit_radius_min != 450 || z.hit_radius_max != 450 || z.hit_height != 1530) {
        fprintf(stderr, "FAIL: Zombie(0x47) 450/450/1530, ist %u/%u/%u\n",
                z.hit_radius_min, z.hit_radius_max, z.hit_height); return 1; }
    if (z.hit_offset_y != -1530) {
        fprintf(stderr, "FAIL: Zombie offset_y -1530, ist %d\n", z.hit_offset_y); return 1; }

    re15_actor_t u = {0};
    re15_enemy_apply_hitbox(&u, 0x99);              /* unbekannt → keine Hitbox */
    if (u.hit_radius_min != 0 || u.hit_height != 0) {
        fprintf(stderr, "FAIL: unbekannter Typ muss 0 bleiben, ist %u/%u\n",
                u.hit_radius_min, u.hit_height); return 1; }

    printf("PASS: test_real_hitbox_values\n");
    return 0;
}

/* ----- Zombie-AI-Primitive (FUN_8011d6d4 Distanz + FUN_8001a9cc Arc) ----- *
 * Zombie @(0,0,0), Blickrichtung +Z (rot_y = -1024 = atan2(+Z)-1024). Distanz
 * byte-true; Arc: Spieler vorne → 0, seitlich/hinten → ±cone. */
static int test_zombie_ai_primitives(void)
{
    re15_actor_t z = {0};
    re15_actor_t p = {0};
    z.rot_y = -1024;                               /* faces +Z */

    /* Distanz: Spieler bei (1200,0,0) → dx=1200,dz=0 → 1200. */
    p.x = 1200; p.z = 0;
    int32_t d = re15_enemy_player_dist(&z, &p);
    if (d != 1200) { fprintf(stderr, "FAIL: dist 1200, war %d\n", d); return 1; }

    /* Arc cone 0x2c8: Spieler VORNE (+Z) → 0 (im Frontkegel). */
    if (re15_ai_arc_test(&z, 0, 1000, 0x2c8) != 0) {
        fprintf(stderr, "FAIL: arc Spieler vorne muss 0 sein\n"); return 1; }
    /* Spieler RECHTS (+X, 90°) → außerhalb → != 0. */
    if (re15_ai_arc_test(&z, 1000, 0, 0x2c8) == 0) {
        fprintf(stderr, "FAIL: arc Spieler rechts muss != 0 sein\n"); return 1; }
    /* Spieler HINTEN (-Z) → außerhalb → != 0. */
    if (re15_ai_arc_test(&z, 0, -1000, 0x2c8) == 0) {
        fprintf(stderr, "FAIL: arc Spieler hinten muss != 0 sein\n"); return 1; }

    printf("PASS: test_zombie_ai_primitives\n");
    return 0;
}

/* ----- re15_enemy_should_attack (3-handler-confirmed: dist<2000 && arc(0x2c8)!=0) ----- */
static int test_enemy_should_attack(void)
{
    re15_actor_t z = {0};
    re15_actor_t p = {0};
    z.rot_y = -1024;                               /* faces +Z; arc!=0 = player off-front */

    /* nah (dist 1000<2000) + arc!=0 (Spieler rechts) → commit. */
    p.x = 1000; p.z = 0;
    if (re15_enemy_should_attack(&z, &p) != 1) {
        fprintf(stderr, "FAIL: nah+arc!=0 muss attack(1) sein\n"); return 1; }

    /* nah + arc==0 (Spieler vorne) → kein attack. */
    p.x = 0; p.z = 1000;
    if (re15_enemy_should_attack(&z, &p) != 0) {
        fprintf(stderr, "FAIL: nah+arc==0 (vorne) darf nicht attacken\n"); return 1; }

    /* fern (dist 3000>=2000) + arc!=0 → kein attack (Distanz-Gate). */
    p.x = 3000; p.z = 0;
    if (re15_enemy_should_attack(&z, &p) != 0) {
        fprintf(stderr, "FAIL: fern muss Distanz-Gate ziehen (0)\n"); return 1; }

    printf("PASS: test_enemy_should_attack\n");
    return 0;
}

/* ----- Lunge-Action-Driver (LAB_80017eb0 setup + LAB_80017f50 tick) ----- *
 * 0x20-Frame-Window feuert die Hitbox jeden Frame; beißt genau 1× (Reset bei Connect);
 * out-of-range zählt das Fenster runter ohne Treffer. */
static int test_enemy_lunge(void)
{
    /* (a) In-range: erster Tick trifft (Player -10), Lunge endet (bite-once). */
    re15_actor_init();
    re15_actor_t *pl = &g_actors[0];
    set_hitbox(pl, 1000, 0, 100);                       /* Player am Treffpunkt */
    re15_actor_t *atk = &g_actors[1];
    atk->active = 1; atk->type = 0x10; atk->hp = 100;
    atk->atk_pt_x = 1000; atk->atk_pt_y = 0; atk->atk_pt_z = 0;   /* Attack-Point auf Player */

    re15_enemy_lunge_begin(1);
    if (atk->lunge_frames != 0x20) { fprintf(stderr, "FAIL: lunge_begin Fenster 0x20, ist %d\n", atk->lunge_frames); return 1; }

    int h1 = re15_enemy_lunge_tick(1);                  /* erster aktiver Frame → Connect */
    if (h1 < 1)            { fprintf(stderr, "FAIL: lunge tick1 connect (>=1), war %d\n", h1); return 1; }
    if (pl->hp != 90)      { fprintf(stderr, "FAIL: lunge Player 100-10=90, ist %d\n", pl->hp); return 1; }
    if (atk->lunge_frames != 0) { fprintf(stderr, "FAIL: lunge bei Connect beenden, frames=%d\n", atk->lunge_frames); return 1; }

    int h2 = re15_enemy_lunge_tick(1);                  /* Lunge vorbei → inaktiv */
    if (h2 != 0)           { fprintf(stderr, "FAIL: nach Connect inaktiv (0), war %d\n", h2); return 1; }

    /* (b) Out-of-range: Fenster zählt 32 Frames runter, kein Treffer. */
    re15_actor_init();
    re15_actor_t *pl2 = &g_actors[0];
    set_hitbox(pl2, 1000, 0, 100);
    re15_actor_t *atk2 = &g_actors[1];
    atk2->active = 1; atk2->type = 0x10; atk2->hp = 100;
    atk2->atk_pt_x = 40000; atk2->atk_pt_y = 0; atk2->atk_pt_z = 0;   /* weit weg */
    re15_enemy_lunge_begin(1);
    int frames = 0, anyhit = 0;
    while (re15_enemy_lunge_tick(1) >= 0 && atk2->lunge_frames > 0) {
        frames++;
        if (frames > 40) break;                         /* Sicherung */
    }
    if (pl2->hp != 100)    { fprintf(stderr, "FAIL: out-of-range darf nicht treffen, HP=%d\n", pl2->hp); return 1; }
    if (frames != 31)      { fprintf(stderr, "FAIL: Fenster 0x20 → 31 weitere Ticks bis 0, war %d\n", frames); return 1; }
    (void)anyhit;

    printf("PASS: test_enemy_lunge\n");
    return 0;
}

/* ----- Phase 8.1: faithful-line attack-point skeleton mapping ----- *
 * re15_skel_bone_to_world = the byte-true model->world transform (the SAME Ryaw math the
 * NPC render loop + the stair foot-probe apply). re15_enemy_update_attack_point poses a
 * skeleton as a QUERY (g_anim_pose_actor preserved) and writes the attack bone's world pos
 * into atk_pt_*. Synthetic 2-bone skeleton: keyframe_count 0 -> identity poses, so the
 * child bone's model-local trans == its bone_relative_pos -> fully deterministic. */
static int test_attack_point_mapping(void)
{
    /* (1) Pure transform at yaw 0 (cos=1,sin=0): world = origin + trans, no rotation. */
    int32_t trans[3] = { 0, -200, 300 };
    int32_t w[3];
    re15_skel_bone_to_world(trans, 0, 1000, 500, 2000, w);
    if (w[0] != 1000 || w[1] != 300 || w[2] != 2300) {
        fprintf(stderr, "FAIL: bone_to_world yaw0 = (1000,300,2300), war (%d,%d,%d)\n", w[0], w[1], w[2]); return 1; }

    /* (2) yaw 1024 (90°): local +Z (300) rotates to +X; +X (0) leaves Z. Tolerance for the
     * isin/sinf LSB disagreement (same APPROX stance as test_skeleton.c). */
    re15_skel_bone_to_world(trans, 1024, 1000, 500, 2000, w);
    if (w[0] < 1297 || w[0] > 1303 || w[1] != 300 || w[2] < 1997 || w[2] > 2003) {
        fprintf(stderr, "FAIL: bone_to_world yaw1024 ~ (1300,300,2000), war (%d,%d,%d)\n", w[0], w[1], w[2]); return 1; }

    /* (3) Updater: synthetic skeleton, attack bone 1 local (0,-200,300); enemy at
     * (1000,500,2000) facing +X (yaw 0) -> atk_pt = (1000,300,2300). */
    re15_actor_init();
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x10; e->x = 1000; e->y = 500; e->z = 2000; e->rot_y = 0;

    re15_emd_skeleton_t skel; memset(&skel, 0, sizeof skel);
    skel.bone_count = 2;
    skel.bone_parent[0] = -1; skel.bone_parent[1] = 0;
    skel.bone_relative_pos[1][0] = 0;
    skel.bone_relative_pos[1][1] = -200;
    skel.bone_relative_pos[1][2] = 300;
    skel.keyframe_size_bytes = 80;

    void *sentinel = (void *)0x1234;
    g_anim_pose_actor = sentinel;             /* pose QUERY must restore it (no blend mutation) */
    re15_enemy_update_attack_point(1, &skel, 0, 1);
    if (g_anim_pose_actor != sentinel) {
        fprintf(stderr, "FAIL: update_attack_point muss g_anim_pose_actor restaurieren\n"); return 1; }
    g_anim_pose_actor = NULL;
    if (e->atk_pt_x != 1000 || e->atk_pt_y != 300 || e->atk_pt_z != 2300) {
        fprintf(stderr, "FAIL: atk_pt = (1000,300,2300), war (%d,%d,%d)\n",
                e->atk_pt_x, e->atk_pt_y, e->atk_pt_z); return 1; }

    /* (4) bad bone index -> no-op (atk_pt unchanged, no OOB read). */
    e->atk_pt_x = 7; e->atk_pt_y = 7; e->atk_pt_z = 7;
    re15_enemy_update_attack_point(1, &skel, 0, 5);    /* bone 5 >= bone_count 2 */
    if (e->atk_pt_x != 7 || e->atk_pt_y != 7 || e->atk_pt_z != 7) {
        fprintf(stderr, "FAIL: bad bone index muss no-op sein, war (%d,%d,%d)\n",
                e->atk_pt_x, e->atk_pt_y, e->atk_pt_z); return 1; }

    printf("PASS: test_attack_point_mapping\n");
    return 0;
}

int main(void)
{
    int failures = 0;
    printf("=== Player-Damage Unit Tests (#13, FUN_80012d60) ===\n\n");

    failures += test_bite_10_damage();
    failures += test_hit_once_guard();
    failures += test_death_on_negative_hp();
    failures += test_instakill_type2();
    failures += test_bleed_gate();
    failures += test_damage_table_bytes();
    failures += test_enemy_take_damage();
    failures += test_hitbox_overlap_circular();
    failures += test_resolve_attack();
    failures += test_enemy_attack();
    failures += test_enemy_lunge();
    failures += test_attack_point_mapping();
    failures += test_real_hitbox_values();
    failures += test_zombie_ai_primitives();
    failures += test_enemy_should_attack();

    if (failures == 0) printf("\nALL PLAYER-DAMAGE TESTS PASSED\n");
    else               fprintf(stderr, "\n%d TEST(S) FAILED\n", failures);
    return failures;
}
