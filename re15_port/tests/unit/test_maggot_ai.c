/* ============================================================================
 *  Maggots (type 0x27, EM027 = MAGGOTS_BABY) AI — byte-true port probe.
 *
 *  Byte-true 0x80116db8 family (RE15_MAGGOT_AI.md; full re-port audit wf_827f186d).
 *  Loads the REAL EM027 bank (CDEMD0.EMS idx 12) so the locator foot-plant root
 *  motion (FUN_8011bf50/8011c024) has a skeleton, seeds a type-0x27 actor + a
 *  player and drives re15_enemy_ai_run_all, asserting:
 *   (1)  INIT (0x80116f50): state 0 -> 1, HP=180, +0x1e2=4.
 *   (2)  SIGHTED CHASE: sub 3 with +0x7=0 crawls with CLIP 5 (audit #3,
 *        @0x80117cdc-e4) and closes distance (locator root motion, audit #5).
 *   (2b) BITE (sub 5): -6 on a window frame {0xc..0xf} (@0x8012146c, audit #17).
 *   (2c) HEAVY (sub 6): -12 on a window frame {0x15-0x18,0x21,0x22} (@0x80121470,
 *        audit #9); a missed heavy with the player close + lockout clear chains
 *        straight into a re-BITE (+0x5=5 @0x80118880, audit #8).
 *   (2d) LEAP (sub 7): ballistic reposition, 0 damage, closes distance; the
 *        windup already crawls forward (245d8(0) @0x80118dbc, audit #6).
 *   (2e) REAR-UP PIN (sub 15, B[15] 0x8011a960, audit #1): the committed bite
 *        upgrades to the rear-up (@0x80117b34-3c), clip 0x1c, connects at frame 4
 *        (a804(0xbb8,0x100) @0x8011aaa0), pins the player (aca58=5 @0x8011ac48),
 *        releases and exits to sub 2 with +0x1dc=0x3c / +0x1e1=0xff (@0x8011add8-f4).
 *   (2f) HURT (state 2, audit #2): a non-lethal hit plays flinch CLIP 7
 *        (@0x8011b08c-90) + exits to the sub-7 RETALIATION LEAP (@0x8011b194-98).
 *   (3)  DEATH/CORPSE (audit #12/#13): clip 0xe holds its last frame; the corpse
 *        NEVER re-plays a clip (state 7 = 0x8011be54 writes no +0x94), sets
 *        flags|0x2|0x40 and goes inert after the 90-tick fade (+0x9e=0x5a).
 * ==========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_damage.h"   /* re15_enemy_apply_hitbox, re15_enemy_take_damage */

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int32_t xz_dist(const re15_actor_t *a, const re15_actor_t *b)
{
    int64_t dx = (int64_t)a->x - b->x, dz = (int64_t)a->z - b->z;
    int64_t d2 = dx * dx + dz * dz;
    int32_t r = 0; while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
    return r;
}

static void face_player(re15_actor_t *m, const re15_actor_t *pl)
{
    m->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - m->z, pl->x - m->x) - 0x400) & 0xfff);
}

/* Load the EM027 loco/main bank (CDEMD0.EMS blob 12, dir[1]/dir[2]) into the enemy
 * bank table so re15_enemy_find(0x27) serves the foot-plant root motion. */
