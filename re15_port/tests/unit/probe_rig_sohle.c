/* probe_rig_sohle.c — MESSSONDE (kein ctest-Assert), Nutzer-Batch 2026-08-26.
 *
 * FRAGE: "Bei RE2-KI schwebt der Zombie leicht in der Luft."  Gemessen wird NICHT ein
 * einzelner Bone, sondern die ECHTE SOHLE: der tiefste MESH-VERTEX in WELT-Koordinaten,
 * exakt mit der Renderer-Kette aus platform/pc/main.c:6957-7050:
 *      v_world = (nyaw * pose[b].rot) * v_local + (nyaw * pose[b].trans) + (e->x,e->y,e->z)
 * mit yaw = 0 und e = (0,0,0), also  sohle = max_b max_v  (rot*v + trans).y
 * PSX-Y zeigt nach UNTEN:  sohle == 0 -> steht auf der Bodenebene,
 *                          sohle <  0 -> SCHWEBT um |sohle|,
 *                          sohle >  0 -> steckt im Boden.
 *
 * Der Mesh-Index folgt mesh_remap wie im Renderer (main.c:7043-7049): -1 = Slot wird
 * NICHT gezeichnet (RE2-Hundepfoten ohne RE1.5-Gegenstueck).
 *
 * Gemessen werden DREI Konfigurationen pro Typ:
 *   RE15  = RE1.5-Rig + RE1.5-Keyframes + RE1.5-Mesh   (der RE1.5-Flavor)
 *   RE2   = RE2-Rig   + RE2-Keyframes   + RE2-Mesh     (RE2-Flavor OHNE Hybrid)
 *   HYB   = RE2-Keyframes + RE1.5-Bindlaengen + RE1.5-Mesh (WELLE-G-Hybrid = das Spiel)
 * und zusaetzlich drei KORREKTUR-KANDIDATEN auf HYB (siehe unten).
 *
 * Aufruf:
 *   probe_rig_sohle sweep <typehex>     — ALLE Clips ALLER Baenke, Frame fuer Frame
 *   probe_rig_sohle room  <roomhex> <0|1>  — Live-Lauf (0 = RE1.5-Flavor, 1 = RE2)
 *   probe_rig_sohle chain <typehex>     — Bind-Ketten + Wurzel-Bind beider Rigs
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

extern int re15_actor_clip_len(const re15_actor_t *a);
extern int re15_actor_uses_loco_bank(const re15_actor_t *a);
extern int re15_compute_actor_kf(const re15_emd_animation_t *an, const re15_emd_skeleton_t *sk,
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

/* ---------- Assets -------------------------------------------------------
 * WICHTIG: jeder Typ bekommt seinen EIGENEN Puffer. md1/skel/anim zeigen IN den Puffer;
 * ein gemeinsamer Puffer wuerde beim Laden des zweiten Typs die Zeiger des ersten
 * auf fremde Bytes umbiegen (das erzeugt die absurden Sohlen-Werte 3000..45000). */
typedef struct {
    uint8_t used, type;
    uint8_t *buf; size_t len;
    re15_emd_skeleton_t  sk, sk_loco, sk_own;
    re15_emd_animation_t an, an_loco, an_own;
    re15_md1_t           md;
    int loco_ok, own_ok;
} m15_t;
static m15_t s_m15[6];

static m15_t *load_re15_slot(uint8_t type)
{
    for (int i = 0; i < 6; i++) if (s_m15[i].used && s_m15[i].type == type) return &s_m15[i];
    m15_t *m = NULL;
    for (int i = 0; i < 6; i++) if (!s_m15[i].used) { m = &s_m15[i]; break; }
    if (!m) return NULL;
    size_t n = 0; uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) { fprintf(stderr, "FAIL: PSX/EMD/CDEMD0.EMS\n"); return NULL; }
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; m15_t *rv = NULL;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0) {
        m->buf = (uint8_t *)malloc(len);
        if (m->buf) {
            memcpy(m->buf, ems + off, len); m->len = len;
            re15_tim_t tim; memset(&tim, 0, sizeof tim);
            if (re15_emd_parse_container(m->buf, len, &m->md, &m->sk, &m->an, &tim) == 0) {
                m->loco_ok = (re15_emd_parse_loco_bank(m->buf, len, &m->sk_loco, &m->an_loco) == 0);
                m->own_ok  = (re15_emd_parse_own_bank (m->buf, len, &m->sk_own,  &m->an_own)  == 0);
                m->used = 1; m->type = type; rv = m;
            }
        }
    }
    free(ems);
    return rv;
}

