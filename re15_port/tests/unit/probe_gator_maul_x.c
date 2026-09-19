/* probe_gator_maul_x.c — MESSSCHIENE fuer die X-Luecke des Fress-Finishers.
 *
 * Beantwortet mit ZAHLEN aus dem echten Engine-Tick (kein Nachbau):
 *  1) Wo liegt Leons Wurzel im MODELLRAUM des Gators im P3-Park-Frame?
 *     (Rueck-Rotation der Weltdifferenz; unabhaengig von Yaw und Anker)
 *  2) Wie weit reicht das Maul (MD1-Meshes an den posierten Bones) im
 *     MODELLRAUM — ungeskaliert und mit render_scale_q12?
 *  3) Was aendert die Original-P3-Platzierung (Gator mit CLIP 11 statt dem
 *     zurueckgesprungenen Clip-5-Frame-0)?
 *
 * Aufbau 1:1 aus probe_gator_fress.c uebernommen (echter Boss-Tick + Klemme).
 * NUR Messung, keine Verhaltensaenderung.
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
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_md1.h"
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

/* Modellraum-AABB eines Mesh fuer eine gegebene Pose (Mesh i haengt an Bone i). */
static void mesh_local_aabb(const re15_enemy_bank_t *b, const re15_skel_pose_t *poses,
                            int mesh, int32_t mn[3], int32_t mx[3], int *nv)
{
    int s, i, k;
    mn[0] = mn[1] = mn[2] = 1 << 29;
    mx[0] = mx[1] = mx[2] = -(1 << 29);
    *nv = 0;
    if (mesh < 0 || mesh >= b->md1.mesh_count) return;
    if (mesh >= RE15_EMD_MAX_BONES) return;
    {
        const re15_md1_mesh_t *ms = &b->md1.meshes[mesh];
        const re15_md1_vertex_t *sets[2];
        int counts[2];
        sets[0] = ms->tri_vertices;  counts[0] = ms->tri_vertex_count;
        sets[1] = ms->quad_vertices; counts[1] = ms->quad_vertex_count;
        for (s = 0; s < 2; s++) {
            const re15_md1_vertex_t *vv = sets[s];
            if (!vv) continue;
            for (i = 0; i < counts[s]; i++) {
                int32_t v[3]; int32_t o[3];
                const int32_t *R = poses[mesh].rot;
                v[0] = vv[i].x; v[1] = vv[i].y; v[2] = vv[i].z;
                o[0] = (R[0]*v[0] + R[1]*v[1] + R[2]*v[2]) >> 12;
                o[1] = (R[3]*v[0] + R[4]*v[1] + R[5]*v[2]) >> 12;
                o[2] = (R[6]*v[0] + R[7]*v[1] + R[8]*v[2]) >> 12;
                o[0] += poses[mesh].trans[0];
                o[1] += poses[mesh].trans[1];
                o[2] += poses[mesh].trans[2];
                for (k = 0; k < 3; k++) {
                    if (o[k] < mn[k]) mn[k] = o[k];
                    if (o[k] > mx[k]) mx[k] = o[k];
                }
                (*nv)++;
            }
        }
    }
}

