/* probe_r16_gator_finisher_anker.c - MESS-SONDE Runde 16 (2026-09-19), kein add_test.
 *
 * Nutzer: "Der Aligator hat Leon immer noch nicht im Maul im Finisher, sondern er
 * liegt darunter" (befund_2090_F309_marke1).
 *
 * Metrik (yaw-invariant, massstabsfrei): Leons GERENDERTE Wurzel (Entity-Pos +
 * RotY(yaw)*POSE-Kanal des Opfer-Keyframes, wie re15_skel_bone_to_world sie legt) wird
 * in den GATOR-LOKALEN Rahmen zurueckgerechnet: local = RotY(-yaw)*(root - G) / scale.
 * Im Original (kein Scale, FUN_80015B94/CB8: gemeinsamer Anker) ist diese lokale Lage
 * exakt (off_v - off_g) + POS_v - die authored Relation. Der Port muss dieselbe lokale
 * Lage liefern, sonst sitzt Leon woanders am skalierten Koerper.
 * "6=?/7=?" = liegt die lokale Wurzel in der lokalen AABB von MD1-Mesh 6 (Oberkiefer) /
 * 7 (Unterkiefer) der GEPOSTEN Gator-Bindkette (re15_skel_compute_pose, unskaliert).
 *
 * Teil A: analytisch aus EM23.EMD (Varianten T=heute, Y=+y-Leitung, S=skaliert).
 * Teil B: der ECHTE Boss-Tick in ROOM2090 (re15_gator_boss_tick), dieselbe Metrik.
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
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
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

static re15_enemy_bank_t B;

static int kf_of(const re15_emd_animation_t *an, int clip, int fr)
{
    const re15_emd_clip_t *c = &an->clips[clip];
    int slot = fr % c->frame_count;
    int fi = c->first_frame + slot;
    int fend = c->first_frame + c->frame_count - 1;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(an->frames[fi] & 0xFFFu);
}
static void spd(const re15_emd_skeleton_t *sk, int kf, int32_t o[3])
{ int16_t x=0,y=0,z=0; re15_emd_get_keyframe_speed(sk, kf, &x,&y,&z); o[0]=x;o[1]=y;o[2]=z; }
static void pos(const re15_emd_skeleton_t *sk, int kf, int32_t o[3])
{ int16_t x=0,y=0,z=0; re15_emd_get_keyframe_position(sk, kf, &x,&y,&z); o[0]=x;o[1]=y;o[2]=z; }

/* lokale (unskalierte, ungedrehte) AABB + minDist eines MD1-Meshes in der Pose kf */
static int mesh_local(int kf, int mesh, int32_t lo[3], int32_t hi[3],
                      const int32_t p[3], double *dmin)
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    const re15_md1_mesh_t *m;
    int pass, i, r, k, n = 0;
    double best = 1e18;
    void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
    r = re15_skel_compute_pose(&B.skel, kf, poses);
    g_anim_pose_actor = sv;
    if (r != 0) return -1;
    if (mesh < 0 || mesh >= B.md1.mesh_count) return -2;
    m = &B.md1.meshes[mesh];
    lo[0]=lo[1]=lo[2]=0x7fffffff; hi[0]=hi[1]=hi[2]=-0x7fffffff;
    for (pass = 0; pass < 2; pass++) {
        const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
        int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
        for (i = 0; i < NV; i++) {
            int32_t v[3] = { V[i].x, V[i].y, V[i].z }, w[3];
            for (r = 0; r < 3; r++) {
                int64_t s = 0;
                for (k = 0; k < 3; k++) s += (int64_t)poses[mesh].rot[r*3+k] * v[k];
                w[r] = (int32_t)(s >> 12) + poses[mesh].trans[r];
            }
            for (r = 0; r < 3; r++) { if (w[r] < lo[r]) lo[r] = w[r]; if (w[r] > hi[r]) hi[r] = w[r]; }
            { double dx = (double)w[0]-p[0], dy=(double)w[1]-p[1], dz=(double)w[2]-p[2];
              double d = sqrt(dx*dx+dy*dy+dz*dz); if (d < best) best = d; }
            n++;
        }
    }
    if (dmin) *dmin = best;
    return n ? 0 : -3;
}
static int inside(const int32_t p[3], const int32_t lo[3], const int32_t hi[3])
{ return p[0]>=lo[0]&&p[0]<=hi[0]&&p[1]>=lo[1]&&p[1]<=hi[1]&&p[2]>=lo[2]&&p[2]<=hi[2]; }

