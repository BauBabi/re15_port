/* probe_gator_maul.c - MESS-SONDE (NUR MESSUNG, kein Engine-Eingriff).
 *
 * Auftrag 2026-09-14 (Nutzer-Marke ROOM2090 Bild 301): "Beim Alligator-Finisher
 * ist Leon nicht im Maul" - gemessener Abstand in z 5385.
 *
 * Gemessen wird:
 *   A) je Frame der FRESS-Sequenz: Gator-Pos/Yaw/Anker, Leon-Pos/Anker, Delta,
 *      dazu die ROHEN Wurzel-Versaetze der Clips (Gator Clip 4/5/11, Opfer-Clip 1).
 *   B) im Kau-Loop (P3): alle 22 Gator-Bone-Weltpositionen + Abstand zu Leon,
 *      damit die MAULSPITZE benannt werden kann.
 *   C) ob die Opfer-Bank (Paar 3) ueberhaupt geladen ist.
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

/* Roher Wurzel-Versatz eines Clip-Frames (dieselbe Aufloesung wie
 * re15_clip_root_motion_abs: 0x8000-Skip + kf&0xfff). */
static void clip_off(const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an,
                     int clip, int frame, int32_t out[3])
{
    out[0] = out[1] = out[2] = 0;
    if (!sk || !an || clip < 0 || clip >= an->clip_count) return;
    const re15_emd_clip_t *c = &an->clips[clip];
    if (c->frame_count <= 0) return;
    int slot = frame % c->frame_count;
    int fi = c->first_frame + slot, fend = c->first_frame + c->frame_count - 1;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    int kf = (int)(an->frames[fi] & 0xFFFu);
    int16_t sx = 0, sy = 0, sz = 0;
    re15_emd_get_keyframe_speed(sk, kf, &sx, &sy, &sz);
    out[0] = sx; out[1] = sy; out[2] = sz;
}

