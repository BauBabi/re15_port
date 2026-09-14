/* probe_gegen_dkf2.c - UNABHAENGIGE GEGENPRUEFUNG des Befundes
 * "uebersprungener Keyframe -> Port liefert NULL Vorschub".
 * Nur Messung, kein Engine-Edit.
 *
 * Teil 1 DIREKT-PIN : Kopie eines lebenden RE2-Zombies, Clip 0, Yaw 0.
 *                     Anker K -> Bild K+1 bzw. K+2, jeweils Verschiebung + +0x144.
 * Teil 2 ZENSUS     : Lauf A (unveraendert), 3000 Ticks, je Zombie
 *                     dkf-Histogramm + wieviele davon still standen / r144==0.
 * Teil 3 GANGSTRECKE: Lauf A vs. Lauf C (Bit +0x21A&0x8000 jeden Tick geloescht),
 *                     Bilder 0..400, Summe |dx|+|dz|.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include "re2_ems.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define MAXF 3000

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

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

static void world_init(int32_t plx, int32_t plz)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->state = 0; pl->motion = 0;
    pl->floor = 0; pl->y = 0; pl->x = plx; pl->z = plz;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);
}
static void tick(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

/* --------- Teil 1: DIREKT-PIN --------------------------------------------------------------- */
static void direkt_pin(const re15_actor_t *tmpl, const char *tag)
{
    printf("\n--- DIREKT-PIN %s (typ 0x%02x, Clip 0, Yaw 0) ---\n", tag, tmpl->type);
    re15_enemy_bank_t *b = re15_enemy_find(tmpl->type);
    if (!b || !b->ok) { printf("   keine Bank\n"); return; }
    printf("   K |  K->K+1 (dx,dz) r144 |  K->K+2 (dx,dz) r144\n");
    for (int K = 0; K < 12; K++) {
        int32_t d1x=0,d1z=0,d2x=0,d2z=0; int16_t r1=0,r2=0;
        for (int mode = 0; mode < 2; mode++) {
            re15_actor_t e = *tmpl;
            e.rot_y = 0; e.motion = 0; e.x = 0; e.z = 0;
            e.root_prev_kf = -1; e.root_prev_motion = 0;
            e.anim_frame = K;
            re15_re2z_move_root(&e);            /* Re-Anker */
            e.anim_frame = K + 1 + mode;
            e.x = 0; e.z = 0;
            re15_re2z_move_root(&e);
            if (mode == 0) { d1x = e.x; d1z = e.z; r1 = e.re2z_root144; }
            else           { d2x = e.x; d2z = e.z; r2 = e.re2z_root144; }
        }
        printf("  %2d | (%6d,%6d) %6d | (%6d,%6d) %6d\n", K, d1x, d1z, r1, d2x, d2z, r2);
    }
}


/* --------- Teil 4: Rohe Wurzel-Offsets R(f) von Clip 0 (fuer den Re-Anker-Vergleich) -------- */
static void dump_root(const re15_actor_t *tmpl)
{
    re15_enemy_bank_t *b = re15_enemy_find(tmpl->type);
    if (!b || !b->ok) return;
    for (int which = 0; which < 2; which++) {
        const re15_emd_skeleton_t  *sk = which ? &b->skel_loco : &b->skel;
        const re15_emd_animation_t *an = which ? &b->anim_loco : &b->anim;
        if (which && !b->loco_ok) continue;
        if (an->clip_count <= 0) continue;
        const re15_emd_clip_t *c = &an->clips[0];
        printf("\n--- R(f) Clip 0, Bank %s: fc=%d ---\n", which ? "LOCO(Paar1)" : "0", c->frame_count);
        for (int f = 0; f < c->frame_count && f < 40; f++) {
            int kf = (int)(an->frames[c->first_frame + f] & 0xFFFu);
            int16_t sx=0, sy=0, sz=0;
            if (!re15_emd_get_keyframe_speed(sk, kf, &sx, &sy, &sz)) { printf("  f%-3d kf=%-4d --\n", f, kf); continue; }
            printf("  f%-3d kf=%-4d R=(%6d,%6d,%6d)\n", f, kf, sx, sy, sz);
        }
    }
    printf("  poses_loco_bank(tmpl)=%d uses_loco_bank(tmpl)=%d loco_ok=%d\n",
           re15_re2z_poses_loco_bank(tmpl), re15_actor_uses_loco_bank(tmpl), b->loco_ok);
}

/* --------- Teil 2/3 -------------------------------------------------------------------------- */
typedef struct { int32_t fr; uint8_t mo; int32_t x, z; int16_t r144; uint16_t f21a; uint8_t s1, s2; } rec_t;
static rec_t s_rec[RE15_ACTOR_MAX][MAXF];
static int   s_live[RE15_ACTOR_MAX];

