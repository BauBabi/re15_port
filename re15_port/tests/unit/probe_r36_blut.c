/**
 * @file probe_r36_blut.c
 * @brief Runde 36 Spur C — MESSSONDE + PIN fuer den Blut-Anker des RE2-Zombie-Treffers.
 *
 * Nutzer: "Bei den Zombies habe ich manchmal das Gefuehl, dass das Blut nicht an der richtigen
 * Stelle auftaucht, wenn sie getroffen werden."
 *
 * ORIGINAL (RE2 EMZ0.BIN, selbst disassembliert, Dossier C_schuss.md §2.2): die drei Treffer-
 * Emitter spawnen FUN_8001BF10(id, Gier, a2 = Part-WELTMATRIX (+0x198 + part*172 + 72), a3 =
 * VERSATZ im Part-Rahmen) — Lage = R_part * a3 + t_part (re15_esp.c:1455 `Anker.R*Versatz +
 * Anker.T`):
 *   Zone 0 (Beine)  : Part 1, a3 = (0, 800, 0)                      (@0x801056DC-F4 / @0x80105D90-A8)
 *   Zone 1/2 (Rumpf/Kopf): Part 0, a3 = (P8.t.x - 100*Seite, P8.t.y + 300, P8.t.z)
 *                     mit P8.t = lokale Translation von Part 8 (+0x198 + 8*172 + 0x2C, `lw 1420/1424/
 *                     1428`), Seite = +0x15A (= 1 / -1 bei Ruecken @0x801055BC / @0x80105638) bzw.
 *                     +0x16A (Stagger @0x80105D2C); Wurzel-Flinch: y + 500 ohne Seite (@0x80105124-48)
 * PORT (vorher): Anker = Knochen-WELTPOSITION (Part 1 bzw. 0) OHNE a3 -> immer am Becken.
 *
 * Messung: ROOM1140, RE2-KI, echter game_step; Leon steht vor einem stehenden Zombie und schiesst
 * (Pistole 3, EBEN -> Zone 1 Rumpf; TIEF -> Zone 0 Beine). Je Bild wird die GEZEICHNETE Pose
 * (g_anim_pose_actor = Aktor, wie der Renderer) berechnet und die Part-Weltlagen 0/1/8 gemerkt.
 * Beim Blut-Spawn (re15_re2z_last_fx_pos aendert sich) wird die Lage des Bluts gegen
 *   (a) den RE2-Sollpunkt R_part*a3 + t_part aus der im VORBILD gezeichneten Pose (Original),
 *   (b) dieselbe Formel aus der Pose DIESES Bildes
 * verglichen. Pin (Modus "pin"): Abstand zu (a) oder (b) <= Schranke.
 *
 * Aufruf: probe_r36_blut [mess|pin] [eben|tief]
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
#include "re15_skeleton.h"
#include "re2_ems.h"
#include "re15_schuss_r36.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern re15_actor_t g_actors[];
extern re15_enemy_bank_t *re15_enemy_alloc(uint8_t type);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_re2z_last_fx_part(void);
extern void re15_re2z_last_fx_pos(int32_t out[3]);
extern int  re15_compute_actor_kf(const re15_emd_animation_t *anim, const re15_emd_skeleton_t *skel,
                                  const re15_actor_t *a, int clip_override, uint32_t cur);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static void load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}
static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* Gezeichnete Pose wie main.c (g_anim_pose_actor = Aktor, Crossfade laeuft mit). */
typedef struct { int32_t rot[3][9]; int32_t t[3][3]; int ok; int32_t p8loc[3]; } drawn_t;
static void zeichne(re15_actor_t *e, drawn_t *d)
{
    memset(d, 0, sizeof *d);
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || b->skel.bone_count < 9) return;
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, e, -1, e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = e;
    int rv = re15_skel_compute_pose(&b->skel, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return;
    static const int P[3] = { 0, 1, 8 };
    int32_t c = re15_cos_q12(e->rot_y), s = re15_sin_q12(e->rot_y);
    for (int k = 0; k < 3; k++) {
        const re15_skel_pose_t *po = &poses[P[k]];
        /* Welt-R = Ryaw * R_part; Ryaw = [c 0 s; 0 1 0; -s 0 c] */
        for (int col = 0; col < 3; col++) {
            int32_t r0 = po->rot[0*3+col], r1 = po->rot[1*3+col], r2 = po->rot[2*3+col];
            d->rot[k][0*3+col] = (int32_t)(((int64_t)c*r0 + (int64_t)s*r2) >> 12);
            d->rot[k][1*3+col] = r1;
            d->rot[k][2*3+col] = (int32_t)(((int64_t)-s*r0 + (int64_t)c*r2) >> 12);
        }
        re15_skel_bone_to_world(po->trans, e->rot_y, e->x, e->y, e->z, d->t[k]);
    }
    for (int k = 0; k < 3; k++) d->p8loc[k] = b->skel.bone_relative_pos[8][k];
    d->ok = 1;
}
/* RE2-Sollpunkt aus einer gezeichneten Pose: Zone 0 -> Part 1 * (0,800,0); sonst Part 0 * (P8 + (-100*seite, 300, 0)). */
static void sollpunkt(const drawn_t *d, int zone, int seite, int32_t out[3])
{
    int k; int32_t a3[3];
    if (zone == 0) { k = 1; a3[0] = 0; a3[1] = 800; a3[2] = 0; }
    else { k = 0; a3[0] = d->p8loc[0] - 100 * seite; a3[1] = d->p8loc[1] + 300; a3[2] = d->p8loc[2]; }
    for (int r = 0; r < 3; r++)
        out[r] = d->t[k][r] + (int32_t)(((int64_t)d->rot[k][r*3+0]*a3[0] + (int64_t)d->rot[k][r*3+1]*a3[1] +
                                          (int64_t)d->rot[k][r*3+2]*a3[2]) >> 12);
}
static int32_t abst(const int32_t a[3], const int32_t b[3])
{
    int64_t dx = a[0]-b[0], dy = a[1]-b[1], dz = a[2]-b[2];
    int64_t q = dx*dx + dy*dy + dz*dz; int32_t r = 0;
    while ((int64_t)(r+1)*(r+1) <= q) r++;
    return r;
}

