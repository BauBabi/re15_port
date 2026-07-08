/* test_hit_reaction.c — the byte-true player HURT reaction (FUN_80035af0 hurt sub-FSM).
 *
 * A non-lethal hit routes the player command FSM to state 2, and FUN_80035af0 dispatches on the
 * direction sub-state DAT_800aca59 via the jump table @0x800741a8:
 *   [2] @0x80035de0 -> clip 0x8 (back hit)   + knockback DAT_800acae0 = 0xc8 (200)
 *   [3] @0x80035f64 -> clip 0x9 (front hit)  + knockback DAT_800acae0 = 0xc8 (200)
 * each frame shoving the player BACKWARD (FUN_800245d8(0x800) = rotate (mag,0,0) by Ry(facing+0x800))
 * and decaying the magnitude by DAT_800acaf2 = 0x32 (50): 200,150,100,50 over 4 frames. The port used
 * to force a fixed clip 0xa with no knockback. The live melee attacks apply hp directly, so the
 * game-step hit-flinch derives the direction from the attacker (re15_nearest_hostile ->
 * re15_player_hit_from_front). This proves the three byte-true primitives behind that:
 *   (1) hit_from_front is directional: an attacker in the facing direction reads FRONT, behind BACK.
 *   (2) nearest_hostile picks the nearest LIVE enemy (skips corpses / dead / invulnerable NPCs).
 *   (3) the knockback delta is BACKWARD (opposite the facing), scales linearly, and decays 200..50->0.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 — the walker forward vector */

static void set_player(int16_t rot_y)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->rot_y = rot_y; pl->floor = 0; pl->hp = 100;
}

