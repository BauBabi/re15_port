/* probe_rig_root_y.c — MESSREIHE zur Wurzelhoehe des WELLE-G-Hybrid-Rigs.
 * Nutzer-Batch 2026-08-26, FINDING 1: "Bei RE2-KI schwebt der Zombie in 10D0 leicht in der Luft."
 *
 * Das Vor-Dossier (analysis/nutzer_batch_2026-08-25/schweben-10d0.md) hat den Fehler an EINER
 * Pose (Steh-/Walk-Pose) gemessen und daraus einen KONSTANTEN Wurzel-Versatz (+168) abgeleitet.
 * Diese Sonde prueft genau diese Verallgemeinerung: sie misst den Hybrid-Fehler POSE FUER POSE
 * ueber ALLE Clips ALLER Baenke ALLER Hybrid-Typen — und zusaetzlich im echten Raumlauf
 * (10D0/1140/1030/1190 x beide Flavors).
 *
 * Modi:
 *   0 <type>   POSE-SWEEP: jeder Clip/jedes Keyframe jeder Bank, tiefster Bone (Welt-Y)
 *              mit Hybrid AN vs. AUS. Spalten: kf-Wurzel-Y, tiefster Bone, D.
 *   1 <type>   wie 0, aber nur die Zusammenfassung je Clip (min/max/mittel D).
 *   2 <room> <flavor>  RAUMLAUF: echte RDT, echte Spawns, game_step; je Aktor Tick fuer Tick
 *              e->y/floor, tiefster Bone, tiefster MESH-Punkt (Sohle), Clip/Bank/State.
 *   3 <type>   KORREKTUR-VERGLEICH: pro Keyframe der IST-Fehler D gegen die beiden
 *              Kandidaten-Korrekturen ADDITIV(konst) und MULTIPLIKATIV(Skalierung der
 *              Keyframe-Wurzel-Y um chain15/chain2).
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
#include "re15_esp.h"
#include "re15_skeleton.h"
#include "re15_md1.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

extern int  re15_actor_clip_len(const re15_actor_t *a);
extern int  re15_actor_uses_loco_bank(const re15_actor_t *a);
extern int  re15_compute_actor_kf(const re15_emd_animation_t *an, const re15_emd_skeleton_t *sk,
                                  const re15_actor_t *a, int clip_override, uint32_t frame);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- RE1.5-Bank (Geometrie-Spender des Hybrids) ------------------------------------------ *
 * ⚠ PRO TYP eine EIGENE Kopie: skel.keyframe_data und md1.*_vertices zeigen IN den Puffer.
 * Ein gemeinsamer Puffer (frueherer Sondenstand) laesst die zuerst geladene Bank auf die
 * Bytes der zuletzt geladenen zeigen -> Muell-Hoehen (25560, -24178). */
#define P15_MAX 4
static uint8_t  s_em15buf[P15_MAX][0x80000];
static struct { uint8_t type; size_t len; re15_emd_skeleton_t sk; re15_emd_animation_t an;
                re15_md1_t md; } s_p15[P15_MAX];
