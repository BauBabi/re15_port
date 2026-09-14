/* probe_wrapgain_check.c - GEGENPRUEFUNG des Befundes "Port verliert am Clip-Wrap den
 * Anschub von Bild 0".
 *
 * Zwei Fragen, beide gemessen (nichts modelliert):
 *  (1) WELCHE Port-Zeile weist den Wrap ab? Der Befund nennt enemy_ai_common.c:706
 *      (s_now != s_prev+1). Die Guard-Eingaben sind exakt rekonstruierbar: move_root setzt
 *      am Ende root_prev_kf = fr. Also ist das Paar, mit dem der Guard im Tick t lief,
 *      (fr_now = root_prev_kf nach Tick t, fr_prev = root_prev_kf nach Tick t-1).
 *      Damit laesst sich Zeile 700 (fr_now != fr_prev+1) gegen Zeile 706 auszaehlen.
 *  (2) WIE GROSS ist der Verlust pro Wrap im LIVE-Lauf? Original (FUN_80015E7C, a3=0,
 *      +0x14D==0: sw zero,524(t0) @0x80015F14 / sh zero,528(t0) @0x80015F1C, dann
 *      subu @0x80015FCC) addiert dort root(kf[0]) - 0. Der Port addiert 0.
 */
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int re15_actor_clip_len(const re15_actor_t *a);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint8_t *s_re2 = NULL; static size_t s_re2n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2) s_re2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2n);
    if (!s_re2) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2, s_re2n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

int main(void)
{
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("SKIP: ROOM1030.RDT\n"); return 0; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("SKIP: parse\n"); return 0; }
    g_room_rdt_ok = 1; s_rdt = g_room_rdt;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -18050; pl->z = -8300;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);

    static int16_t  prev_r[RE15_ACTOR_MAX];
    static uint16_t prev_mo[RE15_ACTOR_MAX];
    static int      have[RE15_ACTOR_MAX];
    long l700 = 0, l706 = 0, pass = 0, clipswitch = 0;
    long wraps = 0; long long lost_sum = 0; int max_af = 0, min_af = 9999;
    long negjump = 0;

    for (int s = 1; s < RE15_ACTOR_MAX; s++) { have[s] = 0; }

    for (int f = 0; f < 3000; f++) {
        const unsigned char *raw; int len, id;
        re15_msg_tick(&raw, &len, &id);
        s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
        scd_vm_tick();
        re15_game_step(&s_ctx);
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || !re15_re2z_owns_type(e->type)) { have[s] = 0; continue; }
            if (!(e->state == 1 && e->sub_state_1 == 1)) { have[s] = 0;
                prev_r[s] = e->root_prev_kf; prev_mo[s] = e->motion; continue; }
            int16_t r_now = e->root_prev_kf;
            if ((int)e->anim_frame > max_af) max_af = (int)e->anim_frame;
            if ((int)e->anim_frame < min_af) min_af = (int)e->anim_frame;
            if (have[s]) {
                int16_t r_prev = prev_r[s];
                if (r_now < r_prev) negjump++;
                if (prev_mo[s] != e->motion) {
                    clipswitch++;                       /* Zeile 2623: Clipwechsel -> kein Delta */
                } else {
                    int fc = re15_actor_clip_len(e);
                    if (fc <= 0) fc = 1;
                    if (r_prev < 0 || r_now != r_prev + 1) {
                        l700++;
                        if (r_prev == fc - 1 && r_now == 0) {
                            wraps++;
                            /* was das Original hier addiert haette: root(kf[slot 0]) */
                            re15_enemy_bank_t *b = re15_enemy_find(e->type);
                            const re15_emd_skeleton_t  *sk = b ? &b->skel : NULL;
                            const re15_emd_animation_t *an = b ? &b->anim : NULL;
                            if (b && b->loco_ok && re15_re2z_poses_loco_bank(e)) {
                                sk = &b->skel_loco; an = &b->anim_loco;
                            }
                            if (sk && an && (int)e->motion < an->clip_count) {
                                const re15_emd_clip_t *c = &an->clips[e->motion];
                                int kf0 = (int)(an->frames[c->first_frame] & 0xFFFu);
                                int16_t sx = 0, sy = 0, sz = 0;
                                if (re15_emd_get_keyframe_speed(sk, kf0, &sx, &sy, &sz)) {
                                    int a = sx < 0 ? -sx : sx, bb = sz < 0 ? -sz : sz;
                                    lost_sum += a + bb;
                                }
                            }
                        }
                    } else {
                        int s_now  = r_now  % fc;
                        int s_prev = r_prev % fc;
                        if (s_now != s_prev + 1) l706++; else pass++;
                    }
                }
            }
            prev_r[s] = r_now; prev_mo[s] = e->motion; have[s] = 1;
        }
    }
    printf("=== GUARD-AUSZAEHLUNG (LIVE ROOM1030, RE2, 3000 Bilder, nur Gang-Ticks) ===\n");
    printf("  Zeile 700 (fr_now != fr_prev+1) feuert : %ld\n", l700);
    printf("  Zeile 706 (s_now  != s_prev +1) feuert : %ld\n", l706);
    printf("  Guard passiert (Delta wird addiert)    : %ld\n", pass);
    printf("  Clipwechsel (Zeile 2623 blockt)        : %ld\n", clipswitch);
    printf("  anim_frame Bereich beobachtet          : %d .. %d   (root_prev_kf-Rueckspruenge %ld)\n",
           min_af, max_af, negjump);
    printf("  echte Clip-Wraps (fr_prev==fc-1, fr_now==0): %ld\n", wraps);
    printf("  dabei NICHT addierter Betrag |sx0|+|sz0| gesamt: %lld  (Mittel %.2f/Wrap)\n",
           lost_sum, wraps ? (double)lost_sum / (double)wraps : 0.0);
    free(buf);
    printf("\nPROBE-OK\n");
    return 0;
}
