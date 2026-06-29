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

    free(buf);
    if (fail) { fprintf(stderr, "\nROOM1140 COMBAT-WIRING TEST FAILED\n"); return 1; }
    printf("\nPASS: ROOM1140 live-AI game_step wiring (spawn->tick->[wake deferred]; armed lunge->HP; type-gated)\n");
    return 0;
}
