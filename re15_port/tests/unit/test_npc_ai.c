/* ============================================================================
 *  NPC (types 0x40..0x4d) — shared EXE behaviour library, byte-true probes.
 *
 *  REWRITTEN for audit wf_827f186d npc #1/#2/#3/#5/#7/#8 (the old test (2) pinned the
 *  pre-audit "state-1 NPC idles + animates forever" fold; byte-true state 1 is the
 *  WATCHER machine — mode-1 near-player frames are decide-ONLY with no anim advance,
 *  table @0x801215ac[1]/@0x8012174c[1] = 0x8004f100).
 *
 *  Sections:
 *   (1) INIT (0x8011c6dc): state 0 -> 1, HP = -1, idle clip 2.
 *   (2) STATE 1, player NEAR (< 0x3a98): 0x40 LOS pre-pass +0x9=1 (@0x8011c8d8) ->
 *       watcher-only frame: sub mutates (dist>=0x5dd -> +0x5=1 @0x8004f124), NO
 *       position change, NO anim advance (mode-1 row is 0x8004f100 alone).
 *   (3) STATE 1, player FAR (>= 0x3a98): +0x9=0 (@0x8011c904) -> mode-0 decide+act;
 *       the decide chain settles in the lost-player row +0x5=5 (search @0x8004f434:
 *       dist >= 0xbb9 -> 5).
 *   (4) WATCHER escalation: player.hit_react != 0 -> +0x5=6 (@0x8004f1e0).
 *   (5) Executor GESTURE sub 2 (0x80050f00, wf npc #3): plays the clip ONCE then
 *       HOLDS at the last frame (phase 2 @0x80050ec8), no forever-loop.
 *   (6) Executor RUN sub 5 (0x80051484, wf npc #7/#8): speed 200 from @0x80076c80,
 *       clip 5 -> clip 0 at pivot end (@0x8005157c-9c), arrival dist<300
 *       (@0x80051764) -> zone-5 bit[+0x1c3] (@0x80051780-84) + sub 6 event-reach.
 *   (7) Plc_dest mode 6 result state (state 4 sub 6, wf npc #2): the executor's
 *       event-reach plays clip 1 once then loops idle clip 2 (@0x800517f0) — no
 *       permanent walk-to-(0,0) statue.
 *   (8) TRANSFER sub 0x13 (0x80052864, wf npc #4): reaches the clip-0x12 wait phase,
 *       then the zone-5 bit release (@0x80052b44) drives the reverse stand-up into
 *       sub 6 (@0x80052bdc-f0).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_scd.h"      /* re15_game_flag_get/set — zone-5 arrival bits @0x800b1028 */

static re15_actor_t *reset_world(int32_t plx, int32_t plz)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = plx; pl->y = 0; pl->z = plz; pl->hp = 100;
    return pl;
}

