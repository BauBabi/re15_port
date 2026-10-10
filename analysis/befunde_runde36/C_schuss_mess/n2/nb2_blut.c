/* Nachbesserung 2 Spur C — Mess-Sonde (Scratchpad, nicht im Repo).
 * Aufbau wie abn1_blut.c der Abnahme 1 (ROOM1140, RE2-KI, RE2-Bank CDEMD0.EMS, echter game_step), aber die
 * "gezeichnete" Pose ist hier ein ZWILLING DES PORT-RENDERERS (platform/pc/main.c, Zeichenblock NPC):
 *   - Bank wie main.c:9798-9925 (re15_actor_anim_select -> clip_override; Loco-Bank bei
 *     re15_actor_uses_loco_bank; RE1.5-Sitz-Import re2z_re15_pose),
 *   - Crossfade mit g_anim_pose_actor = Aktor (main.c:9960, mutiert wie der Renderer EINMAL je Bild),
 *   - Unterhaelfte Bones 1..6 aus re15_re2z_lower_kf (main.c:10195-10220),
 *   - Ragdoll-Bodenanker (main.c:10315-10330), dann Gore-Teil mit eigener Matrix + Physik
 *     re15_re2z_gore_part_matrix (main.c:10339) — Spawns waehrend des Zeichnens (Aufschlag-Blut) werden mitgezaehlt.
 * Jeder Spawn des Mess-Rings wird gegen R*a3 + T dieses Zwillings (Bild T) gemessen ("d_r"), dazu die Metrik der
 * Abnahme ("d_alt" = Hauptbank, ohne clip_override/Ragdoll, Schatten-Kopie).
 * Aufruf: nb2_blut <waffe> <px|pz|nx> <eben|tief> [abstand=1800] [platz=2] */
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
#include "re15_anim_select.h"
#include "re15_engine.h"
#include "re2_ems.h"
#include "re15_schuss_r36.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern re15_actor_t g_actors[];
extern re15_enemy_bank_t *re15_enemy_alloc(uint8_t type);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern void re15_player_set_aim_elevation_for_test(int);

static re15_rdt_t s_rdt; static re15_camera_view_t s_cam; static re15_game_ctx_t s_ctx;
static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
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
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; } else eb->type = 0;
}
static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}
typedef struct { int ok, n, loco, rag; int flieg[RE15_EMD_MAX_BONES]; int32_t R[RE15_EMD_MAX_BONES][9]; int32_t T[RE15_EMD_MAX_BONES][3]; } pose_t;

