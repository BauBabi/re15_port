/* probe_maul_gegen.c - GEGENMESSUNG zu probe_gator_maul (NUR MESSUNG).
 *
 * Frage: ist die dort gemeldete MAUL-WELT-AABB wirklich "genau die
 * Transformation des Renderers"? Der NPC-Renderer legt VOR der Bone-Schleife
 * den uniformen Entity-Render-Scale +0x166 auf die 3x3 (main.c:8237-8241,
 * Original ScaleMatrix-Pfad FUN_8001e8c8 @0x8001e904-40) und posiert MIT
 * gesetztem g_anim_pose_actor (main.c:8193), wodurch der jaw_vz/arc_vz/
 * pitch_vz-Hook (skeleton_common.c, gated auf bact->type==0x23) laeuft.
 * probe_gator_maul tut BEIDES NICHT.
 */
#include "re15_rdt.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_collision.h"
#include "re15_boss_gator.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz;
    return b;
}

static int load_re2_bank(uint8_t type)
{
    static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
        s_ems_sz = (long)n; }
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

/* Exakte Renderer-Kette (main.c:8227-8241 + 8399-8418 + 8482): nyaw = scale * R_y;
 * bone_rot = nyaw x pose.rot; bone_trans = nyaw x pose.trans + actor_xyz;
 * vertex_welt = bone_rot x v + bone_trans. */
static void mesh_aabb(const re15_actor_t *e, const re15_md1_mesh_t *ms,
                      const re15_skel_pose_t *p, int use_scale,
                      int32_t mn[3], int32_t mx[3])
{
    int32_t fs = re15_sin_q12((int)e->rot_y), fc = re15_cos_q12((int)e->rot_y);
    int32_t nyaw[9] = { fc, 0, fs, 0, 0x1000, 0, -fs, 0, fc };
    int k, r, c, i, pass, cnt;
    int32_t br[9], bt[3];
    if (use_scale && e->render_scale_q12)
        for (k = 0; k < 9; k++) nyaw[k] = (nyaw[k] * (int32_t)e->render_scale_q12) >> 12;
    for (r = 0; r < 3; r++) for (c = 0; c < 3; c++) {
        int64_t s = 0;
        for (k = 0; k < 3; k++) s += (int64_t)nyaw[r*3+k] * (int64_t)p->rot[k*3+c];
        br[r*3+c] = (int32_t)(s >> 12);
    }
    for (r = 0; r < 3; r++) {
        int64_t s = 0;
        for (k = 0; k < 3; k++) s += (int64_t)nyaw[r*3+k] * (int64_t)p->trans[k];
        bt[r] = (int32_t)(s >> 12);
    }
    bt[0] += e->x; bt[1] += e->y; bt[2] += e->z;
    mn[0] = mn[1] = mn[2] =  999999;
    mx[0] = mx[1] = mx[2] = -999999;
    for (pass = 0; pass < 2; pass++) {
        const re15_md1_vertex_t *vv = pass ? ms->quad_vertices : ms->tri_vertices;
        cnt = pass ? ms->quad_vertex_count : ms->tri_vertex_count;
        for (i = 0; i < cnt; i++) {
            for (k = 0; k < 3; k++) {
                int32_t w = bt[k] + (int32_t)(((int64_t)br[k*3+0]*vv[i].x
                                             + (int64_t)br[k*3+1]*vv[i].y
                                             + (int64_t)br[k*3+2]*vv[i].z) >> 12);
                if (w < mn[k]) mn[k] = w;
                if (w > mx[k]) mx[k] = w;
            }
        }
    }
}


