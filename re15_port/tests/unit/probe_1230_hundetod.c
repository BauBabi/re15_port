/**
 * @file probe_1230_hundetod.c
 * @brief MESS-Sonde (kein PIN) — Nutzer-Befund 2026-09-14 "Zombiehunde laufen nicht aus,
 *        wenn sie tot sind" (ROOM1230, Bild 1001, beide Hunde st=7 ss=1/0 clip=18 bild=8,
 *        einer mit spd=30).
 *
 * Sie faehrt den ECHTEN Weg: ROOM1230.RDT + SCD-Spawn + echte RE2-EM020-Bank, RE2-Flavor,
 * stempelt den Tod genau so, wie der RE2-Applier FUN_800470C0 es tut (`sw 3,4(s1)` nullt
 * +0x5/+0x6/+0x7, danach `sb weapon,5(s1)`), und protokolliert dann BILD FUER BILD:
 *   state / sub1 / sub2 / sub3 / clip / anim_frame / speed_h / x / z / rel220 / t158
 * plus den zurueckgelegten Weg (Rutschstrecke) und den Frame, in dem CORPSE (state 7)
 * erreicht wird.
 *
 * argv: [weapon=0] [frames=60]
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re2_ems.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_tim.h"

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

int main(int argc, char **argv)
{
    int weapon = (argc > 1) ? atoi(argv[1]) : 0;
    int frames = (argc > 2) ? atoi(argv[2]) : 60;
    int flav   = (argc > 3) ? atoi(argv[3]) : 2;   /* 2 = RE2, 1 = RE1.5 */
    int d223   = (argc > 4) ? atoi(argv[4]) : -1;  /* >=0: +0x223 vor dem Kill setzen */
    int air    = (argc > 5) ? atoi(argv[5]) : -1;  /* >=0: +0x219 (Luft) vor dem Kill */

    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1230.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "cannot open %s\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        fprintf(stderr, "RDT parse failed\n"); return 1;
    }
    long ems_sz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
    if (!ems) { fprintf(stderr, "cannot open CDEMD0.EMS\n"); return 1; }

    re15_ai_flavor_set(flav == 2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);

    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();
    if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;

    int dslots[RE15_ACTOR_MAX], nd = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) dslots[nd++] = s;
    printf("ROOM1230: %d Hunde\n", nd);
    if (nd < 1) return 1;

    if (flav == 2) {
        re15_enemy_bank_t *eb = re15_enemy_find(0x20);
        if (!eb) eb = re15_enemy_alloc(0x20);
        if (eb && re2_ems_load_bank(ems, (size_t)ems_sz, 0x20, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    } else {
        /* RE1.5-EM020-Bank (Gegenprobe) — wie probe_1010_kriecher.c load_bank_re15 */
        static uint8_t blob[0x80000];
        long n15 = 0; uint8_t *e15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n15);
        int idx = re15_ems_index_for_type(0x20); size_t off = 0, len = 0;
        if (e15 && idx >= 0 && re15_ems_get_entry(e15, (size_t)n15, idx, &off, &len) == 0
            && len <= sizeof blob) {
            re15_enemy_bank_t *eb = re15_enemy_find(0x20);
            if (!eb) eb = re15_enemy_alloc(0x20);
            memcpy(blob, e15 + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (eb && re15_emd_parse_container(blob, len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL;
                re15_emd_parse_own_bank(blob, len, &eb->skel_own, &eb->anim_own);
                eb->own_ok = (eb->anim_own.clip_count > 0);
                eb->loco_ok = (re15_emd_parse_loco_bank(blob, len, &eb->skel_loco, &eb->anim_loco) == 0);
            }
        }
    }
    {   re15_enemy_bank_t *eb = re15_enemy_find(0x20);
        printf("Bank: ok=%d clips=%d  clip17=%d clip18=%d\n",
               eb ? eb->ok : -1,
               eb ? eb->anim.clip_count : -1,
               (eb && eb->anim.clip_count > 17) ? eb->anim.clips[17].frame_count : -1,
               (eb && eb->anim.clip_count > 18) ? eb->anim.clips[18].frame_count : -1);
        if (eb && eb->ok)
            for (int c = 0; c < eb->anim.clip_count; c++)
                printf("   clip %2d len=%d\n", c, eb->anim.clips[c].frame_count);
    }

    /* nur EIN Hund, die anderen abschalten */
    for (int i = 1; i < nd; i++) g_actors[dslots[i]].active = 0;
    re15_actor_t *d = &g_actors[dslots[0]];

    /* ein paar Frames leben lassen, damit INIT durch ist */
    for (int f = 0; f < 8; f++) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }
    printf("vor Kill: state=%d sub=%d/%d/%d clip=%d frame=%d spd=%d pos=(%d,%d,%d) rot=%d hp=%d\n",
           d->state, d->sub_state_1, d->sub_state_2, d->sub_state_3, (int)d->motion,
           (int)d->anim_frame, (int)d->speed_h, d->x, d->y, d->z, (int)d->rot_y, (int)d->hp);

    /* TODES-STEMPEL == FUN_800470C0: `sw 3,4(s1)` (@0x80047290) nullt +0x5/+0x6/+0x7,
     * danach `sb weapon,5(s1)` (@0x80047324). HP < 0 (`lh v1,342`). */
    if (d223 >= 0) d->re2d_dbl223  = (uint8_t)d223;
    if (air  >= 0) d->re2d_air219   = (uint8_t)air;
    d->hp = -1;
    d->state = 3; d->sub_state_1 = (uint8_t)weapon; d->sub_state_2 = 0; d->sub_state_3 = 0;
    d->re2z_prev_hp = d->hp;

    int32_t x0 = d->x, z0 = d->z;
    int corpse_frame = -1;
    for (int f = 0; f < frames; f++) {
        int32_t px = d->x, pz = d->z;
        re15_enemy_ai_run_all(1);
        re15_actors_anim_advance();
        int32_t dx = d->x - px, dz = d->z - pz;
        printf("f%-3d state=%d sub=%d/%d/%d clip=%2d frame=%2d frac=%d spd=%4d "
               "d=(%5d,%5d) pos=(%7d,%7d) rel220=%d t158=%d t15a=%d air=%d\n",
               f, d->state, d->sub_state_1, d->sub_state_2, d->sub_state_3,
               (int)d->motion, (int)d->anim_frame, (int)d->anim_frac, (int)d->speed_h,
               dx, dz, d->x, d->z,
               (int)d->re2d_rel220, (int)d->re2z_t158, (int)d->re2z_t15a, (int)d->re2d_air219);
        if (corpse_frame < 0 && d->state == 7) corpse_frame = f;
    }
    printf("== CORPSE bei Frame %d; Rutschstrecke gesamt dx=%d dz=%d\n",
           corpse_frame, d->x - x0, d->z - z0);
    return 0;
}