/* Zwilling des Renderers (s. Kopf). mut = 1: wie der Renderer (Blend-Zustand + Gore-Physik schreiten fort). */
static void zeichne_r(re15_actor_t *e, pose_t *d, int mut)
{
    memset(d, 0, sizeof *d);
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || b->skel.bone_count < 9) return;
    re15_anim_banks_t banks; memset(&banks, 0, sizeof banks);
    re15_anim_view_t av; re15_actor_anim_select(e, 0, &banks, &av);
    const re15_emd_skeleton_t *sk = av.skel; const re15_emd_animation_t *an = av.anim;
    if (re15_actor_uses_loco_bank(e)) {
        if (b->loco_ok && (int)e->motion < b->anim_loco.clip_count) { sk = &b->skel_loco; an = &b->anim_loco; av.clip_override = (int)e->motion; d->loco = 1; }
    } else if (e->re2z_re15_pose) {
        const re15_emd_skeleton_t *rs = re15_re2z_re15_pose_skel(); const re15_emd_animation_t *ra = re15_re2z_re15_pose_anim();
        if (rs && ra && (int)e->motion < ra->clip_count) { sk = rs; an = ra; av.clip_override = (int)e->motion; d->loco = 2; }
    }
    if (!sk || !an || an->clip_count <= 0) return;
    static re15_actor_t sh;
    re15_actor_t *pa = e;
    if (!mut) { sh = *e; pa = &sh; }
    int kf = re15_compute_actor_kf(an, sk, pa, av.clip_override, (uint32_t)e->anim_frame);
    re15_skel_pose_t po[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = pa;
    int rv = re15_skel_compute_pose(sk, kf, po);
    g_anim_pose_actor = save;
    if (rv != 0) return;
    int nb = sk->bone_count; if (nb > RE15_EMD_MAX_BONES) nb = RE15_EMD_MAX_BONES;
    re15_skel_pose_t lo[RE15_EMD_MAX_BONES]; int lo_ok = 0;
    if (e->re2z_rag_anchor_on) {
        const re15_emd_skeleton_t *ls = NULL; int lkf = re15_re2z_lower_kf(e, &ls); if (!ls) ls = sk;
        void *s2 = g_anim_pose_actor; g_anim_pose_actor = NULL;
        lo_ok = (re15_skel_compute_pose(ls, lkf, lo) == 0);
        g_anim_pose_actor = s2;
    }
    int32_t c = re15_cos_q12(e->rot_y), s = re15_sin_q12(e->rot_y);
    int32_t ny[9] = { c, 0, s, 0, 0x1000, 0, -s, 0, c };
    uint8_t gd[RE15_EMD_MAX_BONES]; uint32_t gt[RE15_EMD_MAX_BONES]; uint8_t gm[RE15_EMD_MAX_BONES];
    for (int i = 0; i < RE15_EMD_MAX_BONES; i++) { gd[i] = 1; gt[i] = 0x808080u; gm[i] = (uint8_t)i; }
    int gore_on = re15_re2z_gore_resolve(e, sk->bone_parent, nb, gd, gt, gm);
    d->rag = e->re2z_rag_anchor_on;
    d->n = nb;
    for (int k = 0; k < nb; k++) {
        const re15_skel_pose_t *np = &po[k];
        if (e->re2z_rag_anchor_on && k >= 1 && k <= 6 && lo_ok) np = &lo[k];
        int32_t yr[9], yt[3];
        for (int r = 0; r < 3; r++) for (int cc = 0; cc < 3; cc++) {
            int64_t a = 0; for (int q = 0; q < 3; q++) a += (int64_t)ny[r*3+q] * np->rot[q*3+cc];
            yr[r*3+cc] = (int32_t)(a >> 12);
        }
        for (int r = 0; r < 3; r++) { int64_t a = 0; for (int q = 0; q < 3; q++) a += (int64_t)ny[r*3+q] * np->trans[q]; yt[r] = (int32_t)(a >> 12); }
        int32_t T[3] = { yt[0] + e->x, yt[1] + e->y, yt[2] + e->z };
        if (e->re2z_rag_anchor_on) {
            int32_t p0w[3];
            for (int r = 0; r < 3; r++) { int64_t a = 0; for (int q = 0; q < 3; q++) a += (int64_t)ny[r*3+q] * po[0].trans[q]; p0w[r] = (int32_t)(a >> 12); }
            p0w[0] += e->x; p0w[1] += e->y; p0w[2] += e->z;
            re15_re2z_ragdoll_part_anchor(e, k, p0w, T);
        }
        if (gore_on && mut) d->flieg[k] = re15_re2z_gore_part_matrix(e, k, g_engine.frame_count, yr, T);
        memcpy(d->R[k], yr, sizeof yr); memcpy(d->T[k], T, sizeof T);
    }
    d->ok = 1;
}
/* Metrik der Abnahme 1 (abn1_blut.c zeichne): Hauptbank, clip_override -1, Schatten (nicht mutierend). */
static void zeichne_alt(re15_actor_t *e, pose_t *d)
{
    memset(d, 0, sizeof *d);
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || b->skel.bone_count < 9) return;
    static re15_actor_t sh; sh = *e;
    int kf = re15_compute_actor_kf(&b->anim, &b->skel, &sh, -1, e->anim_frame);
    re15_skel_pose_t po[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = &sh;
    int rv = re15_skel_compute_pose(&b->skel, kf, po);
    g_anim_pose_actor = save;
    if (rv != 0) return;
    int32_t c = re15_cos_q12(e->rot_y), s = re15_sin_q12(e->rot_y);
    d->n = b->skel.bone_count;
    for (int k = 0; k < d->n; k++) {
        for (int col = 0; col < 3; col++) {
            int32_t r0 = po[k].rot[0*3+col], r1 = po[k].rot[1*3+col], r2 = po[k].rot[2*3+col];
            d->R[k][0*3+col] = (int32_t)(((int64_t)c*r0 + (int64_t)s*r2) >> 12);
            d->R[k][1*3+col] = r1;
            d->R[k][2*3+col] = (int32_t)(((int64_t)-s*r0 + (int64_t)c*r2) >> 12);
        }
        re15_skel_bone_to_world(po[k].trans, e->rot_y, e->x, e->y, e->z, d->T[k]);
    }
    d->ok = 1;
}
static void erwart(const pose_t *d, int k, const int32_t a[3], int32_t o[3])
{
    for (int r = 0; r < 3; r++)
        o[r] = d->T[k][r] + (int32_t)(((int64_t)d->R[k][r*3+0]*a[0] + (int64_t)d->R[k][r*3+1]*a[1] + (int64_t)d->R[k][r*3+2]*a[2]) >> 12);
}
static int32_t abst(const int32_t a[3], const int32_t b[3])
{
    int64_t dx = a[0]-b[0], dy = a[1]-b[1], dz = a[2]-b[2]; int64_t q = dx*dx+dy*dy+dz*dz; int32_t r = 0;
    while ((int64_t)(r+1)*(r+1) <= q) r++;
    return r;
}

