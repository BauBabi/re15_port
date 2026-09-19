/* probe_r16_sk_birkin_g5.c — SKEPTIKER-Gegensonde (kein Pin), Runde 16, Thema birkin-g5.
 *
 * Prueft drei Dinge am Dossier analysis/befunde_2026-09-19/birkin-g5.md, auf demselben
 * Ablauf wie probe_r16_birkin_g5.c (ROOM5090.RDT -> SCD-VM -> re15_game_step ->
 * re15_enemy_ai_run_all, RE2-Baenke EM036/EM037, Spieler an Tuer 4, Lauf nach Westen):
 *
 *  Modus "A <x0>": nach dem Kampfgate wird der Boss-Ursprung auf x0 gesetzt (0 = nichts
 *     tun = Skriptwert 1200). Gemessen: erstes Bild mit Ursprung im Cut-12-Viereck, erstes
 *     Bild mit Massen-FRONT (Ursprung+4494) im Viereck, Intro-Ende (Clip 5), Ursprung und
 *     |dx| zum Spieler am Intro-Ende, Bild an dem die Front den Spieler ueberholt.
 *     Damit laesst sich der Fix-Plan 4.1.1/4.1.2 (X0 = -10167, Front-Cull) gegen den
 *     byte-true RE2-Wert X = -9000 (@0x801011d0) vergleichen.
 *  Modus "C": 3000 Bilder, Spieler jedes Bild 6500 oestlich, HP je Bild zurueck. Bei JEDEM
 *     HP-Treffer: Clip + Spitzen-Abstand ALLER vier Arme (die Dossier-Sonde zaehlt den
 *     Treffer bei jedem Arm, egal wer ihn verursacht hat). Zusaetzlich: in welchem Bild
 *     g_room_change.pending gesetzt wird (die DOOR-FIRE-Meldungen des Dossier-Laufs).
 *  Modus "S": nach dem Intro allen vier Armen 0xA01 (sub10) schicken und 300 Bilder lang
 *     Anker-x vs Boss-x messen -> zeigt, ob der Port-Default-Zweig den Anker nachfuehrt.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_collision.h"
#include "re15_msg.h"
#include "re15_game_step.h"
#include "re15_camera.h"
#include "re15_damage.h"
#include "re15_skeleton.h"
#include "re15_tim.h"
#include "re15_fade.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_g5_tentakel_spitze(int idx, int32_t out[3]);
extern uint8_t re15_g5_tentakel_maske(void);
extern void re15_g5_tentakel_cmd(int idx, uint32_t wort);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_shown = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_re2_banks(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) return 0;
    for (int k = 0; k < 2; k++) {
        uint8_t type = k ? 0x37u : 0x36u;
        re15_enemy_bank_t *eb = re15_enemy_find(type);
        if (!eb) eb = re15_enemy_alloc(type);
        if (!eb) return 0;
        if (re2_ems_load_bank(ems, n, type, eb, NULL) != 0) return 0;
        eb->buf = NULL; eb->ok = 1;
    }
    return 1;
}

static int s_cine_was_active = 0;
static void frame(void)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    {
        int cine_active = re15_game_flag_get(1, 27) || re15_game_flag_get(2, 7);
        re15_letterbox_tick(re15_game_flag_get(1, 27));
        if (cine_active) { g_scd.player_mode = 2; g_scd.letterbox_countdown = -1; }
        else if (s_cine_was_active) { g_scd.letterbox_countdown = 15; }
        s_cine_was_active = cine_active;
        if (g_scd.letterbox_countdown > 0 && --g_scd.letterbox_countdown == 0) {
            g_scd.player_mode = 0;
            re15_aot_settle_at(g_actors[RE15_ACTOR_SLOT_PLAYER].x,
                               g_actors[RE15_ACTOR_SLOT_PLAYER].z);
        }
    }
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = 0;
    s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

#define G5_FRONT 4494
static void g5_front(const re15_actor_t *e, int32_t *fx, int32_t *fz)
{
    int32_t c = re15_cos_q12((int)e->rot_y), s = re15_sin_q12((int)e->rot_y);
    *fx = e->x + (int32_t)(((int64_t)c * G5_FRONT) >> 12);
    *fz = e->z - (int32_t)(((int64_t)s * G5_FRONT) >> 12);
}
static int find_boss(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x36) return s;
    return -1;
}
static int tent_slots(int *out)
{
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x37u && n < 8) out[n++] = s;
    return n;
}
static int32_t dist2d(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{
    int64_t dx = (int64_t)ax - bx, dz = (int64_t)az - bz;
    int64_t d2 = dx * dx + dz * dz; int32_t d = 0;
    while ((int64_t)d * d < d2) d += 25;
    return d;
}

int main(int argc, char **argv)
{
    size_t rsz = 0;
    const char *mode = argc > 1 ? argv[1] : "A";
    int32_t x0 = argc > 2 ? (int32_t)atol(argv[2]) : 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/STAGE5/ROOM5090.RDT", &rsz);
    if (!raw) { printf("FEHLT: ROOM5090.RDT\n"); return 77; }
    if (re15_rdt_parse(raw, rsz, &s_rdt) != 0) { printf("FEHLT: RDT-Parse\n"); return 77; }

    printf("=== R16 SKEPTIKER birkin-g5: Modus %s x0=%d ===\n", mode, (int)x0);

    int16_t qx[4], qz[4];
    int has_q = re15_rdt_get_region_quad(&s_rdt, 12, qx, qz);

    memset(&s_cam, 0, sizeof s_cam);
    memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 14;

    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x5090; g_room_change.pending = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0;
    pl->x = 25600; pl->y = 0; pl->z = -23350; pl->rot_y = 1024;
    re15_collision_set_band(0);

    if (!load_re2_banks()) { printf("FEHLT: RE2-Baenke (CDEMD0.EMS)\n"); return 77; }

    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_register_room_events(&s_rdt);
    scd_room_reenter(&s_rdt, pl->x, pl->z, 14);
    g_scd.cut_auto_enabled = 1;
    s_shown = 14;

    int pend_first = -1;
    for (int f = 0; f < 30; f++) { frame(); if (g_room_change.pending && pend_first < 0) pend_first = -100 + f; }
    int trigger_f = -1;
    for (int f = 30; f < 500; f++) {
        int32_t nx = pl->x - 75, nz = pl->z;
        re15_collision_set_band(0);
        re15_collision_constrain(&s_rdt, pl->x, pl->z, &nx, &nz);
        pl->x = nx; pl->z = nz;
        frame();
        if (g_room_change.pending && pend_first < 0) pend_first = -100 + f;
        if (trigger_f < 0 && s_shown == 12) trigger_f = f;
        if (trigger_f >= 0 && f > trigger_f + 3) break;
    }
    int bslot = find_boss();
    if (bslot < 0) { printf("FEHLT: kein Boss\n"); return 77; }
    re15_actor_t *e = &g_actors[bslot];
    printf("KAMPF-GATE: Kamera 12 bei Bild %d, Spieler x=%d, Boss pos=(%d,%d) clip=%d pending_vor_gate=%d\n",
           trigger_f, (int)pl->x, (int)e->x, (int)e->z, (int)e->motion, pend_first);
    if (x0 != 0) { e->x = x0; e->z = -23400; }

    if (mode[0] == 'A') {
        int first_in_q = -1, first_front_in_q = -1, first_clip5 = -1, first_front_past = -1;
        int32_t dx_min = 0x7fffffff, x_end = 0, dx_end = 0;
        for (int f = 0; f < 1500; f++) {
            frame();
            if (g_room_change.pending && pend_first < 0) pend_first = f;
            int32_t fx, fz; g5_front(e, &fx, &fz);
            int in_o = has_q ? re15_aot_point_in_quad(e->x, e->z, qx, qz) : -1;
            int in_f = has_q ? re15_aot_point_in_quad(fx, fz, qx, qz) : -1;
            int32_t dx = pl->x - e->x; if (dx < 0) dx = -dx;
            if (dx < dx_min) dx_min = dx;
            if (in_o && first_in_q < 0) first_in_q = f;
            if (in_f && first_front_in_q < 0) first_front_in_q = f;
            if (e->motion == 5 && first_clip5 < 0) { first_clip5 = f; x_end = e->x; dx_end = pl->x - e->x; }
            if (e->x < pl->x && fx >= pl->x && first_front_past < 0) first_front_past = f;
            if (f == 0 || f == 90 || f == 120 || f == 150 || f == 180 || f == 270 || f == 400 || f == 527 || f == 630 || f == first_clip5)
                printf("   Bild %4d | boss x=%6d | Urspr.imQ=%d | Front x=%6d (imQ=%d) | Spieler x=%d | clip %d/%u\n",
                       f, (int)e->x, in_o, (int)fx, in_f, (int)pl->x, (int)e->motion, (unsigned)e->anim_frame);
        }
        printf("ERGEBNIS A(x0=%d): Ursprung im Viereck ab Bild %d, Front ab Bild %d; Intro fertig (Clip 5) bei %d "
               "mit Ursprung %d und Spieler-Abstand %d; Front ueberholt Spieler bei %d; min|dx|=%d; pending ab Bild %d\n",
               (int)x0, first_in_q, first_front_in_q, first_clip5, (int)x_end, (int)dx_end, first_front_past,
               (int)dx_min, pend_first);
        return 0;
    }

    /* Intro durchlaufen lassen (1500 Bilder wie im Dossier). */
    for (int f = 0; f < 1500; f++) frame();
    int tslots[8]; int nt = tent_slots(tslots);

    if (mode[0] == 'S') {
        printf("-- S) 0xA01 (sub10) an alle vier Arme, dann 300 Bilder: Anker-x - Boss-x --\n");
        for (int i = 0; i < nt; i++) re15_g5_tentakel_cmd(i, 0xA01u);
        for (int f = 0; f < 300; f++) {
            frame();
            if (f % 60 == 0 || f == 299) {
                printf("   f%3d boss x=%6d |", f, (int)e->x);
                for (int i = 0; i < nt; i++)
                    printf(" arm%d anker-boss=%6d c%d", i, (int)(g_actors[tslots[i]].x - e->x), (int)g_actors[tslots[i]].motion);
                printf("\n");
            }
        }
        return 0;
    }

    /* Modus C */
    printf("-- C) 3000 Bilder, Spieler |dx|=6500 oestlich, HP je Bild zurueck; je Treffer alle Arme --\n");
    int hits = 0, hits_arm1_c21 = 0, hits_some_tip_near = 0;
    for (int f = 0; f < 3000; f++) {
        pl->x = e->x + 6500; pl->z = e->z; pl->y = 0;
        pl->hp = 100; pl->hit_react = 0;
        frame();
        if (g_room_change.pending && pend_first < 0) { pend_first = f; printf("   pending gesetzt bei C-Bild %d (Spieler x=%d)\n", f, (int)pl->x); }
        if (pl->hp < 100) {
            hits++;
            printf("   TREFFER f%4d hp=%d boss clip %d x=%d |", f, (int)pl->hp, (int)e->motion, (int)e->x);
            int near = 0;
            for (int i = 0; i < nt; i++) {
                re15_actor_t *t = &g_actors[tslots[i]];
                int32_t sp[3]; int32_t d = -1;
                if (re15_g5_tentakel_spitze(i, sp) == 0) d = dist2d(pl->x, pl->z, sp[0], sp[2]);
                if (d >= 0 && d < 1500) near++;
                printf(" arm%d c%d af%u s%d spitze-d=%d anker-x=%d", i, (int)t->motion, (unsigned)t->anim_frame,
                       (int)t->render_scale_q12, (int)d, (int)t->x);
            }
            printf("\n");
            if (g_actors[tslots[1]].motion == 21) hits_arm1_c21++;
            if (near) hits_some_tip_near++;
        }
    }
    printf("ERGEBNIS C: Treffer=%d, davon Arm1 in c21=%d, mit irgendeiner Spitze <1500=%d; pending ab Bild %d\n",
           hits, hits_arm1_c21, hits_some_tip_near, pend_first);
    return 0;
}
