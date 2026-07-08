/* test_getup_advance.c — the zombie KNOCKDOWN GET-UP clip must ADVANCE, not freeze (audit ANIM-1).
 *
 * re15_actors_anim_advance pins clips 0x0C/0x0E/0x12/0x13 to frame 0 for a downed/spawn lie-down.
 * But clips 0x12/0x13 are DUAL-USE: re15_enemy_ai_live_knockdown case 4 sets motion 0x12 for the
 * GET-UP, and case 5 waits on re15_enemy_clip_done (anim_frame>=frames-1) to reach case 6 (re-engage).
 * With the unconditional pin, anim_frame never advanced past 0, so a poise-broken ROOM1140 zombie
 * froze in the get-up start pose forever and never re-attacked. The fix releases the pin only for the
 * get-up state (sub_state_1==0x11, sub_state_2>=4). Asserts:
 *   (1) a get-up zombie (motion 0x12, sub_state_1=0x11, sub_state_2=5) ADVANCES anim_frame each tick.
 *   (2) a lie-WAIT zombie (same clip, sub_state_2=3 < 4) still HOLDS frame 0.
 *   (3) a spawn lie-down (same clip, sub_state_1=0) still HOLDS frame 0 (no regression).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_player.h"   /* re15_actors_anim_advance */

static re15_actor_t *mk_zombie(uint16_t motion, uint8_t ss1, uint8_t ss2, uint16_t frame0)
{
    memset(g_actors, 0, sizeof g_actors);
    re15_actor_t *z = &g_actors[1];
    z->active = 1; z->type = 0x10;         /* zombie: NOT self-advancing -> uses the global pass */
    z->state = 1;                          /* not corpse */
    z->motion = (int16_t)motion;
    z->sub_state_1 = ss1; z->sub_state_2 = ss2;
    z->anim_frame = frame0; z->anim_frac = 0; z->motion_init_delay = 0;
    return z;
}

int main(void)
{
    int fail = 0;
    printf("=== zombie knockdown GET-UP clip advances (audit ANIM-1) ===\n");

    /* (1) GET-UP: motion 0x12, sub_state_1=0x11, sub_state_2=5 -> anim_frame advances +1/tick. */
    {
        re15_actor_t *z = mk_zombie(0x12, 0x11, 5, 0);
        for (int t = 1; t <= 3; t++) {
            re15_actors_anim_advance();
            if (z->anim_frame != (uint16_t)t) {
                fprintf(stderr, "FAIL(1): get-up anim_frame after %d ticks = %u, want %d (pin not released?)\n",
                        t, z->anim_frame, t);
                fail = 1; break;
            }
        }
        if (!fail) printf("  (1) get-up (0x11/ss2=5) clip 0x12 advances (frame reaches 3 over 3 ticks)\n");
    }

    /* (2) lie-WAIT: same clip but sub_state_2=3 (< 4, the timer wait) -> holds frame 0. */
    {
        re15_actor_t *z = mk_zombie(0x12, 0x11, 3, 7);
        re15_actors_anim_advance();
        if (z->anim_frame != 0) { fprintf(stderr, "FAIL(2): lie-wait (ss2=3) must hold frame 0, got %u\n", z->anim_frame); fail = 1; }
        else printf("  (2) lie-wait (0x11/ss2=3) clip 0x12 still holds frame 0\n");
    }

    /* (3) spawn lie-down: same clip, sub_state_1=0 (not the knockdown state) -> holds frame 0. */
    {
        re15_actor_t *z = mk_zombie(0x12, 0, 0, 7);
        re15_actors_anim_advance();
        if (z->anim_frame != 0) { fprintf(stderr, "FAIL(3): spawn lie-down (ss1=0) must hold frame 0, got %u\n", z->anim_frame); fail = 1; }
        else printf("  (3) spawn lie-down (ss1=0) clip 0x12 still holds frame 0 (no regression)\n");
    }

    /* (4) a normal walk clip still advances (sanity: the pin only affects the blocklist clips). */
    {
        re15_actor_t *z = mk_zombie(0x06, 0, 0, 0);   /* clip 6 = not in the pin blocklist */
        re15_actors_anim_advance();
        if (z->anim_frame != 1) { fprintf(stderr, "FAIL(4): normal clip 0x06 must advance, got %u\n", z->anim_frame); fail = 1; }
        else printf("  (4) normal clip advances (pin scoped to 0x0C/0x0E/0x12/0x13)\n");
    }

    if (fail) { printf("GETUP-ADVANCE: FAIL\n"); return 1; }
    printf("GETUP-ADVANCE: all checks passed\n");
    return 0;
}