/* Leons gerenderte Wurzel (Welt) -> Gator-lokal (RotY(-yaw), /scale) */
static void leon_local(const re15_actor_t *e, const re15_actor_t *pl, int vkf, int32_t out[3])
{
    int32_t P[3], tr[3], w[3], d[3];
    int32_t cs = re15_cos_q12((int)pl->rot_y), sn = re15_sin_q12((int)pl->rot_y);
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    pos(&B.skel_victim, vkf, P);
    tr[0]=P[0]; tr[1]=P[1]; tr[2]=P[2];
    re15_skel_bone_to_world(tr, (int16_t)pl->rot_y, pl->x, pl->y, pl->z, w);   /* wie der Renderer */
    d[0] = w[0]-e->x; d[1] = w[1]-e->y; d[2] = w[2]-e->z;
    cs = re15_cos_q12((int)e->rot_y); sn = re15_sin_q12((int)e->rot_y);
    /* Inverse von (wx = cos*lx + sin*lz, wz = -sin*lx + cos*lz) */
    out[0] = (int32_t)(((int64_t)cs*d[0] - (int64_t)sn*d[2]) >> 12);
    out[2] = (int32_t)(((int64_t)sn*d[0] + (int64_t)cs*d[2]) >> 12);
    out[1] = d[1];
    out[0] = (int32_t)((int64_t)out[0]*4096/s);
    out[1] = (int32_t)((int64_t)out[1]*4096/s);
    out[2] = (int32_t)((int64_t)out[2]*4096/s);
}

static void report(const char *tag, const re15_actor_t *e, const re15_actor_t *pl,
                   int gclip, int gfr, int vclip, int vfr, const int32_t ref[3])
{
    int gkf = kf_of(&B.anim, gclip, gfr), vkf = kf_of(&B.anim_victim, vclip, vfr);
    int32_t L[3], lo6[3], hi6[3], lo7[3], hi7[3];
    double d6 = -1, d7 = -1;
    leon_local(e, pl, vkf, L);
    mesh_local(gkf, 6, lo6, hi6, L, &d6);
    mesh_local(gkf, 7, lo7, hi7, L, &d7);
    printf("  %-34s G=(%6d,%6d,%6d) yaw=%4d s=%4d | Leon-Ent=(%6d,%6d,%6d) | LOKAL=(%6d,%6d,%6d) dRef=(%6d,%6d,%6d) | 6=%d/7=%d min6=%.0f min7=%.0f\n",
           tag, e->x, e->y, e->z, (int)e->rot_y, e->render_scale_q12 ? e->render_scale_q12 : 4096,
           pl->x, pl->y, pl->z, L[0], L[1], L[2],
           L[0]-ref[0], L[1]-ref[1], L[2]-ref[2],
           inside(L, lo6, hi6), inside(L, lo7, hi7), d6, d7);
}

/* Referenz: das Original (kein Scale): local = (off_v - off_g) + POS_v ; y-Leitung sy */
static void ref_local(int gclip, int gfr, int vclip, int vfr, int32_t out[3])
{
    int32_t og[3], ov[3], P[3];
    spd(&B.skel, kf_of(&B.anim, gclip, gfr), og);
    spd(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), ov);
    pos(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), P);
    out[0] = ov[0]-og[0]+P[0]; out[1] = ov[1]-og[1]+P[1]; out[2] = ov[2]-og[2]+P[2];
}

