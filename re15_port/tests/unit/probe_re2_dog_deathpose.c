/**
 * @file probe_re2_dog_deathpose.c
 * @brief MESS-Sonde (kein PIN): welches Bild haelt der Port-Hund am Ende der Todeskette?
 *
 * Prueft NUR nach, misst nicht mehr: EDD-Clip-Laengen der echten RE2-EM020-Bank
 * (CDEMD0.EMS kind 0x20 rec 3) + die gefahrene Kette state 3 / Phase 2 (0x80104200)
 * bis state 7 (CORPSE 0x801049EC) ueber den ECHTEN re15_re2dog_tick, inklusive
 * des globalen Advancers (re15_actors_anim_advance), wie im game_step.
 */
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re2_ems.h"
void re15_actors_anim_advance(void);

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
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

int main(void)
{
    long ems_sz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
    if (!ems) { fprintf(stderr, "cannot open RE2/CDEMD0.EMS\n"); return 1; }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(0);

    re15_enemy_bank_t *eb = re15_enemy_find(0x20);
    if (!eb) eb = re15_enemy_alloc(0x20);
    if (!eb || re2_ems_load_bank(ems, (size_t)ems_sz, 0x20, eb, NULL) != 0) {
        fprintf(stderr, "re2_ems_load_bank failed\n"); return 1;
    }
    eb->buf = NULL; eb->ok = 1;

    printf("== Bank kind 0x20 (CDEMD0.EMS rec 3) ==\n");
    printf("  anim.clip_count = %d   anim_loco.clip_count = %d (loco_ok=%d)\n",
           eb->anim.clip_count, eb->anim_loco.clip_count, eb->loco_ok);
    for (int c = 15; c <= 19 && c < eb->anim.clip_count; c++)
        printf("  clip %2d: frame_count=%3d first_frame=%d\n",
               c, eb->anim.clips[c].frame_count, eb->anim.clips[c].first_frame);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;

    const int slot = 1;
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x20; e->x = 2000; e->z = 0;
    /* Genau der Zustand nach P0 0x80103344 (Pfad +0x219==0): state 3, +0x6=2, +0x7=0,
     * Anim-Wort 0x00070011 = Clip 17 / Bild 0 / frac 7, +0x220 != 0 (kein Rutschen). */
    e->state = 3; e->sub_state_1 = 0; e->sub_state_2 = 2; e->sub_state_3 = 0;
    e->motion = 17; e->anim_frame = 0; e->anim_frac = 7;
    e->hp = -7; e->re2z_prev_hp = -7;
    e->re2d_rel220 = 1;                    /* @0x8010421C-24: kein Move/Speed-Down */
    e->re2z_t15a = 0x200;

    printf("\n== Kette (re15_re2dog_tick + re15_actors_anim_advance) ==\n");
    int corpse_at = -1;
    for (int t = 0; t < 40; t++) {
        printf("  t=%2d  state=%d sub2=%d sub3=%d  clip=%2d frame=%2d frac=%d\n",
               t, e->state, e->sub_state_2, e->sub_state_3,
               (int)e->motion, (int)e->anim_frame, (int)e->anim_frac);
        if (e->state == 7 && corpse_at < 0) corpse_at = t;
        re15_re2dog_tick(slot);
        re15_actors_anim_advance();
    }
    printf("\n  CORPSE erstmals sichtbar bei t=%d; Endstand clip=%d frame=%d\n",
           corpse_at, (int)e->motion, (int)e->anim_frame);
    return 0;
}