int main(void)
{
    int fail = 0;

    printf("=== NPC shared EXE lib — byte-true probes (audit wf_827f186d npc) ===\n");

    /* (1) INIT */
    reset_world(0, 3000);
    re15_actor_t *n = &g_actors[1];
    n->active = 1; n->type = 0x40; n->state = 0; n->x = 1000; n->z = 1000;
    re15_enemy_ai_run_all(0);
    if (n->state != 1) { fprintf(stderr, "FAIL(1): INIT->state 1, got %d\n", n->state); fail = 1; }
    if (n->hp != -1)   { fprintf(stderr, "FAIL(1): HP=-1 expected, got %d\n", n->hp); fail = 1; }
    if (n->motion != 2){ fprintf(stderr, "FAIL(1): idle clip 2 expected, got %d\n", n->motion); fail = 1; }
    printf("  (1) INIT: state=1 hp=%d clip=%d\n", n->hp, n->motion);

    /* (2) state 1, player NEAR: mode 1 = watcher-only (dist 2236: >= 0x5dd -> sub 1;
     *     no act row -> anim frame FROZEN, position held). */
    {
        int32_t nx0 = n->x, nz0 = n->z;
        uint8_t af0 = n->anim_frame;
        for (int f = 0; f < 60; f++) re15_enemy_ai_run_all(0);
        if ((n->grid_id & 0x0f) != 1) { fprintf(stderr, "FAIL(2): near player -> +0x9=1 (@0x8011c8d8), got 0x%02x\n", n->grid_id); fail = 1; }
        if (n->sub_state_1 != 1 && n->sub_state_1 != 2) {
            fprintf(stderr, "FAIL(2): watcher must route +0x5 to 1/2 (@0x8004f124/@0x8004f164), got %d\n", n->sub_state_1); fail = 1; }
        if (n->x != nx0 || n->z != nz0) { fprintf(stderr, "FAIL(2): mode-1 frame has no act -> no movement\n"); fail = 1; }
        if (n->anim_frame != af0) { fprintf(stderr, "FAIL(2): mode-1 frame has no anim_set -> frame frozen (was the old idle-loop pin)\n"); fail = 1; }
        printf("  (2) NEAR: mode=%d sub=%d pos held, anim frozen (byte-true watcher-only)\n",
               n->grid_id & 0xf, n->sub_state_1);
    }

    /* (3) state 1, player FAR: mode 0 decide+act; settles in the lost-player row +0x5=5. */
    {
        re15_actor_t *pl = reset_world(0, 30000);
        (void)pl;
        n = &g_actors[1];
        n->active = 1; n->type = 0x40; n->state = 0; n->x = 1000; n->z = 1000;
        for (int f = 0; f < 120; f++) re15_enemy_ai_run_all(0);
        if ((n->grid_id & 0x0f) != 0) { fprintf(stderr, "FAIL(3): far player -> +0x9=0 (@0x8011c904), got 0x%02x\n", n->grid_id); fail = 1; }
        if (n->sub_state_1 != 5) { fprintf(stderr, "FAIL(3): decide chain must settle at +0x5=5 (@0x8004f434 dist>=0xbb9), got %d\n", n->sub_state_1); fail = 1; }
        printf("  (3) FAR: mode=0 sub=%d (lost-player row)\n", n->sub_state_1);
    }

    /* (4) watcher escalation on player hit_react (@0x8004f1e0 sb 6,5). */
    {
        re15_actor_t *pl = reset_world(0, 3000);
        n = &g_actors[1];
        n->active = 1; n->type = 0x40; n->state = 0; n->x = 1000; n->z = 1000;
        re15_enemy_ai_run_all(0);
        pl->hit_react = 1;
        re15_enemy_ai_run_all(0);
        if (n->sub_state_1 != 6) { fprintf(stderr, "FAIL(4): player.hit_react -> +0x5=6 (@0x8004f1e0), got %d\n", n->sub_state_1); fail = 1; }
        printf("  (4) hit_react escalation: sub=%d\n", n->sub_state_1);
    }

    /* (5) executor GESTURE sub 2 (Plc_motion pc[1]=2, e.g. ROOM1141 sub02 `3f 02 06 00`):
     *     clip 6 (EM040 = 50 frames) plays ONCE, HOLDS at frame 49, phase 2 (wf npc #3). */
    {
        reset_world(0, 3000);
        n = &g_actors[1];
        n->active = 1; n->type = 0x40; n->hp = -1;
        n->state = 4; n->sub_state_1 = 2; n->sub_state_2 = 0;   /* op_plc_motion result (@0x80041bb0-c4) */
        n->motion = 6; n->anim_frame = 0; n->anim_flags = 0;
        int wrapped = 0; uint8_t last = 0;
        for (int f = 0; f < 200; f++) {
            re15_enemy_ai_run_all(0);
            if (n->anim_frame < last) wrapped = 1;
            last = n->anim_frame;
        }
        if (wrapped)                { fprintf(stderr, "FAIL(5): gesture must NOT loop (HOLD @0x80050ec8)\n"); fail = 1; }
        if (n->sub_state_2 != 2)    { fprintf(stderr, "FAIL(5): phase must latch 2 (HOLD), got %d\n", n->sub_state_2); fail = 1; }
        if (n->anim_frame != 49)    { fprintf(stderr, "FAIL(5): must hold last frame 49 of clip 6, got %d\n", n->anim_frame); fail = 1; }
        printf("  (5) gesture sub 2: held at frame %d phase %d (play-once)\n", n->anim_frame, n->sub_state_2);
    }

    /* (6) executor RUN sub 5: per-type speed 200 (@0x80076c80 c8), pivot clip 5 -> run clip 0,
     *     arrival dist<300 -> zone-5 bit + sub 6 (wf npc #7/#8). */
    {
        reset_world(0, 30000);   /* player far away — irrelevant to the executor walk */
        n = &g_actors[1];
        n->active = 1; n->type = 0x40; n->hp = -1;
        n->state = 4; n->sub_state_1 = 5; n->sub_state_2 = 0;   /* op_plc_dest(mode 5) result (@0x80041c14-18) */
        n->steer_x = 2000; n->steer_z = 0;                       /* +0x1bc/+0x1be @0x80041c38/58 */
        n->walk_flag_bit = 32; n->anim_flags = 0;                /* +0x1c3 @0x80041c24 */
        re15_game_flag_set(5, 32, 0);
        int saw_speed = 0, saw_runclip = 0;
        for (int f = 0; f < 200 && n->sub_state_1 == 5; f++) {
            re15_enemy_ai_run_all(0);
            if (n->crow_speed == 200) saw_speed = 1;
            if (n->sub_state_2 == 2 && n->motion == 0) saw_runclip = 1;
        }
        if (!saw_speed)   { fprintf(stderr, "FAIL(6): run speed must be 200 (@0x80076c80), never saw it\n"); fail = 1; }
        if (!saw_runclip) { fprintf(stderr, "FAIL(6): run phase 2 must switch to clip 0 (@0x8005157c-9c)\n"); fail = 1; }
        if (n->sub_state_1 != 6) { fprintf(stderr, "FAIL(6): arrival must hand to sub 6 (@0x80051794), got %d\n", n->sub_state_1); fail = 1; }
        if (!re15_game_flag_get(5, 32)) { fprintf(stderr, "FAIL(6): arrival must set zone-5 bit 32 (@0x80051780-84)\n"); fail = 1; }
        printf("  (6) run sub 5: speed=200 clip0=%d arrived sub=%d flag(5,32)=%d\n",
               saw_runclip, n->sub_state_1, re15_game_flag_get(5, 32));
    }

    /* (7) Plc_dest MODE 6 = event-reach, not a statue (wf npc #2): sub 6 plays clip 1 once
     *     then loops idle clip 2 (@0x80051844/@0x800518b4-dc). */
    {
        reset_world(0, 3000);
        n = &g_actors[1];
        n->active = 1; n->type = 0x42; n->hp = -1;
        n->state = 4; n->sub_state_1 = 6; n->sub_state_2 = 0;   /* op_plc_dest(0,6,...) result */
        n->motion = 0; n->walk_active = 0;
        int saw_clip1 = 0, saw_clip2_advance = 0; uint8_t last = 0;
        for (int f = 0; f < 200; f++) {
            re15_enemy_ai_run_all(0);
            if (n->motion == 1) saw_clip1 = 1;
            if (n->motion == 2 && n->anim_frame != last) saw_clip2_advance = 1;
            last = n->anim_frame;
        }
        if (!saw_clip1)          { fprintf(stderr, "FAIL(7): sub 6 must play arrival clip 1 (@0x80051844-54)\n"); fail = 1; }
        if (!saw_clip2_advance)  { fprintf(stderr, "FAIL(7): sub 6 must settle into the clip-2 idle loop (@0x800518b4-dc)\n"); fail = 1; }
        if (n->state != 4 || n->sub_state_1 != 6) { fprintf(stderr, "FAIL(7): must stay in executor sub 6\n"); fail = 1; }
        printf("  (7) mode 6: clip1 played=%d, idle clip2 loops=%d (no statue)\n", saw_clip1, saw_clip2_advance);
    }

    /* (8) TRANSFER sub 0x13 (Plc_dest(0,19,20) — ROOM11B0 sub06/ROOM1171 sub02): reaches the
     *     clip-0x12 wait (phase 5 @0x80052a74-b0), then flag(5,20) releases it through the
     *     reverse stand-up (phase 6 @0x80052b8c) into sub 6 (@0x80052bdc-f0). */
    {
        reset_world(0, 3000);
        n = &g_actors[1];
        n->active = 1; n->type = 0x40; n->hp = -1;
        n->state = 4; n->sub_state_1 = 0x13; n->sub_state_2 = 0;
        n->walk_flag_bit = 20; n->anim_flags = 0; n->rot_y = 0;   /* yaw already cardinal-aligned */
        re15_game_flag_set(5, 20, 0);
        int reached_wait = 0;
        for (int f = 0; f < 300; f++) {
            re15_enemy_ai_run_all(0);
            if (n->sub_state_2 == 5 && n->motion == 0x12) { reached_wait = 1; break; }
        }
        if (!reached_wait) { fprintf(stderr, "FAIL(8): transfer must reach the clip-0x12 wait phase 5, at phase %d clip %d\n", n->sub_state_2, n->motion); fail = 1; }
        for (int f = 0; f < 60; f++) re15_enemy_ai_run_all(0);
        if (n->sub_state_1 != 0x13) { fprintf(stderr, "FAIL(8): must WAIT on flag(5,20) (@0x80052b44), left early to sub %d\n", n->sub_state_1); fail = 1; }
        re15_game_flag_set(5, 20, 1);
        for (int f = 0; f < 120 && n->sub_state_1 == 0x13; f++) re15_enemy_ai_run_all(0);
        if (n->sub_state_1 != 6) { fprintf(stderr, "FAIL(8): released transfer must hand to sub 6 (@0x80052bdc-f0), got %d\n", n->sub_state_1); fail = 1; }
        printf("  (8) transfer sub 0x13: waited on flag(5,20), released -> sub=%d\n", n->sub_state_1);
    }

    if (fail) { printf("NPC BYTE-TRUE: FAIL\n"); return 1; }
    printf("NPC BYTE-TRUE: all checks passed\n");
    return 0;
}
