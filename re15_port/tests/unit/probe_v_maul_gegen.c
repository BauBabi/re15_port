/* probe_v_maul_gegen.c - UNABHAENGIGE GEGENPROBE zum Befund "x-luecke".
 * Nur Messung. Prueft die ZWISCHENSCHRITTE des Befunds:
 *  (a) welchen Bank/Clip/kf der RENDERER im P3-Park posiert (nicht die Sonde),
 *  (b) ob Mesh-Index == Bone-Index gilt (remap_ok),
 *  (c) WELT-RAUM-Rechnung (statt Rueckrotation) mit render_scale_q12,
 *  (d) ob Mesh 6/7 ueberhaupt das Maul sind (Bone-Kette + Extremwerte),
 *  (e) Crossfade (anim_frac) an/aus.
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

extern void *g_anim_pose_actor;

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

/* WELT-AABB eines Mesh - exakt die Renderer-Kette main.c:8257-8261 + 8440-8465:
 * nyaw = scale*R(yaw); rot = nyaw*np->rot; trans = nyaw*np->trans; welt = trans + pos. */
static void mesh_world_aabb(const re15_enemy_bank_t *b, const re15_skel_pose_t *poses,
                            int mesh, const int32_t nyaw[9], const re15_actor_t *e,
                            int32_t mn[3], int32_t mx[3], int *nv)
{
    int s, i, k, r, c;
    int32_t nr[9], nt[3];
    mn[0] = mn[1] = mn[2] = 1 << 29;
    mx[0] = mx[1] = mx[2] = -(1 << 29);
    *nv = 0;
    if (mesh < 0 || mesh >= b->md1.mesh_count) return;
    if (mesh >= RE15_EMD_MAX_BONES) return;
    for (r = 0; r < 3; r++) {
        for (c = 0; c < 3; c++) {
            int64_t sum = 0;
            for (k = 0; k < 3; k++) sum += (int64_t)nyaw[r*3+k] * (int64_t)poses[mesh].rot[k*3+c];
            nr[r*3+c] = (int32_t)(sum >> 12);
        }
    }
    for (r = 0; r < 3; r++) {
        int64_t sum = 0;
        for (k = 0; k < 3; k++) sum += (int64_t)nyaw[r*3+k] * (int64_t)poses[mesh].trans[k];
        nt[r] = (int32_t)(sum >> 12);
    }
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
                int32_t v[3], o[3];
                v[0] = vv[i].x; v[1] = vv[i].y; v[2] = vv[i].z;
                o[0] = (int32_t)(((int64_t)nr[0]*v[0] + (int64_t)nr[1]*v[1] + (int64_t)nr[2]*v[2]) >> 12) + nt[0] + e->x;
                o[1] = (int32_t)(((int64_t)nr[3]*v[0] + (int64_t)nr[4]*v[1] + (int64_t)nr[5]*v[2]) >> 12) + nt[1] + e->y;
                o[2] = (int32_t)(((int64_t)nr[6]*v[0] + (int64_t)nr[7]*v[1] + (int64_t)nr[8]*v[2]) >> 12) + nt[2] + e->z;
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
    int slot, f, f_p3 = -1, k, m, nv;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int kf;

#ifdef _WIN32
    _putenv("RE15_GB_TEST=1");
#else
    setenv("RE15_GB_TEST", "1", 1);
#endif

    buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
    if (!buf) { printf("FAIL: RDT\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: parse\n"); return 1; }
    g_room_rdt_ok = 1;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x2090;

    if (!load_re2_bank(0x23u)) { printf("FAIL: bank\n"); return 1; }
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
    if (f_p3 < 0) { printf("FAIL: kein P3\n"); return 1; }
    for (k = 0; k < 20; k++) re15_gator_boss_tick(slot);

    printf("=== (a) ZUSTAND im P3-Park ===\n");
    printf("GATOR st=%d s1=%d s2=%d grid=0x%02x anim_flags=0x%04x frac=%d prev_valid=%d "
           "motion=%d af=%u scale=%d pos=(%d,%d,%d) yaw=%d\n",
           (int)e->state, (int)e->sub_state_1, (int)e->sub_state_2, (unsigned)e->grid_id,
           (unsigned)e->anim_flags, (int)e->anim_frac, (int)e->anim_prev_valid,
           (int)e->motion, (unsigned)e->anim_frame, (int)e->render_scale_q12,
           (int)e->x, (int)e->y, (int)e->z, (int)e->rot_y);
    printf("LEON  st=%d s1=%d motion=%d af=%u frac=%d pos=(%d,%d,%d) yaw=%d scale=%d\n",
           (int)pl->state, (int)pl->sub_state_1, (int)pl->motion, (unsigned)pl->anim_frame,
           (int)pl->anim_frac, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y,
           (int)pl->render_scale_q12);
    printf("BANK: remap_ok=%d loco_ok=%d own_ok=%d victim_ok=%d bones=%d meshes=%d "
           "anim.clips=%d rootyfix=%p/%d victim_rootyfix=%p/%d\n",
           bk->remap_ok, bk->loco_ok, bk->own_ok, bk->victim_ok,
           bk->skel.bone_count, bk->md1.mesh_count, bk->anim.clip_count,
           (void*)bk->skel.root_y_fix, bk->skel.root_y_fix_count,
           (void*)bk->skel_victim.root_y_fix, bk->skel_victim.root_y_fix_count);
    printf("GATES: uses_loco_bank=%d uses_own_bank=%d  clip11.frame_count=%d first=%d\n",
           re15_actor_uses_loco_bank(e), re15_actor_uses_own_bank(e),
           (11 < bk->anim.clip_count) ? bk->anim.clips[11].frame_count : -1,
           (11 < bk->anim.clip_count) ? bk->anim.clips[11].first_frame : -1);

    /* (b) RENDERER-BANKWAHL exakt nachbauen */
    {
        re15_md1_t dummy_mesh; re15_emd_skeleton_t dummy_skel; re15_emd_animation_t dummy_anim;
        re15_anim_banks_t banks; re15_anim_view_t av;
        memset(&dummy_mesh,0,sizeof dummy_mesh); memset(&dummy_skel,0,sizeof dummy_skel);
        memset(&dummy_anim,0,sizeof dummy_anim);
        memset(&banks, 0, sizeof banks);
        banks.def_mesh = &dummy_mesh; banks.def_skel = &dummy_skel; banks.def_anim = &dummy_anim;
        re15_actor_anim_select(e, 0, &banks, &av);
        printf("ANIM_SELECT: mesh==bank.md1? %d skel==bank.skel? %d anim==bank.anim? %d "
               "clip_override=%d\n",
               av.mesh == &bk->md1, av.skel == &bk->skel, av.anim == &bk->anim,
               av.clip_override);
        {
            int kf_r, kf_probe;
            re15_skel_pose_t pr[RE15_EMD_MAX_BONES], pp[RE15_EMD_MAX_BONES];
            void *sav = g_anim_pose_actor;
            g_anim_pose_actor = NULL;
            kf_probe = re15_compute_actor_kf(&bk->anim, &bk->skel, e, -1, e->anim_frame);
            re15_skel_compute_pose(&bk->skel, kf_probe, pp);
            g_anim_pose_actor = e;
            kf_r = re15_compute_actor_kf(av.anim, av.skel, e, av.clip_override, e->anim_frame);
            re15_skel_compute_pose(av.skel, kf_r, pr);
            g_anim_pose_actor = sav;
            printf("KF: sonde(-1,no-fade)=%d  renderer(ov=%d,fade)=%d  gleich=%d\n",
                   kf_probe, av.clip_override, kf_r, kf_probe == kf_r);
            printf("POSE-DIFF bone6 sonde=(%d,%d,%d) renderer=(%d,%d,%d)\n",
                   (int)pp[6].trans[0], (int)pp[6].trans[1], (int)pp[6].trans[2],
                   (int)pr[6].trans[0], (int)pr[6].trans[1], (int)pr[6].trans[2]);
            memcpy(poses, pr, sizeof poses);
            kf = kf_r;
        }
    }
    (void)kf;

    /* (d) Ist Mesh 6/7 das Maul? Bone-Elternkette ausgeben. */
    printf("=== (d) BONE-KETTE ===\n");
    for (k = 0; k < bk->skel.bone_count; k++)
        printf("  bone%-2d parent=%d\n", k, (int)bk->skel.bone_parent[k]);

    /* (c) WELT-AABB je Mesh mit Scale, plus Leons gerenderte Bone0-Welt */
    {
        int32_t fs = re15_sin_q12((int)e->rot_y), fc = re15_cos_q12((int)e->rot_y);
        int32_t nyaw[9] = { fc, 0, fs, 0, 0x1000, 0, -fs, 0, fc };
        int32_t wmn[3], wmx[3];
        int32_t lb0[3];
        re15_skel_pose_t vp[RE15_EMD_MAX_BONES];
        int vkf = -1;
        lb0[0] = pl->x; lb0[1] = pl->y; lb0[2] = pl->z;
        if (e->render_scale_q12) for (k = 0; k < 9; k++)
            nyaw[k] = (nyaw[k] * (int32_t)e->render_scale_q12) >> 12;
        printf("=== (c) WELT-AABB (Renderer-Kette, scale=%d) ===\n", (int)e->render_scale_q12);
        for (m = 0; m < bk->md1.mesh_count && m < bk->skel.bone_count; m++) {
            mesh_world_aabb(bk, poses, m, nyaw, e, wmn, wmx, &nv);
            if (!nv) continue;
            printf("MESH%-2d welt x[%7d..%7d] y[%7d..%7d] z[%7d..%7d] nv=%d\n", m,
                   (int)wmn[0], (int)wmx[0], (int)wmn[1], (int)wmx[1],
                   (int)wmn[2], (int)wmx[2], nv);
        }
        if (bk->victim_ok && bk->anim_victim.clip_count > 1) {
            void *sav = g_anim_pose_actor;
            g_anim_pose_actor = NULL;
            vkf = re15_compute_actor_kf(&bk->anim_victim, &bk->skel_victim, pl,
                                        (int)pl->motion, (uint32_t)pl->anim_frame);
            if (re15_skel_compute_pose(&bk->skel_victim, vkf, vp) == 0) {
                re15_skel_bone_to_world(vp[0].trans, pl->rot_y, pl->x, pl->y, pl->z, lb0);
                printf("LEON vkf(engine-kette, ov=%d, af=%u)=%d  bone0 lokal=(%d,%d,%d) "
                       "welt=(%d,%d,%d)\n", (int)pl->motion, (unsigned)pl->anim_frame, vkf,
                       (int)vp[0].trans[0], (int)vp[0].trans[1], (int)vp[0].trans[2],
                       (int)lb0[0], (int)lb0[1], (int)lb0[2]);
            }
            {
                const re15_emd_clip_t *vc = &bk->anim_victim.clips[1];
                int fi = vc->first_frame + 119;
                int fend = vc->first_frame + vc->frame_count - 1;
                int vkf2;
                while ((bk->anim_victim.frames[fi] & 0x8000u) && fi < fend) fi++;
                vkf2 = (int)(bk->anim_victim.frames[fi] & 0xFFFu);
                printf("LEON vkf(Befund-Handrechnung)=%d  clip1.fc=%d  gleich=%d\n",
                       vkf2, vc->frame_count, vkf2 == vkf);
            }
            g_anim_pose_actor = sav;
        }
        {
            int32_t a6n[3], a6x[3], a7n[3], a7x[3];
            int i6, i7;
            mesh_world_aabb(bk, poses, 6, nyaw, e, a6n, a6x, &nv);
            mesh_world_aabb(bk, poses, 7, nyaw, e, a7n, a7x, &nv);
            i6 = (lb0[0] >= a6n[0] && lb0[0] <= a6x[0] && lb0[1] >= a6n[1] && lb0[1] <= a6x[1] &&
                  lb0[2] >= a6n[2] && lb0[2] <= a6x[2]);
            i7 = (lb0[0] >= a7n[0] && lb0[0] <= a7x[0] && lb0[1] >= a7n[1] && lb0[1] <= a7x[1] &&
                  lb0[2] >= a7n[2] && lb0[2] <= a7x[2]);
            printf("WELT-TEST Bone0=(%d,%d,%d) in Maul6=%d in Maul7=%d\n",
                   (int)lb0[0], (int)lb0[1], (int)lb0[2], i6, i7);
            printf("WELT Maul6 x[%d..%d] y[%d..%d] z[%d..%d]\n",
                   (int)a6n[0], (int)a6x[0], (int)a6n[1], (int)a6x[1], (int)a6n[2], (int)a6x[2]);
            printf("WELT Maul7 x[%d..%d] y[%d..%d] z[%d..%d]\n",
                   (int)a7n[0], (int)a7x[0], (int)a7n[1], (int)a7x[1], (int)a7n[2], (int)a7x[2]);
            {
                int32_t fwx = re15_cos_q12((int)e->rot_y), fwz = -re15_sin_q12((int)e->rot_y);
                int64_t dL = (((int64_t)(pl->x - e->x) * fwx) + ((int64_t)(pl->z - e->z) * fwz)) >> 12;
                int64_t d6 = (((int64_t)(a6x[0] - e->x) * fwx) + ((int64_t)(a6x[2] - e->z) * fwz)) >> 12;
                printf("PROJ Leon-Wurzel auf Gator-Vorwaerts = %d  (AABB-Ecke6 = %d)\n",
                       (int)dL, (int)d6);
            }
        }
    }
    printf("=== ENDE ===\n");
    return 0;
}
