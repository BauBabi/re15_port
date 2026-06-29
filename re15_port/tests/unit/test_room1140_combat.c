/**
 * @file test_room1140_combat.c
 * @brief Phase 8.6 — the LIVE-zombie game_step wiring, verified in the REAL ROOM1140.
 *
 * This is the port-side dynamic verification (skill re15-room-probe) of the Phase 8.6
 * integration: the single entry re15_enemy_ai_run_all() — the type-gated slice of the
 * original per-frame entity loop FUN_8001a50c (@0x8001ce04) that game_step now calls.
 * It loads the REAL ROOM1140.RDT, spawns the 5 briefing zombies via the room's own SCD
 * sub00 (exactly as test_room1140_spawn does — Sce_em_set, byte-true), then drives
 * re15_enemy_ai_run_all and asserts:
 *
 *   (1) WIRING RUNS — every live zombie (0x10/0x11/0x16) is ticked through the live
 *       handler: state INIT->ACTIVE, ai_timer seeded 0x14 (FUN_80100688), distance cached
 *       @+0x1d0 (FUN_80100424). i.e. the per-frame loop reaches the live AI in a real room.
 *
 *   (2) FAITHFUL NO-ATTACK AS SPAWNED — the briefing zombies spawn in the feeding/lying
 *       sub-modes (grid_id & 0xf = 6/8, NOT 0), so the combat decision brain is NOT entered
 *       even with combat fully active: they do not attack until the feeding->combat WAKE-UP
 *       handler (@0x8011f80c[6]/[8], DEFERRED) transitions them to sub 0. Player HP holds.
 *
 *   (3) FULL CHAIN THROUGH run_all — arm one zombie (the windup the wake-up + decision/arm
 *       would set; covered standalone by test_live_decision_arm) and drive run_all: the armed
 *       ACTIVE handler counts the windup to 300, fires the lunge (FUN_80101224), the shared
 *       lunge slice fires the hitbox, and the player takes the byte-true 10-dmg bite. Proves
 *       the game_step entry carries an attack end-to-end onto the player.
 *
 *   (4) NON-ZOMBIE UNTOUCHED — an Elliot (type 0x47) actor added to the roster is never
 *       ticked by run_all (state/fields unchanged). This is the type-gate that keeps a room
 *       with no live zombie (the ROOM1170 boot/helipad) a pure no-op = no 1170 regression.
 *
 * No DuckStation, no game_ctx: a fast deterministic ctest. The final original-vs-port
 * comparison (real lunge geometry, 0x10/0x11 hitbox dims) is the savestate step
 * (skill re15-room-capture), separate.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_damage_seed_rng */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* Is `t` one of the live STAGE1 zombie types run_all ticks? */