static uint8_t *s_ems2 = NULL; static size_t s_ems2_n = 0;
static int load_re2(uint8_t type, re15_enemy_bank_t *eb)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) { fprintf(stderr, "FAIL: RE2/CDEMD0.EMS\n"); return -1; }
    memset(eb, 0, sizeof *eb);
    eb->type = type;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) != 0) return -1;
    eb->ok = 1; eb->buf = NULL;
    return 0;
}

/* ---------- Sohle: tiefster WELT-Vertex, Renderer-Kette ------------------- */
typedef struct {
    int32_t sole;        /* max world Y ueber alle gezeichneten Vertices    */
    int     sole_bone;
    int32_t lowbone;     /* max world Y ueber alle Bone-Origins             */
    int     lowbone_i;
    int32_t rooty;       /* pose[0].trans[1] = Keyframe-Wurzel-Y            */
} sole_t;

static int measure(const re15_emd_skeleton_t *sk, const re15_md1_t *md,
                   const int8_t *remap, int kf, sole_t *out)
{
    memset(out, 0, sizeof *out);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;   /* kein Crossfade */
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return -1;
    int nb = sk->bone_count;
    if (!remap && nb > md->mesh_count) nb = md->mesh_count;      /* main.c:6916 */
    if (nb > RE15_EMD_MAX_BONES) nb = RE15_EMD_MAX_BONES;
    int32_t best = -0x7fffffff, bestb = -1, blow = -0x7fffffff, blowi = -1;
    for (int b = 0; b < nb; b++) {
        const re15_skel_pose_t *p = &poses[b];
        if (p->trans[1] > blow) { blow = p->trans[1]; blowi = b; }
        int mi = b;
        if (remap) { mi = (b < RE15_EMD_MAX_BONES) ? (int)remap[b] : -1; if (mi < 0) continue; }
        if (mi >= md->mesh_count) continue;
        const re15_md1_mesh_t *m = &md->meshes[mi];
        const re15_md1_vertex_t *lists[2] = { m->tri_vertices, m->quad_vertices };
        int counts[2] = { m->tri_vertex_count, m->quad_vertex_count };
        for (int L = 0; L < 2; L++) {
            for (int i = 0; i < counts[L]; i++) {
                int32_t v[3] = { lists[L][i].x, lists[L][i].y, lists[L][i].z };
                int64_t s = 0;
                for (int k = 0; k < 3; k++) s += (int64_t)p->rot[3 + k] * (int64_t)v[k];
                int32_t wy = (int32_t)(s >> 12) + p->trans[1];
                if (wy > best) { best = wy; bestb = b; }
            }
        }
    }
    out->sole = best; out->sole_bone = bestb;
    out->lowbone = blow; out->lowbone_i = blowi;
    out->rooty = poses[0].trans[1];
    return 0;
}

/* ---------- Kandidat: exakte Ketten-Differenz PRO KEYFRAME ----------------
 * Korrektur = (tiefster Punkt mit RE2-Bindlaengen) - (tiefster Punkt mit RE1.5-Bindlaengen),
 * bei IDENTISCHEN Rotationen. Rein aus den zwei gelesenen Bind-Tabellen; keine Konstante. */
static int32_t cand_perframe(const re15_emd_skeleton_t *sk_re2bind,
                             const re15_emd_skeleton_t *sk_re15bind,
                             const re15_md1_t *md2, const re15_md1_t *md15,
                             const int8_t *remap, int kf, int use_mesh)
{
    sole_t a, b;
    if (measure(sk_re2bind, md2, NULL, kf, &a) != 0) return 0;
    if (measure(sk_re15bind, md15, remap, kf, &b) != 0) return 0;
    return use_mesh ? (a.sole - b.sole) : (a.lowbone - b.lowbone);
}

