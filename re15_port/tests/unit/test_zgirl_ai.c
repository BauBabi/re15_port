/* ============================================================================
 *  Zombie Girl (type 0x13, EM013 = ZOMBIE_GIRL) AI — byte-true FSM probe.
 *
 *  REWRITTEN for the byte-true brain (audit wf_827f186d zombie-girl #1-#5):
 *  the old test PINNED the WRONG flat instant-aggro chase (nav-walk clip 0x1a
 *  + grab at any range + kill-at-any-HP), which modeled the UNREACHABLE mode-1
 *  nav walk (clip-26 fn 0x8010be50 = mode-1 animate[0] @0x80120308 only; no
 *  shipped room spawns mode 1). Byte-true: root FUN_8010a8c8, state table
 *  @0x80120208, ACTIVE mode-0 FUN_8010b6d4 = the STANDARD-ZOMBIE phase FSM
 *  (decide @0x80120264 / animate @0x801202a8 == the standard tables except
 *  row [0xa] = girl floor-drop 0x8010bbe0/bbe8).
 *   (1) INIT (FUN_8010ab2c): state->1, HP=(rng&0x1f)+50 @0x8010ac1c, walk
 *       variant +0x1d4 in {2,3,4,5} (@0x8011f7e4[rng&7] @0x8010ac58), and for
 *       behavior 0x00 NO pose write -> sub 0 idle (NOT clip 0x1a).
 *   (2) WAKE: sub-0 search decide (0x80101b64) escalates to ENGAGE 0x201 via
 *       the LOS gate; the engage animate plays the AWARE walk clip +0x1d4.
 *   (3) GRAB: the SHARED FUN_80102548 machine — impact -10, held -5 bites,
 *       and the player dies ONLY via hp<0 (never at positive HP; the devour
 *       handoff is ((+0x5)+2)<<8|1, audit #2).
 *   (4) HURT: state 2 runs the shared flinch FSM (masters @0x8012039c ==
 *       @0x8011fb90) — NOT a same-frame reset (audit #4).
 *   (5) DEATH: state 3 runs the shared death-fall (master @0x8012063c ==
 *       @0x8011feac) then the corpse SETTLE clips 0x14/0x15 (audit #3).
 *   (6) FLOOR-DROP: girl row [0xa] = FUN_8010bbe8 — floor+=1, y-=0x708.
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_enemy_take_damage, re15_damage_seed_rng */
#include "re15_ai_flavor.h" /* re15_re15_re2z_import_set — byte-true PIN schaltet die Port-Option ab */

extern int re15_player_is_grabbed(void);
extern int16_t re15_atan2_q12(int32_t, int32_t);

static void face_player(re15_actor_t *g, const re15_actor_t *pl)
{
    g->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - g->z, pl->x - g->x) - 0x400) & 0xfff);
}

