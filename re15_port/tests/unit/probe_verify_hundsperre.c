/* probe_verify_hundsperre.c — GEGENPROBE zum Befund "Sperre endet exakt im Bild 120,
 * reine Clip-Laufzeit 13+9+50".
 *
 * Gemessen wird, WOVON die 120 abhaengen:
 *  (1) Clip-Transitions-Protokoll eines Einzelschusses (Zeile 0 und Zeile 2) — welcher
 *      Clip laeuft in welchem Bildfenster?
 *  (2) Dauer-Sweep ueber den Leerlauf-Clip-Frame ZUM SCHUSSZEITPUNKT (anim_frame),
 *      Zeile 0 vs. Zeile 2. Ist 120 eine Konstante oder ein Setup-Artefakt?
 *
 * Reine Messsonde, kein add_test, kein Engine-Eingriff.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_esp.h"
#include "re15_math.h"
#include "re15_ai_flavor.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_elevation_for_test(int elev);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;

static re15_enemy_bank_t *bank_re2(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    memset(eb, 0, sizeof *eb); eb->type = type;
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (s_ems2 && re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; eb->ok = 0; return NULL;
}

static void face(re15_actor_t *e, const re15_actor_t *t)
{
    e->rot_y = (int16_t)(((int)re15_atan2_q12(t->z - e->z, t->x - e->x) - 0x400) & 0xfff);
}

/* Setzt einen RE2-Hund auf, gibt Zeiger zurueck. */
static re15_actor_t *aufsetzen(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_reset(); re15_actor_init(); re15_esp_fx_reset(); re15_player_aim_reset();
    bank_re2(0x20);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->z = 2000; pl->hp = 100;
    re15_actor_t *e = &g_actors[1];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x20; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x20);
    face(e, pl); face(pl, e);
    for (int i = 0; i < 2; i++) {
        re15_enemy_ai_run_all(1);
        e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
    }
    re15_player_set_aim_elevation_for_test(0);
    e->hp = 1000;
    return e;
}

/* (1) Clip-Protokoll */
static void protokoll(const char *titel, int prev_sub, int frame_pin)
{
    re15_actor_t *e = aufsetzen();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("\n--- %s ---\n", titel);
    printf("    NATUERLICH vor dem Schuss: st=%d/%d/%d/%d  re2z_prev_sub=%d  clip=%d fr=%d\n",
           e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
           (int)e->re2z_prev_sub, (int)e->motion, (int)e->anim_frame);
    if (prev_sub >= 0) e->re2z_prev_sub = (uint8_t)prev_sub;
    if (frame_pin >= 0) e->anim_frame = (uint16_t)frame_pin;
    printf("    benutzt: prev_sub=%d, clip=%d fr=%d\n",
           (int)e->re2z_prev_sub, (int)e->motion, (int)e->anim_frame);
    int ret = re15_player_weapon_fire(3);
    if (ret != 2) { printf("    kein Treffer (fire=%d)\n", ret); return; }
    int16_t last_clip = e->motion; int start = 0;
    for (int t = 1; t <= 400; t++) {
        uint8_t hr = e->hit_react;
        e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
        re15_enemy_ai_run_all(1);
        pl->hit_react = 0; pl->hp = 100;
        if (e->motion != last_clip) {
            printf("    Bild %3d..%3d : clip %d  (%d Bilder)  st=%d/%d/%d/%d\n",
                   start, t - 1, (int)last_clip, t - start,
                   e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3);
            last_clip = e->motion; start = t;
        }
        if ((hr & 1u) && !(e->hit_react & 1u)) {
            printf("    Bild %3d..%3d : clip %d  (%d Bilder, laufend)\n",
                   start, t, (int)last_clip, t - start + 1);
            printf("    => SPERRE ENDET nach %d Bildern  st=%d/%d/%d/%d clip=%d\n",
                   t, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3, (int)e->motion);
            return;
        }
    }
    printf("    Sperre haelt >400 Bilder\n");
}

/* (2) Sweep ueber den Leerlauf-Frame zum Schusszeitpunkt */
static void sweep(int prev_sub)
{
    printf("\n--- SWEEP prev_sub=%d : Leerlauf-Frame beim Schuss -> Sperrdauer ---\n", prev_sub);
    int mn = 99999, mx = -1;
    for (int fr = 0; fr < 69; fr += 4) {
        re15_actor_t *e = aufsetzen();
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        e->re2z_prev_sub = (uint8_t)prev_sub;
        e->anim_frame = (uint16_t)fr;
        int clip0 = e->motion;
        if (re15_player_weapon_fire(3) != 2) { printf("    fr=%2d kein Treffer\n", fr); continue; }
        int dauer = -1;
        for (int t = 1; t <= 400; t++) {
            uint8_t hr = e->hit_react;
            e->x = 0; e->z = 0; e->y = 0; face(e, pl); face(pl, e);
            re15_enemy_ai_run_all(1);
            pl->hit_react = 0; pl->hp = 100;
            if ((hr & 1u) && !(e->hit_react & 1u)) { dauer = t; break; }
        }
        printf("    Leerlauf clip=%d fr=%2d  ->  Sperre %3d Bilder (%.2f s)\n",
               clip0, fr, dauer, dauer / 30.0);
        if (dauer >= 0) { if (dauer < mn) mn = dauer; if (dauer > mx) mx = dauer; }
    }
    printf("    SPANNE: min %d  max %d  Bilder\n", mn, mx);
}

int main(void)
{
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("=== GEGENPROBE Hunde-Sperre (RE2-KI, Typ 0x20, Waffe 3) ===\n");
    {
        re15_enemy_bank_t *eb = bank_re2(0x20);
        if (eb && eb->ok) {
            printf("    Bank clips=%d  1:%d  7:%d  17:%d  18:%d\n", eb->anim.clip_count,
                   eb->anim.clips[1].frame_count, eb->anim.clips[7].frame_count,
                   eb->anim.clips[17].frame_count, eb->anim.clips[18].frame_count);
        } else { printf("    BANK FEHLT -> Messung wertlos\n"); return 0; }
    }
    protokoll("P0 NATUERLICH (nichts forciert)", -1, -1);
    protokoll("P1 Zeile 0 (Port-Stand), Leerlauf-Frame wie im Befund (20)", 0, 20);
    protokoll("P2 Zeile 2 (generisch),  Leerlauf-Frame 20",                  2, 20);
    protokoll("P3 Zeile 0, Leerlauf-Frame 0",                                0, 0);
    protokoll("P4 Zeile 0, Leerlauf-Frame 60",                               0, 60);
    sweep(0);
    sweep(2);
    printf("\n=== ENDE ===\n");
    return 0;
}
