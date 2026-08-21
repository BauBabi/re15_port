/**
 * @file probe_re2_dog_kill.c
 * @brief MESS-Sonde (kein PIN) — erreicht der RE2-Hund im ECHTEN ROOM1190 den toedlichen
 *        Griff, wenn der Spieler mit voller HP anfaengt?
 *
 * Nutzer-Report: "im RE2-Modus klemmt die Spieler-HP bei 0 und der Hund geht in Sub 8
 * (Abbruch), statt die toedliche Verriegelung zu erreichen."
 *
 * Die Sonde faehrt den ECHTEN Weg: ROOM1190.RDT + SCD-Spawn + echte RE2-EM020-Bank +
 * re15_enemy_ai_run_all/re15_actors_anim_advance (game_step-Reihenfolge), Spieler HP 100,
 * kein erzwungener Zustand. Protokolliert wird pro Seed:
 *   - jeder HP-Abfall (Frame, Betrag, HP danach)
 *   - jeder Sub-Wechsel des beobachteten Hundes
 *   - ob Sub 7 (Latch) / Spieler-state 7 (Devour) je erreicht wird
 *
 * argv: [seeds=8] [frames=3000] [face=1]   face=1 Spieler schaut den Hund an
 *                                          (0x80015910(dog,PL)==0 -> Latch-Gate OFFEN),
 *                                          face=0 abgewandt.
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
#include "re15_skeleton.h"   /* re15_sin_q12/cos_q12 fuer die Bone-Vorwaerts-Messung */

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

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

static int bringup(const re15_rdt_t *rdt, int dslots[], int *nd, int seed_pumps)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    re15_re2z_rng_reset();
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    scd_thread_start(1, rdt->sub_scd[0]);
    if (rdt->sub_scd_count > 13 && rdt->sub_scd[13]) scd_thread_start(2, rdt->sub_scd[13]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    for (int i = 0; i < seed_pumps; i++) (void)re15_re2_rand();   /* Seed-Sweep = PRNG-Phase NACH
                                                                   * dem Spawn (davor verschluckt
                                                                   * der INIT die Phase) */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;
    *nd = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) dslots[(*nd)++] = s;
    return (*nd >= 1) ? 0 : -1;
}