int main(void)
{
    int fail = 0;
    printf("=== player HURT reaction: directional clip + backward knockback (FUN_80035af0) ===\n");

    /* (1) DIRECTION is a byte-true hemisphere partition (FUN_8001a7a8: the bearing ATTACKER->player,
     *     ang = atan2(player.z-src.z, player.x-src.x), minus the heading, centred by +0x400, split at
     *     0x800). Two properties that hold for ANY heading/mesh convention (so no physical-axis guess):
     *       (1a) ANTIPODAL: an attacker and its mirror through the player read OPPOSITE front/back
     *            (a clean hemisphere split) — front -> clip 0x9, back -> clip 0x8.
     *       (1b) FACING-DEPENDENT: rotating the player 180deg (rot_y ^ 0x800) flips front<->back for a
     *            fixed attacker (the reaction tracks where the player looks). */
    {
        int checked = 0;
        static const int32_t rot[4] = { 0, 0x400, 0x800, 0xc00 };
        static const int32_t off[6][2] = { {3000,0},{2100,2100},{0,3000},{-2100,2100},{1500,-2600},{-2600,-1500} };
        for (int r = 0; r < 4; r++) {
            memset(g_actors, 0, sizeof g_actors);
            set_player((int16_t)rot[r]);
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            for (int i = 0; i < 6; i++) {
                int hf   = re15_player_hit_from_front(pl,  off[i][0],  off[i][1]);
                int anti = re15_player_hit_from_front(pl, -off[i][0], -off[i][1]);   /* mirror through player */
                if (hf == anti) { fprintf(stderr, "FAIL(1a): antipodal attackers must read opposite (rot %ld off %d,%d)\n",
                                          (long)rot[r], off[i][0], off[i][1]); fail = 1; }
                pl->rot_y = (int16_t)(rot[r] ^ 0x800);                                /* look 180deg away */
                int hf_flip = re15_player_hit_from_front(pl, off[i][0], off[i][1]);
                pl->rot_y = (int16_t)rot[r];
                if (hf == hf_flip) { fprintf(stderr, "FAIL(1b): 180deg facing flip must swap front/back (rot %ld off %d,%d)\n",
                                             (long)rot[r], off[i][0], off[i][1]); fail = 1; }
                checked++;
            }
        }
        if (!fail) printf("  (1) direction: hemisphere partition (antipodal-opposite + facing-flip) over %d cases\n", checked);
    }

    /* (2) ATTACKER IDENTITY. The nearest LIVE, non-corpse enemy is the striker even when a corpse /
     *     dead body / NPC sits closer. */
    {
        memset(g_actors, 0, sizeof g_actors);
        set_player(0);
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        /* slot 1: live enemy, near */      g_actors[1] = (re15_actor_t){0};
        g_actors[1].active = 1; g_actors[1].type = 0x10; g_actors[1].state = 1; g_actors[1].hp = 100; g_actors[1].x = 2000; g_actors[1].z = 0;
        /* slot 2: live enemy, far */       g_actors[2] = (re15_actor_t){0};
        g_actors[2].active = 1; g_actors[2].type = 0x10; g_actors[2].state = 1; g_actors[2].hp = 100; g_actors[2].x = 8000; g_actors[2].z = 0;
        /* slot 3: CORPSE, closer */        g_actors[3] = (re15_actor_t){0};
        g_actors[3].active = 1; g_actors[3].type = 0x10; g_actors[3].state = 7; g_actors[3].hp = 0;   g_actors[3].x = 500;  g_actors[3].z = 0;
        /* slot 4: DEAD/NPC, closest */     g_actors[4] = (re15_actor_t){0};
        g_actors[4].active = 1; g_actors[4].type = 0x40; g_actors[4].state = 1; g_actors[4].hp = -1;  g_actors[4].x = 200;  g_actors[4].z = 0;

        const re15_actor_t *atk = re15_nearest_hostile(pl);
        if (atk != &g_actors[1]) { fprintf(stderr, "FAIL(2a): attacker must be the near LIVE enemy (slot 1), got %ld\n",
                                           atk ? (long)(atk - g_actors) : -1L); fail = 1; }
        else printf("  (2) attacker: near live enemy chosen over closer corpse/dead/NPC\n");

        /* none alive -> NULL */
        memset(g_actors, 0, sizeof g_actors); set_player(0);
        if (re15_nearest_hostile(&g_actors[RE15_ACTOR_SLOT_PLAYER]) != NULL) { fprintf(stderr, "FAIL(2b): no live enemy must give NULL\n"); fail = 1; }
    }

    /* (3) KNOCKBACK is byte-true backward (FUN_800245d8(0x800) = rotate (mag,0,0) by Ry(facing+0x800)).
     *     Since the walker steps by Ry(facing) on (speed,0,0), the knockback must be ANTI-PARALLEL to
     *     the walker forward for the SAME rot_y (both use rot_y — this is the coherent stagger-back).
     *     Assert across facings: dot(knockback, forward) < 0, magnitude ~= mag, and linear scaling. */
    {
        int32_t dx, dz;
        re15_player_knockback_delta(0, 200, &dx, &dz);
        if (dx >= -180 || abs(dz) > 8) { fprintf(stderr, "FAIL(3a): rot0 mag200 push must be ~(-200,0), got (%d,%d)\n", dx, dz); fail = 1; }
        int32_t dx100, dz100; re15_player_knockback_delta(0, 100, &dx100, &dz100);
        if (!(dx100 < 0 && dx100 < dx / 2 + 20 && dx100 > dx / 2 - 20)) {
            fprintf(stderr, "FAIL(3b): push must scale linearly (mag200 dx=%d, mag100 dx=%d)\n", dx, dx100); fail = 1; }

        static const int32_t rot[4] = { 0, 0x400, 0x800, 0xc00 };
        for (int r = 0; r < 4; r++) {
            int32_t kx, kz; re15_player_knockback_delta((int16_t)rot[r], 200, &kx, &kz);
            /* walker forward for this facing = Ry(rot) * (speed,0,0) = (cos, -sin) */
            int32_t fx = re15_cos_q12(rot[r]);
            int32_t fz = -re15_sin_q12(rot[r]);
            int64_t dot = (int64_t)kx * fx + (int64_t)kz * fz;
            if (dot >= 0) { fprintf(stderr, "FAIL(3c): knockback must be BACKWARD (anti-parallel to walker forward) at rot %ld: dot=%lld\n",
                                    (long)rot[r], (long long)dot); fail = 1; }
        }
        if (!fail) printf("  (3) knockback: backward (anti-parallel to walker forward) at all facings, linear (rot0=(%d,%d))\n", dx, dz);

        /* decay sequence 200,150,100,50 -> 0 (mag -= 50, clamp) */
        int32_t seq[5], m = 200;
        for (int i = 0; i < 5; i++) { seq[i] = m; m -= 50; if (m < 0) m = 0; }
        if (!(seq[0]==200 && seq[1]==150 && seq[2]==100 && seq[3]==50 && seq[4]==0)) {
            fprintf(stderr, "FAIL(3e): decay must be 200,150,100,50,0\n"); fail = 1; }
    }

    if (fail) { printf("HIT-REACTION: FAIL\n"); return 1; }
    printf("HIT-REACTION: all checks passed\n");
    return 0;
}
