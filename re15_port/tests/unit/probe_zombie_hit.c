/* probe_zombie_hit.c — DIAGNOSTIC (not an assertion test): print the zombie's per-tick state after
 * a hit, for the two live reports:
 *   (a) "a downed zombie shot on the ground still does not get up immediately"
 *   (b) "zombies sometimes freeze oddly while standing when shot"
 * Prints a timeline so the actual behaviour can be measured before anything is changed. */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_player.h"   /* re15_actors_anim_advance */
#include <stdio.h>
#include <string.h>

static re15_actor_t *setup(int slot, uint8_t type, int hp)
{
    re15_actor_t *z = &g_actors[slot];
    memset(z, 0, sizeof(*z));
    z->active = 1; z->type = type; z->hp = (int16_t)hp; z->flags = 1;
    z->state = (uint8_t)RE15_AI_STATE_ACTIVE; z->sub_state_1 = 2;
    z->x = 1200; z->z = 0; z->rot_y = 0; z->em_flag_id = 0xFF;
    re15_enemy_ai_live_init(slot);      /* the REAL spawn init — seeds the poise (rand&3)+4 @0x8010082c */
    z->hp = (int16_t)hp;                /* live_init does not touch hp; restore the probe's value */
    z->sub_state_1 = 2;
    return z;
}

static void line(const char *tag, int f, const re15_actor_t *z)
{
    printf("%-10s f%-4d st=%d ss1=%2d ss2=%2d ss3=%2d mo=%3d af=%3d grid=%02x stun=%4d hr=%d hp=%d\n",
           tag, f, z->state, z->sub_state_1, z->sub_state_2, z->sub_state_3,
           z->motion, z->anim_frame, z->grid_id, z->hit_stun, z->hit_react, z->hp);
}

int main(void)
{
    re15_actor_init();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = 0; pl->z = 0; pl->hp = 100; pl->state = 0; pl->floor = 0; pl->hit_react = 0;

    /* ---------- (a) knock one down, let it settle into the LIE, then shoot it ---------------- */
    re15_actor_t *z = setup(1, 0x10, 200);
    int f = 0;
    for (int k = 0; k < 8 && z->sub_state_1 != 0x11; k++) {     /* break the poise */
        z->hit_react = 0;
        re15_enemy_take_damage(z, 0);
        for (int i = 0; i < 14 && z->state == RE15_AI_STATE_HURT; i++) { re15_enemy_ai_live_tick(1); re15_actors_anim_advance(); f++; }
    }
    printf("== (a) nach dem Niederschlag ==\n");
    line("knock", f, z);
    for (int i = 0; i < 400 && !(z->sub_state_1 == 0x11 && z->sub_state_2 == 3); i++) {
        re15_enemy_ai_live_tick(1); re15_actors_anim_advance(); f++;
    }
    line("liegt", f, z);
    printf("   Liege-Timer ai_timer=%d  (= lie_tbl[rand&0xf]*30)\n", z->ai_timer);

    /* let it lie a while, then shoot it while down */
    for (int i = 0; i < 20; i++) { re15_enemy_ai_live_tick(1); re15_actors_anim_advance(); f++; }
    line("vor Schuss", f, z);
    z->hit_react = 0;
    re15_enemy_take_damage(z, 0);
    line("SCHUSS", f, z);
    int stood = -1;
    for (int i = 0; i < 400; i++) {
        re15_enemy_ai_live_tick(1); re15_actors_anim_advance(); f++;
        if (i < 12 || (i % 25) == 0) line("  nach", f, z);
        if (!(z->grid_id & 0x80) && z->state == RE15_AI_STATE_ACTIVE && stood < 0) { stood = i; break; }
    }
    printf("   -> steht wieder nach %d Ticks (nach dem Treffer)\n", stood);

    /* ---------- (b) standing zombie, repeated hits: does it ever stop advancing? ------------- */
    printf("\n== (b) stehender Zombie, wiederholte Treffer ==\n");
    re15_actor_t *y = setup(2, 0x10, 4000);   /* lots of HP so it never dies */
    f = 0;
    for (int shot = 0; shot < 6; shot++) {
        y->hit_react = 0;
        re15_enemy_take_damage(y, 0);
        printf("-- Schuss %d --\n", shot);
        int prev_mo = -1, prev_af = -1, stall = 0, worst = 0;
        for (int i = 0; i < 90; i++) {
            re15_enemy_ai_live_tick(2); re15_actors_anim_advance(); f++;
            if (y->motion == prev_mo && y->anim_frame == prev_af) { stall++; if (stall > worst) worst = stall; }
            else stall = 0;
            prev_mo = y->motion; prev_af = y->anim_frame;
            if (i < 6) line("  ", f, y);
        }
        printf("   Ende: st=%d ss1=%d ss2=%d ss3=%d mo=%d af=%d  laengster Stillstand=%d Ticks\n",
               y->state, y->sub_state_1, y->sub_state_2, y->sub_state_3, y->motion, y->anim_frame, worst);
    }
    return 0;
}
