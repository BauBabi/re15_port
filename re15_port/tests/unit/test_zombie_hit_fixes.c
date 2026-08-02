/* test_zombie_hit_fixes.c — pins the ROOM1140 hit-reaction fixes from the RE dossier
 * analysis/zombie_hit_1140.md (all findings adversarially CONFIRMED 2026-08-02):
 *
 *  D1 (user symptom a): REVERSE playback is a per-call f314 argument in the original
 *      (grab-recovery @0x80102aec `ori a2,zero,1; jal 0x8001f314`), NOT entity state.
 *      A shot forces +0x4=2/3 (@0x80012520 / @0x80013018-20) and the next handler sets
 *      a new clip FORWARD — so the port's persistent anim_flags 0x80 must drop at the
 *      damage hijack, or a zombie shot during grab-recovery plays EVERYTHING backwards.
 *
 *  D2 (user symptom b): the scripted lyer (type 0x16, grid nibble 8) re-arms the hit
 *      guard +0x93|=1 EVERY passive tick (FUN_80103a58 phase 0 @0x80103aac-ab8) — he is
 *      UNSHOOTABLE until his wake machine reaches phase 3 (+0x9=0, word=0x201,
 *      +0x93&=0xfe — FUN_80103a58.c decompile).
 *
 *  D6: grab-[0] sets +0x1d8|=1 (FUN_80102548.c Z.28), throw-off-[4] clears it (Z.75);
 *      D5: grab-[0] sets the word0-0x1000 choreo latch (Z.21-22), only [8] clears (Z.115). */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_player.h"
#include <stdio.h>
#include <string.h>

static int fails = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("FAIL: %s\n", msg); fails++; } } while (0)

static re15_actor_t *mkzombie(int slot, uint8_t type, int hp)
{
    re15_actor_t *z = &g_actors[slot];
    memset(z, 0, sizeof(*z));
    z->active = 1; z->type = type; z->flags = 1;
    z->state = (uint8_t)RE15_AI_STATE_ACTIVE; z->sub_state_1 = 2;
    z->x = 1200; z->z = 0; z->rot_y = 0; z->em_flag_id = 0xFF;
    re15_enemy_ai_live_init(slot);
    z->hp = (int16_t)hp;
    return z;
}

int main(void)
{
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 0; pl->z = 0; pl->hp = 100; pl->state = 0; pl->hit_react = 0;

    /* ---- D1: shot during grab-recovery [7] must DROP the reverse-playback bit ---- */
    {
        re15_actor_t *z = mkzombie(1, 0x10, 200);
        z->state = 1; z->sub_state_1 = 3; z->sub_state_2 = 7;   /* grab-recovery */
        z->motion = 0x11; z->anim_frame = 3;
        z->anim_flags |= 0x80;                                   /* the [6] reverse latch */
        z->hit_react = 0;
        int r = re15_enemy_take_damage(z, 0);
        CHECK(r == 1, "D1: der Schuss in [7] muss Schaden eintragen");
        CHECK(!(z->anim_flags & 0x80), "D1: anim_flags 0x80 muss am Damage-Hijack fallen");
        for (int i = 0; i < 40; i++) { re15_enemy_ai_live_tick(1); re15_actors_anim_advance(); }
        CHECK(!(z->anim_flags & 0x80), "D1: 0x80 darf durch Stagger/Exit nicht zurueckkommen");
        z->active = 0;
    }

    /* ---- D2: passive scripted lyer is unshootable (per-tick +0x93|=1 re-arm) ---- */
    {
        re15_actor_t *z = mkzombie(2, 0x16, 300);
        z->state = 1; z->sub_state_1 = 0; z->sub_state_2 = 0;
        z->grid_id = 0x88;                                       /* downed + nibble 8 */
        z->hit_react = 0;
        for (int i = 0; i < 5; i++) {
            re15_enemy_ai_live_tick(2);
            CHECK(z->hit_react & 1, "D2: Phase 0 muss +0x93|=1 JEDEN Tick re-armieren");
        }
        int16_t hp0 = z->hp;
        int r = re15_enemy_take_damage(z, 0);
        CHECK(r == 0, "D2: der Schuss auf den passiven Lyer darf NICHT landen");
        CHECK(z->hp == hp0, "D2: HP des passiven Lyers unveraendert");
        CHECK(z->state == 1, "D2: der Lyer bleibt in State 1 (kein HURT)");

        /* wake machine: phase 1 counts +0x9c down (pre-decrement zero -> phase 2 with
         * +0x8f=0xf), phase 2 plays the rise clip (clip_done, no bank -> immediate),
         * phase 3 -> +0x9=0, word 0x201, +0x93&=0xfe (FUN_80103a58.c) */
        z->hit_react = 1;
        z->sub_state_2 = 1; z->ai_timer = 2;
        for (int i = 0; i < 8 && !(z->sub_state_1 == 2 && z->grid_id == 0); i++)
            re15_enemy_ai_live_tick(2);
        CHECK(z->grid_id == 0, "D2-Wake: Phase 3 muss +0x9=0 setzen");
        CHECK(z->state == 1 && z->sub_state_1 == 2, "D2-Wake: Phase 3 muss word 0x201 setzen");
        CHECK(!(z->hit_react & 1), "D2-Wake: Phase 3 muss +0x93 bit0 loeschen");
        z->active = 0;
    }

    /* ---- D5/D6: grab-[0] latches (+0x1d8 bit0 + word0-0x1000), [4] clears bit0 ---- */
    {
        re15_actor_t *z = mkzombie(3, 0x10, 200);
        pl->hit_react = 0;
        z->state = 1; z->sub_state_1 = 3; z->sub_state_2 = 0;    /* grab entry */
        re15_enemy_ai_live_tick(3);                              /* runs grab [0] */
        CHECK(z->ai_flags & 1, "D6: grab-[0] muss +0x1d8 bit0 setzen");
        CHECK(z->grab_choreo == 1, "D5: grab-[0] muss den Choreo-Latch setzen");
        z->sub_state_2 = 4;
        re15_enemy_ai_live_tick(3);                              /* runs throw-off [4] */
        CHECK(!(z->ai_flags & 1), "D6: throw-off-[4] muss +0x1d8 bit0 loeschen");
        CHECK(z->grab_choreo == 1, "D5: [4] darf den Choreo-Latch NICHT loeschen (nur [8])");
        /* drive to the [8] exit: [5] fling (clip_done no-bank -> +1), [6], [7], [8] */
        for (int i = 0; i < 8 && z->sub_state_2 != 0xff && z->state == 1 && z->sub_state_1 >= 3
                        && z->sub_state_1 <= 4; i++)
            re15_enemy_ai_live_tick(3);
        CHECK(z->grab_choreo == 0, "D5: der [8]-Exit muss den Choreo-Latch loeschen");
        z->active = 0;
    }

    if (fails == 0) { printf("test_zombie_hit_fixes: OK\n"); return 0; }
    printf("test_zombie_hit_fixes: %d FAILURES\n", fails);
    return 1;
}