int main(void)
{
    size_t n = 0;
    uint8_t *buf;
    re15_enemy_bank_t *bk;
    re15_actor_t *pl, *e;
    int slot, f, f_p3 = -1, k, b, m, mi, nv;
    int32_t gmin[3], gmax[3];
    int32_t allmin[3], allmax[3];
    int32_t cs, sn, dx, dy, dz, lx, lz;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int kf;

#ifdef _WIN32
    _putenv("RE15_GB_TEST=1");
#else
    setenv("RE15_GB_TEST", "1", 1);
#endif

    buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: ROOM2090.RDT nicht lesbar\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    g_room_rdt_ok = 1;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;

    if (!load_re2_bank(0x23u)) { printf("FAIL: EM23-Bank\n"); return 1; }
    bk = re15_enemy_find(0x23u);

    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    slot = RE15_ACTOR_MAX - 1;
    e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x23u;
    e->x = -6000; e->y = 0; e->z = -22000;
    e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
    re15_enemy_apply_hitbox(e, 0x23u);

    pl->active = 1; pl->type = 0; pl->hp = 30;
    pl->x = -4000; pl->z = -22000; pl->y = 0; pl->floor = 0;

    re15_gator_boss_tick(slot);
    e->x = -6000; e->z = -22000; e->y = -1200;

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
        if (e->motion == 11) { f_p3 = f; break; }
    }
    if (f_p3 < 0) { printf("FAIL: P3 nie erreicht\n"); return 1; }
    for (k = 0; k < 20; k++) re15_gator_boss_tick(slot);

    printf("=== P3-PARK (Eintritt F%d, +20 Ticks) ===\n", f_p3);
    printf("GATOR pos=(%d,%d,%d) yaw=%d clip=%d af=%u anker=(%d,%d) scale=%d\n",
           (int)e->x, (int)e->y, (int)e->z, (int)e->rot_y, (int)e->motion,
           (unsigned)e->anim_frame, (int)e->anchor_x, (int)e->anchor_z,
           (int)e->render_scale_q12);
    printf("LEON  pos=(%d,%d,%d) yaw=%d clip=%d af=%u anker=(%d,%d)\n",
           (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y, (int)pl->motion,
           (unsigned)pl->anim_frame, (int)pl->anchor_x, (int)pl->anchor_z);

    cs = re15_cos_q12((int)e->rot_y); sn = re15_sin_q12((int)e->rot_y);
    dx = pl->x - e->x; dz = pl->z - e->z; dy = pl->y - e->y;
    lx = (int32_t)(((int64_t)cs * dx - (int64_t)sn * dz) >> 12);
    lz = (int32_t)(((int64_t)sn * dx + (int64_t)cs * dz) >> 12);
    printf("LEON im GATOR-MODELLRAUM: (%d, %d, %d)\n", (int)lx, (int)dy, (int)lz);

    kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, -1, e->anim_frame);
    if (re15_skel_compute_pose(&bk->skel, kf, poses) != 0) {
        printf("FAIL: Pose\n"); return 1; }
    printf("kf=%d bones=%d meshes=%d\n", kf, bk->skel.bone_count, bk->md1.mesh_count);
    for (b = 0; b < bk->skel.bone_count; b++)
        printf("  bone%-2d local=(%6d,%6d,%6d)\n", b,
               (int)poses[b].trans[0], (int)poses[b].trans[1], (int)poses[b].trans[2]);

    allmin[0] = allmin[1] = allmin[2] = 1 << 29;
    allmax[0] = allmax[1] = allmax[2] = -(1 << 29);
    for (m = 0; m < bk->md1.mesh_count; m++) {
        mesh_local_aabb(bk, poses, m, gmin, gmax, &nv);
        if (!nv) continue;
        for (k = 0; k < 3; k++) {
            if (gmin[k] < allmin[k]) allmin[k] = gmin[k];
            if (gmax[k] > allmax[k]) allmax[k] = gmax[k];
        }
        printf("MESH%-2d local x[%6d..%6d] y[%6d..%6d] z[%6d..%6d] nv=%d\n",
               m, (int)gmin[0], (int)gmax[0], (int)gmin[1], (int)gmax[1],
               (int)gmin[2], (int)gmax[2], nv);
    }
    printf("MODELL GESAMT local x[%d..%d] y[%d..%d] z[%d..%d] (Laenge %d)\n",
           (int)allmin[0], (int)allmax[0], (int)allmin[1], (int)allmax[1],
           (int)allmin[2], (int)allmax[2], (int)(allmax[0] - allmin[0]));

    for (mi = 6; mi <= 8; mi++) {
        int s = e->render_scale_q12 ? (int)e->render_scale_q12 : 4096;
        mesh_local_aabb(bk, poses, mi, gmin, gmax, &nv);
        if (!nv) continue;
        printf("MAUL-MESH%d: roh x[%d..%d] | mit scale %d x[%d..%d] | LEON lx=%d\n",
               mi, (int)gmin[0], (int)gmax[0], s,
               (int)(((int64_t)gmin[0] * s) >> 12),
               (int)(((int64_t)gmax[0] * s) >> 12), (int)lx);
    }

    /* ---- Leons Opfer-Pose im MODELLRAUM des Gators ---------------------- */
    {
        re15_skel_pose_t vp[RE15_EMD_MAX_BONES];
        int vkf = -1, i;
        int32_t lmn[3], lmx[3];
        if (bk->victim_ok && bk->anim_victim.clip_count > 1) {
            const re15_emd_clip_t *vc = &bk->anim_victim.clips[1];
            int fi = vc->first_frame + 119;
            int fend = vc->first_frame + vc->frame_count - 1;
            while ((bk->anim_victim.frames[fi] & 0x8000u) && fi < fend) fi++;
            vkf = (int)(bk->anim_victim.frames[fi] & 0xFFFu);
        }
        if (vkf >= 0 && re15_skel_compute_pose(&bk->skel_victim, vkf, vp) == 0) {
            int32_t m6n[3], m6x[3], m7n[3], m7x[3];
            int q;
            int cfgdx[4], cfgdy[4], cfgs[4];
            const char *nm[4];
            lmn[0] = lmn[1] = lmn[2] = 1 << 29;
            lmx[0] = lmx[1] = lmx[2] = -(1 << 29);
            printf("-- LEON Opfer-Pose (Paar3 kf=%d, %d Bones) lokal zu SEINER Wurzel --\n",
                   vkf, bk->skel_victim.bone_count);
            for (i = 0; i < bk->skel_victim.bone_count; i++) {
                int k2;
                printf("  vbone%-2d (%6d,%6d,%6d)\n", i,
                       (int)vp[i].trans[0], (int)vp[i].trans[1], (int)vp[i].trans[2]);
                for (k2 = 0; k2 < 3; k2++) {
                    if (vp[i].trans[k2] < lmn[k2]) lmn[k2] = vp[i].trans[k2];
                    if (vp[i].trans[k2] > lmx[k2]) lmx[k2] = vp[i].trans[k2];
                }
            }
            printf("LEON-KOERPER-BOX (zu seiner Wurzel) x[%d..%d] y[%d..%d] z[%d..%d]\n",
                   (int)lmn[0], (int)lmx[0], (int)lmn[1], (int)lmx[1],
                   (int)lmn[2], (int)lmx[2]);

            mesh_local_aabb(bk, poses, 6, m6n, m6x, &nv);
            mesh_local_aabb(bk, poses, 7, m7n, m7x, &nv);
            cfgdx[0] = lx;       cfgdy[0] = dy; cfgs[0] = 2731; nm[0] = "A Port-Ist";
            cfgdx[1] = lx - 329; cfgdy[1] = dy; cfgs[1] = 2731; nm[1] = "B +Clip11-P3";
            cfgdx[2] = lx - 329; cfgdy[2] = 0;  cfgs[2] = 2731; nm[2] = "C +Y-Leitung";
            cfgdx[3] = lx - 329; cfgdy[3] = 0;  cfgs[3] = 4096; nm[3] = "D +Scale1.0";
            for (q = 0; q < 4; q++) {
                int s = cfgs[q];
                int32_t a6n[3], a6x[3], a7n[3], a7x[3];
                int32_t bn[3], bx[3];
                int k3, in6, in7;
                for (k3 = 0; k3 < 3; k3++) {
                    a6n[k3] = (int32_t)(((int64_t)m6n[k3] * s) >> 12);
                    a6x[k3] = (int32_t)(((int64_t)m6x[k3] * s) >> 12);
                    a7n[k3] = (int32_t)(((int64_t)m7n[k3] * s) >> 12);
                    a7x[k3] = (int32_t)(((int64_t)m7x[k3] * s) >> 12);
                }
                /* MESSPUNKT = Leons Wurzel-BONE 0 (das ist der Punkt, den die
                 * fruehere Runde als 'gerendertes y=-2403' gemeldet hat), NICHT
                 * die Ganzkoerper-Box: die ist 12817 hoch und ueberlappt alles. */
                bn[0] = bx[0] = vp[0].trans[0] + cfgdx[q];
                bn[1] = bx[1] = vp[0].trans[1] + cfgdy[q];
                bn[2] = bx[2] = vp[0].trans[2] + lz;
                in6 = (bn[0] <= a6x[0] && bx[0] >= a6n[0] &&
                       bn[1] <= a6x[1] && bx[1] >= a6n[1] &&
                       bn[2] <= a6x[2] && bx[2] >= a6n[2]);
                in7 = (bn[0] <= a7x[0] && bx[0] >= a7n[0] &&
                       bn[1] <= a7x[1] && bx[1] >= a7n[1] &&
                       bn[2] <= a7x[2] && bx[2] >= a7n[2]);
                printf("KONFIG %-13s Wurzel lx=%5d dy=%5d | Bone0 (%d,%d,%d) "
                       "| Maul6 x[%d..%d] y[%d..%d] | Maul7 x[%d..%d] y[%d..%d] "
                       "| BONE0 IM MAUL: 6=%d 7=%d\n",
                       nm[q], cfgdx[q], cfgdy[q],
                       (int)bn[0], (int)bn[1], (int)bn[2],
                       (int)a6n[0], (int)a6x[0], (int)a6n[1], (int)a6x[1],
                       (int)a7n[0], (int)a7x[0], (int)a7n[1], (int)a7x[1],
                       in6, in7);
            }
        } else {
            printf("HINWEIS: Opfer-Pose nicht berechenbar (victim_ok=%d)\n", bk->victim_ok);
        }
    }


    /* ---- LIVE-GEGENPROBE: die drei Befunde im echten Engine-Aufruf --------
     * Kein Engine-Eingriff, nur diese Sonde: (1) Original-P3-Platzierung ueber
     * denselben oeffentlichen Aufruf, den der Port in P2 benutzt, mit CLIP 11
     * (\n0x801011cc + a1=11 \n0x801011d8); (2) Y-Leitung (Leon auf Gator-Y, die
     * dritte Anker-Komponente \n0x80015c88/\n0x80015d6c); (3) Scale 1.0. */
    {
        re15_skel_pose_t vp2[RE15_EMD_MAX_BONES];
        int vkf2 = -1;
        re15_clip_root_motion_abs_pub(e, &bk->skel, &bk->anim, 11, (int)e->anim_frame);
        pl->y = e->y;
        e->render_scale_q12 = 0;                 /* 0 == Flag aus == 1.0 */
        cs = re15_cos_q12((int)e->rot_y); sn = re15_sin_q12((int)e->rot_y);
        dx = pl->x - e->x; dz = pl->z - e->z; dy = pl->y - e->y;
        lx = (int32_t)(((int64_t)cs * dx - (int64_t)sn * dz) >> 12);
        lz = (int32_t)(((int64_t)sn * dx + (int64_t)cs * dz) >> 12);
        printf("-- LIVE nach Clip11-Platzierung + Y-Leitung + Scale 1.0 --\n");
        printf("GATOR pos=(%d,%d,%d) LEON pos=(%d,%d,%d) -> LEON lokal (%d,%d,%d)\n",
               (int)e->x, (int)e->y, (int)e->z, (int)pl->x, (int)pl->y, (int)pl->z,
               (int)lx, (int)dy, (int)lz);
        kf = re15_compute_actor_kf(&bk->anim, &bk->skel, e, -1, e->anim_frame);
        if (re15_skel_compute_pose(&bk->skel, kf, poses) == 0 &&
            bk->victim_ok && bk->anim_victim.clip_count > 1) {
            const re15_emd_clip_t *vc2 = &bk->anim_victim.clips[1];
            int fi2 = vc2->first_frame + 119;
            int fe2 = vc2->first_frame + vc2->frame_count - 1;
            while ((bk->anim_victim.frames[fi2] & 0x8000u) && fi2 < fe2) fi2++;
            vkf2 = (int)(bk->anim_victim.frames[fi2] & 0xFFFu);
            if (re15_skel_compute_pose(&bk->skel_victim, vkf2, vp2) == 0) {
                int32_t q6n[3], q6x[3], q7n[3], q7x[3];
                int32_t bp[3];
                int i6, i7;
                mesh_local_aabb(bk, poses, 6, q6n, q6x, &nv);
                mesh_local_aabb(bk, poses, 7, q7n, q7x, &nv);
                bp[0] = vp2[0].trans[0] + lx;
                bp[1] = vp2[0].trans[1] + dy;
                bp[2] = vp2[0].trans[2] + lz;
                i6 = (bp[0] >= q6n[0] && bp[0] <= q6x[0] && bp[1] >= q6n[1] &&
                      bp[1] <= q6x[1] && bp[2] >= q6n[2] && bp[2] <= q6x[2]);
                i7 = (bp[0] >= q7n[0] && bp[0] <= q7x[0] && bp[1] >= q7n[1] &&
                      bp[1] <= q7x[1] && bp[2] >= q7n[2] && bp[2] <= q7x[2]);
                printf("LIVE Bone0 (%d,%d,%d) | Maul6 x[%d..%d] y[%d..%d] z[%d..%d] "
                       "| Maul7 x[%d..%d] y[%d..%d] z[%d..%d] | IM MAUL: 6=%d 7=%d\n",
                       (int)bp[0], (int)bp[1], (int)bp[2],
                       (int)q6n[0], (int)q6x[0], (int)q6n[1], (int)q6x[1],
                       (int)q6n[2], (int)q6x[2],
                       (int)q7n[0], (int)q7x[0], (int)q7n[1], (int)q7x[1],
                       (int)q7n[2], (int)q7x[2], i6, i7);
            }
        }
    }

    printf("=== ENDE ===\n");
    return 0;
}
