/* ============================================================================
 *  NPC Plc_motion pose — BYTE-TRUE state-4 executor motion-FSM (regression guard).
 *
 *  Bug (Irons/Sherry ROOM1150 + other rooms): a cutscene NPC posed by SCD Plc_motion
 *  LOOPED the wrong animation. Byte-true original mechanism (disasm + savestate +
 *  audit wf_29b40e5d) — NOT a freeze:
 *    - Plc_motion opcode @0x80041b90 sets +0x4=4 (STATE=executor), +0x5=pc[1] (sub),
 *      +0x6=0 (phase), +0x94=clip, +0x1c4=flags.
 *    - Sce_em_set @0x8004216c seeds +0x1c4 = 0 (LOOP clear) for every spawn.
 *    - The NPC root dispatches state 4 EVERY frame -> executor 0x80050be8 -> sub-VM
 *      table @0x80076ca0[0]=0x80050cb8 phase-FSM (= re15_npc_sub_motion): plays the
 *      clip, and at clip-end latches phase +0x6=2 = HOLD unless LOOP +0x1c4 & 0x04.
 *    - anim_set is the SOLE frame stepper for a state-4 actor (re15_actors_anim_advance
 *      skips executor NPCs) -> no double-advance.
 *
 *  This test drives that path with NO SDL / NO assets, reproducible in any session. Asserts:
 *   (A) Plc_motion pose (state=4, sub=0, LOOP clear), self-advancing type 0x40: motion held,
 *       state stays 4, anim_frame advances then HOLDS at last frame (clip_len-1), never wraps.
 *   (B) same for a NON-self-advancing type 0x45 -> proves the double-advance skip (else the
 *       generic advancer + the executor would step it twice and it would wrap/loop).
 *   (C) LOOP flag SET -> anim_frame WRAPS (a continuous clip still loops).
 *   (D) control: a fresh (non-posed) NPC still runs INIT normally (idle clip 2, hp=-1) ->
 *       the ROOM11B0 T-pose fix (2c8d9a69) is preserved.
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_player.h"   /* re15_actors_anim_advance */

/* Fixture-Clip 5 = 30 Frames aus der BANK-1-Tabelle (EM040 dir[3]-EDD @CDEMD0.EMS 0x2d5090).
 * KORREKTUR 2026-08-03 (marvin_spawn_anim.md F1/F3): der Sub-0-Kanal +0x180/+0x184 wird vom
 * Spawn identisch mit +0x170/+0x174 initialisiert = die EIGENE BANK 1 (dir[4]/dir[3],
 * FUN_80022300 @0x800224b8/c8) — NICHT die Container-largest-Bank dir[1]. Der alte Fixture-
 * Clip 3 hat in Bank 1 nur 1 Frame (haelt sofort); Clip 5 traegt eine echte Laenge. */
#define POSE_CLIP     5
#define POSE_CLIP_LEN 30

/* Seed an actor into the exact state op_plc_motion leaves it (state 4, sub 0, phase 0, LOOP clear). */
static void seed_pose(re15_actor_t *a, uint8_t type)
{
    a->active = 1; a->type = type; a->hp = -1;   /* hp=-1 = post-INIT invulnerable */
    a->state = 4; a->sub_state_1 = 0; a->sub_state_2 = 0;
    a->motion = POSE_CLIP; a->anim_frame = 0; a->anim_flags = 0;
}

