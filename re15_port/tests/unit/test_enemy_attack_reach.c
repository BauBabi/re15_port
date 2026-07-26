/* test_enemy_attack_reach.c — melee attacks must CONNECT through the player body-push standoff.
 *
 * Audit wf_555f18eb Part B. re15_body_push_player (game_step) holds the player at exactly
 * hit_radius_min + RE15_BODY_R_PLAYER out of every enemy body (the aec4/b544 standoff). The
 * big-bodied enemies gated their melee connect on a distance SMALLER than that standoff
 * (maggot 2000/1600 < 2050, adult-spider 900 < 1450, alligator 1400 < 2650, spider-baby 600 <
 * 1050), so in-game — with the push active — the attack could NEVER reach the pushed-away player
 * and the enemy was silently harmless. The per-enemy unit tests never caught it because they run
 * re15_enemy_ai_run_all WITHOUT the game_step player-push.
 *
 * This regression drives the SAME loop game_step does — re15_body_push_player() THEN
 * re15_enemy_ai_run_all() — and asserts each affected enemy still lands its attack. On the
 * pre-fix code every case fails (hp stays 100 / never grabbed); the body-contact gate restores it.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include <stdlib.h>
#include "re15_enemy.h"
#include "re15_ems.h"
#include "re15_emd.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
/* The maggot moves via its LOCATOR foot-plant root motion (FUN_8011bf50, EMR-driven — audit
 * wf_827f186d maggot #5): the harness must supply the EM027 bank (CDEMD0.EMS idx 12) the way
 * the room loader does, or the crawl is inert and the reach/push assertions are vacuous. */
static int load_maggot_bank_for_motion(void)
{
    FILE *f = fopen(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *)malloc((size_t)n);
    if (!buf || fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(buf); return 0; }
    fclose(f);
    size_t off, len;
    if (re15_ems_get_entry(buf, (size_t)n, 12, &off, &len)) { free(buf); return 0; }
    re15_enemy_bank_t *bank = re15_enemy_find(0x27);
    if (!bank) bank = re15_enemy_alloc(0x27);
    if (!bank) { free(buf); return 0; }
    if (re15_emd_parse_loco_bank(buf + off, len, &bank->skel, &bank->anim)) { free(buf); return 0; }
    bank->ok = 1; bank->buf = buf;
    return 1;
}

extern int16_t re15_atan2_q12(int32_t, int32_t);
extern int  re15_player_is_grabbed(void);
extern void re15_body_push_player(void);
extern void re15_actors_anim_advance(void);   /* the shared per-frame anim pass game_step runs at :481 */

static int32_t xz_dist(const re15_actor_t *a, const re15_actor_t *b)
{
    int64_t dx = (int64_t)a->x - b->x, dz = (int64_t)a->z - b->z;
    int64_t d2 = dx * dx + dz * dz;
    int32_t r = 0; while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
    return r;
}

/* one game_step-order frame for the enemy loop, byte-true to game_step_common.c:
 * body_push_player (:250) -> re15_actors_anim_advance (:481) -> re15_enemy_ai_run_all (:489).
 * The anim pass MUST be here: it is the shared advance that pinned the maggot's bite clip 0x12 to
 * frame 0 (double-advance / lie-down pin) — a step() without it hid the "harmless maggot" bug. */
static void step(void) { re15_body_push_player(); re15_actors_anim_advance(); re15_enemy_ai_run_all(0); }