/* ---------- Bank-Sweep --------------------------------------------------- */
typedef struct {
    const char *name;
    const re15_emd_skeleton_t *sk; const re15_emd_animation_t *an;
    const re15_md1_t *md; const int8_t *remap;
} cfg_t;

static void sweep_bank(const char *bankname,
                       const cfg_t *c15, const cfg_t *c2, const cfg_t *chy,
                       const re15_emd_skeleton_t *sk2_bind, const re15_md1_t *md2)
{
    printf("\n-- Bank %s --\n", bankname);
    printf("   %-5s %-6s | %-24s | %-24s | %-24s | Korrektur-Kandidaten\n",
           "clip", "frames", "RE15  sohle min/max", "RE2   sohle min/max", "HYB   sohle min/max");
    int nc = 0;
    if (c15 && c15->an) nc = c15->an->clip_count;
    if (c2 && c2->an && c2->an->clip_count > nc) nc = c2->an->clip_count;
    for (int cl = 0; cl < nc; cl++) {
        int32_t mn[3] = { 0x7fffffff, 0x7fffffff, 0x7fffffff };
        int32_t mx[3] = { -0x7fffffff, -0x7fffffff, -0x7fffffff };
        int32_t kmin = 0x7fffffff, kmax = -0x7fffffff;      /* per-frame Kandidat (Mesh)  */
        int32_t bmin = 0x7fffffff, bmax = -0x7fffffff;      /* per-frame Kandidat (Bones) */
        int32_t fbmin = 0x7fffffff, fbmax = -0x7fffffff;    /* Sohle NACH Bone-Korrektur  */
        int32_t fmmin = 0x7fffffff, fmmax = -0x7fffffff;    /* Sohle NACH Mesh-Korrektur  */
        int fr[3] = { 0, 0, 0 };
        const cfg_t *cfgs[3] = { c15, c2, chy };
        for (int k = 0; k < 3; k++) {
            const cfg_t *c = cfgs[k];
            if (!c || !c->an || cl >= c->an->clip_count) continue;
            const re15_emd_clip_t *cp = &c->an->clips[cl];
            fr[k] = cp->frame_count;
            for (int f = 0; f < cp->frame_count; f++) {
                int idx = cp->first_frame + f;
                if (idx < 0 || idx >= c->an->frame_count) break;
                int kf = (int)(c->an->frames[idx] & 0xFFFu);
                if (kf >= c->sk->keyframe_count) continue;
                sole_t s;
                if (measure(c->sk, c->md, c->remap, kf, &s) != 0) continue;
                if (s.sole < mn[k]) mn[k] = s.sole;
                if (s.sole > mx[k]) mx[k] = s.sole;
                if (k == 2 && sk2_bind) {
                    int32_t dm = cand_perframe(sk2_bind, c->sk, md2, c->md, c->remap, kf, 1);
                    int32_t db = cand_perframe(sk2_bind, c->sk, md2, c->md, c->remap, kf, 0);
                    if (dm < kmin) kmin = dm; if (dm > kmax) kmax = dm;
                    if (db < bmin) bmin = db; if (db > bmax) bmax = db;
                    /* KORRIGIERTE Sohle: HYB-Sohle + Korrektur, Keyframe fuer Keyframe. */
                    int32_t cb = s.sole + db, cm = s.sole + dm;
                    if (cb < fbmin) fbmin = cb; if (cb > fbmax) fbmax = cb;
                    if (cm < fmmin) fmmin = cm; if (cm > fmmax) fmmax = cm;
                }
            }
        }
        if (fr[0] == 0 && fr[1] == 0 && fr[2] == 0) continue;
        printf("   %-5d %2d/%2d/%2d |", cl, fr[0], fr[1], fr[2]);
        for (int k = 0; k < 3; k++) {
            if (mx[k] == -0x7fffffff) printf(" %24s |", "-");
            else printf(" %11ld .. %-10ld |", (long)mn[k], (long)mx[k]);
        }
        if (kmax != -0x7fffffff)
            printf(" K: mesh %+ld..%+ld bone %+ld..%+ld | SOHLE nach Fix: bone %ld..%ld  mesh %ld..%ld",
                   (long)kmin, (long)kmax, (long)bmin, (long)bmax,
                   (long)fbmin, (long)fbmax, (long)fmmin, (long)fmmax);
        printf("\n");
    }
}