int main(int argc, char **argv)
{
    int pin  = (argc > 1 && strcmp(argv[1], "pin") == 0);
    int tief = (argc > 2 && strcmp(argv[2], "tief") == 0);
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);
    re15_enemy_bank_t *bk = re15_enemy_find(0x10);
    if (!bk || !bk->ok) { printf("FAIL: RE2-Bank 0x10 fehlt (RE2/CDEMD0.EMS)\n"); return 1; }
    printf("Bank 0x10: bones=%d P8.loc=(%d,%d,%d) P1.loc=(%d,%d,%d)\n", bk->skel.bone_count,
           bk->skel.bone_relative_pos[8][0], bk->skel.bone_relative_pos[8][1], bk->skel.bone_relative_pos[8][2],
           bk->skel.bone_relative_pos[1][0], bk->skel.bone_relative_pos[1][1], bk->skel.bone_relative_pos[1][2]);

    /* Einen stehenden Zombie (Platz 2, (-1800,-19600)) wecken: Leon 1800 vor ihm in +x. */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);
    {   int es = re15_inv_equipped_slot(); if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    int zs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x10 && g_actors[s].x == -1800 && g_actors[s].z == -19600) zs = s;
    if (zs < 0) { printf("FAIL: Platz (-1800,-19600) nicht gefunden\n"); return 1; }
    re15_actor_t *z = &g_actors[zs];
    pl->x = z->x + 1800; pl->z = z->z; pl->rot_y = 2048;          /* Blick -x auf den Zombie */
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    drawn_t prev, cur; memset(&prev, 0, sizeof prev); memset(&cur, 0, sizeof cur);
    int32_t fx0[3]; re15_re2z_last_fx_pos(fx0);
    int treffer = 0, fails = 0, n_spawn = 0;
    for (int f = 0; f < 240; f++) {
        pl->hp = 100;
        uint16_t cr = RE15_PAD_BIT_R1 | (tief ? RE15_PAD_BIT_DOWN : 0), ed = 0;
        /* Schuss, sobald der Zombie steht (nicht mehr Fresser) und alle 12 Bilder */
        if (f >= 30 && ((f - 30) % 12) == 0 && z->state == 1) { cr |= RE15_PAD_BIT_SQUARE; ed = RE15_PAD_BIT_SQUARE; }
        int32_t hp_vor = z->hp;
        frame(cr, ed);
        zeichne(z, &cur);                                        /* "der Zeichner" dieses Bildes */
        int32_t fx[3]; re15_re2z_last_fx_pos(fx);
        int gespawnt = (fx[0] != fx0[0] || fx[1] != fx0[1] || fx[2] != fx0[2]);
        if (gespawnt && cur.ok && prev.ok) {
            int zone = (int)(z->re2z_hits1d2 % 3u);
            int seite = (z->re2z_hitdir1d0 & 0x20u) ? -1 : 1;
            int32_t soll_prev[3], soll_cur[3];
            sollpunkt(&prev, zone, seite, soll_prev);
            sollpunkt(&cur,  zone, seite, soll_cur);
            int32_t d_prev = abst(fx, soll_prev), d_cur = abst(fx, soll_cur);
            int32_t d_p0 = abst(fx, prev.t[0]), d_p1 = abst(fx, prev.t[1]), d_p8 = abst(fx, prev.t[8 == 8 ? 2 : 2]);
            n_spawn++;
            printf("F%-3d SPAWN part=%d fx=(%d,%d,%d) zone=%d seite=%d hp %d->%d st=%u/%u | Soll(T-1)=(%d,%d,%d) d=%d | "
                   "Soll(T)=(%d,%d,%d) d=%d | zu Part0(T-1) %d, Part1 %d, Part8 %d | Becken y=%d Soll y=%d\n",
                   f, re15_re2z_last_fx_part(), fx[0], fx[1], fx[2], zone, seite, hp_vor, z->hp, z->state, z->sub_state_1,
                   soll_prev[0], soll_prev[1], soll_prev[2], d_prev, soll_cur[0], soll_cur[1], soll_cur[2], d_cur,
                   d_p0, d_p1, d_p8, prev.t[0][1], soll_prev[1]);
            if (pin) {
                int32_t best = d_prev < d_cur ? d_prev : d_cur;
                if (best > 160) { printf("FAIL: Blut %d vom RE2-Sollpunkt entfernt (Schranke 160)\n", best); fails++; }
            }
            fx0[0] = fx[0]; fx0[1] = fx[1]; fx0[2] = fx[2];
        }
        if (z->hp < hp_vor) treffer++;
        prev = cur;
        if (z->hp < 0 || z->state == 3 || z->state == 7) break;
    }
    printf("Treffer %d, Blut-Spawns %d\n", treffer, n_spawn);
    if (pin) {
        if (n_spawn < 1) { printf("FAIL: kein Blut-Spawn gemessen\n"); fails++; }
        if (fails) { printf("probe_r36_blut: %d FAILURES\n", fails); return 1; }
        printf("probe_r36_blut: OK\n");
    }
    return 0;
}