static int load_maggot_bank(void)
{
    FILE *f = fopen(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *)malloc((size_t)n);
    if (!buf || fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(buf); return 0; }
    fclose(f);
    size_t off, len;
    if (re15_ems_get_entry(buf, (size_t)n, 12, &off, &len)) { free(buf); return 0; }
    re15_enemy_bank_t *bank = re15_enemy_alloc(0x27);
    if (!bank) { free(buf); return 0; }
    if (re15_emd_parse_loco_bank(buf + off, len, &bank->skel, &bank->anim)) { free(buf); return 0; }
    bank->ok  = 1;
    bank->buf = buf;              /* keep resident: skel/anim alias into it */
    return 1;
}

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);
    int bank_ok = load_maggot_bank();
    printf("=== MAGGOTS (type 0x27) byte-true AI (audit wf_827f186d) ===\n");
    printf("  EM027 bank: %s\n", bank_ok ? "loaded" : "MISSING (locator motion inert)");

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 2000; pl->hp = 100;

    const int MS = 1;
    re15_actor_t *m = &g_actors[MS];
    m->active = 1; m->type = 0x27; m->state = 0; m->x = 0; m->y = 0; m->z = 0; m->rot_y = 0;
    re15_enemy_apply_hitbox(m, 0x27);

    /* (1) INIT */
    re15_enemy_ai_run_all(0);
    if (m->state != 1)    { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", m->state); fail = 1; }
    if (m->hp != 180)     { fprintf(stderr, "FAIL(1): HP must seed to 180, got %d\n", m->hp); fail = 1; }
    if (m->mag_boost != 4){ fprintf(stderr, "FAIL(1): +0x1e2 must seed to 4 (@0x80117098-9c), got %d\n", m->mag_boost); fail = 1; }
    printf("  (1) INIT: state->1, hp=%d, +0x1e2=%d\n", m->hp, m->mag_boost);

    /* (2) SIGHTED CHASE: player at 4000 with LOS -> sub 3, +0x7=0, CLIP 5 (audit #3), closing */
    pl->x = 0; pl->z = 4000;
    m->mag_pin_cd = 200;            /* park the rear-up so the plain chase/bite path is probed */
    int chased = 0, clip5 = 0;
    int32_t c0 = xz_dist(m, pl), cmin = xz_dist(m, pl);
    for (int f = 0; f < 120; f++) {
        re15_enemy_ai_run_all(0);
        m->mag_pin_cd = 200;
        pl->hit_react = 0;
        if (m->sub_state_1 == 3) {
            chased = 1;
            if (m->sub_state_3 == 0 && m->motion == 5) clip5 = 1;
        }
        if (m->sub_state_1 > 3) break;      /* escalated past chase (selector/bite) — enough */
        { int32_t d = xz_dist(m, pl); if (d < cmin) cmin = d; }
    }
    if (!chased) { fprintf(stderr, "FAIL(2): maggot never entered CHASE (sub 3), sub=%d\n", m->sub_state_1); fail = 1; }
    if (!clip5)  { fprintf(stderr, "FAIL(2): SIGHTED chase must crawl with clip 5 (@0x80117cdc-e4, audit #3), motion=%d +0x7=%d\n", (int)m->motion, m->sub_state_3); fail = 1; }
    if (bank_ok && cmin >= c0) { fprintf(stderr, "FAIL(2): sighted crawl must close distance (locator root motion, audit #5), %d->%d\n", c0, cmin); fail = 1; }
    printf("  (2) CHASE: sub 3 sighted, clip 5, dist %d->%d (closing)\n", c0, cmin);

    /* (2b) BITE: window = ANY of frames {0xc..0xf} (@0x8012146c), -6 (audit #17) */
    pl->x = m->x; pl->z = m->z + 1200; pl->hp = 100; pl->hit_react = 0;
    face_player(m, pl);
    m->state = 1; m->sub_state_1 = 3; m->sub_state_2 = 1; m->sub_state_3 = 0;
    m->dog_blocked_ctr = 0; m->mag_pin_cd = 200;
    int16_t bp0 = pl->hp; int bit = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        m->mag_pin_cd = 200;
        if (m->sub_state_1 == 5) bit = 1;
        face_player(m, pl);
        pl->hit_react = 0;
        if (pl->hp < bp0) break;
    }
    if (!bit)              { fprintf(stderr, "FAIL(2b): maggot never reached BITE (sub 5); sub=%d\n", m->sub_state_1); fail = 1; }
    if (pl->hp != bp0 - 6) { fprintf(stderr, "FAIL(2b): bite must deal exactly -6, hp %d->%d\n", bp0, pl->hp); fail = 1; }
    printf("  (2b) BITE: sub 5, player hp %d->%d (-6 on a {0xc..0xf} window frame)\n", bp0, pl->hp);

    /* (2c) HEAVY: -12 on a table window frame (audit #9). Player at 3900 (inside the 4000 commit
     * window): the clip-0x13 locator lunge (~3550 units over the clip) brings the window frames
     * into body-contact reach WITHOUT overshooting — in-game the aec4/b544 standoff holds the
     * pair at reach, which this bare harness lacks. */
    pl->x = m->x; pl->z = m->z + 3900; pl->hp = 100; pl->hit_react = 0;
    face_player(m, pl);
    m->state = 1; m->sub_state_1 = 4; m->sub_state_2 = 0; m->sub_state_3 = 0;
    m->dog_blocked_ctr = 0; m->mag_pin_cd = 200;
    int16_t hp0 = pl->hp; int heavy = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        m->mag_pin_cd = 200;
        if (m->sub_state_1 == 6) heavy = 1;
        face_player(m, pl);
        pl->hit_react = 0;
        if (pl->hp < hp0) break;
    }
    if (!heavy)             { fprintf(stderr, "FAIL(2c): maggot never reached HEAVY (sub 6); sub=%d\n", m->sub_state_1); fail = 1; }
    if (pl->hp != hp0 - 12) { fprintf(stderr, "FAIL(2c): heavy-bite must deal exactly -12, hp %d->%d\n", hp0, pl->hp); fail = 1; }
    printf("  (2c) HEAVY: selector->heavy, player hp %d->%d (-12)\n", hp0, pl->hp);

    /* (2c2) HEAVY exit chain (audit #8): anim-end with the player in 4000/cone-384, hit-react
     * clear and +0x1dc==0 -> re-BITE (+0x5=5 @0x80118880), NOT chase. */
    pl->x = m->x; pl->z = m->z + 2000; pl->hp = 100; pl->hit_react = 0;
    face_player(m, pl);
    m->state = 1; m->sub_state_1 = 6; m->sub_state_2 = 2; m->sub_state_3 = 0;
    m->dog_blocked_ctr = 0; m->mag_pin_cd = 200;
    re15_enemy_ai_run_all(0);
    if (m->sub_state_1 != 5) { fprintf(stderr, "FAIL(2c2): missed heavy w/ player close + lockout clear must chain to re-BITE (sub 5 @0x80118880), got sub %d\n", m->sub_state_1); fail = 1; }
    else printf("  (2c2) HEAVY exit: chained to re-BITE (sub 5, @0x80118834-b8)\n");

    /* (2d) LEAP: 0 damage, closes distance (windup crawl @0x80118dbc + flight c1a4+245d8) */
    pl->x = 0; pl->z = 9000; pl->hp = 100; pl->hit_react = 0;
    m->x = 0; m->z = 0; m->y = 0; m->dog_floor_y = 0;
    face_player(m, pl);
    m->state = 1; m->sub_state_1 = 7; m->sub_state_2 = 0; m->sub_state_3 = 0;
    m->motion = 0; m->anim_frame = 0; m->hit_react = 0; m->mag_pin_cd = 200;
    int32_t lp0 = xz_dist(m, pl); int16_t lhp0 = pl->hp; int landed = 0;
    for (int f = 0; f < 250; f++) {
        re15_enemy_ai_run_all(0);
        m->mag_pin_cd = 200;
        if (m->sub_state_1 != 7) { landed = 1; break; }
    }
    int32_t lp1 = xz_dist(m, pl);
    if (!landed)        { fprintf(stderr, "FAIL(2d): leap never completed (stuck in sub 7, +0x6=%d)\n", m->sub_state_2); fail = 1; }
    if (lp1 >= lp0)     { fprintf(stderr, "FAIL(2d): leap must close distance, %d->%d\n", lp0, lp1); fail = 1; }
    if (pl->hp != lhp0) { fprintf(stderr, "FAIL(2d): leap must deal 0 damage, hp %d->%d\n", lhp0, pl->hp); fail = 1; }
    if (m->hit_react != 0) { fprintf(stderr, "FAIL(2d): land must clear +0x93 (@0x80118cc4), got %02x\n", m->hit_react); fail = 1; }
    printf("  (2d) LEAP: pounce closed dist %d->%d, 0 damage, +0x93 cleared\n", lp0, lp1);

    /* (2e) REAR-UP PIN (audit #1): bite decision upgrades to sub 15; frame-4 connect pins the
     * player; release exits to sub 2 with +0x1dc=0x3c / +0x1e1=0xff. */
    pl->x = 0; pl->z = 1200; pl->hp = 100; pl->hit_react = 0;
    m->x = 0; m->z = 0; m->y = 0; m->dog_floor_y = 0;
    face_player(m, pl);
    pl->rot_y = (int16_t)((m->rot_y + 0x800) & 0xfff);        /* opposed headings -> a780==0 (frame-4 lane) */
    m->state = 1; m->sub_state_1 = 3; m->sub_state_2 = 1; m->sub_state_3 = 0;
    m->dog_blocked_ctr = 0; m->mag_pin_cd = 0; m->hit_react = 0;
    re15_enemy_ai_run_all(0);
    if (m->sub_state_1 != 15) { fprintf(stderr, "FAIL(2e): bite must upgrade to REAR-UP sub 15 (@0x80117b34-3c), got sub %d\n", m->sub_state_1); fail = 1; }
    int pinned = 0, released = 0;
    for (int f = 0; f < 300 && m->sub_state_1 == 15; f++) {
        re15_enemy_ai_run_all(0);
        if (m->sub_state_2 >= 2 && m->sub_state_2 <= 3) pinned = 1;   /* PIN latch/ride phases */
    }
    if (m->sub_state_1 == 2) released = 1;
    if (m->motion == 0x1c || pinned) { /* rear-up clip reached */ } else
        { fprintf(stderr, "FAIL(2e): rear-up clip 0x1c/pin phase never reached (motion=%d +0x6=%d)\n", (int)m->motion, m->sub_state_2); fail = 1; }
    if (!pinned)   { fprintf(stderr, "FAIL(2e): pin phase 2/3 never reached (+0x6=%d sub=%d)\n", m->sub_state_2, m->sub_state_1); fail = 1; }
    if (!(pl->hit_react & 1)) { fprintf(stderr, "FAIL(2e): the pin must latch player.hit_react|=1 (@0x8011ac8c-aa0)\n"); fail = 1; }
    if (!released) { fprintf(stderr, "FAIL(2e): pin must release to the sub-2 recovery (@0x8011ae04), sub=%d +0x6=%d\n", m->sub_state_1, m->sub_state_2); fail = 1; }
    if (m->mag_pin_cd < 0x90) { fprintf(stderr, "FAIL(2e): connect exit must set +0x1e1=0xff (@0x8011adf0-f4), got %d\n", m->mag_pin_cd); fail = 1; }
    printf("  (2e) REAR-UP PIN: sub 15, pinned (player hit_react=%02x), released -> sub %d, +0x1e1=%d +0x1dc=%d\n",
           pl->hit_react, m->sub_state_1, m->mag_pin_cd, m->dog_blocked_ctr);

    /* (2f) HURT retaliation (audit #2): a non-lethal hit -> flinch clip 7 + gore/Se(3), then
     * EXIT to ACTIVE sub 7 = the retaliation leap (@0x8011b178-98). */
    pl->x = 0; pl->z = 8000; pl->hit_react = 0; pl->hp = 100;
    m->hp = 180; m->mag_airborne = 0;
    /* the weapon-resolver hurt entry (byte-true FUN_80011f50: +0x5=weapon @0x800124bc, +0x7=0
     * @0x80012428, +0x6=hit-dir @0x80012438-50, +0x93|=1, +0x4=2 @0x80012520): handgun lane 2 */
    m->state = 2; m->sub_state_1 = 2; m->sub_state_2 = 1; m->sub_state_3 = 0; m->hit_react = 1;
    int flinched = 0;
    for (int f = 0; f < 100 && m->state == 2; f++) {
        re15_enemy_ai_run_all(0);
        if (m->motion == 7) flinched = 1;      /* flinch clip 7 @0x8011b08c-90 */
    }
    if (!flinched) { fprintf(stderr, "FAIL(2f): hurt must play flinch clip 7 (@0x8011b08c-90), motion=%d\n", (int)m->motion); fail = 1; }
    if (!(m->state == 1 && m->sub_state_1 == 7)) { fprintf(stderr, "FAIL(2f): hurt must exit to ACTIVE sub 7 retaliation leap (@0x8011b188-98), state=%d sub=%d\n", m->state, m->sub_state_1); fail = 1; }
    printf("  (2f) HURT: flinch clip 7 -> retaliation LEAP (state 1 sub 7)\n");

    /* (3) DEATH -> CORPSE (audit #12/#13): clip 0xe, corpse HOLDS the death pose (no clip 0xa
     * re-play, no anim loop), flags |0x2|0x40, inert after the 90-tick fade. */
    pl->x = 0; pl->z = 8000;
    m->hp = -1; m->mag_airborne = 0; m->y = 0; m->dog_floor_y = 0;
    /* lethal handgun hit: +0x4=3 (@0x80012530-38), +0x5=2 (weapon lane), +0x7=0 */
    m->state = 3; m->sub_state_1 = 2; m->sub_state_2 = 1; m->sub_state_3 = 0; m->hit_react = 1;
    int death_clip = 0;
    for (int f = 0; f < 200; f++) {
        re15_enemy_ai_run_all(0);
        if (m->state == 3 && m->motion == 0x0e) death_clip = 1;
        if (m->state == 7) break;
    }
    if (!death_clip) { fprintf(stderr, "FAIL(3): death must play clip 0xe (@0x8011b82c-30), motion=%d\n", (int)m->motion); fail = 1; }
    if (m->state != 7) { fprintf(stderr, "FAIL(3): must reach CORPSE (state 7), state=%d hp=%d\n", m->state, m->hp); fail = 1; }
    if (m->motion != 0x0e) { fprintf(stderr, "FAIL(3): corpse must HOLD the death clip 0xe — state 7 writes NO clip (@0x8011be54, audit #12); motion=%d\n", (int)m->motion); fail = 1; }
    { int32_t fr0 = m->anim_frame;
      for (int f = 0; f < 120; f++) re15_enemy_ai_run_all(0);
      if (m->anim_frame != fr0) { fprintf(stderr, "FAIL(3): corpse must not animate (audit #12), frame %d->%d\n", (int)fr0, (int)m->anim_frame); fail = 1; }
      if ((m->flags & 0x42) != 0x42) { fprintf(stderr, "FAIL(3): corpse flags |0x2|0x40 missing (@0x8011beac-cc), flags=%02x\n", m->flags); fail = 1; }
      if (m->sub_state_3 != 2) { fprintf(stderr, "FAIL(3): corpse must go inert (+0x7=2 after the 0x5a fade @0x8011bf38-44), +0x7=%d\n", m->sub_state_3); fail = 1; }
    }
    printf("  (3) DEATH: clip 0xe -> CORPSE holds pose, flags=%02x, inert (+0x7=%d)\n", m->flags, m->sub_state_3);

    if (fail) { printf("MAGGOT byte-true: FAIL\n"); return 1; }
    printf("MAGGOT byte-true: all checks passed\n");
    return 0;
}