static const re15_md1_t *s_sit_md = NULL;

/* ---------- Live-Raumlauf ------------------------------------------------ */
static re15_rdt_t s_rdt; static re15_camera_view_t s_cam; static re15_game_ctx_t s_ctx;
static void frame(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    re15_game_step(&s_ctx);
}

/* Bankwahl EXAKT wie main.c (Sitz-Import 6817-6830, Loco 6752-6759). */
static int live_pose(const re15_actor_t *e, sole_t *out)
{
    const re15_emd_skeleton_t *sk = NULL; const re15_emd_animation_t *an = NULL;
    const re15_md1_t *md = NULL; const int8_t *remap = NULL;
    int clip_override = -1;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (e->re2z_re15_pose && re15_re2z_re15_pose_anim() &&
        (int)e->motion < re15_re2z_re15_pose_anim()->clip_count) {
        sk = re15_re2z_re15_pose_skel(); an = re15_re2z_re15_pose_anim();
        md = s_sit_md; remap = NULL; clip_override = (int)e->motion;
        if (!md) return -1;
    } else {
        if (!b || !b->ok) return -1;
        sk = &b->skel; an = &b->anim; md = &b->md1;
        remap = b->remap_ok ? b->mesh_remap : NULL;
        if (re15_actor_uses_loco_bank(e) && b->loco_ok &&
            (int)e->motion < b->anim_loco.clip_count) {
            sk = &b->skel_loco; an = &b->anim_loco; clip_override = (int)e->motion;
        }
    }
    if (!sk || !an || sk->bone_count <= 0 || an->clip_count <= 0) return -1;
    int kf = re15_compute_actor_kf(an, sk, e, clip_override, (uint32_t)e->anim_frame);
    if (kf < 0 || kf >= sk->keyframe_count) return -1;
    if (measure(sk, md, remap, kf, out) != 0) return -1;
    out->sole  += (int32_t)e->y;
    out->lowbone += (int32_t)e->y;
    return 0;
}

