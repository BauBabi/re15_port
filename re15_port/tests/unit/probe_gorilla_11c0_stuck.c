/* probe_gorilla_11c0_stuck.c — MESSUNG (Diagnose 2026-09-05, Symptom 5): Nutzer-Report
 * "Die Gorillas beim Parking lot koennen kaum angreifen, da sie immer zwischen den Autos
 * haengen bleiben." (ROOM11C0, Typ 0x27, RE1.5-Brain in BEIDEN AI-Modi.)
 *
 * DIAGNOSE-THESE (diag_gorilla_stuck.md, alle @0x aus eigenem Disasm dort):
 *   Das Original ueberquert die Autos per ZONEN-LEAP: A[4]-Decide @0x80117ec8-0x80118024
 *   fragt per FUN_8003b93c SCA-Markerzellen (u1==0x10/0x20, u0=0) ab und committet
 *   +0x5=7/+0x7=1|3; B[7] snapt den Yaw auf die Pad-Richtung und setzt +0x82=1 (Band 1)
 *   @0x80118af0, Landung +0x82=0 @0x80118ca4. Der Port fuehrt die Entscheidung als OPEN
 *   (enemy_ai_common.c:8401-8410) — sie feuert nie; der Dauer-Wandkontakt (sca_wall_hit)
 *   sperrt byte-true REAR-UP (@0x80117b18) und Fern-LEAP (@0x80118054).
 *
 * GEMESSEN WIRD (Messplan Dossier §5): Kampf-Layout (Flag Zone4/Bit 0x40 -> 2x 0x27
 * grid 0x10 @(-9013,-15461)/(9434,2189)), Spieler-Anker West-Gasse, 3600 Frames:
 *   - Klemm-Quote je Gorilla (Frames mit sca_wall_hit!=0 in CHASE/SELECTOR)
 *   - Zonen-Leap-Zaehler (Eintritte sub1==7 && sub3!=0) — Erwartung HEUTE: exakt 0
 *   - Angriffs-Eintritte (sub 5 BITE / 6 HEAVY / 15 REAR-UP) pro 3600 Frames
 *   - Netto-Weg pro 300-Frame-Fenster (Haenger = < ~500 Einheiten) */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_player.h"
#include "re15_collision.h"
#include "re15_aot.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* EM027-Bank laden (Muster probe_laned_1030_spawn.c) */
static int load_em_bank(const char *base, uint8_t type)
{
    static uint8_t s_em[0x80000];
    char emsp[600]; size_t ems_size = 0;
    snprintf(emsp, sizeof emsp, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(emsp, &ems_size);
    if (!ems) { fprintf(stderr, "WARN: CDEMD0.EMS nicht lesbar\n"); return 0; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_size, idx, &off, &len) == 0 &&
        len <= sizeof s_em) {
        re15_enemy_bank_t *eb = re15_enemy_alloc(type);
        if (eb) {
            memcpy(s_em, ems + off, len);
            re15_tim_t tim = (re15_tim_t){0};
            if (re15_emd_parse_container(s_em, len, &eb->md1, &eb->skel,
                                         &eb->anim, &tim) == 0) {
                eb->ok = 1; eb->buf = NULL; ok = 1;
                eb->loco_ok = (re15_emd_parse_loco_bank(s_em, len,
                                  &eb->skel_loco, &eb->anim_loco) == 0);
                eb->own_ok = (re15_emd_parse_own_bank(s_em, len,
                                  &eb->skel_own, &eb->anim_own) == 0);
            }
        }
    }
    free(ems);
    return ok;
}

static void tick_once(int32_t px, int32_t pz)
{
    scd_vm_tick();
    re15_aot_scan(px, pz, 0);
    re15_actor_step_all_walkers();
    re15_actors_anim_advance();
    re15_enemy_ai_run_all(0);
}

typedef struct {
    int slot;
    long clamp_frames, chase_frames;
    int zone_leaps, plain_leaps, bites, heavies, rearups;
    uint8_t prev_sub;
    int32_t win_x, win_z; long win_travel_min; int windows;
} gstat_t;