int main(void)
{
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1");
    _putenv("RE15_GB_STUMM=1");
#else
    setenv("RE15_GB_TEST", "1", 1);
    setenv("RE15_GB_STUMM", "1", 1);
#endif
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: ROOM2090.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT\n"); return 1; }
    g_room_rdt_ok = 1;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;

    if (!load_re2_bank(0x23u)) { printf("FAIL: EM23-Bank\n"); return 1; }
    re15_enemy_bank_t *eb = re15_enemy_find(0x23u);
    printf("BANK bones=%d clips=%d | victim_ok=%d vbones=%d vclips=%d\n",
           eb->skel.bone_count, eb->anim.clip_count, (int)eb->victim_ok,
           eb->skel_victim.bone_count, eb->anim_victim.clip_count);
    if (eb->victim_ok) {
        int c, f;
        int32_t o[3];
        for (c = 0; c < eb->anim_victim.clip_count; c++)
            printf("  OPFER-Clip %d: %d Frames\n", c, eb->anim_victim.clips[c].frame_count);
        for (f = 0; f <= 119; f += 17) {
            clip_off(&eb->skel_victim, &eb->anim_victim, 1, f, o);
            printf("  OPFER1 f%3d off=(%6d,%6d,%6d)\n", f, o[0], o[1], o[2]);
        }
        clip_off(&eb->skel_victim, &eb->anim_victim, 1, 119, o);
        printf("  OPFER1 f119 off=(%6d,%6d,%6d)\n", o[0], o[1], o[2]);
        clip_off(&eb->skel_victim, &eb->anim_victim, 0, 0, o);
        printf("  OPFER0 f  0 off=(%6d,%6d,%6d)\n", o[0], o[1], o[2]);
        clip_off(&eb->skel_victim, &eb->anim_victim, 0, 29, o);
        printf("  OPFER0 f 29 off=(%6d,%6d,%6d)\n", o[0], o[1], o[2]);
        /* POSE-Kanal (kf Bytes 0-5) = die Wurzel-Translation, die der Renderer
         * auf das Modell legt (skeleton_common.c:255). */
        for (f = 0; f <= 119; f += 10) {
            const re15_emd_clip_t *c = &eb->anim_victim.clips[1];
            int fi = c->first_frame + (f % c->frame_count);
            int fe = c->first_frame + c->frame_count - 1;
            int16_t px = 0, py = 0, pz = 0; int kf;
            while ((eb->anim_victim.frames[fi] & 0x8000u) && fi < fe) fi++;
            kf = (int)(eb->anim_victim.frames[fi] & 0xFFFu);
            re15_emd_get_keyframe_position(&eb->skel_victim, kf, &px, &py, &pz);
            printf("  OPFER1 f%3d POSE=(%6d,%6d,%6d)  kf=%d\n", f, px, py, pz, kf);
        }
        {   const re15_emd_clip_t *c = &eb->anim_victim.clips[1];
            int fi = c->first_frame + 119;
            int16_t px = 0, py = 0, pz = 0; int kf;
            kf = (int)(eb->anim_victim.frames[fi] & 0xFFFu);
            re15_emd_get_keyframe_position(&eb->skel_victim, kf, &px, &py, &pz);
            printf("  OPFER1 f119 POSE=(%6d,%6d,%6d)  kf=%d\n", px, py, pz, kf);
        }
        {   int m;
            printf("  --- EM23-MD1: Mesh-Ausdehnung (lokal, +X = vorne) ---\n");
            for (m = 0; m < eb->md1.mesh_count && m < 12; m++) {
                const re15_md1_mesh_t *ms = &eb->md1.meshes[m];
                int i; int32_t mnx = 99999, mxx = -99999, mny = 99999, mxy = -99999;
                for (i = 0; i < ms->tri_vertex_count; i++) {
                    int32_t vx = ms->tri_vertices[i].x, vy = ms->tri_vertices[i].y;
                    if (vx < mnx) mnx = vx; if (vx > mxx) mxx = vx;
                    if (vy < mny) mny = vy; if (vy > mxy) mxy = vy;
                }
                for (i = 0; i < ms->quad_vertex_count; i++) {
                    int32_t vx = ms->quad_vertices[i].x, vy = ms->quad_vertices[i].y;
                    if (vx < mnx) mnx = vx; if (vx > mxx) mxx = vx;
                    if (vy < mny) mny = vy; if (vy > mxy) mxy = vy;
                }
                printf("    mesh %2d: x[%6d..%6d] y[%6d..%6d]  (%d+%d verts)\n",
                       m, mnx, mxx, mny, mxy, ms->tri_vertex_count, ms->quad_vertex_count);
            }
        }
    }
    {   int32_t o[3];
        clip_off(&eb->skel, &eb->anim, 4, 13, o);
        printf("  GATOR C4 f13  off=(%6d,%6d,%6d)\n", o[0], o[1], o[2]);
        clip_off(&eb->skel, &eb->anim, 5, 0, o);
        printf("  GATOR C5 f0   off=(%6d,%6d,%6d)\n", o[0], o[1], o[2]);
        clip_off(&eb->skel, &eb->anim, 5, 119, o);
        printf("  GATOR C5 f119 off=(%6d,%6d,%6d)\n", o[0], o[1], o[2]);
        clip_off(&eb->skel, &eb->anim, 11, 0, o);
        printf("  GATOR C11 f0  off=(%6d,%6d,%6d)\n", o[0], o[1], o[2]);
        /* Traegt IRGENDEIN benutzter Clip einen sy != 0? (Die Y-Leitung des
         * RE2-Zwillings addiert sy auf den Anker-y.) */
        {   int cl, ff, any = 0;
            for (cl = 0; cl < eb->anim.clip_count; cl++) {
                int mn = 99999, mx = -99999;
                for (ff = 0; ff < eb->anim.clips[cl].frame_count; ff++) {
                    clip_off(&eb->skel, &eb->anim, cl, ff, o);
                    if (o[1] < mn) mn = o[1];
                    if (o[1] > mx) mx = o[1];
                }
                if (mn != 0 || mx != 0) any = 1;
                printf("  GATOR-Clip %2d sy[%d..%d]\n", cl, mn, mx);
            }
            for (cl = 0; cl < eb->anim_victim.clip_count; cl++) {
                int mn = 99999, mx = -99999;
                for (ff = 0; ff < eb->anim_victim.clips[cl].frame_count; ff++) {
                    clip_off(&eb->skel_victim, &eb->anim_victim, cl, ff, o);
                    if (o[1] < mn) mn = o[1];
                    if (o[1] > mx) mx = o[1];
                }
                if (mn != 0 || mx != 0) any = 1;
                printf("  OPFER-Clip %2d sy[%d..%d]\n", cl, mn, mx);
            }
            printf("  => sy irgendwo != 0? %s\n", any ? "JA" : "NEIN");
        }
    }

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
    e->x = -6000; e->z = -22000; e->y = -1200;

    {
    int fress_f0 = -1, p3_frames = 0, f;
    for (f = 0; f < 3000; f++) {
        int32_t ox, oz;
        if (pl->hp >= 0) {
            pl->x = -4000; pl->z = -22000; pl->y = 0;
            pl->state = 0; pl->motion = 0; pl->hit_react = 0;
        }
        ox = e->x; oz = e->z;
        re15_gator_boss_tick(slot);
        if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
            int32_t nx = e->x, nz = e->z;
            re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz,
                                           e->hit_radius_min, e->y, 4u);
            e->x = nx; e->z = nz;
        }
        if (re15_gator_fressen_hold() && fress_f0 < 0) fress_f0 = f;
        if (fress_f0 >= 0) {
            int sf = f - fress_f0;
            if (sf < 6 || (sf % 15) == 0 || e->motion == 11) {
                int32_t dx = pl->x - e->x, dz = pl->z - e->z, dy = pl->y - e->y;
                printf("F%-4d gclip=%2d gaf=%-3u G=(%7d,%6d,%7d) yaw=%4d ganker=(%7d,%7d) | "
                       "L=(%7d,%6d,%7d) lclip=%2d laf=%-3u lanker=(%7d,%7d) | d=(%6d,%5d,%6d)\n",
                       f, (int)e->motion, (unsigned)e->anim_frame, e->x, e->y, e->z,
                       (int)e->rot_y, e->anchor_x, e->anchor_z,
                       pl->x, pl->y, pl->z, (int)pl->motion, (unsigned)pl->anim_frame,
                       pl->anchor_x, pl->anchor_z, dx, dy, dz);
            }
        }
        if (e->motion == 11) {
            if (++p3_frames == 1) {
                int b;
                int32_t bp[3];
                printf("\n=== KAU-LOOP: Gator-Bone-Weltpositionen (Clip 11) ===\n");
                for (b = 0; b < eb->skel.bone_count; b++) {
                    int64_t ddx, ddy, ddz, d2;
                    int32_t d = 0, fc, fs, lx, lz, fw;
                    re15_enemy_bone_world_pos(e, b, bp);
                    ddx = bp[0] - pl->x; ddy = bp[1] - pl->y; ddz = bp[2] - pl->z;
                    d2 = ddx*ddx + ddy*ddy + ddz*ddz;
                    while ((int64_t)d*d < d2 && d < 60000) d += 8;
                    fc = re15_cos_q12((int)e->rot_y); fs = re15_sin_q12((int)e->rot_y);
                    lx = bp[0] - e->x; lz = bp[2] - e->z;
                    fw = (int32_t)(((int64_t)fc*lx - (int64_t)fs*lz) >> 12);
                    printf("  bone %2d  welt=(%7d,%6d,%7d)  vorwaerts=%7d  Leon-Abstand=%6d\n",
                           b, bp[0], bp[1], bp[2], fw, d);
                }
                printf("=== Leon: (%d,%d,%d)  Gator-Wurzel: (%d,%d,%d) yaw=%d ===\n",
                       pl->x, pl->y, pl->z, e->x, e->y, e->z, (int)e->rot_y);
                /* WELT-AABB der MAUL-MESHES (6 = Oberkiefer, 7 = Unterkiefer):
                 * genau die Transformation des Renderers (re15_skel_bone_to_world). */
                {   static re15_skel_pose_t ps[RE15_EMD_MAX_BONES];
                    int kf = re15_compute_actor_kf(&eb->anim, &eb->skel, e, -1, e->anim_frame);
                    void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
                    if (re15_skel_compute_pose(&eb->skel, kf, ps) == 0) {
                        int m;
                        for (m = 6; m <= 7; m++) {
                            const re15_md1_mesh_t *ms = &eb->md1.meshes[m];
                            int32_t mn[3] = { 999999, 999999, 999999 };
                            int32_t mx[3] = { -999999, -999999, -999999 };
                            int i, pass, cnt;
                            for (pass = 0; pass < 2; pass++) {
                                const re15_md1_vertex_t *vv = pass ? ms->quad_vertices : ms->tri_vertices;
                                cnt = pass ? ms->quad_vertex_count : ms->tri_vertex_count;
                                for (i = 0; i < cnt; i++) {
                                    int32_t lv[3], wv[3], k;
                                    for (k = 0; k < 3; k++)
                                        lv[k] = ps[m].trans[k]
                                              + (int32_t)(((int64_t)ps[m].rot[k*3+0]*vv[i].x
                                                         + (int64_t)ps[m].rot[k*3+1]*vv[i].y
                                                         + (int64_t)ps[m].rot[k*3+2]*vv[i].z) >> 12);
                                    (void)wv;
                                    for (k = 0; k < 3; k++) {
                                        if (lv[k] < mn[k]) mn[k] = lv[k];
                                        if (lv[k] > mx[k]) mx[k] = lv[k];
                                    }
                                }
                            }
                            printf("  MAUL-Mesh %d LOKAL-AABB vor[%7d..%7d] y[%7d..%7d] z[%7d..%7d]\n",
                                   m, mn[0], mx[0], mn[1], mx[1], mn[2], mx[2]);
                        }
                    }
                    g_anim_pose_actor = sv;
                }
                {   int32_t fc2 = re15_cos_q12((int)e->rot_y), fs2 = re15_sin_q12((int)e->rot_y);
                    int32_t lx2 = pl->x - e->x, lz2 = pl->z - e->z;
                    int32_t vor  = (int32_t)(((int64_t)fc2*lx2 - (int64_t)fs2*lz2) >> 12);
                    int32_t seit = (int32_t)(((int64_t)fs2*lx2 + (int64_t)fc2*lz2) >> 12);
                    printf("  LEON LOKAL Wurzel:    vor=%d hoch=%d seit=%d\n", vor, pl->y - e->y, seit);
                    printf("  LEON LOKAL gerendert: vor=%d hoch=%d seit=%d\n", vor, (pl->y - 2403) - e->y, seit);
                }
                printf("  LEON gerendert (Wurzel + POSE-y -2403): (%d,%d,%d)\n",
                       pl->x, pl->y - 2403, pl->z);
                printf("  LEON MIT Y-Leitung (y=Gator-y %d) waere: (%d,%d,%d)\n\n",
                       e->y, pl->x, e->y - 2403, pl->z);
            }
            if (p3_frames > 4) break;
        }
    }
    printf("ENDE: Fress-Start F%d\n", fress_f0);
    }
    return 0;
}
