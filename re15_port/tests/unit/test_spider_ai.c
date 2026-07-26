/* ============================================================================
 *  Spider-Baby (type 0x26, EM026) AI — byte-true probe (audit wf_827f186d).
 *
 *  Byte-true 0x80116288 family (RE15_SPIDER_AI.md + raw STAGE1.BIN disasm). A
 *  STATIONARY web-spitter pinned to its cached home position every tick:
 *   (1) INIT (0x801164b0): state 0 -> 1 (ACTIVE), HP=100, variant from grid&0x7f;
 *       emerge frame (phase 0 < 13) sets the intangibility flags 2|0x40
 *       (@0x801163dc-404) and pins y = home_y + 20 (@0x80116494-98).
 *   (2) ACTIVE: phase jumps to 0x28 (@0x80116784) -> flags cleared (@0x80116330-5c),
 *       y slaved to home_y - 20*(phase-1) = -780; x/z pinned to home every tick
 *       (@0x80116408-98, audit #2). No locomotion.
 *   (3) CONTACT: aec4-return overlap deals -2 EVERY overlap tick, hp floor via
 *       `slti 4` (@0x801163b4-d0) — NOT hit_react-gated, ANY state (audit #3).
 *   (4) UNKILLABLE: state table @0x80121268 [2]=[3]=[4]=0x8011697c -> flinch
 *       0x80116a04 for HURT *and* hp<0 "DEATH": phase-- (floor 1, @0x80116ae4-b00),
 *       timer re-roll [16,79] (@0x80116cd4-ec, audit #5), reset to ACTIVE 0x10001
 *       (@0x80116b2c-5c). Never a corpse/gib state (audit #1 — the old state-8/9
 *       kill path this test used to PIN was invented and is removed).
 * ==========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_player_apply_hitbox */

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 8000; pl->hp = 100;  /* far away */
    re15_player_apply_hitbox(pl);   /* r 450 / h 1530 (PSX.EXE file 0x64694) — the aec4 y-band needs it */

    const int SS = 1;
    re15_actor_t *s = &g_actors[SS];
    s->active = 1; s->type = 0x26; s->state = 0; s->x = 3000; s->y = 0; s->z = 3000;
    s->grid_id = 0x02;   /* variant 2 (Behavior A), bit 0x80 clear -> emerge step 20 */
    s->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(s, 0x26);

    printf("=== SPIDER-BABY (type 0x26) byte-true AI ===\n");

    /* (1) INIT + emerge frame */
    re15_enemy_ai_run_all(0);
    if (s->state != 1)        { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", s->state); fail = 1; }
    if (s->hp != 100)         { fprintf(stderr, "FAIL(1): HP must seed to 100, got %d\n", s->hp); fail = 1; }
    if (s->sub_state_1 != 2)  { fprintf(stderr, "FAIL(1): +0x5 variant must seed from grid&0x7f=2, got %d\n", s->sub_state_1); fail = 1; }
    if ((s->flags & 0x42) != 0x42) { fprintf(stderr, "FAIL(1): emerge (phase<13) must set flags 2|0x40 (@0x801163dc-404), flags=0x%02x\n", s->flags); fail = 1; }
    if (s->y != 20)           { fprintf(stderr, "FAIL(1): emerge pin y = home_y - 20*(0-1) = +20 (@0x80116494-98), got %d\n", (int)s->y); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d, variant=%d, flags=0x%02x, y=%d\n", s->hp, s->sub_state_1, s->flags, (int)s->y);

    /* (2) ACTIVE: phase 0x28 -> solid + y slaved to home_y - 20*(phase-1) = -780 (audit #2) */
    re15_enemy_ai_run_all(0);
    if (s->spider_phase != 0x28) { fprintf(stderr, "FAIL(2): ACTIVE sub0 must seed phase 0x28 (@0x80116784), got %d\n", s->spider_phase); fail = 1; }
    if (s->flags & 0x42)      { fprintf(stderr, "FAIL(2): solid (phase>=13) must clear flags 2|0x40 (@0x80116330-5c), flags=0x%02x\n", s->flags); fail = 1; }
    if (s->y != -780)         { fprintf(stderr, "FAIL(2): ACTIVE y must be home_y - 20*(0x28-1) = -780 (@0x80116494-98), got %d\n", (int)s->y); fail = 1; }
    int32_t sx0 = s->x, sz0 = s->z;
    for (int f = 0; f < 120; f++) re15_enemy_ai_run_all(0);
    if (s->x != sx0 || s->z != sz0) { fprintf(stderr, "FAIL(2): x/z must stay pinned to home (@0x80116408-34), moved (%d,%d)->(%d,%d)\n", sx0, sz0, s->x, s->z); fail = 1; }
    /* the strike loop increments +0x1d0 per cycle (@0x8011681c) and y is SLAVED to it */
    if (s->y != -20 * ((int)s->spider_phase - 1)) { fprintf(stderr, "FAIL(2): y must track home_y - 20*(phase-1) (@0x80116494-98), phase=%d y=%d\n", s->spider_phase, (int)s->y); fail = 1; }
    printf("  (2) ACTIVE: phase=0x%02x, flags=0x%02x, pinned at (%d,%d), y=%d\n", s->spider_phase, s->flags, s->x, s->z, (int)s->y);

    /* (3) CONTACT: -2 EVERY overlap tick, not hit_react-gated (@0x801163b0-d0, audit #3) */
    pl->x = s->x; pl->z = s->z + 400; pl->hp = 100; pl->hit_react = 0;   /* 400 < 600+450 overlap */
    for (int f = 0; f < 3; f++) re15_enemy_ai_run_all(0);                /* NO hit_react clear between */
    if (pl->hp != 94)         { fprintf(stderr, "FAIL(3): -2 per overlap tick (3 ticks: 100->94), got %d\n", pl->hp); fail = 1; }
    pl->hp = 3;
    re15_enemy_ai_run_all(0);
    if (pl->hp != 3)          { fprintf(stderr, "FAIL(3): hp<4 skips the -2 (`slti 4` @0x801163c0), got %d\n", pl->hp); fail = 1; }
    printf("  (3) CONTACT: 3 overlap ticks 100->94; floor hp=3 held\n");

    /* (3b) contact continues during HURT (tail runs after EVERY dispatch, audit #3) */
    pl->hp = 50; pl->hit_react = 0;
    s->state = 2; s->sub_state_3 = 0;    /* as the EXE damage writer leaves it (@0x80013018, +0x7=0 @0x80012fd4) */
    re15_enemy_ai_run_all(0);
    if (pl->hp != 48)         { fprintf(stderr, "FAIL(3b): -2 must land while the spider flinches (no +0x4 read in the tail), got %d\n", pl->hp); fail = 1; }
    printf("  (3b) CONTACT during HURT: 50->48\n");
    pl->x = 0; pl->z = 8000;             /* move the player away */
    /* let the flinch finish (phases 1,2 -> reset) */
    re15_enemy_ai_run_all(0); re15_enemy_ai_run_all(0);
    if (s->state != 1)        { fprintf(stderr, "FAIL(3b): flinch must reset to ACTIVE 0x10001 (@0x80116b30), state=%d\n", s->state); fail = 1; }

    /* (4) UNKILLABLE: hp<0 -> state 3 -> SAME flinch -> back to ACTIVE, hp stays negative (audit #1) */
    uint8_t ph0 = s->spider_phase;
    s->hp = -5; s->state = 3; s->sub_state_3 = 0; s->hit_react = 1;   /* EXE death write @0x80013020 */
    re15_enemy_ai_run_all(0);   /* flinch phase 0 */
    if (s->hit_react != 3)    { fprintf(stderr, "FAIL(4): flinch[0] must set +0x93=3 (@0x80116a50), got %d\n", s->hit_react); fail = 1; }
    if (s->spider_phase != (uint8_t)(ph0 - 1)) { fprintf(stderr, "FAIL(4): flinch[0] must phase-- floor 1 (@0x80116ae4-b00): %d -> %d\n", ph0, s->spider_phase); fail = 1; }
    re15_enemy_ai_run_all(0);   /* flinch phase 1: commit + timer re-roll */
    if (s->spider_timer < 16 || s->spider_timer > 79) { fprintf(stderr, "FAIL(4): flinch[1] must re-roll +0x1d4 to [16,79] (@0x80116cd4-ec, audit #5), got %d\n", s->spider_timer); fail = 1; }
    re15_enemy_ai_run_all(0);   /* flinch phase 2: reset 0x10001 */
    if (s->state != 1 || s->sub_state_2 != 1 || s->sub_state_3 != 0) { fprintf(stderr, "FAIL(4): reset must be state 1 sub2=1 sub3=0 (word 0x10001 @0x80116b30), got %d/%d/%d\n", s->state, s->sub_state_2, s->sub_state_3); fail = 1; }
    if (s->sub_state_1 != 2)  { fprintf(stderr, "FAIL(4): reset must restore +0x5 = grid&0x7f (@0x80116b4c), got %d\n", s->sub_state_1); fail = 1; }
    if (s->hit_react != 0)    { fprintf(stderr, "FAIL(4): reset must clear +0x93 (@0x80116b5c), got %d\n", s->hit_react); fail = 1; }
    if (s->hp >= 0)           { fprintf(stderr, "FAIL(4): hp must STAY negative (no death mechanism exists), got %d\n", s->hp); fail = 1; }
    for (int f = 0; f < 100; f++) {
        re15_enemy_ai_run_all(0);
        if (s->state == 7 || s->state == 8 || s->state == 9) {
            fprintf(stderr, "FAIL(4): spider must NEVER reach a corpse/gib state (no +0x4 write of 5..9 in 0x80116288-0x80116db4), state=%d\n", s->state);
            fail = 1; break;
        }
    }
    printf("  (4) UNKILLABLE: hp=%d flinched and resumed ACTIVE (state %d), never a corpse\n", s->hp, s->state);

    /* (4b) phase floor 1: drain phase with repeated flinches -> re-emerge intangible, floor 1 */
    for (int k = 0; k < 64; k++) {
        s->state = 2; s->sub_state_3 = 0;
        re15_enemy_ai_run_all(0); re15_enemy_ai_run_all(0); re15_enemy_ai_run_all(0);
    }
    if (s->spider_phase != 1) { fprintf(stderr, "FAIL(4b): repeated hits must drain +0x1d0 to floor 1 (@0x80116ae4), got %d\n", s->spider_phase); fail = 1; }
    if ((s->flags & 0x42) != 0x42) { fprintf(stderr, "FAIL(4b): phase<13 must re-set the intangibility flags (@0x801163dc-404), flags=0x%02x\n", s->flags); fail = 1; }
    printf("  (4b) drain: phase floor=%d, re-emerged intangible (flags=0x%02x)\n", s->spider_phase, s->flags);

    if (fail) { printf("SPIDER: FAIL\n"); return 1; }
    printf("SPIDER: all checks passed\n");
    return 0;
}