int main(int argc, char **argv)
{
    int seeds  = (argc > 1) ? atoi(argv[1]) : 8;
    int frames = (argc > 2) ? atoi(argv[2]) : 3000;
    int face   = (argc > 3) ? atoi(argv[3]) : 1;
    int pin    = (argc > 4) ? atoi(argv[4]) : 0;   /* >0: Spieler jeden Frame in <pin> Einheiten
                                                    * vor den Hund setzen (Assist, erzwingt Bisse) */
    int verb   = (argc > 5) ? atoi(argv[5]) : 0;   /* 1 = NEAR/STATUS-Protokoll */
    int re2bnk = (argc > 6) ? atoi(argv[6]) : 1;   /* 0 = RE1.5-EM020-Bank behalten
                                                    * (Hybrid-Rig-Gegenprobe Bone 4) */

    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "cannot open %s\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        fprintf(stderr, "RDT parse failed\n"); free(buf); return 1;
    }

    static uint8_t *ems = NULL; static long ems_sz = 0;
    ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
    if (!ems) { fprintf(stderr, "cannot open CDEMD0.EMS\n"); return 1; }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    int n_latch = 0, n_devour = 0, n_hp0 = 0, n_hpneg = 0;
    for (int s = 0; s < seeds; s++) {
        int dslots[RE15_ACTOR_MAX], nd = 0;
        if (bringup(&rdt, dslots, &nd, s * 7) != 0) { printf("seed %d: no dogs\n", s); continue; }
        if (re2bnk) {   /* echte RE2-EM020-Bank (Clip-Laengen/Frame-Events) */
            re15_enemy_bank_t *eb = re15_enemy_find(0x20);
            if (!eb) eb = re15_enemy_alloc(0x20);
            if (eb && re2_ems_load_bank(ems, (size_t)ems_sz, 0x20, eb, NULL) == 0)
                { eb->buf = NULL; eb->ok = 1; }
        }
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        re15_actor_t *d0 = &g_actors[dslots[0]];
        /* nur EIN Hund, damit die Kette messbar bleibt */
        for (int i = 1; i < nd; i++) g_actors[dslots[i]].active = 0;

        int latch = -1, devour = -1, bites = 0;
        int16_t hp_prev = pl->hp;
        int last_sub = -1;
        int16_t hp_min = pl->hp;
        int sub8_count = 0, near_log = 0;
        char sublog[512]; int sl = 0; sublog[0] = 0;
        /* Der Spieler STEHT (kein Nachstellen pro Frame — sonst misst die Sonde ihre eigene
         * Distanz-Klemme): einmal 2500 Einheiten neben dem Hund absetzen, dann nur noch die
         * Blickrichtung fuehren. Der Hund muss selbst zustellen. */
        {   /* SWEEP = Geometrie: der Spieler steht 2500 Einheiten vom Hund entfernt, der
             * Winkel variiert pro Lauf (der RE2-PRNG allein variiert den Lauf nicht — gemessen). */
            int ang = (s * 4096) / (seeds > 0 ? seeds : 1);
            pl->x = d0->x + (int32_t)((2500 * (int64_t)re15_cos_q12(ang)) >> 12);
            pl->z = d0->z - (int32_t)((2500 * (int64_t)re15_sin_q12(ang)) >> 12);
            pl->y = d0->y;
        }
        for (int f = 0; f < frames; f++) {
            pl->motion = 100;
            pl->floor  = d0->floor;
            if (pin > 0 && pl->state != 7) { pl->x = d0->x + pin; pl->z = d0->z; pl->y = d0->y; }
            if (pl->state != 7) {
                int yaw = ((int)re15_atan2_q12(d0->z - pl->z, d0->x - pl->x) - 0x400) & 0xfff;
                pl->rot_y = (int16_t)(face ? yaw : ((yaw + 2048) & 0xfff));
            }
            hp_prev = pl->hp;
            frame();
            if (pl->hp < hp_prev) {
                bites++;
                if (verb && bites <= 12)
                    printf("  seed %d f%-5d BITE -%d -> HP %d (dog sub=%d ph=%d bite21e=%d fat=%u)\n",
                           s, f, hp_prev - pl->hp, pl->hp, d0->sub_state_1, d0->sub_state_2,
                           d0->re2d_bite21e, (unsigned)d0->re2z_flags21a);
            }
            if (pl->hp < hp_min) hp_min = pl->hp;
            {   /* Distanz-Protokoll rund um den Kontakt-Radius (700/1000 @0x80104E34-48) */
                int32_t dx = pl->x - d0->x, dz = pl->z - d0->z;
                int64_t d2 = (int64_t)dx * dx + (int64_t)dz * dz;
                if (verb && d2 < 2500LL * 2500LL && d0->re2d_air219 && near_log < 24) {
                    near_log++;
                    int32_t jaw[3];
                    re15_enemy_bone_world_pos(d0, 4, jaw);   /* Part 4 = Kiefer (pool+688 @0x80104E24) */
                    int32_t jx = pl->x - jaw[0], jz = pl->z - jaw[2];
                    int64_t j2 = (int64_t)jx * jx + (int64_t)jz * jz;
                    int rootd = 0; while ((int64_t)(rootd + 1) * (rootd + 1) <= d2) rootd++;
                    int jawd  = 0; while ((int64_t)(jawd  + 1) * (jawd  + 1) <= j2) jawd++;
                    printf("  seed %d f%-5d NEAR root=%d jaw=%d (r=%d) air=%d bite21e=%d ph=%d hp=%d\n",
                           s, f, rootd, jawd, (pl->hp < 21) ? 700 : 1000,
                           d0->re2d_air219, d0->re2d_bite21e, d0->sub_state_2, pl->hp);
                    if (near_log == 1) {   /* einmalig: Vorwaerts-/Hoehen-Offset ALLER Bones */
                        int32_t cs = re15_cos_q12(d0->rot_y), sn = re15_sin_q12(d0->rot_y);
                        for (int b = 0; b < 16; b++) {
                            int32_t w[3]; re15_enemy_bone_world_pos(d0, b, w);
                            int32_t rx = w[0] - d0->x, rz = w[2] - d0->z;
                            /* Vorwaerts = Blickachse (cos,-sin) wie re2d_move */
                            int32_t fwd = (int32_t)(((int64_t)rx * cs - (int64_t)rz * sn) >> 12);
                            printf("      bone %2d: fwd=%6d up=%6d\n", b, (int)fwd, (int)(d0->y - w[1]));
                        }
                    }
                }
            }
            if (d0->sub_state_1 != last_sub) {
                last_sub = d0->sub_state_1;
                if (last_sub == 8) sub8_count++;
                if (sl < 460) sl += snprintf(sublog + sl, sizeof sublog - (size_t)sl, "%d ", last_sub);
            }
            if (verb && f % 500 == 499)
                printf("  seed %d f%-5d STATUS sub=%d ph=%d dist=%u fat=%u speed=%d stuck=%d "
                       "dogpos=(%d,%d) plpos=(%d,%d)\n",
                       s, f, d0->sub_state_1, d0->sub_state_2, (unsigned)d0->ai_dist,
                       (unsigned)d0->re2z_flags21a, (int)d0->speed_h, (int)d0->re2d_stuck230,
                       (int)d0->x, (int)d0->z, (int)pl->x, (int)pl->z);
            if (latch  < 0 && d0->state == 1 && d0->sub_state_1 == 7) latch  = f;
            if (devour < 0 && pl->state == 7) devour = f;
        }
        printf("seed %2d: bites=%d hp_end=%d hp_min=%d latch@%d devour@%d sub8x%d subs=[%s]\n",
               s, bites, pl->hp, hp_min, latch, devour, sub8_count, sublog);
        if (latch  >= 0) n_latch++;
        if (devour >= 0) n_devour++;
        if (pl->hp == 0)  n_hp0++;
        if (pl->hp <  0)  n_hpneg++;
    }
    printf("SUMMARY face=%d seeds=%d frames=%d: latch=%d devour=%d hp_end==0:%d hp_end<0:%d\n",
           face, seeds, frames, n_latch, n_devour, n_hp0, n_hpneg);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}