static int s_p15_n = 0;
static int s_cur15 = -1;                      /* aktiver Slot fuer s_sk15/s_an15/s_md15 */
#define s_sk15    (s_p15[s_cur15].sk)
#define s_an15    (s_p15[s_cur15].an)
#define s_md15    (s_p15[s_cur15].md)
#define s_em15    (s_em15buf[s_cur15])
#define s_em15_len (s_p15[s_cur15].len)
static int load_re15_emd(uint8_t type)
{
    for (int i = 0; i < s_p15_n; i++)
        if (s_p15[i].type == type) { s_cur15 = i; return 0; }
    if (s_p15_n >= P15_MAX) return -9;
    size_t ems_n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &ems_n);
    if (!ems) { fprintf(stderr, "WARN: PSX/EMD/CDEMD0.EMS fehlt\n"); return -1; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int rc = -1;
    int slot = s_p15_n;
    if (idx >= 0 && re15_ems_get_entry(ems, ems_n, idx, &off, &len) == 0 && len <= 0x80000) {
        memcpy(s_em15buf[slot], ems + off, len);
        re15_tim_t tim; memset(&tim, 0, sizeof tim);
        rc = re15_emd_parse_container(s_em15buf[slot], len, &s_p15[slot].md,
                                      &s_p15[slot].sk, &s_p15[slot].an, &tim);
    }
    free(ems);
    if (rc != 0) return rc;
    s_p15[slot].type = type; s_p15[slot].len = len;
    s_cur15 = slot; s_p15_n++;
    return 0;
}
/* Ketten-Verhaeltnis je Typ (fuer die Kandidaten-Korrektur im Raumlauf). */
static struct { uint8_t type; int32_t c2, ch; } s_chain[8]; static int s_chain_n = 0;
static int32_t chain_sum_y(const re15_emd_skeleton_t *sk, int bone);
static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static int load_bank_re2(uint8_t type, int hybrid)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) return -1;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return -2;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) != 0) { eb->type = 0; return -3; }
    eb->buf = NULL; eb->ok = 1;
    int32_t c2 = 0, ch = 0, bb = -1;
    for (int b = 0; b < eb->skel.bone_count; b++) {
        int32_t c = chain_sum_y(&eb->skel, b); if (c > c2) { c2 = c; bb = b; }
    }
    if (hybrid && s_cur15 >= 0 && s_em15_len > 0)
        { int um = -1; re2_hybrid_apply(eb, (int)type, &s_md15, &s_sk15, &um); }
    ch = (bb >= 0) ? chain_sum_y(&eb->skel, bb) : 0;
    if (s_chain_n < 8) { s_chain[s_chain_n].type = type; s_chain[s_chain_n].c2 = c2;
                         s_chain[s_chain_n].ch = ch; s_chain_n++; }
    return 0;
}
static void chain_for(uint8_t type, int32_t *c2, int32_t *ch)
{
    *c2 = 1; *ch = 1;
    for (int i = 0; i < s_chain_n; i++)
        if (s_chain[i].type == type) { *c2 = s_chain[i].c2 ? s_chain[i].c2 : 1; *ch = s_chain[i].ch; return; }
}
static int load_bank_re15(uint8_t type)
{
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb || s_em15_len == 0) return -1;
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(s_em15, s_em15_len, &eb->md1, &eb->skel, &eb->anim, &tim) != 0) return -2;
    eb->ok = 1; eb->buf = NULL;
    eb->loco_ok = (re15_emd_parse_loco_bank(s_em15, s_em15_len, &eb->skel_loco, &eb->anim_loco) == 0);
    eb->own_ok  = (re15_emd_parse_own_bank (s_em15, s_em15_len, &eb->skel_own,  &eb->anim_own)  == 0);
    return 0;
}

/* ---- Geometrie ---------------------------------------------------------------------------- */
/* Tiefster BONE-Ursprung (Modell-lokal, Y nach unten -> Maximum) bei Keyframe kf. */
static int deepest_bone(const re15_emd_skeleton_t *sk, int kf, int *out_bone, int32_t *out_y)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int32_t my = -0x7fffffff; int mb = -1;
    for (int b = 0; b < sk->bone_count; b++)
        if (poses[b].trans[1] > my) { my = poses[b].trans[1]; mb = b; }
    *out_bone = mb; *out_y = my;
    return 1;
}
/* Tiefster MESH-Vertex (Bone-Rotation angewandt) — das ist die echte Sohle. */
static int deepest_mesh(const re15_emd_skeleton_t *sk, const re15_md1_t *md,
                        const int8_t *remap, int remap_ok, int kf, int32_t *out_y)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    int32_t my = -0x7fffffff;
    for (int b = 0; b < sk->bone_count; b++) {
        int mi = remap_ok ? (int)remap[b] : b;
        if (mi < 0 || mi >= md->mesh_count) continue;
        const re15_md1_mesh_t *m = &md->meshes[mi];
        const int32_t *R = poses[b].rot;
        const int32_t *T = poses[b].trans;
        for (int pass = 0; pass < 2; pass++) {
            int n = pass ? m->quad_vertex_count : m->tri_vertex_count;
            const re15_md1_vertex_t *v = pass ? m->quad_vertices : m->tri_vertices;
            for (int i = 0; i < n; i++) {
                int64_t y = (int64_t)R[3] * v[i].x + (int64_t)R[4] * v[i].y + (int64_t)R[5] * v[i].z;
                int32_t wy = (int32_t)(y >> 12) + T[1];
                if (wy > my) my = wy;
            }
        }
    }
    *out_y = my;
    return my != -0x7fffffff;
}
/* Keyframe-Wurzel-Y (die Zahl, um die es geht): pose[0].trans[1] == kf py. */
static int32_t root_kf_y(const re15_emd_skeleton_t *sk, int kf)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    return (rv == 0) ? poses[0].trans[1] : 0;
}

typedef struct { const char *name; const re15_emd_skeleton_t *sk2, *skh;
                 const re15_emd_animation_t *an; } bankpair_t;