int main(void)
{
    int fail = 0;
    if (!load_maggot_bank_for_motion()) fprintf(stderr, "WARN: EM027 bank missing - maggot locomotion inert\n");
    printf("=== enemy melee reach through the body-push standoff (audit wf_555f18eb Part B) ===\n");

    /* (1) MAGGOT 0x27 — bite/heavy must chip the player through the 2050 standoff (was 2000/1600). */
    {
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 3200; pl->hp = 100;
        re15_actor_t *m = &g_actors[1];
        m->active = 1; m->type = 0x27; m->state = 0; m->x = 0; m->z = 0; m->rot_y = 0;
        re15_enemy_apply_hitbox(m, 0x27);
        m->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - m->z, pl->x - m->x) - 0x400) & 0xfff);
        int hit = 0;
        for (int f = 0; f < 400; f++) { step(); if (pl->hp < 100) { hit = 1; break; } pl->hit_react = 0; }
        if (!hit) { fprintf(stderr, "FAIL(1): maggot never chipped the player through the push (hp=%d, dist=%d)\n", pl->hp, xz_dist(m, pl)); fail = 1; }
        else printf("  (1) MAGGOT: lands damage under the push, hp=%d (dist=%d)\n", pl->hp, xz_dist(m, pl));
    }

    /* (2) ADULT SPIDER 0x25 — non-damaging stagger-GRAB must still latch through the 1450 standoff. */
    {
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 1600; pl->hp = 100;
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x25; e->state = 1; e->sub_state_1 = 0; e->grid_id = 1; e->hp = 100; e->x = 0; e->z = 0;
        re15_enemy_apply_hitbox(e, 0x25);
        e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
        int grabbed = 0; int16_t hp0 = pl->hp;
        for (int f = 0; f < 240; f++) { step(); if (re15_player_is_grabbed()) { grabbed = 1; break; } pl->hit_react = 0; }
        if (!grabbed)      { fprintf(stderr, "FAIL(2): adult spider never grabbed through the push (dist=%d)\n", xz_dist(e, pl)); fail = 1; }
        if (pl->hp != hp0) { fprintf(stderr, "FAIL(2): the stagger-grab must deal 0 damage, hp %d->%d\n", hp0, pl->hp); fail = 1; }
        else if (grabbed)  printf("  (2) ADULT SPIDER: stagger-grab latches under the push (hp %d, byte-true 0 dmg)\n", pl->hp);
    }

    /* (3) ALLIGATOR 0x23 — the BITE must reach through the 2650 standoff. Byte-true rebuild
     * (audit wf_efd92a2c alligator #0/#1/#2): connect is the jaw-bone box (skel+2644, +2644 FORWARD
     * of the pushed-away body center -> reaches via re15_body_contact_reach) at anim frames {19,20,21},
     * and it KNOCKS the player DOWN (aca58=2 -> hit_react latch), NOT a grab-swallow. The player is
     * never pinned and never killed (the old "jaws-hold -> pl->hp=-1 swallow" was invented). */
    {
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 2400; pl->hp = 100;
        re15_actor_t *e = &g_actors[1];
        e->active = 1; e->type = 0x23; e->state = 1; e->sub_state_1 = 6; e->hp = 300; e->x = 0; e->z = 0;
        re15_enemy_apply_hitbox(e, 0x23);
        e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
        int knocked = 0, pinned = 0;
        for (int f = 0; f < 300; f++) { step(); if (pl->hit_react & 1) knocked = 1; if (re15_player_is_grabbed()) pinned = 1; pl->hit_react = 0; }
        if (!knocked)   { fprintf(stderr, "FAIL(3): alligator bite never landed through the push (dist=%d)\n", xz_dist(e, pl)); fail = 1; }
        if (pinned)     { fprintf(stderr, "FAIL(3): the alligator bite is a KNOCKDOWN, not a pin/grab\n"); fail = 1; }
        if (pl->hp < 0) { fprintf(stderr, "FAIL(3): the bite must NOT swallow/kill the player, hp=%d\n", pl->hp); fail = 1; }
        if (!fail || knocked) printf("  (3) ALLIGATOR: bite (knockdown) reaches under the push, knocked=%d pinned=%d hp=%d\n", knocked, pinned, pl->hp);
    }

    /* (4) SPIDER-BABY 0x26 — REWRITTEN byte-true (audit wf_827f186d spider #3 + body-push claim):
     * the -2 IS the aec4 body-push RETURN (`jal 0x8002aec4` @0x80116368, `beq v0,zero,exit`
     * @0x80116370) — it fires on a REAL overlap (a walking-in player penetrates before the
     * player-side b544 push resolves) and is byte-true SILENT once the push-out has settled the
     * pair at/over the standoff (pen<1 -> aec4 returns 0; both sides share the same SquareRoot0,
     * whose underestimate makes the resolving push overshoot R). The old expectation here (-2 at
     * the RESOLVED standoff) pinned the removed port-invented distance gate, not the original. */
    {
        memset(g_actors, 0, sizeof g_actors);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 3000; pl->z = 8000; pl->hp = 100;   /* far while it emerges */
        re15_player_apply_hitbox(pl);   /* r 450 / h 1530 — the aec4 y-band (@0x8002b1f0-b228) must span
                                         * the ACTIVE spider hanging at y = home_y - 20*(phase-1) ~ -800
                                         * (unconditional pin @0x80116408-98, audit wf_827f186d spider #2) */
        re15_actor_t *s = &g_actors[1];
        s->active = 1; s->type = 0x26; s->state = 0; s->x = 3000; s->z = 3000; s->grid_id = 0x02;
        re15_enemy_apply_hitbox(s, 0x26);
        for (int f = 0; f < 140; f++) re15_enemy_ai_run_all(0);   /* emerge to SOLID (no player nearby) */
        pl->x = s->x; pl->z = s->z + 400; pl->hp = 100; pl->hit_react = 0;   /* penetration frame (walk-in) */
        re15_enemy_ai_run_all(0);        /* the spider's own tick sees the overlap -> aec4 ret != 0 -> -2 */
        if (pl->hp != 98) { fprintf(stderr, "FAIL(4): -2 must land on the penetration frame (@0x801163c8-d0), hp=%d\n", pl->hp); fail = 1; }
        /* now the game_step loop: the player-side push resolves the pair out of overlap = SOLID */
        for (int f = 0; f < 20; f++) step();
        if (xz_dist(s, pl) < 1049) { fprintf(stderr, "FAIL(4): the push must hold the player at the ~1050 standoff (solid body), dist=%d\n", xz_dist(s, pl)); fail = 1; }
        if (!fail) printf("  (4) SPIDER-BABY: -2 on overlap (hp 100->%d), then pushed solid to dist=%d\n", pl->hp, xz_dist(s, pl));
    }

    if (fail) { printf("ENEMY-ATTACK-REACH: FAIL\n"); return 1; }
    printf("ENEMY-ATTACK-REACH: all checks passed\n");
    return 0;
}