int main(int argc, char **argv)
{
    const char *cmd = (argc > 1) ? argv[1] : "sweep";
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);

    if (!strcmp(cmd, "chain") || !strcmp(cmd, "sweep")) {
        uint8_t ty = (uint8_t)strtol((argc > 2) ? argv[2] : "10", NULL, 16);
        m15_t *M = load_re15_slot(ty);
        if (!M) { printf("FAIL: RE1.5 EM%02X\n", ty); return 1; }
        static re15_enemy_bank_t b2, bh;
        if (load_re2(ty, &b2) != 0) { printf("FAIL: RE2 EM%02X\n", ty); return 1; }
        if (load_re2(ty, &bh) != 0) { printf("FAIL: RE2 EM%02X (2)\n", ty); return 1; }
        int um = -1;
        int rc = re2_hybrid_apply(&bh, (int)ty, &M->md, &M->sk, &um);
        printf("== EM%02X ==  RE1.5 bones=%d meshes=%d clips=%d loco=%d own=%d | "
               "RE2 bones=%d meshes=%d clips=%d loco=%d own=%d | hybrid rc=%d unmapped=%d\n",
               ty, M->sk.bone_count, M->md.mesh_count, M->an.clip_count, M->loco_ok, M->own_ok,
               b2.skel.bone_count, b2.md1.mesh_count, b2.anim.clip_count, b2.loco_ok, b2.own_ok,
               rc, um);
        if (rc != 0) { printf("  (kein Hybrid fuer diesen Typ)\n"); }

        if (!strcmp(cmd, "chain")) {
            const int8_t *perm = NULL; int n = re2_hybrid_perm((int)ty, &perm);
            printf("  Wurzel-BIND  RE1.5 bone0 = (%d,%d,%d) | RE2 bone0 = (%d,%d,%d)\n",
                   M->sk.bone_relative_pos[0][0], M->sk.bone_relative_pos[0][1],
                   M->sk.bone_relative_pos[0][2],
                   b2.skel.bone_relative_pos[0][0], b2.skel.bone_relative_pos[0][1],
                   b2.skel.bone_relative_pos[0][2]);
            printf("  i par perm | RE2 y | RE15 y | Kette-Y RE2 | Kette-Y RE15(nach Hybrid)\n");
            for (int i = 0; i < n && i < b2.skel.bone_count; i++) {
                int32_t s2 = 0, sh = 0, j = i;
                while (j >= 0 && b2.skel.bone_parent[j] >= 0)
                { s2 += b2.skel.bone_relative_pos[j][1]; j = b2.skel.bone_parent[j]; }
                j = i;
                while (j >= 0 && bh.skel.bone_parent[j] >= 0)
                { sh += bh.skel.bone_relative_pos[j][1]; j = bh.skel.bone_parent[j]; }
                printf("  %-2d %-3d %-4d | %6d | %6d | %8d | %8d   (D=%+d)\n",
                       i, (int)b2.skel.bone_parent[i], perm ? (int)perm[i] : -1,
                       b2.skel.bone_relative_pos[i][1],
                       bh.skel.bone_relative_pos[i][1], s2, sh, sh - s2);
            }
            return 0;
        }

        cfg_t c15m = { "RE15", &M->sk, &M->an, &M->md, NULL };
        cfg_t c2m  = { "RE2",  &b2.skel, &b2.anim, &b2.md1, NULL };
        cfg_t chym = { "HYB",  &bh.skel, &bh.anim, &bh.md1, bh.remap_ok ? bh.mesh_remap : NULL };
        sweep_bank("MAIN", &c15m, &c2m, &chym, &b2.skel, &b2.md1);

        if (M->loco_ok && b2.loco_ok) {
            cfg_t a = { "RE15", &M->sk_loco, &M->an_loco, &M->md, NULL };
            cfg_t b = { "RE2",  &b2.skel_loco, &b2.anim_loco, &b2.md1, NULL };
            cfg_t c = { "HYB",  &bh.skel_loco, &bh.anim_loco, &bh.md1,
                        bh.remap_ok ? bh.mesh_remap : NULL };
            sweep_bank("LOCO", &a, &b, &c, &b2.skel_loco, &b2.md1);
        }
        if (M->own_ok && b2.own_ok) {
            cfg_t a = { "RE15", &M->sk_own, &M->an_own, &M->md, NULL };
            cfg_t b = { "RE2",  &b2.skel_own, &b2.anim_own, &b2.md1, NULL };
            cfg_t c = { "HYB",  &bh.skel_own, &bh.anim_own, &bh.md1,
                        bh.remap_ok ? bh.mesh_remap : NULL };
            sweep_bank("OWN", &a, &b, &c, &b2.skel_own, &b2.md1);
        }
        return 0;
    }

    /* ---- room <hex> <flavor> ---- */
    if (!strcmp(cmd, "room")) {
        const char *room = (argc > 2) ? argv[2] : "10D0";
        int flavor_re2 = (argc > 3) ? atoi(argv[3]) : 1;
        char path[600];
        snprintf(path, sizeof path, "%s/STAGE1/ROOM%s.RDT", base, room);
        size_t sz = 0; uint8_t *buf = slurp(path, &sz);
        if (!buf) { printf("FAIL: %s\n", path); return 1; }
        if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT\n"); return 1; }

        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
        re15_ai_flavor_set(flavor_re2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset(); re15_esp_fx_reset();
        re15_damage_seed_rng(0x0badf00du);
        g_current_room_id = (uint16_t)strtol(room, NULL, 16);
        if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
        if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
        for (int i = 0; i < 120; i++) scd_vm_tick();

        /* Welche Typen stehen im Raum? */
        uint8_t types[8]; int nt = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active || g_actors[s].type == 0) continue;
            int seen = 0;
            for (int k = 0; k < nt; k++) if (types[k] == g_actors[s].type) seen = 1;
            if (!seen && nt < 8) types[nt++] = g_actors[s].type;
        }
        printf("== ROOM%s flavor=%s | Typen:", room, flavor_re2 ? "RE2" : "RE15");
        for (int k = 0; k < nt; k++) printf(" 0x%02X", types[k]);
        printf("\n");

        /* Pro Typ EIGENE RE1.5-Geometrie (Zeiger-Aliasing vermeiden). */
        for (int k = 0; k < nt; k++) {
            m15_t *M = load_re15_slot(types[k]);
            if (!M) { printf("  (kein RE1.5-EMD fuer 0x%02X)\n", types[k]); continue; }
            re15_enemy_bank_t *eb = re15_enemy_find(types[k]);
            if (!eb) eb = re15_enemy_alloc(types[k]);
            if (!eb) { printf("  (keine Bank frei fuer 0x%02X)\n", types[k]); continue; }
            if (flavor_re2) {
                if (load_re2(types[k], eb) != 0) { eb->type = 0; continue; }
                int um = -1;                       /* Hybrid wie main.c:668-676 */
                int rc = re2_hybrid_apply(eb, (int)types[k], &M->md, &M->sk, &um);
                printf("  bank 0x%02X: RE2 + hybrid rc=%d unmapped=%d\n", types[k], rc, um);
            } else {
                eb->md1 = M->md; eb->skel = M->sk; eb->anim = M->an;
                eb->skel_loco = M->sk_loco; eb->anim_loco = M->an_loco; eb->loco_ok = M->loco_ok;
                eb->skel_own  = M->sk_own;  eb->anim_own  = M->an_own;  eb->own_ok  = M->own_ok;
                eb->ok = 1; eb->buf = NULL; eb->remap_ok = 0;
            }
        }
        if (flavor_re2 && nt > 0) {
            m15_t *M0 = load_re15_slot(0x10);      /* Sitz-Pose-Bank = RE1.5-EM10 (main.c:539-548) */
            if (M0) {
                static re15_emd_skeleton_t sit_sk; static re15_emd_animation_t sit_an;
                sit_sk = M0->sk; sit_an = M0->an;
                re15_re2z_set_re15_pose_bank(&sit_sk, &sit_an);
                s_sit_md = &M0->md;
            }
        }

        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
        pl->state = 1; pl->motion = 0; pl->floor = 0; pl->y = 0;
        re15_collision_set_band(0);
        re15_inv_load_briefing();
        re15_player_set_equipped_weapon(3);

        for (int t = 0; t < 420; t++) {
            pl->hp = 100;
            /* Spieler an den ersten Gegner heranfuehren -> wecken/engagieren. */
            re15_actor_t *tgt = NULL;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x30)
                { tgt = &g_actors[s]; break; }
            if (tgt && t > 20) { pl->x = tgt->x + 1400; pl->z = tgt->z; }
            frame();
            if (t % 20 != 0) continue;
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!e->active || e->type == 0) continue;
                sole_t so;
                if (live_pose(e, &so) != 0) continue;
                printf("t%-4d sl%-2d ty=0x%02X st=%u/%02x/%u clip=%-3d f=%-3u | y=%-6ld fl=%-2u | "
                       "SOHLE=%-7ld (b%d) tiefBone=%-7ld rootY=%-7ld\n",
                       t, s, e->type, e->state, e->sub_state_1, e->sub_state_2,
                       (int)e->motion, e->anim_frame, (long)e->y, e->floor,
                       (long)so.sole, so.sole_bone, (long)so.lowbone, (long)so.rooty);
            }
        }
        return 0;
    }
    printf("usage: probe_rig_sohle sweep <typehex> | room <roomhex> <0|1> | chain <typehex>\n");
    return 1;
}