int main(int argc, char **argv)
{
    int waffe = argc > 1 ? atoi(argv[1]) : 3;
    const char *seite = argc > 2 ? argv[2] : "px";
    int tief = argc > 3 && strcmp(argv[3], "tief") == 0;
    int D = argc > 4 ? atoi(argv[4]) : 1800;
    int platz = argc > 5 ? atoi(argv[5]) : 2;
    int nach = argc > 6 ? atoi(argv[6]) : 120;
    static const int PX[6] = { 0, -800, -1800, -1800, 200, 200 }, PZ[6] = { 0, -20600, -19600, -21600, -21600, -19600 };
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf || re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x1234567u);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_inv_grant(waffe, 250);
    re15_player_set_equipped_weapon(waffe);
    { int es = re15_inv_equipped_slot(); if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    int zs = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].x == PX[platz] && g_actors[s].z == PZ[platz]) zs = s;
    if (zs < 0) { printf("FAIL Platz %d\n", platz); return 1; }
    re15_actor_t *z = &g_actors[zs];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != zs) g_actors[s].active = 0;
    if (!strcmp(seite, "px")) { pl->x = z->x + D; pl->z = z->z; pl->rot_y = 2048; }
    else if (!strcmp(seite, "nx")) { pl->x = z->x - D; pl->z = z->z; pl->rot_y = 0; }
    else { pl->x = z->x; pl->z = z->z + D; pl->rot_y = 1024; }
    printf("nb2_blut: waffe %d seite %s %s D %d platz %d slot %d typ 0x%02x\n", waffe, seite, tief ? "tief" : "eben", D, platz, zs, z->type);
    int autow = (waffe == 12 || waffe == 19);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); g_engine.frame_count++; }
    pose_t cur, alt;
    int tot_ab = -1, steht = -1, n_spawn = 0, n_null = 0, max_r_mit = 0, max_r_null = 0, n_r_null_gt = 0, n_r_mit_gt = 0, n_loco = 0;
    for (int f = 0; f < 3000; f++) {
        pl->hp = 100;
        uint16_t cr = RE15_PAD_BIT_R1 | (tief ? RE15_PAD_BIT_DOWN : 0), ed = 0;
        if (steht < 0 && z->state == 1 && z->sub_state_1 != 8) steht = f;
        if (tot_ab < 0 && steht >= 0 && f >= steht + 20) {
            if (autow) { cr |= RE15_PAD_BIT_SQUARE; if (((f - steht) % 30) == 0) ed = RE15_PAD_BIT_SQUARE; }
            else if (((f - steht - 20) % 12) == 0) { cr |= RE15_PAD_BIT_SQUARE; ed = RE15_PAD_BIT_SQUARE; }
        }
        int32_t hp_vor = z->hp; unsigned st_vor = z->state;
        unsigned n0 = re15_schuss_r36_fx_zahl();
        frame(cr, ed);
        unsigned n1 = re15_schuss_r36_fx_zahl();
        zeichne_alt(z, &alt);                 /* vor dem mutierenden Zwilling */
        zeichne_r(z, &cur, 1);                /* der "Renderer" dieses Bilds */
        unsigned n2 = re15_schuss_r36_fx_zahl();
        g_engine.frame_count++;
        int died = (hp_vor >= 0 && z->hp < 0) || ((st_vor != 3u && st_vor != 7u) && (z->state == 3u || z->state == 7u));
        for (unsigned nr = n0; nr < n2; nr++) {
            int part, mit; int32_t fx[3], a3[3];
            if (!re15_schuss_r36_fx_holen(nr, &part, fx, a3, &mit)) continue;
            n_spawn++;
            int beim_zeichnen = (nr >= n1);
            int32_t null3[3] = { 0, 0, 0 }, er[3], ea[3];
            int dr = -1, da = -1;
            if (cur.ok && part < cur.n) { erwart(&cur, part, mit ? a3 : null3, er); dr = abst(fx, er); }
            if (alt.ok && part < alt.n) { erwart(&alt, part, mit ? a3 : null3, ea); da = abst(fx, ea); }
            int fl = (cur.ok && part < cur.n) ? cur.flieg[part] : 0;
            printf("F%-4d %s%s st %u->%u/%u/%u hp %d->%d rag %d bank %s fl %d | part %2d fx=(%d,%d,%d) a3=(%d,%d,%d)%s | d_r %d | d_alt %d\n",
                   f, died ? "TOD " : (z->hp < hp_vor ? "TREF" : "    "), beim_zeichnen ? "Z" : " ", st_vor, z->state, z->sub_state_1, z->sub_state_2,
                   hp_vor, z->hp, cur.rag, cur.loco == 1 ? "loco" : (cur.loco == 2 ? "re15" : "haupt"), fl, part, fx[0], fx[1], fx[2],
                   a3[0], a3[1], a3[2], mit ? "" : " (ohne)", dr, da);
            if (cur.loco) n_loco++;
            if (!mit) { n_null++; if (dr > max_r_null) max_r_null = dr; if (dr > 40) n_r_null_gt++; }
            else { if (dr > max_r_mit) max_r_mit = dr; if (dr > 40) n_r_mit_gt++; }
        }
        if (tot_ab < 0 && died) tot_ab = f;
        if (tot_ab >= 0 && f > tot_ab + nach) break;
    }
    printf("ERGEBNIS waffe %d %s %s: Tod %d, Spawns %d (ohne a3 %d), Spawns mit Loco/RE15-Bank %d | d_r max mit a3 %d (>40: %d), ohne a3 %d (>40: %d)\n",
           waffe, seite, tief ? "tief" : "eben", tot_ab, n_spawn, n_null, n_loco, max_r_mit, n_r_mit_gt, max_r_null, n_r_null_gt);
    return 0;
}
