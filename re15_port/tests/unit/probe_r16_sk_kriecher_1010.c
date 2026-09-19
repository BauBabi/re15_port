/* probe_r16_sk_kriecher_1010.c — SKEPTIKER-GEGENSONDE (Runde 16, 2026-09-19) zum Dossier
 * analysis/befunde_2026-09-19/kriecher-1010.md. Helfer 1:1 aus probe_r16_kriecher1010.c.
 *
 *   S1: Rueckversatz bei MEHREREN Treffer-Bildern (Spieler-Startabstand variiert): gilt
 *       dx(Wiedereintritt) == sx(r) - Momentaufnahme(rp_x) fuer JEDEN Fall, nicht nur r=7/alt=34?
 *   S2: ungestoertes Kriechen 300 Bilder: gibt es OHNE Treffer einen Sprung (Clip-Wrap f49->f0)?
 *   S3: Totstellen (Teil A des Dossiers) fuer BEIDE Kriecher, Zustandswort f0..f2 + f149.
 * KEIN add_test — reine Messsonde. */
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
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_sin_q12(int), re15_cos_q12(int);

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

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int32_t s_px, s_pz;
static int s_latch = 0;
static void pin_player(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->hp = 100; pl->x = s_px; pl->z = s_pz; pl->y = 0;
    if (s_latch) pl->re2z_self1d3 |= 0x80u;
}

static void line(const char *tag, int f, int slot, int32_t px0, int32_t pz0)
{
    re15_actor_t *e = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t dx = e->x - px0, dz = e->z - pz0;
    printf("%s f%-4d x=%6ld z=%6ld dx=%5ld dz=%5ld | st=%u/%2u/%u mo=0x%02X fr=%3u "
           "| rp_mo=%d rp_kf=%d rp_x=%d r144=%d | grid=0x%02X 10E=0x%04X sca=%u hp=%d ry=%d dist=%u\n",
           tag, f, (long)e->x, (long)e->z, (long)dx, (long)dz,
           e->state, e->sub_state_1, e->sub_state_2, (unsigned)e->motion, (unsigned)e->anim_frame,
           (int)e->root_prev_motion, (int)e->root_prev_kf, (int)e->root_prev_x,
           (int)e->re2z_root144, e->grid_id, (unsigned)e->re2z_f10e, e->sca_mask, (int)e->hp,
           (int)e->rot_y, (unsigned)e->ai_dist);
    (void)pl;
}

static void bringup(re15_ai_flavor_t fl, uint8_t cut, int32_t px, int32_t pz)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(fl);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1010;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = px; pl->z = pz; s_px = px; s_pz = pz;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, cut);
    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11);
    re15_player_set_aim_clip_len(12);
}

static int sx_of(uint8_t type, int clip, int fr)
{
    re15_enemy_bank_t *b = re15_enemy_find(type);
    if (!b || !b->ok) return 0;
    const re15_emd_animation_t *an = &b->anim; const re15_emd_skeleton_t *sk = &b->skel;
    if (clip >= an->clip_count) return 0;
    int fc = an->clips[clip].frame_count; if (fc <= 0) return 0;
    int kf = (int)(re15_emd_get_frame_entry(an, clip, fr % fc) & 0xFFFu);
    int16_t sx = 0, sy = 0, sz = 0;
    re15_emd_get_keyframe_speed(sk, kf, &sx, &sy, &sz);
    return sx;
}

/* S1: ein Treffer, Spieler-Start bei px; 1 wenn dx(Wiedereintritt) == sx(r) - rp_x_alt */
static int s1_hit(int32_t px)
{
    bringup(RE15_AI_FLAVOR_RE2, 4, px, -1700);
    int a = -1, bslot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active) continue;
        if (g_actors[s].x == 950 && a < 0) a = s; else if (bslot < 0) bslot = s;
    }
    if (a < 0) { printf("  FAIL: Kriecher A fehlt\n"); return 0; }
    if (bslot >= 0) { g_actors[bslot].x = 30000; g_actors[bslot].z = 30000; }
    re15_actor_t *e = &g_actors[a];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    re15_player_set_equipped_weapon(3);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    s_latch = 1;
    int f = 0;
    for (; f < 400; f++) {
        pin_player(); frame(0, 0);
        if (e->ai_dist < 1200u && f > 5) { f++; break; }
    }
    for (int k = 0; k < 40 && !re15_player_aim_ready(); k++, f++) {
        pin_player(); frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_DOWN), 0);
    }
    int hp_last = e->hp, hit_f = -1, rp_x_hit = 0, rp_kf_hit = -1;
    int32_t lx = e->x;
    int reentry_seen = 0, ok = 0;
    for (int k = 0; k < 60; k++, f++) {
        pin_player();
        uint16_t cur = (uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_DOWN | (k == 0 ? RE15_PAD_BIT_SQUARE : 0));
        frame(cur, (uint16_t)(k == 0 ? RE15_PAD_BIT_SQUARE : 0));
        int32_t dx = e->x - lx; lx = e->x;
        if (e->hp < hp_last && hit_f < 0) {
            hit_f = f; rp_x_hit = e->root_prev_x; rp_kf_hit = e->root_prev_kf;
            printf("  px=%ld HIT f%d: mo=0x%02X st=%u/%u/%u rp_kf=%d rp_x=%d (sx(rp_kf)=%d)\n",
                   (long)px, f, (unsigned)e->motion, e->state, e->sub_state_1, e->sub_state_2,
                   rp_kf_hit, rp_x_hit, sx_of(0x10, 5, rp_kf_hit));
        }
        hp_last = e->hp;
        if (hit_f >= 0 && !reentry_seen && e->state == 1 && e->sub_state_1 == 0 &&
            e->sub_state_2 == 1 && e->motion == 5) {
            reentry_seen = 1;
            int r = (int)e->root_prev_kf;          /* Bild r = rand&0xF, von move_root abgelegt */
            int pred = sx_of(0x10, 5, r) - rp_x_hit;
            ok = (dx == pred);
            printf("  px=%ld REENTRY f%d: r=%d sx(r)=%d  dx_gemessen=%ld  sx(r)-rp_x_alt=%d  r144=%d -> %s\n",
                   (long)px, f, r, sx_of(0x10, 5, r), (long)dx, pred, (int)e->re2z_root144,
                   ok ? "GLEICH" : "UNGLEICH");
        }
    }
    if (hit_f < 0) printf("  px=%ld KEIN TREFFER\n", (long)px);
    else if (!reentry_seen) printf("  px=%ld Treffer, aber kein Wiedereintritt in 60 Bildern\n", (long)px);
    return ok;
}

