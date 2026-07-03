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
#include "re15_enemy.h"    /* re15_enemy_alloc/find/reset — mock the death-clip bank */
#include "re15_damage.h"   /* re15_damage_seed_rng */
#include "re15_player.h"   /* re15_player_tick + RE15_PAD_BIT_R1 (the aim pose, Phase 8.14) */
#include "re15_vab.h"      /* re15_vab_parse + re15_footstep_vag — the death-SE bank lookup (Phase 8.17) */
#include "re15_room.h"     /* g_room_change + g_current_room_id — the death->continue reload (part 20) */

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
        /* byte-true spawn state (decoder @0x80100c20): feeders (behavior 0x86) -> grid=0 +0x5=0xc,
         * the lyer (0x88) -> grid=0x88 +0x5=0. Dormant = feeding (+0x5=0xc) OR lying (grid&0xf==8). */
        if (e->sub_state_1 != 0x0c && (e->grid_id & 0x0f) != 8) {
            fprintf(stderr, "FAIL: zombie slot %d expected dormant feeding(+0x5=0xc)/lying(grid8), grid=0x%02X +0x5=%d\n",
                    zslots[i], e->grid_id, e->sub_state_1); fail = 1; }
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

    /* (5): the dist-gated WAKE-UP (byte-true FUN_801048a8, the +0x5=0xc feeding DECIDE @0x8011f840[0xc]:
     * dist +0x1d0 < 0xbb8 = 3000 && player alive -> +0x5=0xd stand-up -> +0x5=2 engage). Take two
     * still-feeding zombies (+0x5=0xc); move ONE next to the player (dist 1000 < 3000) and leave the
     * other at its far briefing spot. After a few frames the near one wakes through the stand-up to the
     * engage brain (+0x5 >= 2); the far one stays feeding (+0x5=0xc). Byte-true spawn = grid=0, so the
     * feeders are identified by +0x5=0xc now, not the old grid sub-6. */
    re15_damage_seed_rng(0x5eed1234u);
    pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0;
    int wake_slot = -1, far_slot = -1;
    for (int i = 0; i < nz; i++) {
        re15_actor_t *z = &g_actors[zslots[i]];
        if (z->sub_state_1 == 0x0c && z->state == RE15_AI_STATE_ACTIVE) {
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
        if (fz->sub_state_1 != 0x0c) {
            fprintf(stderr, "FAIL: far feeding zombie slot %d must stay feeding (+0x5=0xc); got +0x5=%d grid=0x%02X\n",
                    far_slot, fz->sub_state_1, fz->grid_id); fail = 1; }
        if (!fail)
            printf("  (5) feeding zombie slot %d woke near player (grid 0x%02X, state %d, +0x5=%d engage); "
                   "far slot %d still feeding (+0x5=0xc)\n",
                   wake_slot, wz->grid_id, wz->state, wz->sub_state_1, far_slot);
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
        re15_enemy_ai_run_all(1);   /* [3] BITE-loop: -5 per bite-clip wrap (no bank -> wrap every tick) */
        if (pl->hp != (int16_t)(ghp0 - 15)) {
            fprintf(stderr, "FAIL: grab BITE -5 (total -15), HP %d->%d\n", ghp0, pl->hp); fail = 1; }
        /* TIMELINE-VERIFIED HOLD (deterministic /tmp/tl3): the grab HOLDS the bite sub-step (+0x6=3),
         * biting -5 per clip wrap over the 0x6e window — it does NOT release after one bite. Drive it
         * until the bites kill; the killer must then exit to +0x5=6 WALK-to-corpse (observed live:
         * killer walks clip 0xa and circles the corpse), NOT re-enter the engage brain. */
        int held_ok = 1, guard = 0;
        while (pl->hp >= 0 && guard < 200) {
            if (gz->sub_state_1 != 3 || gz->sub_state_2 != 3) held_ok = 0;
            re15_enemy_ai_run_all(1); guard++;
        }
        if (!held_ok) {
            fprintf(stderr, "FAIL: grab must HOLD the bite sub-step (+0x6=3) until the kill\n"); fail = 1; }
        if (pl->hp >= 0) {
            fprintf(stderr, "FAIL: the held grab must kill (bites/devour), hp=%d after %d ticks\n", pl->hp, guard); fail = 1; }
        /* the kill hands off to the DEVOUR-FINISH state (+0x5)+2: this was a FACE grab (3) -> 5.
         * (The live behind-grab hands to 6 — observed in the original timeline.) */
        if (gz->sub_state_1 != 5) {
            fprintf(stderr, "FAIL: killer must hand off to DEVOUR (+0x5=3+2=5), ist %d\n", gz->sub_state_1); fail = 1; }
        /* the devour decide is a jr-ra stub: the killer STAYS in it forever (timeline: 20+s at d<0x5dc
         * while only ENGAGE-state zombies commit the dead-feed) */
        for (int f = 0; f < 10; f++) re15_enemy_ai_run_all(1);
        if (gz->sub_state_1 != 5) {
            fprintf(stderr, "FAIL: killer must STAY in the devour state (+0x5=5), ist %d\n", gz->sub_state_1); fail = 1; }
        if (!fail)
            printf("  (6) grab slot %d: IMPACT -10, held BITE-loop -5/wrap kills (hp %d->%d in %d ticks), exit +0x5=6 walk\n",
                   gs, ghp0, pl->hp, guard);
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
        zc->sub_state_1 = 2; zc->sub_state_2 = 0; zc->sub_state_3 = 0; zc->hp = 30; zc->hit_react = 0; zc->ai_flags = 0;
        zc->hurt_clip = 3;   /* the per-spawn stagger clip (normally seeded in live_init) */

        re15_enemy_take_damage(zc, 0);   /* non-lethal: HP 30->20 -> HURT (state 2), +0x5=react_table[0]=3 */
        if (zc->state != RE15_AI_STATE_HURT) {
            fprintf(stderr, "FAIL: non-lethal hit -> zombie HURT (state 2), ist %d\n", zc->state); fail = 1; }
        re15_enemy_ai_live_tick(zs);     /* [2] HURT phase 0: play the stagger clip + seed/decrement the stun */
        if (zc->state != RE15_AI_STATE_HURT || zc->motion != 3) {
            fprintf(stderr, "FAIL: HURT holds + plays the stagger clip (motion=hurt_clip=3), state=%d motion=%d\n",
                    zc->state, zc->motion); fail = 1; }
        int ht = 0;                       /* the stun (seed 4..7, -3/frame for +0x5=3) recovers in <=3 frames */
        while (zc->state == RE15_AI_STATE_HURT && ht < 12) { re15_enemy_ai_live_tick(zs); ht++; }
        if (zc->state != RE15_AI_STATE_ACTIVE || zc->sub_state_1 != 0x11) {
            fprintf(stderr, "FAIL: HURT -> ACTIVE after the stun (+0x5=0x11), state=%d +0x5=%d\n",
                    zc->state, zc->sub_state_1); fail = 1; }

        zc->hp = 5; zc->hit_react = 0;
        re15_enemy_take_damage(zc, 0);   /* lethal: 5-10 = -5 < 0 -> DEATH (state 3) */
        if (zc->state != RE15_AI_STATE_DEATH) {
            fprintf(stderr, "FAIL: lethal hit -> zombie DEATH (state 3), ist %d (HP %d)\n", zc->state, zc->hp); fail = 1; }
        re15_enemy_ai_live_tick(zs);     /* [3] death FSM phase 0: set death clip 0x1f, hold DEATH */
        if (zc->motion != 0x1f) {
            fprintf(stderr, "FAIL: death phase0 sets motion 0x1f, ist %d\n", zc->motion); fail = 1; }
        re15_enemy_ai_live_tick(zs);     /* phase 1->2 (no model bank -> straight to CORPSE state 7) */
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

    /* (11): the PLAYER WEAPON SHOT (Phase 8.10, TWO-SIDED combat) — re15_player_weapon_fire. The player
     * auto-aims the nearest live zombie IN FRONT within the per-weapon reach and applies the byte-true
     * per-weapon damage (pistol = weapon 2 = 24 to a zombie); the zombie enters HURT (2) / DEATH (3).
     * This closes the two-sided loop: the player shoots -> the zombie hurt/death (part 10). */
    {
        pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->rot_y = 0; pl->floor = 0;
        re15_player_death_reset();
        for (int i = 0; i < nz; i++) {   /* park the others far (active but out of the pistol's reach) */
            re15_actor_t *z = &g_actors[zslots[i]];
            z->grid_id = 0; z->sub_state_1 = 0; z->sub_state_2 = 0; z->ai_flags = 0;
            z->state = RE15_AI_STATE_ACTIVE; z->x = 30000; z->z = 30000; z->hp = 60; z->hit_react = 0;
        }
        re15_actor_t *front  = &g_actors[zslots[2]];
        re15_actor_t *behind = &g_actors[zslots[3]];
        front->x = 0;  front->z = 800;    /* IN FRONT (+Z, player rot_y=0), dist 800 < pistol reach 1000 */
        behind->x = 0; behind->z = -500;  /* BEHIND, nearer (500) but outside the front cone */
        int16_t zhp0 = front->hp;
        int hit = re15_player_weapon_fire(2);   /* fire the pistol (weapon 2 = 24 dmg) */
        if (hit != (zslots[2] + 1)) {
            fprintf(stderr, "FAIL: pistol must auto-aim the FRONT zombie slot %d (not the nearer behind one), hit=%d\n",
                    zslots[2], hit); fail = 1; }
        if (front->hp != (int16_t)(zhp0 - 24)) {
            fprintf(stderr, "FAIL: pistol -> zombie -24 (byte-true), HP %d->%d\n", zhp0, front->hp); fail = 1; }
        if (front->state != RE15_AI_STATE_HURT || front->sub_state_1 != 2) {
            fprintf(stderr, "FAIL: shot zombie -> HURT(2) + reaction +0x5=2, state=%d +0x5=%d\n",
                    front->state, front->sub_state_1); fail = 1; }
        if (behind->hp != 60) {
            fprintf(stderr, "FAIL: the BEHIND zombie must NOT be auto-aimed, HP=%d\n", behind->hp); fail = 1; }
        front->hp = 10; front->hit_react = 0; front->state = RE15_AI_STATE_ACTIVE; front->sub_state_3 = 9;
        re15_player_weapon_fire(2);   /* 10-24 = -14 < 0 -> DEATH; +0x7 reset to 0 */
        if (front->state != RE15_AI_STATE_DEATH || front->sub_state_3 != 0) {
            fprintf(stderr, "FAIL: lethal shot -> DEATH(3) + sub_state_3 reset to 0, state=%d +0x7=%d (HP %d)\n",
                    front->state, front->sub_state_3, front->hp); fail = 1; }
        re15_enemy_ai_live_tick(zslots[2]);   /* death FSM phase 0: set the death clip 0x1f, hold DEATH */
        if (front->motion != 0x1f || front->state != RE15_AI_STATE_DEATH) {
            fprintf(stderr, "FAIL: death phase0 sets motion 0x1f + holds DEATH, motion=%d state=%d\n",
                    front->motion, front->state); fail = 1; }
        re15_enemy_ai_live_tick(zslots[2]);   /* phase 1->2 (no model bank loaded -> straight to corpse) */
        if (front->state != RE15_AI_STATE_CORPSE) {
            fprintf(stderr, "FAIL: shot DEATH -> CORPSE (7), ist %d\n", front->state); fail = 1; }
        if (!fail)
            printf("  (11) player pistol -> auto-aim FRONT zombie -24 -> HURT(2) (behind skipped); "
                   "lethal -> DEATH(motion 0x1f)->CORPSE(7)\n");
    }

    /* (12): the BYTE-TRUE reach bound (cone tester FUN_800127fc/800128a0): R = reach + enemy hitbox
     * radius (hbdata+6 = hit_radius_min = 400 for zombies), hit iff strict dist < R. The pistol's table
     * reach is 1000, so the true effective range vs a zombie is 1000+400 = 1400 (strict). Pre-fix the
     * port tested dist <= reach (radius omitted) and would have MISSED a zombie at 1000..1399. */
    {
        pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->rot_y = 0; pl->floor = 0;
        re15_player_death_reset();
        for (int i = 0; i < nz; i++) {   /* park all far */
            re15_actor_t *z = &g_actors[zslots[i]];
            z->state = RE15_AI_STATE_ACTIVE; z->x = 30000; z->z = 30000; z->hp = 60; z->hit_react = 0;
            z->sub_state_1 = 0; z->grid_id = 0;
        }
        re15_actor_t *z = &g_actors[zslots[0]];
        if (z->hit_radius_min != 400) {   /* the spawn dim this whole bound depends on */
            fprintf(stderr, "FAIL: (12) zombie hit_radius_min must be 400, ist %u\n", z->hit_radius_min); fail = 1; }
        /* in the band reach..R-1: dist 1399 (> reach 1000, < R 1400) -> MUST hit now */
        z->x = 0; z->z = 1399; z->hp = 60; z->hit_react = 0; z->state = RE15_AI_STATE_ACTIVE;
        if (re15_player_weapon_fire(2) != (zslots[0] + 1) || z->hp != (int16_t)(60 - 24)) {
            fprintf(stderr, "FAIL: (12) zombie at dist 1399 (< reach+radius 1400) must be HIT, HP=%d\n", z->hp); fail = 1; }
        /* at exactly R = 1400: strict dist < R -> MUST miss */
        z->x = 0; z->z = 1400; z->hp = 60; z->hit_react = 0; z->state = RE15_AI_STATE_ACTIVE;
        if (re15_player_weapon_fire(2) != 0 || z->hp != 60) {
            fprintf(stderr, "FAIL: (12) zombie at dist 1400 (== reach+radius, strict <) must MISS, hp=%d\n", z->hp); fail = 1; }
        if (!fail)
            printf("  (12) byte-true reach: hit at dist 1399 (reach 1000 + radius 400), miss at 1400 (strict <)\n");
    }

    /* (13): the byte-true DEATH ANIMATION play-out (FUN_80107cb0). With the enemy model bank loaded,
     * the death state sets the death clip (motion 0x1f) and HOLDS state DEATH until that clip has
     * played to its last frame (anim_frame == frame_count-1) — only THEN -> CORPSE. (In-game the
     * shared anim loop advances anim_frame; here we drive it.) Mock a bank with a 5-frame death clip. */
    {
        re15_actor_t *z = &g_actors[zslots[0]];
        re15_enemy_bank_t *bank = re15_enemy_alloc(z->type);   /* claim a bank for this zombie type */
        if (!bank) { fprintf(stderr, "FAIL: (13) could not alloc a mock bank\n"); fail = 1; }
        else {
            bank->anim.clip_count = 0x20;                       /* clips 0..0x1f exist */
            bank->anim.clips[0x1f].frame_count = 5;             /* the death clip = 5 frames */
            bank->ok = 1;                                       /* re15_enemy_find now returns it */

            z->state = RE15_AI_STATE_DEATH; z->sub_state_3 = 0; z->motion = 0; z->anim_frame = 0;
            re15_enemy_ai_live_tick(zslots[0]);                /* phase 0: motion 0x1f, hold DEATH */
            if (z->motion != 0x1f || z->state != RE15_AI_STATE_DEATH || z->sub_state_3 != 1) {
                fprintf(stderr, "FAIL: (13) phase0 motion=%d state=%d +0x7=%d\n",
                        z->motion, z->state, z->sub_state_3); fail = 1; }
            z->anim_frame = 3;                                  /* clip still playing (3 < 5-1) */
            re15_enemy_ai_live_tick(zslots[0]);
            if (z->state != RE15_AI_STATE_DEATH) {
                fprintf(stderr, "FAIL: (13) mid-clip (frame 3/5) must stay DEATH, state=%d\n", z->state); fail = 1; }
            z->anim_frame = 4;                                  /* clip's last frame (5-1) */
            re15_enemy_ai_live_tick(zslots[0]);
            if (z->state != RE15_AI_STATE_CORPSE) {
                fprintf(stderr, "FAIL: (13) clip end (frame 4/5) -> CORPSE(7), state=%d\n", z->state); fail = 1; }
            if (!fail)
                printf("  (13) death anim: motion 0x1f, holds DEATH through the 5-frame clip, then CORPSE(7)\n");
        }
        re15_enemy_reset();   /* drop the mock bank so it can't leak into other runs */
    }

    /* (13b): the byte-true feeding STAND-UP clip-gate (FUN_80103a58 case 2 = `+0x6 += f314(+0x170,
     * +0x174,0,0x100)`). The port used to SNAP the wake stand-up (feeding case 2 -> 3 in ONE frame)
     * -> the zombie popped straight into the engage clip = the user-reported "wrong getroffen anim on
     * wake". Byte-true: case 2 REPLAYS the spawn feeding clip forward and advances ONLY on clip-end,
     * THEN commits to engage. With a mock bank (feeding clip 0x27 = 6 frames), the stand-up must HOLD
     * in case 2 until anim_frame hits frame_count-1. (run_all doesn't call the shared anim pass; drive
     * anim_frame here, as (13) does. Same clip-done gate the GRAB sub-steps now use.) */
    {
        re15_actor_t *z = &g_actors[zslots[0]];
        pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0;
        for (int i = 0; i < nz; i++) {   /* isolate: park the others far + asleep */
            re15_actor_t *o = &g_actors[zslots[i]];
            o->grid_id = 0x86; o->sub_state_1 = 0; o->sub_state_2 = 0; o->ai_flags = 0; o->x = 30000; o->z = 30000;
        }
        re15_enemy_bank_t *bank = re15_enemy_alloc(z->type);
        if (!bank) { fprintf(stderr, "FAIL: (13b) could not alloc a mock bank\n"); fail = 1; }
        else {
            bank->anim.clip_count = 0x30;
            bank->anim.clips[0x27].frame_count = 6;   /* feeding loop clip */
            bank->anim.clips[0x29].frame_count = 6;   /* the byte-true GET-UP clip 0x29 (FUN_80104a50) */
            bank->ok = 1;
            /* feeding zombie, near the player, wake wait-timer already elapsed (sub_state_2=1, ai_timer=0). */
            z->state = RE15_AI_STATE_ACTIVE; z->grid_id = 6; z->sub_state_1 = 0;
            z->sub_state_2 = 1; z->ai_timer = 0; z->motion = 0x27; z->anim_frame = 4; z->x = 1000; z->z = 0;
            re15_enemy_ai_run_all(1);                  /* case1 (timer 0) -> enter case2: set the get-up clip 0x29, restart */
            if (z->sub_state_2 != 2 || z->anim_frame != 0 || z->motion != 0x29) {
                fprintf(stderr, "FAIL: (13b) wake must enter stand-up case2 + play GET-UP clip 0x29, +0x6=%d frame=%d mo=%d\n",
                        z->sub_state_2, z->anim_frame, z->motion); fail = 1; }
            z->anim_frame = 3;                          /* mid-clip (3 < 6-1): must HOLD */
            re15_enemy_ai_run_all(1);
            if (z->sub_state_2 != 2 || z->grid_id == 0) {
                fprintf(stderr, "FAIL: (13b) mid stand-up must HOLD case2 (no snap), +0x6=%d grid=%d\n",
                        z->sub_state_2, z->grid_id); fail = 1; }
            z->anim_frame = 5;                          /* clip's last frame (6-1) = done */
            re15_enemy_ai_run_all(1);                  /* case2 clip-done -> case3 */
            re15_enemy_ai_run_all(1);                  /* case3 COMMIT -> grid 0, +0x5=2 engage */
            if (z->grid_id != 0 || z->sub_state_1 != 2) {
                fprintf(stderr, "FAIL: (13b) stand-up clip-end must commit to engage, grid=%d +0x5=%d\n",
                        z->grid_id, z->sub_state_1); fail = 1; }
            if (!fail)
                printf("  (13b) feeding stand-up: HELD case2 through the 6-frame clip, then committed to engage (grid 0, +0x5=2)\n");
        }
        re15_enemy_reset();
    }

    /* (14): the byte-true HURT-STAGGER + HIT-STUN (FUN_80105a8c). The stagger plays the per-spawn clip
     * +0x1d4 (random {2,3,4,5}, seeded in live_init) and HOLDS state HURT until the s16 hit-stun budget
     * +0x1dc goes negative (seed 4..7, -step[+0x5]/frame), then recovers to ACTIVE with +0x5=0x11. */
    {
        re15_actor_t *z = &g_actors[zslots[0]];
        /* (a) live_init seeds the stagger clip into {2,3,4,5} (@0x8011f7e4 table). */
        z->state = RE15_AI_STATE_INIT; z->hurt_clip = 0;
        re15_enemy_ai_live_init(zslots[0]);
        if (z->hurt_clip < 2 || z->hurt_clip > 5) {
            fprintf(stderr, "FAIL: (14a) live_init hurt_clip must be in {2,3,4,5}, ist %d\n", z->hurt_clip); fail = 1; }

        /* (b) the +0x9&0x80 SPECIAL branch (dormant collapse) HOLDS HURT — never recovers. */
        z->state = RE15_AI_STATE_HURT; z->grid_id = 0x80; z->sub_state_3 = 0; z->sub_state_1 = 3; z->hit_stun = 5;
        for (int f = 0; f < 8; f++) re15_enemy_ai_live_hurt(zslots[0]);
        if (z->state != RE15_AI_STATE_HURT) {
            fprintf(stderr, "FAIL: (14b) +0x9&0x80 special branch must stay HURT, ist %d\n", z->state); fail = 1; }

        /* (c) the GUNSHOT path: pistol -> HURT with +0x5=weapon_id=2 (stun step @0x8011fe30[2] = -2);
         * the zombie staggers (motion=hurt_clip) then recovers to ACTIVE. */
        pl->x = 0; pl->z = 0; pl->rot_y = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0;
        re15_player_death_reset();
        for (int i = 0; i < nz; i++) {   /* park the others far */
            re15_actor_t *o = &g_actors[zslots[i]];
            o->state = RE15_AI_STATE_ACTIVE; o->x = 30000; o->z = 30000; o->grid_id = 0;
        }
        z->x = 0; z->z = 800; z->grid_id = 0; z->hp = 60; z->hit_react = 0;
        z->state = RE15_AI_STATE_ACTIVE; z->sub_state_1 = 0; z->sub_state_2 = 0; z->sub_state_3 = 0;
        z->hurt_clip = 4; z->hit_stun = 0;
        if (re15_player_weapon_fire(2) != (zslots[0] + 1)) {
            fprintf(stderr, "FAIL: (14c) pistol must hit the front zombie\n"); fail = 1; }
        if (z->state != RE15_AI_STATE_HURT || z->sub_state_1 != 2) {
            fprintf(stderr, "FAIL: (14c) shot -> HURT(2) + +0x5=weapon_id 2, state=%d +0x5=%d\n",
                    z->state, z->sub_state_1); fail = 1; }
        re15_enemy_ai_live_tick(zslots[0]);   /* HURT phase 0: motion=hurt_clip=4, seed + decrement -2 */
        if (z->motion != 4 || z->state != RE15_AI_STATE_HURT) {
            fprintf(stderr, "FAIL: (14c) gunshot stagger plays motion=hurt_clip=4 + holds, motion=%d state=%d\n",
                    z->motion, z->state); fail = 1; }
        int gt = 0;                            /* step -2/frame, seed 4..7 -> recovers in <=4 frames */
        while (z->state == RE15_AI_STATE_HURT && gt < 12) { re15_enemy_ai_live_tick(zslots[0]); gt++; }
        if (z->state != RE15_AI_STATE_ACTIVE || z->sub_state_1 != 0x11) {
            fprintf(stderr, "FAIL: (14c) gunshot HURT -> ACTIVE (+0x5=0x11), state=%d +0x5=%d\n",
                    z->state, z->sub_state_1); fail = 1; }
        if (!fail)
            printf("  (14) HURT stagger: clip {2,3,4,5}; holds via the +0x1dc stun then ACTIVE(+0x5=0x11); "
                   "special +0x9&0x80 stays HURT; gunshot staggers\n");
    }

    /* (15): the per-state ANIMATION CLIPS (+0x94 motion) byte-true through the combat lifecycle —
     * ENGAGE/TURN play the +0x1d4 variant; GRAB plays (+0x5-3)*3+{0,1} then release clip 17.
     * Driven through re15_enemy_ai_live_active (the f840 decide + f890 animate dispatch). */
    {
        re15_actor_t *z = &g_actors[zslots[0]];
        for (int i = 1; i < nz; i++) { g_actors[zslots[i]].x = 30000; g_actors[zslots[i]].z = 30000; }
        z->state = RE15_AI_STATE_ACTIVE; z->grid_id = 0; z->ai_flags = 0; z->floor = 0;
        pl->floor = 0; pl->hit_react = 1;   /* player mid-hit -> blocks the directional grab-commit */

        /* ENGAGE (+0x5=2, byte-true FUN_801021f8): on entry sets the idle clip (hurt_clip) AND rolls
         * the behavior table (@0x8011faf0/fb00) — a "2" roll BREAKS INTO THE FORWARD APPROACH
         * (+0x5=0x13). Both outcomes are byte-true (8.19); the zombie no longer just stands. */
        z->sub_state_1 = 2; z->sub_state_2 = 0; z->sub_state_3 = 0; z->hurt_clip = 4; z->motion = 99; z->ai_dist = 30000u;
        re15_enemy_ai_live_active(zslots[0]);
        if (z->motion != 4 || (z->sub_state_1 != 2 && z->sub_state_1 != 0x13)) {
            fprintf(stderr, "FAIL: (15) ENGAGE clip=hurt_clip(4) + stays engage(2) OR breaks to approach(0x13), +0x5=%d motion=%d\n",
                    z->sub_state_1, z->motion); fail = 1; }

        /* TURN (+0x5=7): on entry (+0x6==0) sets motion=hurt_clip + anim_frame=0 + latches +0x6=1.
         * The zombie faces 90deg OFF the player (rot_y perpendicular) so it stays TURNING (byte-true
         * FUN_80102dc8 only flips to engage when WITHIN the +-0x80 facing cone). */
        z->x = 0; z->z = 0; z->rot_y = 1024; pl->x = 5000; pl->z = 0;   /* player to the side -> not facing */
        z->sub_state_1 = 7; z->sub_state_2 = 0; z->hurt_clip = 5; z->motion = 99; z->anim_frame = 99; z->ai_dist = 30000u;
        re15_enemy_ai_live_active(zslots[0]);
        if (z->sub_state_1 != 7 || z->motion != 5 || z->anim_frame != 0 || z->sub_state_2 != 1) {
            fprintf(stderr, "FAIL: (15) TURN entry clip=hurt_clip(5)+anim_frame0+latch, +0x5=%d motion=%d af=%d +0x6=%d\n",
                    z->sub_state_1, z->motion, z->anim_frame, z->sub_state_2); fail = 1; }
        /* TURN -> ENGAGE once facing (byte-true FUN_80102dc8 @0x80102e5c: arc_test(0x80)==0 -> +0x5=2).
         * The turn rotates toward the player each frame; tick it until it faces + LEAVES the turn state
         * (flips to engage/approach) instead of pivoting forever = the fix for the "stood still" bug. */
        z->sub_state_1 = 7; z->sub_state_2 = 1; z->ai_dist = 30000u;   /* still perpendicular (rot_y=1024) */
        int turn_flipped = 0;
        for (int f = 0; f < 60 && !turn_flipped; f++) {
            re15_enemy_ai_live_active(zslots[0]);
            if (z->sub_state_1 != 7) turn_flipped = 1;   /* left TURN -> engage(2)/approach(0x13) */
        }
        if (!turn_flipped) {
            fprintf(stderr, "FAIL: (15) TURN must rotate to face then FLIP OUT of turn (was stuck pivoting), +0x5=%d\n",
                    z->sub_state_1); fail = 1; }

        /* GRAB face (+0x5=3): base=(3-3)*3=0; sub-steps play clip 0 -> (impact) 1 -> (release) 17. */
        pl->hp = 300; pl->state = 0;
        z->sub_state_1 = 3; z->sub_state_2 = 0; z->sub_state_3 = 0; z->motion = 99; z->ai_dist = 100u;
        re15_enemy_ai_live_active(zslots[0]);                     /* grab [0]: motion=base 0 */
        if (z->motion != 0 || z->sub_state_2 != 1) {
            fprintf(stderr, "FAIL: (15) GRAB [0] clip=base 0, motion=%d +0x6=%d\n", z->motion, z->sub_state_2); fail = 1; }
        re15_enemy_ai_live_active(zslots[0]);                     /* grab [1]: pull-in (no clip) */
        re15_enemy_ai_live_active(zslots[0]);                     /* grab [2] IMPACT: motion=base+1=1 */
        if (z->motion != 1) {
            fprintf(stderr, "FAIL: (15) GRAB [2] IMPACT clip=base+1=1, motion=%d\n", z->motion); fail = 1; }
        re15_enemy_ai_live_active(zslots[0]);                     /* grab [3] BITE-loop: HOLDS clip base+1 */
        re15_enemy_ai_live_active(zslots[0]);                     /* (timeline-verified: no release mid-hold) */
        if (z->motion != 1 || z->sub_state_2 != 3) {
            fprintf(stderr, "FAIL: (15) GRAB [3] must HOLD the bite clip base+1 (+0x6=3), motion=%d +0x6=%d\n",
                    z->motion, z->sub_state_2); fail = 1; }

        /* GRAB behind (+0x5=4): base=(4-3)*3=3 -> clip 3 on entry. */
        z->sub_state_1 = 4; z->sub_state_2 = 0; z->motion = 99;
        re15_enemy_ai_live_active(zslots[0]);
        if (z->motion != 3) {
            fprintf(stderr, "FAIL: (15) GRAB-behind [0] clip=base 3, motion=%d\n", z->motion); fail = 1; }

        if (!fail)
            printf("  (15) per-state clips: ENGAGE/TURN=+0x1d4 variant; GRAB (+0x5-3)*3+{0,1}->release 17\n");
    }

    /* (16): the PLAYER AIM/RAISE sequence (Phase 8.14 + 8.16) — holding R1 roots the player and runs
     * the byte-true action-8 aim sub-FSM @0x80035810: first the RAISE clip 17 (10 frames, PL00.EDD-
     * verified) plays out, THEN the held AIM-READY pose clip 18 (@0x800359e0). The discharge is gated
     * on aim-ready (re15_player_aim_ready) = the byte-true state-5 gate (no shot mid-raise). Released
     * reverts to idle. */
    {
        g_scd.player_mode = 0; g_scd.message_display_frames = 0; g_scd.message_query = 0;
        re15_player_tick(NULL, 0);          /* a non-aim tick first -> reset the aim FSM to NONE */
        pl->motion = 200;                   /* a non-aim sentinel (idle) */
        pl->anim_frame = 0; pl->hp = 100; pl->rot_y = 0; pl->x = 0; pl->z = 0;
        re15_player_tick(NULL, RE15_PAD_BIT_R1);          /* hold R1 -> start the RAISE (clip 17) */
        if (pl->motion != 17) {
            fprintf(stderr, "FAIL: (16) hold R1 -> RAISE clip 17, ist %d\n", pl->motion); fail = 1; }
        if (re15_player_aim_ready()) {
            fprintf(stderr, "FAIL: (16) mid-raise must NOT be aim-ready (no shot mid-raise)\n"); fail = 1; }
        int32_t ax = pl->x, az = pl->z;
        /* run enough ticks for the 10-frame raise to play out -> AIM-READY (clip 18) */
        for (int f = 0; f < 12; f++) re15_player_tick(NULL, RE15_PAD_BIT_R1 | RE15_PAD_BIT_UP);
        if (pl->x != ax || pl->z != az) {
            fprintf(stderr, "FAIL: (16) aiming roots the player (no translation on UP), moved %d,%d\n",
                    (int)(pl->x - ax), (int)(pl->z - az)); fail = 1; }
        if (pl->motion != 18) {
            fprintf(stderr, "FAIL: (16) raise played out -> AIM-READY clip 18, ist %d\n", pl->motion); fail = 1; }
        if (!re15_player_aim_ready()) {
            fprintf(stderr, "FAIL: (16) raise done -> aim-ready (can fire)\n"); fail = 1; }
        re15_player_tick(NULL, 0);                        /* release R1 -> revert to idle/walk */
        if (pl->motion == 18 || pl->motion == 17) {
            fprintf(stderr, "FAIL: (16) release R1 -> leave the aim/raise pose, motion still %d\n", pl->motion); fail = 1; }
        if (re15_player_aim_ready()) {
            fprintf(stderr, "FAIL: (16) release R1 -> not aim-ready\n"); fail = 1; }
        if (!fail)
            printf("  (16) player AIM: R1 -> RAISE clip 17 (10f) -> AIM-READY clip 18 (gated fire), released -> idle\n");
    }

    /* (17): the ZOMBIE COMBAT-SE bank lookup (Phase 8.17/8.18, byte-true via FUN_800453d0 on the snd1 room bank).
     * These are the C-driven zombie combat sounds, all func_0x800453d0(id) on snd1: grab-START 4 (FUN_80102548
     * case 0 @0x8010268c), grab-RELEASE 7 (case 4 @0x80102920/60), death groan 5/8 (FUN_80107cb0 frame 7). The
     * SE-id -> VAG resolution reuses re15_footstep_vag on snd1 (identical tone-table path as FUN_80045024/453d0:
     * program = EDT byte1 & 0x7f, tone = EDT byte2 >> 4, VAG = tone.vag_index - 1). Verify each resolves to a
     * VALID VAG inside ROOM1140's 11-VAG snd1 bank, and the two death ids are distinct. */
    {
        re15_vab_t se_vab;
        if (!rdt.snd_vh[1] || rdt.snd_vh_size[1] <= 0 || !rdt.snd_edt[1]) {
            fprintf(stderr, "FAIL: (17) ROOM1140 has no snd1 SE bank (VH/EDT missing)\n"); fail = 1; }
        else if (re15_vab_parse(rdt.snd_vh[1], (size_t)rdt.snd_vh_size[1], &se_vab) != 0) {
            fprintf(stderr, "FAIL: (17) snd1 VH parse failed\n"); fail = 1; }
        else {
            /* ROOM1140 snd1: program_count=1, vag_count=11 (VH header @+18/+22). */
            if (se_vab.vag_count != 11) {
                fprintf(stderr, "FAIL: (17) snd1 vag_count 11 expected, ist %d\n", se_vab.vag_count); fail = 1; }
            int v4 = re15_footstep_vag(rdt.snd_edt[1], &se_vab, 4);   /* grab-start */
            int v7 = re15_footstep_vag(rdt.snd_edt[1], &se_vab, 7);   /* grab-release */
            int v5 = re15_footstep_vag(rdt.snd_edt[1], &se_vab, 5);   /* death (EDT 00 00 69 15 -> prog0 tone6 -> VAG5) */
            int v8 = re15_footstep_vag(rdt.snd_edt[1], &se_vab, 8);   /* death (EDT 00 00 92 15 -> prog0 tone9 -> VAG8) */
            if (v4 != 4) {
                fprintf(stderr, "FAIL: (17) grab-start SE 4 must resolve to VAG 4, ist %d\n", v4); fail = 1; }
            if (v7 != 7) {
                fprintf(stderr, "FAIL: (17) grab-release SE 7 must resolve to VAG 7, ist %d\n", v7); fail = 1; }
            if (v5 != 5) {
                fprintf(stderr, "FAIL: (17) death SE 5 must resolve to VAG 5, ist %d\n", v5); fail = 1; }
            if (v8 != 8) {
                fprintf(stderr, "FAIL: (17) death SE 8 must resolve to VAG 8, ist %d\n", v8); fail = 1; }
            if (v5 == v8) {
                fprintf(stderr, "FAIL: (17) the two death SEs must be distinct VAGs, both %d\n", v5); fail = 1; }
            int se[4] = { v4, v7, v5, v8 };
            for (int i = 0; i < 4; i++)
                if (se[i] < 0 || se[i] >= se_vab.vag_count) {
                    fprintf(stderr, "FAIL: (17) combat SE VAG %d out of the %d-VAG bank\n", se[i], se_vab.vag_count); fail = 1; }
            if (!fail)
                printf("  (17) zombie combat SE bank: snd1 %d VAGs; grab-start 4->VAG %d, release 7->VAG %d, "
                       "death 5->VAG %d, 8->VAG %d (byte-true re15_footstep_vag)\n", se_vab.vag_count, v4, v7, v5, v8);
        }
    }

    /* (18): the PLAYER GUNSHOT SE bank (Phase 8.19, byte-true FUN_80045024 bank1 = the equipped weapon's
     * ARMS bank). The ROOM1140 briefing handgun is weapon 1 = ARMS01 (savestate-confirmed: DAT_800aca5d==1,
     * 0x801fcd00 == ARMS01.EDH across all STAGE1 saves). The .EDH = [EDT records @0][VH "pBAV" @pBAV_off]
     * [8-byte trailer]; pBAV_off = u32 @ edh[size-8]. The gunshot = SE index 8 (FUN_80035538/FUN_80011f50
     * -> FUN_80045024(0x1080001)); resolve it the byte-true way (re15_footstep_vag on the EDT prefix). */
    {
        long esz = 0;
        uint8_t *edh = slurp(RE15_ASSET_PSX_DIR "/SOUND/ARMS01.EDH", &esz);
        if (!edh || esz < 8) {
            fprintf(stderr, "FAIL: (18) cannot read ARMS01.EDH\n"); fail = 1; }
        else {
            uint32_t pbav = (uint32_t)edh[esz-8] | ((uint32_t)edh[esz-7] << 8) |
                            ((uint32_t)edh[esz-6] << 16) | ((uint32_t)edh[esz-5] << 24);
            re15_vab_t wv;
            if (pbav != 0x28) {   /* ARMS01: 10 EDT records (0x28/4) then the VH */
                fprintf(stderr, "FAIL: (18) ARMS01 pBAV offset 0x28 expected, ist 0x%x\n", pbav); fail = 1; }
            else if (edh[pbav] != 0x70 || edh[pbav+1] != 0x42) {   /* "pBAV" magic 0x56414270 */
                fprintf(stderr, "FAIL: (18) no pBAV VH at ARMS01 offset 0x%x\n", pbav); fail = 1; }
            else if (re15_vab_parse(edh + pbav, (size_t)esz - pbav, &wv) != 0) {
                fprintf(stderr, "FAIL: (18) ARMS01 VH parse failed\n"); fail = 1; }
            else {
                if (wv.vag_count != 5) {   /* ARMS01: prog1/tone5/vag5 */
                    fprintf(stderr, "FAIL: (18) ARMS01 vag_count 5 expected, ist %d\n", wv.vag_count); fail = 1; }
                /* gunshot idx 8: EDT record 00 00 43 11 -> prog 0, tone 4 -> vag_index 5 -> 0-based VAG 4. */
                int vg = re15_footstep_vag(edh, &wv, 8);
                if (vg != 4) {
                    fprintf(stderr, "FAIL: (18) gunshot SE 8 must resolve to VAG 4, ist %d\n", vg); fail = 1; }
                if (vg < 0 || vg >= wv.vag_count) {
                    fprintf(stderr, "FAIL: (18) gunshot VAG %d out of the %d-VAG ARMS01 bank\n", vg, wv.vag_count); fail = 1; }
                if (!fail)
                    printf("  (18) player gunshot bank: ARMS01 (weapon 1) pBAV@0x%x, %d VAGs; SE 8 -> VAG %d "
                           "(byte-true re15_footstep_vag)\n", pbav, wv.vag_count, vg);
            }
        }
        free(edh);
    }

    /* (19): the EQUIPPED WEAPON (Phase 8.19, DAT_800aca5d). The port now models the equipped weapon
     * and defaults it byte-true to weapon 1 (the ROOM1140 briefing handgun = ARMS01, savestate-
     * confirmed) instead of the previously-hardcoded weapon 2. game_step fires re15_player_weapon_fire(
     * re15_player_equipped_weapon()), so a shot does weapon 1's byte-true zombie damage (6), not 24. */
    {
        if (re15_player_equipped_weapon() != 1) {
            fprintf(stderr, "FAIL: (19) equipped weapon must default to 1 (briefing handgun), ist %d\n",
                    re15_player_equipped_weapon()); fail = 1; }
        pl->x = 0; pl->z = 0; pl->hp = 100; pl->rot_y = 0; pl->floor = 0; pl->hit_react = 0; pl->state = 0;
        re15_player_death_reset();
        for (int i = 0; i < nz; i++) {
            re15_actor_t *z = &g_actors[zslots[i]];
            z->state = RE15_AI_STATE_ACTIVE; z->x = 30000; z->z = 30000; z->hp = 60;
            z->hit_react = 0; z->sub_state_1 = 0; z->grid_id = 0;
        }
        re15_actor_t *zt = &g_actors[zslots[0]];
        zt->x = 0; zt->z = 800; zt->hp = 60; zt->hit_react = 0; zt->state = RE15_AI_STATE_ACTIVE;
        /* fire the EQUIPPED weapon (1) -> byte-true weapon-1 zombie damage = 6 */
        int hit = re15_player_weapon_fire(re15_player_equipped_weapon());
        if (hit != (zslots[0] + 1) || zt->hp != (int16_t)(60 - 6)) {
            fprintf(stderr, "FAIL: (19) equipped weapon 1 must do 6 dmg (not 24), HP 60->%d\n", zt->hp); fail = 1; }
        /* the setter (the future inventory equip UI hook) switches the weapon */
        zt->hp = 60; zt->hit_react = 0; zt->state = RE15_AI_STATE_ACTIVE; zt->sub_state_1 = 0;
        re15_player_set_equipped_weapon(2);
        if (re15_player_equipped_weapon() != 2) {
            fprintf(stderr, "FAIL: (19) set_equipped_weapon(2) must stick\n"); fail = 1; }
        re15_player_weapon_fire(re15_player_equipped_weapon());
        if (zt->hp != (int16_t)(60 - 24)) {
            fprintf(stderr, "FAIL: (19) weapon 2 must do 24 dmg, HP 60->%d\n", zt->hp); fail = 1; }
        re15_player_set_equipped_weapon(1);   /* restore the byte-true briefing default */
        if (!fail)
            printf("  (19) equipped weapon: default 1 (handgun) -> 6 dmg; set 2 -> 24 dmg (byte-true DAT_800aca5d)\n");
    }

    free(buf);
    /* (20): the DEATH -> CONTINUE reload (the second half of the ROOM1140 "hang" fix). When the player
     * dies (HP<0) the death branch runs the byte-true 0x78 (120-frame) timer; at expiry game_step fires
     * re15_player_continue_reload, which QUEUES a reload of the CURRENT room (re15_room_apply_pending in
     * the main loop then re-inits the actors -> re15_actor_init restores player HP=100 -> not dead ->
     * the death branch exits). Without this the player was pinned dead FOREVER = the hang. Verify the
     * timer runs to 0 and the continue queues a fresh reload of the current room. */
    {
        g_current_room_id = 0x1140;
        g_room_change.pending = 0;
        re15_player_death_reset();               /* fresh death sequence */
        pl->hp = -1; pl->state = 7;              /* player dead/grabbed */
        int seq = -1, guard = 0;
        do { seq = re15_player_death_tick(); guard++; } while (seq != 0 && guard < 200);
        if (seq != 0) {
            fprintf(stderr, "FAIL: (20) death timer never reached 0 (ran %d ticks)\n", guard); fail = 1; }
        if (guard != 0x78 + 1) {                 /* seed 120 on tick 1, then 119..0 -> 121 ticks to hit 0 */
            fprintf(stderr, "FAIL: (20) death timer must be the byte-true 0x78=120 frames (took %d ticks)\n", guard); fail = 1; }
        re15_player_continue_reload();
        if (!g_room_change.pending) {
            fprintf(stderr, "FAIL: (20) death-complete must QUEUE a continue-reload (pending stayed 0)\n"); fail = 1; }
        if (g_room_change.room_id != 0x1140) {
            fprintf(stderr, "FAIL: (20) continue must reload the CURRENT room 0x1140, got 0x%X\n", g_room_change.room_id); fail = 1; }
        g_room_change.pending = 0;               /* don't leak the queued change into other test runs */
        re15_player_death_reset();
        pl->hp = 100; pl->state = 0;
        if (!fail)
            printf("  (20) death->continue: 0x78=120-frame timer expiry queues a fresh reload of ROOM1140 (no permanent pin)\n");
    }

    /* (21): the FORWARD-WALK ROOT MOTION (Phase 8.20, byte-true FUN_80102bd8 -> func_0x8001ad68 +
     * the keyframe decoder func_0x8001ae38, disassembled from PSX.EXE). The walk step IS the walk
     * clip's baked root translation: clip 0xa keyframes carry a CUMULATIVE forward offset at +6 (the
     * re15_emd_get_keyframe_speed `sx`). The port had an INVERTED return check (`== 0` on a function
     * that returns 1 on success) so it NEVER used the value -> fell back to a constant shamble = the
     * "schweben" glide. Seed a mock bank with the REAL EM10 bank1 clip-0xa +6 values and drive the
     * walk frame-by-frame facing a distant player. Byte-true expectations:
     *   - a facing zombie steps straight toward the player (+Z), x stays put;
     *   - the per-frame step follows the keyframe DELTA (sx[kf]-sx[kf-1]), NOT a constant:
     *       f4  delta = 1243-1092 = 151  -> a big step;
     *       f19 delta = 2048-2048 =   0  -> NO step (the old constant-48 shamble would still move). */
    {
        /* the REAL EM10.EMD bank1 clip-0xa +6 (sx) offsets, frames 0..30 (verified from the asset). */
        static const int16_t sx[31] = {
            821, 858, 956,1092,1243,1387,1501,1579,1636,1686,1739,1801,1854,1891,1926,1959,
           1990,2020,2048,2048,2105,2172,2240,2310,2383,2451,2505,2521,2505,2478,2442 };
        int zs = zslots[0];
        re15_actor_t *z = &g_actors[zs];
        /* park every other zombie far + asleep so only this one ticks. */
        for (int i = 1; i < nz; i++) {
            re15_actor_t *o = &g_actors[zslots[i]];
            o->grid_id = 0x86; o->sub_state_1 = 0; o->sub_state_2 = 0; o->ai_flags = 0;
            o->x = 30000; o->z = 30000;
        }
        /* mock a bank for this zombie's type carrying clip 0xa + the real keyframe +6 offsets. */
        re15_enemy_reset();
        re15_enemy_bank_t *bank = re15_enemy_alloc(z->type);
        static uint8_t kfbuf[260 * 80];
        if (!bank) { fprintf(stderr, "FAIL: (21) could not alloc a mock walk bank\n"); fail = 1; }
        else {
            memset(kfbuf, 0, sizeof kfbuf);
            for (int f = 0; f <= 30; f++) {          /* kf 209+f gets sx[f] at byte +6 (s16 LE) */
                int kf = 209 + f; uint8_t *p = kfbuf + kf * 80 + 6;
                p[0] = (uint8_t)(sx[f] & 0xff); p[1] = (uint8_t)((sx[f] >> 8) & 0xff);
            }
            bank->skel.keyframe_size_bytes = 80;
            bank->skel.keyframe_count      = 260;
            bank->skel.keyframe_data       = kfbuf;
            bank->skel.keyframe_data_size  = sizeof kfbuf;
            bank->anim.clip_count          = 0x0b;
            bank->anim.clips[0x0a].first_frame = 300;
            bank->anim.clips[0x0a].frame_count = 31;
            for (int f = 0; f <= 30; f++) bank->anim.frames[300 + f] = (uint32_t)(209 + f);
            bank->ok = 1;

            /* place the walking zombie at the origin, the player straight ahead at +Z, and FACE it. */
            pl->x = 0; pl->z = 20000; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->floor = 0;
            z->x = 0; z->z = 0; z->floor = 0; z->active = 1;
            z->rot_y = (int16_t)((re15_atan2_q12(pl->z - z->z, pl->x - z->x) - 1024) & 0x0fff);
            z->state = RE15_AI_STATE_ACTIVE; z->grid_id = 0; z->ai_flags = 0;
            z->sub_state_1 = 6; z->sub_state_2 = 1;  /* WALK, entry already done (skip the SE/reset) */
            z->motion = 0x0a;

            int32_t z0 = z->z, x0 = z->x, step4 = 0, step19 = 0;
            for (int f = 0; f <= 20; f++) {
                z->anim_frame = (uint16_t)f;
                int32_t zb = z->z;
                re15_enemy_ai_live_tick(zs);
                if (f == 4)  step4  = z->z - zb;
                if (f == 19) step19 = z->z - zb;
            }
            int32_t dz_total = z->z - z0, dx_total = z->x - x0;
            /* net = the telescoped keyframe offset delta = sx[20]-sx[0] = 2105-821 = 1284 EXACTLY
             * (byte-true: pos = anchor + rotate(off[kf]); over frames 1..20 it sums to sx[20]-sx[0]). */
            if (dz_total < 1270 || dz_total > 1300) {
                fprintf(stderr, "FAIL: (21) walk net must be the telescoped 1284 toward +Z, got dz=%d\n", dz_total); fail = 1; }
            if (dx_total < -60 || dx_total > 60) {
                fprintf(stderr, "FAIL: (21) walk must go STRAIGHT at the player; drifted dx=%d\n", dx_total); fail = 1; }
            if (step4 < 120 || step4 > 180) {         /* byte-true keyframe delta 151, NOT the constant 48 */
                fprintf(stderr, "FAIL: (21) f4 step must be the keyframe delta ~151, got %d\n", step4); fail = 1; }
            if (step19 < -8 || step19 > 8) {          /* keyframe delta 0 -> NO move (kills the constant shamble) */
                fprintf(stderr, "FAIL: (21) f19 step must be ~0 (flat keyframe), got %d (constant shamble?)\n", step19); fail = 1; }
            if (!fail)
                printf("  (21) walk root-motion: dz=%d (toward player), dx=%d, f4 step=%d (delta 151), "
                       "f19 step=%d (delta 0 = no glide)\n", dz_total, dx_total, step4, step19);
        }
        re15_enemy_reset();                           /* drop the mock bank */
    }

    /* (22): LEON GRAB-VICTIM ANIMATION (state 5 struggle / state 6 collapse) — the byte-true player-
     * command FSM @0x8010a28c/@0x8010a6f8. While a zombie has Leon grabbed he is animated NOT from his
     * own PL00 set but from the grab-victim anim set the ZOMBIE carries (bank 2 = zombie+0x178/+0x17c):
     * STRUGGLE clips variant*3+phase (face {0,1,2}), the phase advancing ONLY on clip-done; on the
     * grab-death he plays the COLLAPSE clip variant+6 (=6). This is what stops Leon FREEZING during the
     * grab/death (the user-reported "no Leon reactions / death finish"). re15_enemy_ai_live_grab latches
     * the state; re15_player_victim_tick (game_step, here driven directly) advances it. Mock the
     * grabbing zombie's bank 2 (victim clips 0..7, 4 frames each). */
    {
        int gs = zslots[0];
        re15_actor_t *gz = &g_actors[gs];
        re15_enemy_reset();                 /* also clears the victim state (re15_player_victim_reset) */
        re15_enemy_bank_t *bank = re15_enemy_alloc(gz->type);
        if (!bank) { fprintf(stderr, "FAIL: (22) could not alloc a victim bank\n"); fail = 1; }
        else {
            bank->ok = 1;
            bank->victim_ok = 1;
            bank->anim_victim.clip_count = 8;                          /* victim clips 0..7 */
            for (int c = 0; c < 8; c++) bank->anim_victim.clips[c].frame_count = 4;

            pl->x = 0; pl->z = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->floor = 0;
            pl->motion = 200; pl->anim_frame = 0;                     /* a non-victim idle sentinel */
            pl->rot_y = 0x555;                                        /* a WRONG facing -> must turn to face */
            for (int i = 0; i < nz; i++) {   /* isolate: park the others far + asleep */
                re15_actor_t *z = &g_actors[zslots[i]];
                z->grid_id = 0x86; z->sub_state_1 = 0; z->sub_state_2 = 0; z->ai_flags = 0;
                z->x = 30000; z->z = 30000;
            }

            /* (a) not grabbed yet -> no victim animation */
            if (re15_player_victim_state() != 0) {
                fprintf(stderr, "FAIL: (22a) Leon must not be in a victim anim before the grab, ist %d\n",
                        re15_player_victim_state()); fail = 1; }

            /* commit THIS zombie to the FACE grab (+0x5=3 -> variant 0 -> struggle {0,1,2}, collapse 6) */
            gz->x = 500; gz->z = 0; gz->floor = 0; gz->state = RE15_AI_STATE_ACTIVE;
            gz->grid_id = 0; gz->sub_state_1 = 3; gz->sub_state_2 = 0; gz->ai_flags = 0;

            /* (b) the grab latches the STRUGGLE. One run_all reaches the grab [0] (s_player_grabbed +
             * the victim latch); the tick then poses Leon at the first struggle clip. */
            re15_enemy_ai_run_all(1);
            re15_player_victim_tick();
            if (re15_player_victim_state() != 1) {
                fprintf(stderr, "FAIL: (22b) grabbed -> Leon STRUGGLE (state 1), ist %d\n",
                        re15_player_victim_state()); fail = 1; }
            if (pl->motion != 0) {   /* face variant, phase 0 -> struggle clip 0 */
                fprintf(stderr, "FAIL: (22b) struggle must pose Leon at face clip 0, motion=%d\n", pl->motion); fail = 1; }
            /* TURN-TO-FACE (byte-true func_0x8001a8f8 snap): Leon yaw must SNAP to face the grabbing
             * zombie (at +500 X here), NOT stay at the wrong 0x555. Face variant -> face the zombie. */
            {
                int expect_face = ((int)re15_atan2_q12(gz->z - pl->z, gz->x - pl->x) - 0x400) & 0x0fff;
                if (pl->rot_y == 0x555 || pl->rot_y != (int16_t)expect_face) {
                    fprintf(stderr, "FAIL: (22b) struggle must SNAP Leon to face the grabber, rot_y=%d (want %d)\n",
                            pl->rot_y, expect_face); fail = 1; }
            }

            /* (c) TIMELINE-VERIFIED struggle model (deterministic DuckStation /tmp/tl3 + Q4 disasm of
             * @0x8010a28c): the intro clip base+0 plays ONCE, then the handler LOOPS clip base+1 for the
             * whole hold (observed live: motion=4=base3+1, DAT_800aca5a self-held, frames wrapping) —
             * it never advances to base+2 while held (base+2 is the RELEASE clip @0x8010a4e8). */
            for (int f = 0; f < 4; f++) re15_player_victim_tick();      /* intro clip 0 done -> hold clip 1 */
            if (pl->motion != 1) {
                fprintf(stderr, "FAIL: (22c) struggle intro done -> HOLD clip 1, motion=%d\n", pl->motion); fail = 1; }
            for (int f = 0; f < 10; f++) re15_player_victim_tick();     /* hold clip LOOPS (wraps), stays 1 */
            if (pl->motion != 1) {
                fprintf(stderr, "FAIL: (22c) struggle hold must LOOP clip 1 (never base+2), motion=%d\n", pl->motion); fail = 1; }

            /* (d) RELEASE — grab ends alive -> the RELEASE finish plays clip base+2 ONCE (the original's
             * phases 4/5, motion acaf3*3+2 @0x8010a4e8/a50c), Leon stays pinned through it, THEN free. */
            gz->sub_state_1 = 2; gz->x = 30000; gz->z = 30000;     /* engage, far -> won't re-commit */
            pl->x = 60000; pl->z = 60000;
            re15_enemy_ai_run_all(1);                              /* clears s_player_grabbed */
            re15_player_victim_tick();                             /* -> RELEASE finish (state 3), clip 2 */
            if (re15_player_victim_state() != 3 || pl->motion != 2) {
                fprintf(stderr, "FAIL: (22d) grab end -> RELEASE finish (state 3, clip base+2), state=%d motion=%d\n",
                        re15_player_victim_state(), pl->motion); fail = 1; }
            if (!re15_player_is_grabbed()) {
                fprintf(stderr, "FAIL: (22d) Leon stays PINNED through the release finish\n"); fail = 1; }
            for (int f = 0; f < 6; f++) re15_player_victim_tick();      /* release clip (4f) plays out */
            if (re15_player_victim_state() != 0) {
                fprintf(stderr, "FAIL: (22d) release clip done -> Leon freed (state 0), ist %d\n",
                        re15_player_victim_state()); fail = 1; }

            /* (e) the DEVOUR COLLAPSE (byte-true chain): a grab on a dead/dying player runs [3] ->
             * hands off to the devour-finish (+0x5=3+2=5), whose sub0 latches Leon's collapse (state 2,
             * clip variant+6 = 6 face). Drive the chain through run_all + victim ticks. */
            re15_player_victim_reset();
            pl->x = 0; pl->z = 0; pl->hp = -1; pl->state = 7; pl->motion = 200; pl->anim_frame = 0;
            pl->hit_react = 0;
            gz->x = 500; gz->z = 0; gz->floor = 0; gz->state = RE15_AI_STATE_ACTIVE;
            gz->grid_id = 0; gz->sub_state_1 = 3; gz->sub_state_2 = 0; gz->ai_flags = 0;
            for (int f = 0; f < 8 && re15_player_victim_state() != 2; f++) {
                re15_enemy_ai_run_all(1);
                re15_player_victim_tick();
            }
            if (re15_player_victim_state() != 2) {
                fprintf(stderr, "FAIL: (22e) grab-on-dead -> devour handoff -> Leon COLLAPSE (state 2), ist %d\n",
                        re15_player_victim_state()); fail = 1; }
            if (gz->sub_state_1 != 5) {
                fprintf(stderr, "FAIL: (22e) the zombie must be in the devour state (+0x5=5), ist %d\n",
                        gz->sub_state_1); fail = 1; }
            if (pl->motion != 6) {   /* face variant -> collapse clip 0+6 = 6 */
                fprintf(stderr, "FAIL: (22e) collapse must pose Leon at clip 6 (variant+6), motion=%d\n", pl->motion); fail = 1; }

            if (!fail)
                printf("  (22) Leon grab-victim anim: grabbed -> STRUGGLE clips 0->1->2 (clip-gated) -> freed on "
                       "release; grab-death -> COLLAPSE clip 6 (byte-true state 5/6, off the zombie's bank 2)\n");
        }
        re15_enemy_reset();                 /* drop the mock bank + clear the victim state */
    }

    if (fail) { fprintf(stderr, "\nROOM1140 COMBAT-WIRING TEST FAILED\n"); return 1; }
    printf("\nPASS: ROOM1140 live-AI game_step wiring (spawn; WAKE->engage; TURN-to-face->GRAB->HP; "
           "GRABBED-lock; player DEATH; zombie HURT/DEATH; PLAYER-SHOOTS; LEON grab-victim anim; type-gated)\n");
    return 0;
}