static void run_record(int killbit, int frames)
{
    world_init(-6000, -21000);
    memset(s_live, 0, sizeof s_live);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) s_live[s] = 1;
    for (int f = 0; f < frames; f++) {
        if (killbit)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (s_live[s]) g_actors[s].re2z_flags21a &= (uint16_t)~0x8000u;
        tick();
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!s_live[s]) continue;
            const re15_actor_t *e = &g_actors[s];
            s_rec[s][f].fr = e->anim_frame; s_rec[s][f].mo = (uint8_t)e->motion;
            s_rec[s][f].x = e->x; s_rec[s][f].z = e->z;
            s_rec[s][f].r144 = e->re2z_root144; s_rec[s][f].f21a = e->re2z_flags21a;
            s_rec[s][f].s1 = e->sub_state_1; s_rec[s][f].s2 = e->sub_state_2;
        }
    }
}

int main(void)
{
    size_t n = 0; uint8_t *rdtbuf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!rdtbuf || re15_rdt_parse(rdtbuf, n, &s_rdt) != 0) { printf("ROOM1030 fehlt\n"); return 1; }

    /* ---- Lauf A ---- */
    run_record(0, MAXF);
    printf("== LAUF A (unveraendert), %d Ticks, ROOM1030, RE2-Geschmack ==\n", MAXF);
    printf("slot typ  Bit0x8000(t0) f10e&0x80 | dkf-Histogramm | dkf=2: still / r144==0\n");
    int pin_slot_bit = -1, pin_slot_nobit = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        int h[6]; memset(h,0,sizeof h); int other=0, d2still=0, d2r0=0, mochg=0;
        for (int f = 1; f < MAXF; f++) {
            rec_t *a = &s_rec[s][f-1], *b = &s_rec[s][f];
            if (a->mo != b->mo) { mochg++; continue; }
            int d = (int)b->fr - (int)a->fr;
            if (d >= 0 && d <= 5) h[d]++; else other++;
            if (d == 2) {
                if (a->x == b->x && a->z == b->z) d2still++;
                if (b->r144 == 0) d2r0++;
            }
        }
        printf(" z%-2d 0x%02x  %-13s %-9s | 0:%d 1:%d 2:%d 3+:%d sonst:%d mo:%d | %d / %d\n",
               s, g_actors[s].type,
               (s_rec[s][0].f21a & 0x8000u) ? "SET" : "0",
               (g_actors[s].re2z_f10e & 0x80u) ? "SET" : "0",
               h[0], h[1], h[2], h[3]+h[4]+h[5], other, mochg, d2still, d2r0);
        if ((s_rec[s][0].f21a & 0x8000u) && pin_slot_bit < 0)  pin_slot_bit = s;
        if (!(s_rec[s][0].f21a & 0x8000u) && pin_slot_nobit < 0) pin_slot_nobit = s;
    }

    /* ---- Teil 1: DIREKT-PIN auf Kopien ---- */
    if (pin_slot_bit   > 0) direkt_pin(&g_actors[pin_slot_bit],   "Zombie MIT Bit");
    if (pin_slot_nobit > 0) direkt_pin(&g_actors[pin_slot_nobit], "Zombie OHNE Bit");

    if (pin_slot_bit > 0) dump_root(&g_actors[pin_slot_bit]);

    /* ---- Teil 3: GANGSTRECKE A vs C ---- */
    double aPath[RE15_ACTOR_MAX]; memset(aPath, 0, sizeof aPath);
    double cPath[RE15_ACTOR_MAX]; memset(cPath, 0, sizeof cPath);
    int    aFr[RE15_ACTOR_MAX];   memset(aFr, 0, sizeof aFr);
    int    cFr[RE15_ACTOR_MAX];   memset(cFr, 0, sizeof cFr);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        for (int f = 1; f < 401; f++) {
            double dx = (double)(s_rec[s][f].x - s_rec[s][f-1].x);
            double dz = (double)(s_rec[s][f].z - s_rec[s][f-1].z);
            aPath[s] += (dx<0?-dx:dx) + (dz<0?-dz:dz);
            int d = s_rec[s][f].fr - s_rec[s][f-1].fr;
            if (s_rec[s][f].mo == s_rec[s][f-1].mo && d > 0) aFr[s] += d;
        }
    }
    run_record(1, 401);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        for (int f = 1; f < 401; f++) {
            double dx = (double)(s_rec[s][f].x - s_rec[s][f-1].x);
            double dz = (double)(s_rec[s][f].z - s_rec[s][f-1].z);
            cPath[s] += (dx<0?-dx:dx) + (dz<0?-dz:dz);
            int d = s_rec[s][f].fr - s_rec[s][f-1].fr;
            if (s_rec[s][f].mo == s_rec[s][f-1].mo && d > 0) cFr[s] += d;
        }
    }
    printf("\n== GANGSTRECKE Bilder 0..400 (A = mit Bit, C = Bit jeden Tick geloescht) ==\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!s_live[s]) continue;
        printf("  z%-2d A=%-8.0f C=%-8.0f A/C=%.3f  | Bilder A=%d C=%d\n",
               s, aPath[s], cPath[s], cPath[s] > 0 ? aPath[s]/cPath[s] : 0.0, aFr[s], cFr[s]);
    }
    printf("\nSONDE-OK\n");
    free(rdtbuf);
    return 0;
}