/* S2: ungestoert kriechen — groesster |dx|+|dz| je Bild und dx am Clip-Wrap */
static void s2_wrap(void)
{
    printf("\n===== S2) RE2 ungestoertes Kriechen, 300 Bilder: Sprung ohne Treffer? =====\n");
    bringup(RE15_AI_FLAVOR_RE2, 4, 6000, -1700);
    int a = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active && g_actors[s].x == 950) { a = s; break; }
    if (a < 0) { printf("  FAIL\n"); return; }
    re15_actor_t *e = &g_actors[a];
    s_latch = 1;
    int32_t lx = e->x, lz = e->z; int32_t maxabs = 0; int maxf = -1; int wraps = 0;
    for (int f = 0; f < 300; f++) {
        pin_player(); frame(0, 0);
        int32_t dx = e->x - lx, dz = e->z - lz; lx = e->x; lz = e->z;
        int32_t m = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
        if (m > maxabs) { maxabs = m; maxf = f; }
        if (e->motion == 5 && e->root_prev_kf == 0 && f > 1) {
            wraps++;
            if (wraps <= 3) printf("  wrap f%d: fr=%u rp_kf=%d rp_x=%d dx=%ld dz=%ld r144=%d\n", f,
                                   (unsigned)e->anim_frame, (int)e->root_prev_kf, (int)e->root_prev_x,
                                   (long)dx, (long)dz, (int)e->re2z_root144);
        }
    }
    printf("  max |dx|+|dz| je Bild = %ld (f%d), Wraps = %d, Ende st=%u/%u/%u mo=0x%02X x=%ld z=%ld\n",
           (long)maxabs, maxf, wraps, e->state, e->sub_state_1, e->sub_state_2,
           (unsigned)e->motion, (long)e->x, (long)e->z);
}

/* S3: Teil A nachgestellt — beide Kriecher, Zustandswort f0..f2 und f149 */
static void s3_wait(void)
{
    printf("\n===== S3) RE2 Spawn Cut 4, Spieler am Tuer-Ziel (3650,-3950): beide Kriecher =====\n");
    bringup(RE15_AI_FLAVOR_RE2, 4, 3650, -3950);
    s_latch = 0;
    int slots[8], n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && n < 8; s++) if (g_actors[s].active) slots[n++] = s;
    int32_t x0[8], z0[8];
    for (int i = 0; i < n; i++) { x0[i] = g_actors[slots[i]].x; z0[i] = g_actors[slots[i]].z; }
    for (int f = 0; f < 150; f++) {
        pin_player(); frame(0, 0);
        if (f < 3 || f == 149) for (int i = 0; i < n; i++) line("   ", f, slots[i], x0[i], z0[i]);
    }
}

int main(void)
{
    printf("== probe_r16_sk_kriecher_1010: Skeptiker-Gegensonde ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1010.RDT", &n);
    if (!buf) { printf("FAIL: ROOM1010.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    printf("\n===== S1) RE2 Rueckversatz bei verschiedenen Treffer-Bildern =====\n");
    static const int32_t pxs[] = { 2300, 2450, 2600, 2750, 2900, 3100, 3300 };
    int okc = 0, tot = 0;
    for (size_t i = 0; i < sizeof pxs / sizeof pxs[0]; i++) { tot++; okc += s1_hit(pxs[i]); }
    printf("  S1: %d/%d Faelle mit dx(Wiedereintritt) == sx(r) - Momentaufnahme\n", okc, tot);
    s2_wrap();
    s3_wait();
    printf("\n(Gegensonde: reine Messung)\n");
    return 0;
}