static re15_enemy_bank_t s_snapshot;    /* RE2-Bank VOR dem Hybrid */

/* Bein-Ketten-Summe Wurzel->tiefster-Fuss in Y (gerade Kette, ohne Rotation). */
static int32_t chain_sum_y(const re15_emd_skeleton_t *sk, int bone)
{
    int32_t s = 0, j = bone;
    while (j >= 0 && sk->bone_parent[j] >= 0) { s += sk->bone_relative_pos[j][1]; j = sk->bone_parent[j]; }
    return s;
}

static void sweep(uint8_t type, int summary_only, int compare_fixes)
{
    if (load_re15_emd(type) != 0) { printf("EM%02X: kein RE1.5-EMD\n", type); return; }
    re15_enemy_reset();
    if (load_bank_re2(type, 0) != 0) { printf("EM%02X: keine RE2-Bank\n", type); return; }
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    s_snapshot = *eb;                                  /* Skelette VOR dem Hybrid sichern */
    int um = -1; int rc = re2_hybrid_apply(eb, (int)type, &s_md15, &s_sk15, &um);
    printf("\n===== EM%02X  (hybrid rc=%d unmapped=%d) =====\n", type, rc, um);

    /* Ketten-Laengen fuer die multiplikative Kandidaten-Korrektur. */
    int32_t best2 = 0, besth = 0;
    for (int b = 0; b < s_snapshot.skel.bone_count; b++) {
        int32_t c2 = chain_sum_y(&s_snapshot.skel, b);
        if (c2 > best2) { best2 = c2; besth = chain_sum_y(&eb->skel, b); }
    }
    printf("  laengste Y-Kette Wurzel->Bone: RE2=%d  Hybrid(RE1.5)=%d  Delta=%d  Faktor=%.5f\n",
           best2, besth, besth - best2, best2 ? (double)besth / (double)best2 : 0.0);

    bankpair_t banks[3] = {
        { "MAIN", &s_snapshot.skel,      &eb->skel,      &eb->anim      },
        { "LOCO", &s_snapshot.skel_loco, &eb->skel_loco, &eb->anim_loco },
        { "OWN ", &s_snapshot.skel_own,  &eb->skel_own,  &eb->anim_own  },
    };
    int have[3] = { 1, eb->loco_ok, eb->own_ok };

    for (int bi = 0; bi < 3; bi++) {
        if (!have[bi]) continue;
        const bankpair_t *B = &banks[bi];
        if (B->an->clip_count <= 0) continue;
        printf("  -- Bank %s: %d Clips, %d Bones, %d Keyframes\n", B->name,
               B->an->clip_count, B->sk2->bone_count, B->sk2->keyframe_count);
        for (int c = 0; c < B->an->clip_count; c++) {
            const re15_emd_clip_t *cl = &B->an->clips[c];
            if (cl->frame_count <= 0) continue;
            int32_t dmin = 0x7fffffff, dmax = -0x7fffffff; long dsum = 0; int dn = 0;
            int32_t rymin = 0x7fffffff, rymax = -0x7fffffff;
            int32_t amin = 0x7fffffff, amax = -0x7fffffff;   /* Restfehler nach ADDITIV */
            int32_t mmin = 0x7fffffff, mmax = -0x7fffffff;   /* Restfehler nach MULTIPLIKATIV */
            for (int f = 0; f < cl->frame_count; f++) {
                uint32_t fr = B->an->frames[cl->first_frame + f];
                if (fr & 0x8000u) continue;                 /* Marker = Tween, nie posiert */
                int kf = (int)(fr & 0xFFFu);
                if (kf >= B->sk2->keyframe_count) continue;
                int b2 = -1, bh = -1; int32_t y2 = 0, yh = 0;
                if (!deepest_bone(B->sk2, kf, &b2, &y2)) continue;
                if (!deepest_bone(B->skh, kf, &bh, &yh)) continue;
                int32_t ry = root_kf_y(B->sk2, kf);
                int32_t d = yh - y2;                        /* <0 = Hybrid steht HOEHER = schwebt */
                if (d < dmin) dmin = d; if (d > dmax) dmax = d; dsum += d; dn++;
                if (ry < rymin) rymin = ry; if (ry > rymax) rymax = ry;
                /* Kandidat A (additiv): Wurzel-Y += (best2-besth) -> Rest = d + (best2-besth) */
                int32_t ra = d + (best2 - besth);
                if (ra < amin) amin = ra; if (ra > amax) amax = ra;
                /* Kandidat B (multiplikativ): Wurzel-Y *= besth/best2 -> Wurzel wandert um
                 * ry*(besth/best2) - ry = ry*(besth-best2)/best2 ; jeder Bone erbt das. */
                int32_t shift = best2 ? (int32_t)(((int64_t)ry * (besth - best2)) / best2) : 0;
                int32_t rb = d + shift;
                if (rb < mmin) mmin = rb; if (rb > mmax) mmax = rb;
                if (!summary_only && !compare_fixes)
                    printf("     clip%-3d f%-3d kf%-4d rootY=%-7d  tief: RE2 b%-2d=%-7d  HYB b%-2d=%-7d  D=%+d\n",
                           c, f, kf, ry, b2, y2, bh, yh, d);
            }
            if (dn == 0) continue;
            if (compare_fixes)
                printf("     clip%-3d n=%-3d rootY[%6d..%6d] D[%+5d..%+5d avg%+5d] | Rest ADD[%+5d..%+5d] MUL[%+5d..%+5d]\n",
                       c, dn, rymin, rymax, dmin, dmax, (int)(dsum / dn), amin, amax, mmin, mmax);
            else
                printf("     clip%-3d n=%-3d rootY[%6d..%6d]  D[%+d .. %+d]  avg %+d\n",
                       c, dn, rymin, rymax, dmin, dmax, (int)(dsum / dn));
        }
    }
}