static int is_live_zombie(uint8_t t) { return t == 0x10 || t == 0x11 || t == 0x16; }

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: cannot open %s\n", path); return 1; }

    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) {
        fprintf(stderr, "FAIL: RDT parse of %s\n", path); free(buf); return 1;
    }
    if (!rdt.sub_scd[0]) { fprintf(stderr, "FAIL: ROOM1140 has no sub00\n"); free(buf); return 1; }

    /* Minimal room bring-up (== scd_room_reenter / test_room1140_spawn): clear flags so
     * Ck 0xd2 is clear = first-visit branch (the 5 lying/feeding briefing zombies). */
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);

    if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    /* Player at the origin; the zombies spawned at their fixed briefing positions far away. */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0;

    /* Snapshot the spawned zombie slots (1..5 per test_room1140_spawn). */
    int zslots[RE15_ACTOR_MAX], nz = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && is_live_zombie(g_actors[s].type)) zslots[nz++] = s;

    int fail = 0;
    printf("=== ROOM1140 live-AI game_step wiring (Phase 8.6) ===\n");
    if (nz != 5) { fprintf(stderr, "FAIL: expected 5 live zombies spawned, got %d\n", nz); fail = 1; }

    /* (1)+(2): drive the wiring with combat ACTIVE for a few frames. The live tick must run on
     * every zombie (INIT->ACTIVE, ai_timer=0x14, dist cached); none may attack (feeding/lying). */
    int16_t hp_before_idle = pl->hp;
    for (int f = 0; f < 4; f++) re15_enemy_ai_run_all(1);

    for (int i = 0; i < nz; i++) {
        re15_actor_t *e = &g_actors[zslots[i]];
        if (e->state != RE15_AI_STATE_ACTIVE) {
            fprintf(stderr, "FAIL: zombie slot %d INIT->ACTIVE expected, state=0x%02X\n", zslots[i], e->state); fail = 1; }
        if (e->ai_timer != 0x14) {
            fprintf(stderr, "FAIL: zombie slot %d live_init ai_timer=0x14, ist 0x%x\n", zslots[i], e->ai_timer); fail = 1; }
        if (e->ai_dist == 0) {
            fprintf(stderr, "FAIL: zombie slot %d player-dist not cached (ai_dist=0)\n", zslots[i]); fail = 1; }
        if ((e->grid_id & 0x0f) == 0) {
            fprintf(stderr, "FAIL: zombie slot %d expected feeding/lying sub-mode (grid&0xf!=0), grid=0x%02X\n",
                    zslots[i], e->grid_id); fail = 1; }
        if (e->sub_state_1 == 7) {
            fprintf(stderr, "FAIL: zombie slot %d must NOT commit attack as spawned (+0x5=7)\n", zslots[i]); fail = 1; }
        if (e->ai_flags & 0x100) {
            fprintf(stderr, "FAIL: zombie slot %d must NOT be armed as spawned\n", zslots[i]); fail = 1; }
        /* Hitbox dims (+0x78) wired at spawn = byte-true 400/1440 from the live STAGE1 combat
         * RAM (mzd_stage1_combat_death.sav). Was missing for 0x10/0x11 (fell through to none). */
        if (e->hit_radius_min != 400 || e->hit_radius_max != 400 || e->hit_height != 1440) {
            fprintf(stderr, "FAIL: zombie slot %d hitbox 400/1440 expected, got %u/%u(min..max %u)\n",
                    zslots[i], e->hit_radius_min, e->hit_height, e->hit_radius_max); fail = 1; }
    }
    if (pl->hp != hp_before_idle) {
        fprintf(stderr, "FAIL: briefing zombies must not damage the player as spawned, HP %d->%d\n",
                hp_before_idle, pl->hp); fail = 1; }
    printf("  (1)+(2) %d zombies INIT->ACTIVE + dist cached, all in feeding/lying, player HP=%d intact\n",
           nz, pl->hp);

    /* (3): full chain through run_all. Arm one zombie's lunge windup (the state the deferred
     * wake-up + decision/arm would reach; the decision->arm coupling itself is covered byte-true
     * by test_live_decision_arm). Point its attack-reach at the player; run_all must carry the
     * lunge onto the player = the byte-true 10-dmg bite (FUN_80012d60 attack_type 0). */
    re15_damage_seed_rng(0x0badf00du);
    int za = zslots[0];
    re15_actor_t *z = &g_actors[za];
    z->state = RE15_AI_STATE_ACTIVE;
    z->ai_flags = 0x100;             /* armed (+0x1d8 & 0x100) */
    z->ai_attack_timer = 0x12d;      /* -> 0x12c (300) this active frame = the lunge fires */
    z->lunge_frames = 0;
    z->atk_pt_x = 0; z->atk_pt_y = 0; z->atk_pt_z = 0;   /* reach point == the player @origin */
    int16_t hp_before_bite = pl->hp;

    re15_enemy_ai_run_all(1);

    if (z->ai_attack_timer != 0x12c) {
        fprintf(stderr, "FAIL: armed windup 0x12d->0x12c through run_all, ist 0x%x\n", z->ai_attack_timer); fail = 1; }
    if (pl->hp != (int16_t)(hp_before_bite - 10)) {
        fprintf(stderr, "FAIL: lunge bite -10 through run_all, HP %d->%d\n", hp_before_bite, pl->hp); fail = 1; }
    if (pl->state != 2) {
        fprintf(stderr, "FAIL: bitten player -> hurt state 2, ist %d\n", pl->state); fail = 1; }
    printf("  (3) armed zombie slot %d lunge through run_all -> player HP %d->%d, hurt state %d\n",
           za, hp_before_bite, pl->hp, pl->state);

    /* (4): a non-zombie type (Elliot 0x47) must be invisible to run_all — the type gate that
     * keeps a no-live-zombie room (ROOM1170 boot) a pure no-op. */
    int es = re15_actor_alloc(0x47);
    if (es < 1) { fprintf(stderr, "FAIL: could not alloc Elliot 0x47 slot\n"); fail = 1; }
    else {
        re15_actor_t *el = &g_actors[es];
        el->state = 0x55; el->ai_timer = 0x0777; el->ai_dist = 0; el->sub_state_1 = 0x33;
        re15_enemy_ai_run_all(1);
        if (el->state != 0x55 || el->ai_timer != 0x0777 || el->ai_dist != 0 || el->sub_state_1 != 0x33) {
            fprintf(stderr, "FAIL: non-zombie 0x47 was ticked by run_all (state=0x%02X ai_timer=0x%x dist=%u sub=0x%02X)\n",
                    el->state, el->ai_timer, el->ai_dist, el->sub_state_1); fail = 1; }
        else printf("  (4) Elliot 0x47 in slot %d untouched by run_all (type-gated) = 1170-safe\n", es);
    }

    /* (5): the dist-gated WAKE-UP (Phase 8.7, byte-true gate 0x80103980: feeding sub 6 wakes when
     * cached dist +0x1d0 < 0xfa0 = 4000). Take two still-asleep feeding zombies (grid sub 6); move
     * ONE next to the player (dist 1000 < 4000) and leave the other at its far briefing spot. After
     * a few frames of run_all the near one must wake to combat (grid_id -> 0) / ACTIVE and land in
     * the engage brain (+0x5 >= 2 = engage, or the grab/attack it then commits); the far one stays
     * asleep (still grid sub 6). This is the in-engine wake the live combat savestate confirms. */
    re15_damage_seed_rng(0x5eed1234u);
    pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0;
    int wake_slot = -1, far_slot = -1;
    for (int i = 0; i < nz; i++) {
        re15_actor_t *z = &g_actors[zslots[i]];
        if ((z->grid_id & 0x0f) == 6 && z->state == RE15_AI_STATE_ACTIVE && z->sub_state_2 == 0) {
            if (wake_slot < 0) wake_slot = zslots[i];
            else if (far_slot < 0) far_slot = zslots[i];
        }
    }
    if (wake_slot < 0 || far_slot < 0) {
        fprintf(stderr, "FAIL: need two asleep feeding zombies for the wake-up test (got %d/%d)\n",
                wake_slot, far_slot); fail = 1;
    } else {
        re15_actor_t *wz = &g_actors[wake_slot];
        re15_actor_t *fz = &g_actors[far_slot];   /* stays at its briefing pos (~20000 from origin) */
        wz->x = 1000; wz->z = 0;                   /* within 4000 of the player @origin */
        for (int f = 0; f < 40; f++) re15_enemy_ai_run_all(1);
        if (wz->grid_id != 0 || wz->state != RE15_AI_STATE_ACTIVE || wz->sub_state_1 < 2) {
            fprintf(stderr, "FAIL: near feeding zombie slot %d must wake to combat/engage; "
                    "got grid=0x%02X state=%d +0x5=%d\n",
                    wake_slot, wz->grid_id, wz->state, wz->sub_state_1); fail = 1; }
        if ((fz->grid_id & 0x0f) != 6) {
            fprintf(stderr, "FAIL: far feeding zombie slot %d must stay asleep (grid sub 6); got grid=0x%02X\n",
                    far_slot, fz->grid_id); fail = 1; }
        if (!fail)
            printf("  (5) feeding zombie slot %d woke near player (grid 0x86->0x%02X, state %d, +0x5=%d engage); "
                   "far slot %d still feeding (grid 0x%02X)\n",
                   wake_slot, wz->grid_id, wz->state, wz->sub_state_1, far_slot, fz->grid_id);
    }

    /* (6): the GRAB execution (Phase 8.8) — the actual in-game attack. Put a zombie directly in the
     * committed grab state (combat sub 0, +0x5=3 = the face-to-face grab the engage commits, +0x6=0)
     * and step run_all through the sub-step machine: [0] init -> [1] pull-in -> [2] IMPACT (player
     * -10) -> [3] BITE (player -5) -> [6]/[8] EXIT back to the engage brain (+0x5=2). The byte-true
     * per-hit damage (-10 then -5) lands on the player; one grab cycle = -15 HP. */
    {
        int gs = zslots[2];                 /* a spare briefing zombie slot */
        re15_actor_t *gz = &g_actors[gs];
        pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0;
        /* Isolate the test zombie: park every OTHER zombie far away + asleep (feeding sub 6) so it
         * can neither wake nor grab — part 5 may have left one mid-grab, which would double the HP. */
        for (int i = 0; i < nz; i++) {
            re15_actor_t *z = &g_actors[zslots[i]];
            z->grid_id = 0x86; z->sub_state_1 = 0; z->sub_state_2 = 0; z->ai_flags = 0;
            z->x = 30000; z->z = 30000;
        }
        gz->x = 500; gz->z = 0; gz->state = RE15_AI_STATE_ACTIVE;
        gz->grid_id = 0; gz->sub_state_1 = 3; gz->sub_state_2 = 0; gz->ai_flags = 0;
        int16_t ghp0 = pl->hp;
        re15_enemy_ai_run_all(1);   /* [0] init  -> +0x6=1 */
        re15_enemy_ai_run_all(1);   /* [1] pull  -> +0x6=2 */
        re15_enemy_ai_run_all(1);   /* [2] IMPACT-> player -10, +0x6=3 */
        if (pl->hp != (int16_t)(ghp0 - 10)) {
            fprintf(stderr, "FAIL: grab IMPACT -10, HP %d->%d\n", ghp0, pl->hp); fail = 1; }
        re15_enemy_ai_run_all(1);   /* [3] BITE  -> player -5, +0x6=6 */
        if (pl->hp != (int16_t)(ghp0 - 15)) {
            fprintf(stderr, "FAIL: grab BITE -5 (total -15), HP %d->%d\n", ghp0, pl->hp); fail = 1; }
        re15_enemy_ai_run_all(1);   /* [6] release -> +0x6=8 */
        re15_enemy_ai_run_all(1);   /* [8] EXIT  -> +0x5=2 (engage) */
        if (gz->sub_state_1 != 2) {
            fprintf(stderr, "FAIL: grab must exit to engage (+0x5=2), ist %d\n", gz->sub_state_1); fail = 1; }
        if (!fail)
            printf("  (6) grab zombie slot %d: IMPACT+BITE -> player HP %d->%d (-15 byte-true), exited to engage (+0x5=%d)\n",
                   gs, ghp0, pl->hp, gz->sub_state_1);
    }

    /* (7): the TURN-to-face -> grab chain (Phase 8.9). Put a zombie in the turn state (+0x5=7, grid 0
     * combat) CLOSE to the player but FACING 90deg AWAY; tick run_all: the turn animate rotates rot_y
     * toward the player (byte-true ±0x80/frame, arc_test(0x80) residual) until within the ±0x200 grab
     * cone, then the decide [7] commits the grab -> the grab drains the player's HP. Proves the woken
     * zombie turns to face the approaching player and attacks (the forward walk stays deferred). */
    {
        int ts = zslots[3];
        re15_actor_t *tz = &g_actors[ts];
        pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->floor = 0;
        /* Isolate the test zombie (park the others far + asleep), as in part 6. */
        for (int i = 0; i < nz; i++) {
            re15_actor_t *z = &g_actors[zslots[i]];
            z->grid_id = 0x86; z->sub_state_1 = 0; z->sub_state_2 = 0; z->ai_flags = 0;
            z->x = 30000; z->z = 30000;
        }
        tz->x = 600; tz->z = 0; tz->floor = 0;       /* close (dist 600 < 1200), same floor */
        tz->rot_y = 1024;                            /* facing +Z = 90deg off the player (at -X) */
        tz->state = RE15_AI_STATE_ACTIVE; tz->grid_id = 0; tz->sub_state_1 = 7; tz->sub_state_2 = 0;
        tz->ai_flags = 0; tz->ai_dist = 600;
        int16_t trot0 = tz->rot_y, thp0 = pl->hp;
        int committed = 0;
        for (int f = 0; f < 30; f++) {
            re15_enemy_ai_run_all(1);
            if (tz->sub_state_1 == 3 || tz->sub_state_1 == 4) committed = 1;   /* reached the grab */
        }
        if (tz->rot_y == trot0) {
            fprintf(stderr, "FAIL: turn-to-face must rotate rot_y toward the player (still %d)\n", tz->rot_y); fail = 1; }
        if (!committed) {
            fprintf(stderr, "FAIL: turn must reach facing -> grab commit (+0x5 never 3/4)\n"); fail = 1; }
        if (pl->hp >= thp0) {
            fprintf(stderr, "FAIL: turn->grab must drain player HP, ist %d (was %d)\n", pl->hp, thp0); fail = 1; }
        if (!fail)
            printf("  (7) turn-to-face slot %d: rot_y %d->%d (turned to face), committed grab, player HP %d->%d\n",
                   ts, trot0, tz->rot_y, thp0, pl->hp);
    }

    /* (8): the PLAYER-GRABBED LOCK (Phase 8.10). While a live zombie is in the grab sub-mode the
     * player is latched (DAT_800aca58 = cmd 5, byte-true @0x80102640) and game_step pins him
     * (re15_player_is_grabbed -> skip re15_player_tick = the player cannot steer/walk away). Verify
     * the flag TRACKS the grab: 0 with no zombie grabbing, 1 the frame a zombie is in the grab
     * sub-mode, back to 0 once the grab releases and the player is out of range (the no-soft-lock
     * faithful-line release for the deferred player grabbed-FSM). */
    {
        int gs = zslots[4];
        re15_actor_t *gz = &g_actors[gs];
        pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->floor = 0;
        /* Isolate: park every zombie far + asleep so none grabs spuriously. */
        for (int i = 0; i < nz; i++) {
            re15_actor_t *z = &g_actors[zslots[i]];
            z->grid_id = 0x86; z->sub_state_1 = 0; z->sub_state_2 = 0; z->ai_flags = 0;
            z->x = 30000; z->z = 30000;
        }
        re15_enemy_ai_run_all(1);                     /* no zombie grabbing -> latch clears */
        int free_before = !re15_player_is_grabbed();
        if (!free_before) {
            fprintf(stderr, "FAIL: player must NOT be grabbed with all zombies asleep/far\n"); fail = 1; }

        /* commit one zombie to the face-to-face grab (+0x5=3, grid 0) right next to the player */
        gz->x = 500; gz->z = 0; gz->floor = 0; gz->state = RE15_AI_STATE_ACTIVE;
        gz->grid_id = 0; gz->sub_state_1 = 3; gz->sub_state_2 = 0; gz->ai_flags = 0;
        re15_enemy_ai_run_all(1);                     /* the grab dispatches -> latch set */
        int grabbed_during = re15_player_is_grabbed();
        if (!grabbed_during) {
            fprintf(stderr, "FAIL: player must be grabbed while a zombie is in the grab sub-mode\n"); fail = 1; }

        /* release: move the player out of the ±0x4b0 grab cone and put the zombie back in engage so
         * it cannot re-commit; after a run_all no zombie is in grab state -> the latch clears. */
        pl->x = 60000; pl->z = 60000;
        gz->sub_state_1 = 2; gz->sub_state_2 = 0;     /* engage; player far -> no re-commit */
        re15_enemy_ai_run_all(1);
        int free_after = !re15_player_is_grabbed();
        if (!free_after) {
            fprintf(stderr, "FAIL: player must be released once no zombie is grabbing (player far)\n"); fail = 1; }
        if (!fail)
            printf("  (8) player-grabbed lock: free=%d before -> grabbed=%d during grab -> free=%d after release\n",
                   free_before, grabbed_during, free_after);
    }

    /* (9): the PLAYER DEATH FSM (Phase 8.10). When the grab drains the player's HP < 0 he DIES:
     * re15_player_is_dead() goes true, the byte-true GRABBED-death state 7 is set (the live combat-
     * death save shows 0x800aca58 = 7), and the death-sequence timer (0x78 = 120 frames @0x8003694c
     * INIT) seeds + counts down. game_step would then freeze the player (skip re15_player_tick) for
     * the whole sequence; here we drive the grab to a kill and check the death state + timer directly. */
    {
        int ds = zslots[0];
        re15_actor_t *dz = &g_actors[ds];
        re15_player_death_reset();
        pl->x = 0; pl->z = 0; pl->hp = 8; pl->hit_react = 0; pl->state = 0; pl->floor = 0;
        for (int i = 0; i < nz; i++) {   /* isolate the test zombie (others far + asleep) */
            re15_actor_t *z = &g_actors[zslots[i]];
            z->grid_id = 0x86; z->sub_state_1 = 0; z->sub_state_2 = 0; z->ai_flags = 0;
            z->x = 30000; z->z = 30000;
        }
        dz->x = 500; dz->z = 0; dz->floor = 0; dz->state = RE15_AI_STATE_ACTIVE;
        dz->grid_id = 0; dz->sub_state_1 = 3; dz->sub_state_2 = 0; dz->ai_flags = 0;
        if (re15_player_is_dead()) {
            fprintf(stderr, "FAIL: player must be alive before the kill (HP %d)\n", pl->hp); fail = 1; }
        re15_enemy_ai_run_all(1);   /* [0] init */
        re15_enemy_ai_run_all(1);   /* [1] pull */
        re15_enemy_ai_run_all(1);   /* [2] IMPACT -> HP 8-10 = -2 < 0 -> DEAD */
        if (!re15_player_is_dead()) {
            fprintf(stderr, "FAIL: grab IMPACT must kill the player (HP %d, expected <0)\n", pl->hp); fail = 1; }
        if (pl->state != 7) {
            fprintf(stderr, "FAIL: grabbed death must set player state 7, ist %d\n", pl->state); fail = 1; }
        int t0 = re15_player_death_tick();   /* INIT  -> seed 0x78 (120) */
        int t1 = re15_player_death_tick();   /* sub-1 -> 0x77 (119) */
        if (t0 != 0x78 || t1 != 0x77) {
            fprintf(stderr, "FAIL: death timer 0x78->0x77, got 0x%x->0x%x\n", t0, t1); fail = 1; }
        if (!fail)
            printf("  (9) player death: grab kill -> HP %d, dead -> grabbed-death state %d, death timer 0x%x->0x%x\n",
                   pl->hp, pl->state, t0, t1);
    }

    /* (10): the zombie HURT/DEATH states [2]/[3] (Phase 8.10) — the receiving end of the (coming)
     * player attack (two-sided combat). re15_enemy_take_damage sets the zombie to state 2 (HURT) on a
     * hit / state 3 (DEATH) on HP<0; the live tick now dispatches [2] HURT -> back to ACTIVE (the
     * stagger) and [3] DEATH -> state 7 (an inert CORPSE, no longer dispatched = can't engage/grab). */
    {
        pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0;
        re15_player_death_reset();
        int zs = zslots[1];
        re15_actor_t *zc = &g_actors[zs];
        for (int i = 0; i < nz; i++) {   /* park the others far + asleep */
            re15_actor_t *z = &g_actors[zslots[i]];
            z->grid_id = 0x86; z->sub_state_1 = 0; z->sub_state_2 = 0; z->ai_flags = 0;
            z->x = 30000; z->z = 30000;
        }
        zc->x = 500; zc->z = 0; zc->state = RE15_AI_STATE_ACTIVE; zc->grid_id = 0;
        zc->sub_state_1 = 2; zc->sub_state_2 = 0; zc->hp = 30; zc->hit_react = 0; zc->ai_flags = 0;

        re15_enemy_take_damage(zc, 0);   /* non-lethal: HP 30->20 -> HURT (state 2) */
        if (zc->state != RE15_AI_STATE_HURT) {
            fprintf(stderr, "FAIL: non-lethal hit -> zombie HURT (state 2), ist %d\n", zc->state); fail = 1; }
        re15_enemy_ai_live_tick(zs);     /* [2] HURT -> ACTIVE (stagger done) */
        if (zc->state != RE15_AI_STATE_ACTIVE) {
            fprintf(stderr, "FAIL: HURT -> ACTIVE after the stagger, ist %d\n", zc->state); fail = 1; }

        zc->hp = 5; zc->hit_react = 0;
        re15_enemy_take_damage(zc, 0);   /* lethal: 5-10 = -5 < 0 -> DEATH (state 3) */
        if (zc->state != RE15_AI_STATE_DEATH) {
            fprintf(stderr, "FAIL: lethal hit -> zombie DEATH (state 3), ist %d (HP %d)\n", zc->state, zc->hp); fail = 1; }
        re15_enemy_ai_live_tick(zs);     /* [3] DEATH -> CORPSE (state 7) */
        if (zc->state != RE15_AI_STATE_CORPSE) {
            fprintf(stderr, "FAIL: DEATH -> CORPSE (state 7), ist %d\n", zc->state); fail = 1; }

        int16_t hp_corpse = pl->hp;
        for (int f = 0; f < 5; f++) re15_enemy_ai_run_all(1);   /* the corpse must stay inert */
        if (zc->state != RE15_AI_STATE_CORPSE) {
            fprintf(stderr, "FAIL: corpse must stay inert (state 7), ist %d\n", zc->state); fail = 1; }
        if (pl->hp != hp_corpse) {
            fprintf(stderr, "FAIL: a corpse must not damage the player, HP %d->%d\n", hp_corpse, pl->hp); fail = 1; }
        if (!fail)
            printf("  (10) zombie shot -> HURT(2)->ACTIVE; lethal -> DEATH(3)->CORPSE(7), inert (no player damage)\n");
    }

    free(buf);
    if (fail) { fprintf(stderr, "\nROOM1140 COMBAT-WIRING TEST FAILED\n"); return 1; }
    printf("\nPASS: ROOM1140 live-AI game_step wiring "
           "(spawn; WAKE->engage; TURN-to-face->GRAB->HP; GRABBED-lock; player DEATH; zombie HURT/DEATH; type-gated)\n");
    return 0;
}