/* Varianten der Port-Platzierung fuer einen Frame; gemeinsamer Anker aus Clip 4 f13 */
static void place(re15_actor_t *e, re15_actor_t *pl, int scale, int gclip, int gfr,
                  int vclip, int vfr, char variant)
{
    int32_t ov[3], og[3], P[3], ux, uz;
    memset(e, 0, sizeof *e); memset(pl, 0, sizeof *pl);
    e->type = 0x23; e->render_scale_q12 = (int16_t)(scale == 4096 ? 0 : scale);
    e->x = -5608; e->y = -1200; e->z = -22446; e->rot_y = 113;   /* Spielwerte gator_boss.log */
    e->motion = 4; e->anim_frame = 13;
    re15_clip_anchor_set_pub(e, &B.skel, &B.anim, 4, 13);         /* @0x80101110-34 */
    pl->anchor_x = e->anchor_x; pl->anchor_z = e->anchor_z; pl->rot_y = e->rot_y;
    e->motion = (uint8_t)gclip; e->anim_frame = (uint32_t)gfr;
    re15_clip_root_motion_abs_pub(e, &B.skel, &B.anim, gclip, gfr);         /* @0x80101168-78 */
    pl->motion = (uint8_t)vclip; pl->anim_frame = (uint32_t)vfr;
    re15_clip_root_motion_abs_pub(pl, &B.skel_victim, &B.anim_victim, vclip, vfr);
    ux = pl->x; uz = pl->z;
    spd(&B.skel, kf_of(&B.anim, gclip, gfr), og);
    spd(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), ov);
    pos(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), P);
    switch (variant) {
    case 'T': pl->y = 0; break;                                   /* heute: pl->y unberuehrt (=0 im Spiel) */
    case 'Y': pl->y = e->y + (ov[1]-og[1]); break;                /* nur y-Leitung @0x80015d6c-7c */
    case 'S': {                                                   /* skaliert: local*s */
        int s = scale;
        pl->x = e->x + (int32_t)(((int64_t)(ux - e->x) * s) >> 12);
        pl->z = e->z + (int32_t)(((int64_t)(uz - e->z) * s) >> 12);
        /* gerenderte Wurzel-y soll G.y + s*((sy_v-sy_g) + POSy) sein; der Renderer addiert
         * POSy unskaliert -> Entity-y traegt den Rest */
        pl->y = e->y + (int32_t)(((int64_t)(ov[1]-og[1]) * s) >> 12)
                     - (int32_t)(((int64_t)P[1] * (4096 - s)) >> 12);
        break; }
    }
}