static int run_pose(re15_actor_t *a, const char *tag)
{
    int fail = 0, advanced = 0, wrapped = 0;
    uint8_t last = a->anim_frame;
    for (int f = 0; f < 240; f++) {
        re15_actors_anim_advance();
        re15_enemy_ai_run_all(0);
        if (a->anim_frame > last) advanced = 1;
        if (a->anim_frame < last) wrapped  = 1;
        last = a->anim_frame;
    }
    if (a->motion != POSE_CLIP)             { fprintf(stderr, "FAIL(%s): pose clip stomped — motion %d != %d\n", tag, a->motion, POSE_CLIP); fail = 1; }
    if (a->state != 4)                      { fprintf(stderr, "FAIL(%s): left the executor — state %d != 4\n", tag, a->state); fail = 1; }
    if (!advanced)                          { fprintf(stderr, "FAIL(%s): pose never advanced (frozen)\n", tag); fail = 1; }
    if (wrapped)                            { fprintf(stderr, "FAIL(%s): anim_frame wrapped — LOOP/double-advance regression\n", tag); fail = 1; }
    if (a->anim_frame != POSE_CLIP_LEN - 1) { fprintf(stderr, "FAIL(%s): not held at last frame — %d != %d\n", tag, a->anim_frame, POSE_CLIP_LEN-1); fail = 1; }
    if (a->hp != -1)                        { fprintf(stderr, "FAIL(%s): must stay invulnerable, hp=%d\n", tag, a->hp); fail = 1; }
    printf("  (%s) motion=%d state=%d anim_frame=%d (held at %d) wrapped=%d hp=%d\n",
           tag, a->motion, a->state, a->anim_frame, POSE_CLIP_LEN-1, wrapped, a->hp);
    return fail;
}

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 3000; pl->hp = 100;

    printf("=== NPC Plc_motion pose — byte-true state-4 motion-FSM (wf_29b40e5d) ===\n");

    /* (A) self-advancing type 0x40 */
    seed_pose(&g_actors[1], 0x40); g_actors[1].x = 1000; g_actors[1].z = 1000;
    fail |= run_pose(&g_actors[1], "A:0x40");

    /* (B) NON-self-advancing type 0x45 — double-advance skip */
    memset(g_actors, 0, sizeof g_actors);
    pl->active = 1; pl->type = 0; pl->z = 3000; pl->hp = 100;
    seed_pose(&g_actors[1], 0x45); g_actors[1].x = 1000; g_actors[1].z = 1000;
    fail |= run_pose(&g_actors[1], "B:0x45");

    /* (C) LOOP flag set -> must wrap */
    memset(g_actors, 0, sizeof g_actors);
    pl->active = 1; pl->type = 0; pl->z = 3000; pl->hp = 100;
    seed_pose(&g_actors[1], 0x40); g_actors[1].anim_flags = 0x04; g_actors[1].x = 1000; g_actors[1].z = 1000;
    {
        int wrapped = 0; uint8_t last = g_actors[1].anim_frame;
        for (int f = 0; f < 120; f++) { re15_actors_anim_advance(); re15_enemy_ai_run_all(0);
            if (g_actors[1].anim_frame < last) wrapped = 1; last = g_actors[1].anim_frame; }
        if (!wrapped) { fprintf(stderr, "FAIL(C): LOOP-flagged clip did NOT wrap\n"); fail = 1; }
        printf("  (C) LOOP flag set: wrapped=%d\n", wrapped);
    }

    /* (D) control fresh NPC -> INIT idle clip 2 */
    memset(g_actors, 0, sizeof g_actors);
    pl->active = 1; pl->type = 0; pl->z = 3000; pl->hp = 100;
    re15_actor_t *c = &g_actors[1];
    c->active = 1; c->type = 0x40; c->state = 0; c->grid_id = 0; c->x = -1500; c->z = 1000;
    re15_enemy_ai_run_all(0);
    if (c->motion != 2) { fprintf(stderr, "FAIL(D): control NPC must idle clip 2, got %d\n", c->motion); fail = 1; }
    if (c->hp != -1)    { fprintf(stderr, "FAIL(D): control NPC must be invulnerable, hp=%d\n", c->hp); fail = 1; }
    printf("  (D) control (no pose): idle clip=%d hp=%d\n", c->motion, c->hp);

    if (fail) { printf("NPC PLC-MOTION POSE: FAIL\n"); return 1; }
    printf("NPC PLC-MOTION POSE: all checks passed\n");
    return 0;
}