/* ---- Raumlauf ------------------------------------------------------------------------------ */
static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static void frame(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

/* Renderer-Bankwahl exakt wie platform/pc/main.c (Sitz-Import 6817-6830, Loco 6752-6759). */
static int actor_pose(const re15_actor_t *e, int32_t *sole_bone, int *sole_idx,
                      int32_t *sole_mesh, int *bank, int *kf_out,
                      int32_t *sole_mesh_add, int32_t *sole_mesh_mul, int32_t *root_y)
{
    const re15_emd_skeleton_t  *sk = NULL;
    const re15_emd_animation_t *an = NULL;
    const re15_md1_t *md = NULL; const int8_t *remap = NULL; int remap_ok = 0;
    int clip_override = -1, which = 0;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (e->re2z_re15_pose && re15_re2z_re15_pose_anim() &&
        (int)e->motion < re15_re2z_re15_pose_anim()->clip_count) {
        sk = re15_re2z_re15_pose_skel(); an = re15_re2z_re15_pose_anim();
        clip_override = (int)e->motion; which = 1;
        /* main.c:6825 setzt hier npc_remap = NULL — das RE1.5-Skelett ist identitaets-gemappt. */
        if (b) { md = &b->md1; remap = NULL; remap_ok = 0; }
    } else {
        if (!b || !b->ok) return 0;
        sk = &b->skel; an = &b->anim; which = 0;
        md = &b->md1; remap = b->mesh_remap; remap_ok = b->remap_ok;
        if (re15_actor_uses_loco_bank(e) && b->loco_ok && (int)e->motion < b->anim_loco.clip_count) {
            sk = &b->skel_loco; an = &b->anim_loco; clip_override = (int)e->motion; which = 2;
        }
    }
    if (!sk || !an || sk->bone_count <= 0 || sk->keyframe_count <= 0) return 0;
    int kf = re15_compute_actor_kf(an, sk, e, clip_override, (uint32_t)e->anim_frame);
    *kf_out = kf; *bank = which;
    int mb = -1; int32_t my = 0;
    if (!deepest_bone(sk, kf, &mb, &my)) return 0;
    *sole_bone = my + e->y; *sole_idx = mb;
    int32_t mesh_y = 0;
    *sole_mesh = (md && deepest_mesh(sk, md, remap, remap_ok, kf, &mesh_y)) ? mesh_y + e->y : 0x7fffffff;
    /* Kandidaten-Korrekturen: BEIDE verschieben nur die Wurzel-Y, alle Bones erben das 1:1. */
    int32_t ry = root_kf_y(sk, kf); *root_y = ry;
    int32_t c2 = 1, ch = 1; chain_for(e->type, &c2, &ch);
    /* ADDITIV: rootY += (c2 - ch)  (der 168er-Vorschlag des Vor-Dossiers)                    */
    *sole_mesh_add = (*sole_mesh == 0x7fffffff) ? 0x7fffffff : *sole_mesh + (c2 - ch);
    /* MULTIPLIKATIV: rootY *= ch/c2 -> Verschiebung ry*(ch-c2)/c2                             */
    *sole_mesh_mul = (*sole_mesh == 0x7fffffff) ? 0x7fffffff
                   : *sole_mesh + (int32_t)(((int64_t)ry * (ch - c2)) / c2);
    /* Im RE1.5-Flavor gibt es keinen Hybrid: beide Korrekturen sind dort 0 (c2 == ch). */
    return 1;
}

static void room_run(int room, int flavor_re2, int ticks)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE%d/ROOM%04X.RDT", base, ((room >> 8) & 0xF) + 0, room);
    /* STAGE-Ordner: Raum 0x1xxx -> STAGE1 */
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", base, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse %s\n", path); return; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(flavor_re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)room;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();

    printf("\n===== ROOM%04X  flavor=%s =====\n", room, flavor_re2 ? "RE2" : "RE15");
    uint8_t types[8]; int nt = 0;
    static re15_emd_skeleton_t  sit_sk; static re15_emd_animation_t sit_an;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);

    for (int t = 0; t < ticks; t++) {
        pl->hp = 100;
        /* Neu aufgetauchte Typen nachladen (1190 spawnt seine Hunde per Skript-Event). */
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active) continue;
            uint8_t ty = g_actors[s].type; if (ty == 0 || ty >= 0x40) continue;
            int seen = 0; for (int k = 0; k < nt; k++) if (types[k] == ty) seen = 1;
            if (seen || nt >= 8) continue;
            types[nt++] = ty;
            printf("  [t%d] Typ 0x%02X aufgetaucht\n", t, ty);
            if (load_re15_emd(ty) != 0) { printf("  EM%02X: kein RE1.5-EMD\n", ty); continue; }
            if (flavor_re2) {
                if (load_bank_re2(ty, 1) != 0) printf("  EM%02X: keine RE2-Bank\n", ty);
                if (nt == 1) { sit_sk = s_sk15; sit_an = s_an15;
                               re15_re2z_set_re15_pose_bank(&sit_sk, &sit_an); }
            } else {
                load_bank_re15(ty);
            }
        }
        /* Spieler an den ERSTEN Gegner heranfuehren -> weckt/engagiert ihn. */
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (g_actors[s].active && nt > 0 && g_actors[s].type == types[0]) {
                pl->x = g_actors[s].x + 1400; pl->z = g_actors[s].z; break;
            }
        }
        frame();
        if (t % 8 != 0) continue;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!e->active || e->type == 0 || e->type >= 0x40) continue;
            int32_t sb = 0, sm = 0, sa = 0, su = 0, ry = 0; int si = -1, bank = -1, kf = -1;
            if (!actor_pose(e, &sb, &si, &sm, &bank, &kf, &sa, &su, &ry)) continue;
            printf("  t%-4d slot%-2d EM%02X st=%u s1=0x%02x clip=%-3d af=%-3u bank=%d kf=%-4d "
                   "y=%-5ld fl=%u rootY=%-6ld | BONE(b%-2d)=%-7ld SOHLE=%-7ld | +ADD=%-7ld +MUL=%-7ld\n",
                   t, s, e->type, e->state, e->sub_state_1, (int)e->motion, e->anim_frame,
                   bank, kf, (long)e->y, e->floor, (long)ry, si, (long)sb,
                   (sm == 0x7fffffff) ? -99999L : (long)sm,
                   (sa == 0x7fffffff) ? -99999L : (long)sa,
                   (su == 0x7fffffff) ? -99999L : (long)su);
        }
    }
    free(buf);
}

int main(int argc, char **argv)
{
    int mode = (argc > 1) ? (int)strtol(argv[1], NULL, 0) : 0;
    if (mode == 0 || mode == 1 || mode == 3) {
        if (argc > 2) {
            sweep((uint8_t)strtol(argv[2], NULL, 0), mode == 1, mode == 3);
        } else {
            static const uint8_t all[] = { 0x10, 0x11, 0x16, 0x13, 0x20, 0x21 };
            for (unsigned k = 0; k < sizeof all; k++) sweep(all[k], mode == 1, mode == 3);
        }
        return 0;
    }
    if (mode == 2) {
        int room   = (argc > 2) ? (int)strtol(argv[2], NULL, 16) : 0x10D0;
        int flavor = (argc > 3) ? (int)strtol(argv[3], NULL, 0)  : 1;
        int ticks  = (argc > 4) ? (int)strtol(argv[4], NULL, 0)  : 300;
        room_run(room, flavor, ticks);
        return 0;
    }
    printf("usage: probe_rig_root_y {0|1|3} [type] | 2 <roomhex> <flavor 0/1> [ticks]\n");
    return 0;
}