int main(void)
{
    size_t sz = 0;
    uint8_t *emd = slurp(RE15_ASSET_RE2_DIR "/EM23.EMD", &sz);
    int f, c, k;
    if (!emd) { printf("FAIL EM23.EMD\n"); return 1; }
    memset(&B, 0, sizeof B);
    if (re2_emd_parse_bank(emd, sz, &B) != 0) { printf("FAIL parse\n"); return 1; }
    B.ok = 1;
    printf("EM23.EMD %lu B clips=%d victim_ok=%d vclips=%d meshes=%d bones=%d\n",
           (unsigned long)sz, B.anim.clip_count, B.victim_ok, B.anim_victim.clip_count,
           B.md1.mesh_count, B.skel.bone_count);

    /* ---- Datenzensus: POSE-Kanal x/z und SPEED-y der Opferbank + Gator-Clips ---- */
    {   int nzx = 0, nzz = 0, nsy = 0, kfc = B.skel_victim.keyframe_count;
        int32_t P[3], S[3];
        for (k = 0; k < kfc; k++) { pos(&B.skel_victim, k, P); spd(&B.skel_victim, k, S);
            if (P[0]) nzx++; if (P[2]) nzz++; if (S[1]) nsy++; }
        printf("OPFER-Bank: %d kf; POSE x!=0: %d, POSE z!=0: %d, SPEED y!=0: %d\n", kfc, nzx, nzz, nsy);
        nzx = nzz = nsy = 0;
        for (c = 0; c < B.anim.clip_count; c++) for (f = 0; f < B.anim.clips[c].frame_count; f++) {
            int kk = kf_of(&B.anim, c, f); pos(&B.skel, kk, P); spd(&B.skel, kk, S);
            if (P[0]) nzx++; if (P[2]) nzz++; if (S[1]) nsy++; }
        printf("GATOR-Bank (alle Clips/Frames): POSE x!=0: %d, POSE z!=0: %d, SPEED y!=0: %d\n", nzx, nzz, nsy);
    }

    /* ---- Teil A: analytisch ---- */
    printf("\n=== TEIL A: analytisch, Anker Clip4 f13, Gator Clip5 sf / Clip11 f0, Leon Opfer-Clip1 ===\n");
    printf("Referenz (Original, kein Scale): LOKAL = (off_v-off_g)+POS_v je Frame; dRef = Abweichung davon\n");
    {   static const int frames[] = { 0, 30, 60, 75, 90, 119 };
        unsigned i;
        for (i = 0; i < sizeof frames/sizeof frames[0]; i++) {
            int sf = frames[i];
            int32_t ref[3]; re15_actor_t e, pl; char nm[64];
            ref_local(5, sf, 1, sf, ref);
            printf("-- P2 sf=%3d  Original-LOKAL=(%d,%d,%d)\n", sf, ref[0], ref[1], ref[2]);
            place(&e, &pl, 4096, 5, sf, 1, sf, 'Y'); snprintf(nm, sizeof nm, "ORIGINAL s=4096 (y-Leitung)");
            report(nm, &e, &pl, 5, sf, 1, sf, ref);
            place(&e, &pl, 2731, 5, sf, 1, sf, 'T'); report("T heute  s=2731 (pl.y=0)", &e, &pl, 5, sf, 1, sf, ref);
            place(&e, &pl, 2731, 5, sf, 1, sf, 'Y'); report("Y heute+y-Leitung s=2731", &e, &pl, 5, sf, 1, sf, ref);
            place(&e, &pl, 2731, 5, sf, 1, sf, 'S'); report("S SKALIERT s=2731", &e, &pl, 5, sf, 1, sf, ref);
        }
        {   int32_t ref[3]; re15_actor_t e, pl;
            ref_local(11, 0, 1, 119, ref);
            printf("-- P3 Kau-Loop Clip11 f0 / Opfer f119  Original-LOKAL=(%d,%d,%d)\n", ref[0], ref[1], ref[2]);
            place(&e, &pl, 4096, 11, 0, 1, 119, 'Y'); report("ORIGINAL s=4096 (y-Leitung)", &e, &pl, 11, 0, 1, 119, ref);
            place(&e, &pl, 2731, 11, 0, 1, 119, 'T'); report("T heute  s=2731 (pl.y=0)", &e, &pl, 11, 0, 1, 119, ref);
            place(&e, &pl, 2731, 11, 0, 1, 119, 'Y'); report("Y heute+y-Leitung s=2731", &e, &pl, 11, 0, 1, 119, ref);
            place(&e, &pl, 2731, 11, 0, 1, 119, 'S'); report("S SKALIERT s=2731", &e, &pl, 11, 0, 1, 119, ref);
            /* Kiefer-AABBs lokal zur Einordnung */
            {   int32_t lo[3], hi[3], p0[3] = {0,0,0}; double d;
                int gkf = kf_of(&B.anim, 11, 0);
                mesh_local(gkf, 6, lo, hi, p0, &d);
                printf("   Mesh6 (Oberkiefer) LOKAL-AABB x[%d..%d] y[%d..%d] z[%d..%d]\n", lo[0],hi[0],lo[1],hi[1],lo[2],hi[2]);
                mesh_local(gkf, 7, lo, hi, p0, &d);
                printf("   Mesh7 (Unterkiefer) LOKAL-AABB x[%d..%d] y[%d..%d] z[%d..%d]\n", lo[0],hi[0],lo[1],hi[1],lo[2],hi[2]);
            }
        }
        /* Yaw-Invarianz der Metrik: dieselbe Rechnung bei yaw 0 */
        {   int32_t ref[3]; re15_actor_t e, pl;
            ref_local(11, 0, 1, 119, ref);
            place(&e, &pl, 2731, 11, 0, 1, 119, 'S'); e.rot_y = 0; pl.rot_y = 0;
            re15_clip_anchor_set_pub(&e, &B.skel, &B.anim, 4, 13); pl.anchor_x = e.anchor_x; pl.anchor_z = e.anchor_z;
            re15_clip_root_motion_abs_pub(&e, &B.skel, &B.anim, 11, 0);
            re15_clip_root_motion_abs_pub(&pl, &B.skel_victim, &B.anim_victim, 1, 119);
            pl.x = e.x + (int32_t)(((int64_t)(pl.x - e.x) * 2731) >> 12);
            pl.z = e.z + (int32_t)(((int64_t)(pl.z - e.z) * 2731) >> 12);
            report("S SKALIERT s=2731 bei yaw=0", &e, &pl, 11, 0, 1, 119, ref);
        }
        /* P0-Teleport (@0x8010106C-84): Gator+RotY*(10643,-915) lokal vs skaliert */
        printf("-- P0 Teleport (10643,-915): unskaliert = lokal (10643,-915); skaliert 2731 = (%d,%d)\n",
               (int)(((int64_t)10643*2731)>>12), (int)(((int64_t)-915*2731)>>12));
    }

    /* ---- Teil B: der echte Boss-Tick in ROOM2090 ---- */
    printf("\n=== TEIL B: echter Boss-Tick ROOM2090 (RE15_GB_TEST=1), Metrik am Live-Zustand ===\n");
#ifdef _WIN32
    _putenv("RE15_GB_TEST=1"); _putenv("RE15_GB_STUMM=1");
#else
    setenv("RE15_GB_TEST", "1", 1); setenv("RE15_GB_STUMM", "1", 1);
#endif
    {
        size_t n = 0;
        uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE2/ROOM2090.RDT", &n);
        static uint8_t *s_ems = NULL; size_t es = 0;
        re15_enemy_bank_t *eb;
        re15_actor_t *pl, *e; int slot, fress_f0 = -1, p3 = 0;
        if (!buf || re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL RDT\n"); return 1; }
        g_room_rdt_ok = 1;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_init(); re15_aot_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset();
        re15_damage_seed_rng(0x2545f491u);
        g_current_room_id = 0x2090;
        s_ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &es);
        if (!s_ems) { printf("FAIL CDEMD0.EMS\n"); return 1; }
        eb = re15_enemy_alloc(0x23u);
        if (!eb || re2_ems_load_bank(s_ems, es, 0x23, eb, NULL) != 0) { printf("FAIL Bank 0x23\n"); return 1; }
        eb->buf = NULL; eb->ok = 1;
        pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        slot = RE15_ACTOR_MAX - 1; e = &g_actors[slot];
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x23u; e->x = -6000; e->y = 0; e->z = -22000;
        e->grid_id = 0; e->state = 0; e->em_flag_id = 0xFF;
        re15_enemy_apply_hitbox(e, 0x23u);
        pl->active = 1; pl->type = 0; pl->hp = 30; pl->x = -4000; pl->z = -22000; pl->y = 0; pl->floor = 0;
        re15_gator_boss_tick(slot);
        e->x = -6000; e->z = -22000; e->y = -1200;
        for (f = 0; f < 3000; f++) {
            int32_t ox, oz;
            if (pl->hp >= 0) { pl->x = -4000; pl->z = -22000; pl->y = 0; pl->state = 0; pl->motion = 0; pl->hit_react = 0; }
            ox = e->x; oz = e->z;
            re15_gator_boss_tick(slot);
            if ((e->x != ox || e->z != oz) && !re15_gator_boss_skip_clamp(e)) {
                int32_t nx = e->x, nz = e->z;
                re15_collision_constrain_enemy(&g_room_rdt, ox, oz, &nx, &nz, e->hit_radius_min, e->y, 4u);
                e->x = nx; e->z = nz;
            }
            if (re15_gator_fressen_hold() && fress_f0 < 0) fress_f0 = f;
            if (fress_f0 >= 0 && e->motion == 5 && (e->anim_frame % 30 == 0 || e->anim_frame == 119)) {
                int32_t ref[3]; char nm[64];
                ref_local(5, (int)e->anim_frame, 1, (int)pl->anim_frame, ref);
                snprintf(nm, sizeof nm, "LIVE P2 gaf=%u laf=%u vs=%d", (unsigned)e->anim_frame,
                         (unsigned)pl->anim_frame, re15_player_victim_state());
                report(nm, e, pl, 5, (int)e->anim_frame, (int)pl->motion, (int)pl->anim_frame, ref);
            }
            if (e->motion == 11) {
                if (++p3 == 1) {
                    int32_t ref[3]; char nm[64];
                    ref_local(11, (int)e->anim_frame, 1, 119, ref);
                    snprintf(nm, sizeof nm, "LIVE P3 gaf=%u laf=%u", (unsigned)e->anim_frame, (unsigned)pl->anim_frame);
                    report(nm, e, pl, 11, (int)e->anim_frame, (int)pl->motion, (int)pl->anim_frame, ref);
                    printf("   (Spielwerte zum Vergleich gator_boss.log 2026-09-14: FRESS-P2 Anker=(-8368,-21962) yaw=113, sf=120 pl=(-65,0,-23215))\n");
                    printf("   LIVE: anker=(%d,%d) yaw=%d pl=(%d,%d,%d) scale=%d\n", e->anchor_x, e->anchor_z, (int)e->rot_y,
                           pl->x, pl->y, pl->z, (int)e->render_scale_q12);
                }
                if (p3 > 2) break;
            }
        }
        printf("ENDE Teil B: Fress-Start F%d\n", fress_f0);
    }
    free(emd);
    return 0;
}