/* Min-Abstand von einem Weltpunkt zu den Vertices eines Mesh (gleiche Kette). */
static void mesh_punkt(const re15_actor_t *e, const re15_md1_mesh_t *ms,
                       const re15_skel_pose_t *p, int use_scale,
                       const int32_t pt[3], int64_t *best, int32_t ctr[3])
{
    int32_t fs = re15_sin_q12((int)e->rot_y), fc = re15_cos_q12((int)e->rot_y);
    int32_t nyaw[9] = { fc, 0, fs, 0, 0x1000, 0, -fs, 0, fc };
    int k, r, c, i, pass, cnt, total = 0;
    int32_t br[9], bt[3];
    int64_t sum[3] = {0,0,0};
    if (use_scale && e->render_scale_q12)
        for (k = 0; k < 9; k++) nyaw[k] = (nyaw[k] * (int32_t)e->render_scale_q12) >> 12;
    for (r = 0; r < 3; r++) for (c = 0; c < 3; c++) {
        int64_t s = 0;
        for (k = 0; k < 3; k++) s += (int64_t)nyaw[r*3+k] * (int64_t)p->rot[k*3+c];
        br[r*3+c] = (int32_t)(s >> 12);
    }
    for (r = 0; r < 3; r++) {
        int64_t s = 0;
        for (k = 0; k < 3; k++) s += (int64_t)nyaw[r*3+k] * (int64_t)p->trans[k];
        bt[r] = (int32_t)(s >> 12);
    }
    bt[0] += e->x; bt[1] += e->y; bt[2] += e->z;
    for (pass = 0; pass < 2; pass++) {
        const re15_md1_vertex_t *vv = pass ? ms->quad_vertices : ms->tri_vertices;
        cnt = pass ? ms->quad_vertex_count : ms->tri_vertex_count;
        for (i = 0; i < cnt; i++) {
            int32_t w[3]; int64_t d2 = 0;
            for (k = 0; k < 3; k++) {
                w[k] = bt[k] + (int32_t)(((int64_t)br[k*3+0]*vv[i].x
                                        + (int64_t)br[k*3+1]*vv[i].y
                                        + (int64_t)br[k*3+2]*vv[i].z) >> 12);
                sum[k] += w[k];
            }
            total++;
            for (k = 0; k < 3; k++) { int64_t d = w[k]-pt[k]; d2 += d*d; }
            if (d2 < *best) *best = d2;
        }
    }
    if (total) for (k = 0; k < 3; k++) ctr[k] = (int32_t)(sum[k]/total);
}

static int64_t isqrt64(int64_t v){ int64_t r=0; while (r*r < v && r < 200000) r++; return r; }