int main(void)
{
    int fail = 0;
    /* Dieser Test pinnt den byte-true RE1.5-AUSLIEFERUNGSSTAND. Seit 2026-08-22 zieht die
     * Port-Option re15_re15_re2z_import() (Default AN, Nutzer-Entscheidung "der fette Zombie muss
     * nicht jeden 2. Schuss umfallen") zusaetzlich die RE2-STURZREGEL in den RE1.5-Modus: die
     * Leiste +0x1DC traegt dann 16+(rand&0xf) @0x8010089C und wird um cost[Zeile] @0x8010CC33
     * gesenkt, mit Nachladen nur bei HP >= 81 (`slti 81` @0x80105604). Fall (4) setzt die Leiste
     * hier bewusst auf 6 und die HP auf 60 — unter der RE2-Regel bricht sie damit sofort und der
     * Flinch geht in den Knockdown. Der Test schaltet die Option deshalb ab und pinnt weiter
     * GENAU das, was er immer gepinnt hat (dasselbe Verfahren wie test_room1140_combat.c).
     * Was der SPIELMODUS heute tut, misst probe_re15_poise_re2.c / pinnt
     * test_re15_poise_re2_import.c. */
    re15_re15_re2z_import_set(0);
    memset(g_actors, 0, sizeof g_actors);
    re15_damage_seed_rng(0x1234u);      /* deterministic xorshift stream */

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 1000; pl->hp = 100;

    const int GS = 1;
    re15_actor_t *g = &g_actors[GS];
    g->active = 1; g->type = 0x13; g->state = 0; g->x = 0; g->y = 0; g->z = 0; g->rot_y = 0;
    re15_enemy_apply_hitbox(g, 0x13);

    printf("=== ZOMBIE GIRL (type 0x13, EM013) byte-true FSM ===\n");

    /* (1) INIT FUN_8010ab2c */
    face_player(g, pl);
    re15_enemy_ai_run_all(0);
    if (g->state != 1)            { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", g->state); fail = 1; }
    if (g->hp < 50 || g->hp > 81) { fprintf(stderr, "FAIL(1): HP must seed 50..81 (@0x8010ac1c), got %d\n", g->hp); fail = 1; }
    if (g->hurt_clip < 2 || g->hurt_clip > 5)
                                  { fprintf(stderr, "FAIL(1): +0x1d4 walk variant must be 2..5 (@0x8010ac58), got %d\n", g->hurt_clip); fail = 1; }
    if (g->sub_state_1 != 0)      { fprintf(stderr, "FAIL(1): behavior 0x00 spawns into sub 0 (idle), got 0x%x\n", g->sub_state_1); fail = 1; }
    if (g->motion == 0x1a)        { fprintf(stderr, "FAIL(1): clip 0x1a is the UNREACHABLE mode-1 walk (audit #1) — must not spawn with it\n"); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d, variant=%d, sub=0\n", g->hp, g->hurt_clip);

    /* (2) WAKE -> ENGAGE: the LOS probe verdict lands on tick 4 -> the search decide commits 0x201;
     * the engage animate entry plays the AWARE walk clip +0x1d4 (2..5), NOT the unaware clip 1 or
     * the mode-1 clip 0x1a. (A behavior-roll of 2 passes through to sub 0x13, which byte-true runs
     * the SAME engage-animate walk via the table-overflow decide @0x801202b0 = 0x801021f8.) */
    int woke = 0;
    for (int f = 0; f < 30 && !woke; f++) {
        re15_enemy_ai_run_all(0);
        if (g->sub_state_1 == 2 || g->sub_state_1 == 0x13) woke = 1;
    }
    if (!woke) { fprintf(stderr, "FAIL(2): search must escalate to ENGAGE 0x201 (LOS gate), sub=0x%x\n", g->sub_state_1); fail = 1; }
    if (g->motion != g->hurt_clip)
        { fprintf(stderr, "FAIL(2): the engage walk plays clip +0x1d4=%d, got motion=%d\n", g->hurt_clip, g->motion); fail = 1; }
    printf("  (2) WAKE: sub=0x%x, walk clip=%d\n", g->sub_state_1, g->motion);

    /* (3) GRAB via the SHARED FUN_80102548: within grab range (dist 1000 < 0x4b0) the engage decide
     * commits 0x301/0x401; impact -10, held -5 bites; the player dies ONLY via hp<0 and the girl
     * hands off to the devour states 5/6 (audit #2: never a kill at positive HP). */
    int grabbed = 0, hurt10 = 0, died_at_positive_hp = 0, devour = 0;
    int16_t php0 = pl->hp;
    for (int f = 0; f < 600; f++) {
        re15_enemy_ai_run_all(0);
        if (re15_player_is_grabbed()) grabbed = 1;
        if (pl->hp <= php0 - 10) hurt10 = 1;
        if (pl->state == 7 && pl->hp >= 0) died_at_positive_hp = 1;
        if (g->sub_state_1 == 5 || g->sub_state_1 == 6) { devour = 1; break; }
    }
    if (!grabbed) { fprintf(stderr, "FAIL(3): the girl must grab + pin the player (sub=0x%x)\n", g->sub_state_1); fail = 1; }
    if (!hurt10)  { fprintf(stderr, "FAIL(3): the grab impact must deal -10 (hp %d->%d)\n", php0, pl->hp); fail = 1; }
    if (died_at_positive_hp)
                  { fprintf(stderr, "FAIL(3): the player must die ONLY via hp<0 (audit #2), died at hp=%d\n", pl->hp); fail = 1; }
    if (!devour)  { fprintf(stderr, "FAIL(3): the held grab must hand off to DEVOUR 5/6 ((+0x5)+2)<<8|1, sub=0x%x\n", g->sub_state_1); fail = 1; }
    if (devour && pl->hp >= 0)
                  { fprintf(stderr, "FAIL(3): at the devour handoff the bitten player hp must be <0 here (headless -5/tick), hp=%d\n", pl->hp); fail = 1; }
    printf("  (3) GRAB: pinned=%d, -10=%d, devour handoff sub=0x%x, player hp=%d\n",
           grabbed, hurt10, g->sub_state_1, pl->hp);

    /* (4) HURT: non-lethal hit -> the SHARED flinch FSM, NOT a same-frame reset (audit #4). */
    memset(g_actors, 0, sizeof g_actors);
    re15_damage_seed_rng(0x5678u);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER]; pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 6000; pl->hp = 100;
    g = &g_actors[GS]; g->active = 1; g->type = 0x13; g->state = 1; g->sub_state_1 = 2; g->hp = 60;
    g->hurt_clip = 3; g->hit_stun = 6;                 /* poise as INIT seeds (rand&3)+4 */
    re15_enemy_apply_hitbox(g, 0x13);
    re15_enemy_take_damage(g, 0);                      /* attack 0 = -10 (re15_damage_table[0]) -> non-lethal state 2 */
    if (g->state != 2) { fprintf(stderr, "FAIL(4): take_damage must set state 2, got %d\n", g->state); fail = 1; }
    re15_enemy_ai_run_all(0);
    if (g->state != 2) { fprintf(stderr, "FAIL(4): HURT must NOT reset same-frame (audit #4), state=%d\n", g->state); fail = 1; }
    int recovered = 0;
    for (int f = 0; f < 60; f++) {
        re15_enemy_ai_run_all(0);
        if (g->state == 1) { recovered = 1; break; }
    }
    if (!recovered) { fprintf(stderr, "FAIL(4): the flinch must recover to ACTIVE (0x10201 / knockdown), state=%d\n", g->state); fail = 1; }
    if (recovered && !(g->sub_state_1 == 2 || g->sub_state_1 == 0x13 || g->sub_state_1 == 0x11 ||
                       g->sub_state_1 == 8 || g->sub_state_1 == 7))
        { fprintf(stderr, "FAIL(4): recovery exits into engage/approach/knockdown/charge/turn, sub=0x%x\n", g->sub_state_1); fail = 1; }
    printf("  (4) HURT: flinch held >=1 tick, recovered to state 1 sub=0x%x\n", g->sub_state_1);

    /* (5) DEATH + CORPSE SETTLE: lethal -> state 3 death-fall clip, then state 7 with the settle
     * clips 0x14/0x15 (shared FUN_80109554) — NOT a frozen replay of the walk clip (audit #3). */
    memset(g_actors, 0, sizeof g_actors);
    re15_damage_seed_rng(0x9abcu);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER]; pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 8000; pl->hp = 100;
    g = &g_actors[GS]; g->active = 1; g->type = 0x13; g->state = 1; g->sub_state_1 = 2; g->hp = 4; g->motion = 3;
    re15_enemy_apply_hitbox(g, 0x13);
    re15_enemy_take_damage(g, 2);                      /* hp 4 - dmg < 0 -> state 3 */
    if (g->state != 3) { fprintf(stderr, "FAIL(5): lethal hit must set state 3 (death), got %d\n", g->state); fail = 1; }
    int reached_corpse = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (g->state == 7 && g->sub_state_1 >= 1) { reached_corpse = 1; break; }  /* settle entered */
    }
    if (!reached_corpse) { fprintf(stderr, "FAIL(5): death must reach the CORPSE settle, state=%d sub=%d\n", g->state, g->sub_state_1); fail = 1; }
    if (g->motion != 0x14 && g->motion != 0x15)
        { fprintf(stderr, "FAIL(5): the corpse settles into lying clip 0x14/0x15 (audit #3), motion=%d\n", g->motion); fail = 1; }
    printf("  (5) DEATH: fall -> CORPSE settle (state %d, clip=0x%x)\n", g->state, g->motion);

    /* (6) FLOOR-DROP (girl animate row [0xa] = FUN_8010bbe8): aligned contact -> clip 0x16 ->
     * floor += 1 (@0x8010bde8), y -= 0x708 (@0x8010be04). */
    memset(g_actors, 0, sizeof g_actors);
    re15_damage_seed_rng(0xdef0u);
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER]; pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 8000; pl->hp = 100;
    g = &g_actors[GS]; g->active = 1; g->type = 0x13; g->state = 1; g->sub_state_1 = 0x0a;
    g->sub_state_2 = 0; g->hp = 60; g->floor = 0; g->y = 0;
    g->ai_contact = 0x00;                              /* contact heading 0 == rot_y 0 -> aligned */
    g->rot_y = 0;
    re15_enemy_apply_hitbox(g, 0x13);
    int dropped = 0;
    for (int f = 0; f < 60; f++) {
        re15_enemy_ai_run_all(0);
        g->ai_contact = 0x00;                          /* run_all's clamp bookkeeping clears/rewrites it */
        if (g->floor == 1 && g->y == -0x708) { dropped = 1; break; }
    }
    if (!dropped) { fprintf(stderr, "FAIL(6): floor-drop must land floor+=1 / y-=0x708, floor=%d y=%d sub=0x%x\n",
                            g->floor, (int)g->y, g->sub_state_1); fail = 1; }
    printf("  (6) FLOOR-DROP: floor=%d, y=%d, exits to sub=0x%x\n", g->floor, (int)g->y, g->sub_state_1);

    if (fail) { printf("ZOMBIE GIRL byte-true FSM: FAIL\n"); return 1; }
    printf("ZOMBIE GIRL byte-true FSM: all checks passed\n");
    return 0;
}