int main(int argc, char **argv)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t sz = 0;
    int ticks = (argc > 1) ? atoi(argv[1]) : 3600;

    snprintf(path, sizeof path, "%s/STAGE1/ROOM11C0.RDT", base);
    uint8_t *dat = read_file(path, &sz);
    if (!dat) { fprintf(stderr, "FAIL: %s nicht lesbar\n", path); return 1; }
    static re15_rdt_t rdt;
    if (re15_rdt_parse(dat, sz, &rdt) != 0) { fprintf(stderr, "FAIL: parse\n"); return 1; }

    g_room_rdt    = rdt;           /* Pflicht: sonst laeuft der SCA-Wand-Clamp nicht */
    g_room_rdt_ok = 1;

    re15_actor_init();
    scd_vm_init();
    re15_enemy_reset();
    g_current_room_id = 0x11c0;
    re15_game_flag_set(4, 0x40, 1);              /* Kampf-Layout: else-Zweig spawnt 2x 0x27 */

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -14000; pl->z = 6000; pl->y = 0; pl->rot_y = 0;   /* West-Gasse (Dossier §5) */
    scd_room_reenter(&rdt, pl->x, pl->z, 0);

    int bank_ok = load_em_bank(base, 0x27);
    printf("[setup] EM027-Bank geladen = %d\n", bank_ok);

    for (int f = 0; f < 8; f++) scd_vm_tick();

    gstat_t gs[4]; int ng = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && ng < 4; s++)
        if (g_actors[s].active && g_actors[s].type == 0x27) {
            memset(&gs[ng], 0, sizeof gs[ng]);
            gs[ng].slot = s; gs[ng].prev_sub = g_actors[s].sub_state_1;
            gs[ng].win_x = g_actors[s].x; gs[ng].win_z = g_actors[s].z;
            gs[ng].win_travel_min = 0x7fffffff;
            ng++;
        }
    printf("[setup] %d Gorillas gespawnt:", ng);
    for (int i = 0; i < ng; i++)
        printf("  slot %d @(%ld,%ld) grid=0x%02x", gs[i].slot,
               (long)g_actors[gs[i].slot].x, (long)g_actors[gs[i].slot].z,
               g_actors[gs[i].slot].grid_id);
    printf("\n");
    if (ng == 0) { printf("!! keine 0x27-Spawns — Flag-Aufbau pruefen\n"); return 1; }

    for (int f = 0; f < ticks; f++) {
        pl->x = -14000; pl->z = 6000; pl->hp = 100; pl->hit_react = 0;
        tick_once(pl->x, pl->z);
        for (int i = 0; i < ng; i++) {
            re15_actor_t *e = &g_actors[gs[i].slot];
            if (!e->active) continue;
            uint8_t s1 = e->sub_state_1;
            if (e->state == 1 && (s1 == 3 || s1 == 4)) {
                gs[i].chase_frames++;
                if (e->sca_wall_hit) gs[i].clamp_frames++;
            }
            if (s1 != gs[i].prev_sub) {
                if (s1 == 7)  { if (e->sub_state_3) gs[i].zone_leaps++; else gs[i].plain_leaps++; }
                if (s1 == 5)  gs[i].bites++;
                if (s1 == 6)  gs[i].heavies++;
                if (s1 == 15) gs[i].rearups++;
                gs[i].prev_sub = s1;
            }
            if (((f + 1) % 300) == 0) {
                long dx = (long)e->x - gs[i].win_x, dz = (long)e->z - gs[i].win_z;
                long trav = labs(dx) + labs(dz);
                if (trav < gs[i].win_travel_min) gs[i].win_travel_min = trav;
                gs[i].win_x = e->x; gs[i].win_z = e->z; gs[i].windows++;
            }
        }
        if ((f % 600) == 0)
            for (int i = 0; i < ng; i++) {
                const re15_actor_t *e = &g_actors[gs[i].slot];
                printf("  t=%4d slot=%d st=%d s1=%d s2=%d s3=%d mo=%d pos=(%ld,%ld) "
                       "wall=%d contact=0x%02x lock=%d dist=%ld\n",
                       f, gs[i].slot, e->state, e->sub_state_1, e->sub_state_2,
                       e->sub_state_3, (int)e->motion, (long)e->x, (long)e->z,
                       e->sca_wall_hit, e->ai_contact, (int)e->dog_blocked_ctr,
                       (long)e->ai_dist);
            }
    }

    printf("\n== ERGEBNIS (%d Frames) ==\n", ticks);
    for (int i = 0; i < ng; i++) {
        const re15_actor_t *e = &g_actors[gs[i].slot];
        long q = gs[i].chase_frames ? (gs[i].clamp_frames * 100 / gs[i].chase_frames) : 0;
        printf("  slot %d: Klemm-Quote=%ld%% (%ld/%ld CHASE/SEL-Frames), ZONEN-LEAPS=%d "
               "(Soll heute 0, Original >0 am Pad), plain-Leaps=%d, BITE=%d HEAVY=%d "
               "REARUP=%d, min. Netto-Weg/300F=%ld, Ende s1=%d pos=(%ld,%ld)\n",
               gs[i].slot, q, gs[i].clamp_frames, gs[i].chase_frames,
               gs[i].zone_leaps, gs[i].plain_leaps, gs[i].bites, gs[i].heavies,
               gs[i].rearups, gs[i].win_travel_min, e->sub_state_1,
               (long)e->x, (long)e->z);
    }
    free(dat);
    return 0;
}