int main(void)
{
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1"); _putenv("RE15_GB_STUMM=1");
#else
    setenv("RE15_GB_TEST","1",1); setenv("RE15_GB_STUMM","1",1);
#endif
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL RDT\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL parse\n"); return 1; }
    g_room_rdt_ok = 1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;
    if (!load_re2_bank(0x23u)) { printf("FAIL bank\n"); return 1; }
    re15_enemy_bank_t *eb = re15_enemy_find(0x23u);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int slot = RE15_ACTOR_MAX - 1;
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x23u;
    e->x = -6000; e->y = 0; e->z = -22000;
    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x23u);
    pl->active = 1; pl->type = 0; pl->hp = 30;
    pl->x = -4000; pl->z = -22000; pl->y = 0; pl->floor = 0;
    re15_gator_boss_tick(slot);
    printf("NACH INIT: Gator render_scale_q12=%d  (2731 = 2/3)\n", (int)e->render_scale_q12);
    printf("NACH INIT: Gator e->y aus der KI = %d\n", e->y);
    printf("SPIELER  : render_scale_q12=%d\n", (int)pl->render_scale_q12);
    e->x = -6000; e->z = -22000; e->y = -1200;

    {   /* VOLLE POSE-y-Kurve des Opfer-Clips 1 (alle 120 Frames) */
        const re15_emd_clip_t *c = &eb->anim_victim.clips[1];
        int ff, mn = 99999, mnf = -1;
        for (ff = 0; ff < c->frame_count; ff++) {
            int fi = c->first_frame + ff, fe = c->first_frame + c->frame_count - 1;
            int16_t px=0, py=0, pz=0; int kf;
            while ((eb->anim_victim.frames[fi] & 0x8000u) && fi < fe) fi++;
            kf = (int)(eb->anim_victim.frames[fi] & 0xFFFu);
            re15_emd_get_keyframe_position(&eb->skel_victim, kf, &px, &py, &pz);
            if (py < mn) { mn = py; mnf = ff; }
        }
        printf("OPFER1 POSE-y ECHTE Spitze = %d bei f%d (Sonde sampelte nur alle 10 Frames)\n", mn, mnf);
    }

    int fress_f0 = -1, p3 = 0, f;
    for (f = 0; f < 3000; f++) {
        int32_t ox, oz;
        if (pl->hp >= 0) { pl->x = -4000; pl->z = -22000; pl->y = 0;
                           pl->state = 0; pl->motion = 0; pl->hit_react = 0; }
        ox = e->x; oz = e->z;
        re15_gator_boss_tick(slot);
        if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
            int32_t nx = e->x, nz = e->z;
            re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz,
                                           e->hit_radius_min, e->y, 4u);
            e->x = nx; e->z = nz;
        }
        if (re15_gator_fressen_hold() && fress_f0 < 0) fress_f0 = f;
        if (e->motion == 11 && ++p3 == 1) {
            static re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
            int kf = re15_compute_actor_kf(&eb->anim, &eb->skel, e, -1, e->anim_frame);
            int variante, m;
            printf("\n=== KAU-LOOP F%d: Gator=(%d,%d,%d) yaw=%d | Leon=(%d,%d,%d) mo=%d af=%d\n",
                   f, e->x, e->y, e->z, (int)e->rot_y, pl->x, pl->y, pl->z,
                   (int)pl->motion, (unsigned)pl->anim_frame);
            printf("    Hook-vz: bone0=%d bone1=%d bone7=%d\n",
                   re15_gator_spine_arc_vz(e, 0), re15_gator_spine_arc_vz(e, 1),
                   re15_gator_spine_arc_vz(e, 7));
            for (variante = 0; variante < 3; variante++) {
                void *sv = g_anim_pose_actor;
                g_anim_pose_actor = (variante == 0) ? NULL : (void *)e;
                if (re15_skel_compute_pose(&eb->skel, kf, ps) != 0) { g_anim_pose_actor = sv; continue; }
                g_anim_pose_actor = sv;
                printf("  -- Variante %d (%s, %s) --\n", variante,
                       variante == 0 ? "ohne Hook" : "mit Hook",
                       variante == 2 ? "MIT Scale = RENDERER" : "ohne Scale");
                for (m = 6; m <= 7; m++) {
                    int32_t mn[3], mx[3];
                    mesh_aabb(e, &eb->md1.meshes[m], &ps[m], variante == 2, mn, mx);
                    printf("     Mesh %d WELT-AABB x[%7d..%7d] y[%7d..%7d] z[%7d..%7d]\n",
                           m, mn[0], mx[0], mn[1], mx[1], mn[2], mx[2]);
                }
            }
            {
                static re15_skel_pose_t lps[RE15_EMD_MAX_BONES];
                int lkf, wurzel_y = 0, rootfix;
                void *sv = g_anim_pose_actor; g_anim_pose_actor = (void *)pl;
                lkf = re15_compute_actor_kf(&eb->anim_victim, &eb->skel_victim, pl, -1, pl->anim_frame);
                if (re15_skel_compute_pose(&eb->skel_victim, lkf, lps) == 0)
                    wurzel_y = lps[0].trans[1];
                g_anim_pose_actor = sv;
                rootfix = re15_skel_root_y_fix(&eb->skel_victim, lkf);
                printf("  LEON: kf=%d pose-root=(%d,%d,%d) root_y_fix=%d -> gerendert y=%d\n",
                       lkf, (int)lps[0].trans[0], (int)lps[0].trans[1], (int)lps[0].trans[2],
                       rootfix, pl->y + wurzel_y);
            }

            {   /* MIN-ABSTAND zur echten Mesh-Geometrie (nicht AABB) */
                static re15_skel_pose_t ps2[RE15_EMD_MAX_BONES];
                int32_t leon_ist[3] = { pl->x, pl->y - 2403, pl->z };
                int32_t leon_yl[3]  = { pl->x, e->y - 2403,  pl->z };
                int vv2;
                void *sv2 = g_anim_pose_actor; g_anim_pose_actor = (void *)e;
                if (re15_skel_compute_pose(&eb->skel, kf, ps2) == 0) {
                  for (vv2 = 0; vv2 < 2; vv2++) {
                    int use = (vv2 == 1);
                    int64_t b1 = (int64_t)1<<60, b2 = (int64_t)1<<60;
                    int32_t c6[3]={0,0,0}, c7[3]={0,0,0};
                    mesh_punkt(e, &eb->md1.meshes[6], &ps2[6], use, leon_ist, &b1, c6);
                    mesh_punkt(e, &eb->md1.meshes[7], &ps2[7], use, leon_ist, &b1, c7);
                    mesh_punkt(e, &eb->md1.meshes[6], &ps2[6], use, leon_yl, &b2, c6);
                    mesh_punkt(e, &eb->md1.meshes[7], &ps2[7], use, leon_yl, &b2, c7);
                    printf("  [%s] Maul-Schwerpunkt m6=(%d,%d,%d) m7=(%d,%d,%d)\n",
                           use ? "MIT Scale" : "ohne Scale", c6[0],c6[1],c6[2], c7[0],c7[1],c7[2]);
                    printf("  [%s] Leon-IST  (%d,%d,%d) min-Abstand zur Maulgeometrie = %d\n",
                           use ? "MIT Scale" : "ohne Scale",
                           leon_ist[0],leon_ist[1],leon_ist[2], (int)isqrt64(b1));
                    printf("  [%s] Leon-YLTG (%d,%d,%d) min-Abstand zur Maulgeometrie = %d\n",
                           use ? "MIT Scale" : "ohne Scale",
                           leon_yl[0],leon_yl[1],leon_yl[2], (int)isqrt64(b2));
                  }
                }
                g_anim_pose_actor = sv2;
            }
            break;
        }
        if (f > 400 && fress_f0 < 0) { printf("kein FRESSEN bis F%d\n", f); break; }
    }
    return 0;
}
