/* NACHMESS-SONDE (Gegenpruefung Befund anker-y, 2026-09-14).
 * Misst die ECHTE Port-P3-Lage (Gator Clip 11, Position AUS DEM WRAP von Clip5 f120)
 * gegen die vom Befund behaupteten Varianten. Kein add_test. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re2_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_actor.h"
#include "re15_skeleton.h"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static uint8_t *slurp(const char *path, size_t *out_sz)
{
    FILE *f = fopen(path, "rb"); long sz; uint8_t *b;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); if (out_sz) *out_sz = (size_t)sz; return b;
}

static re15_enemy_bank_t s_bank;

static void bone_matrix(const re15_actor_t *e, const re15_skel_pose_t *np,
                        int32_t R[9], int32_t T[3])
{
    int32_t fs = re15_sin_q12((int)e->rot_y), fc = re15_cos_q12((int)e->rot_y);
    int32_t nyaw[9]; int r, c, k;
    nyaw[0]=fc; nyaw[1]=0; nyaw[2]=fs; nyaw[3]=0; nyaw[4]=0x1000; nyaw[5]=0;
    nyaw[6]=-fs; nyaw[7]=0; nyaw[8]=fc;
    if (e->render_scale_q12)
        for (k=0;k<9;k++) nyaw[k] = (nyaw[k]*(int32_t)e->render_scale_q12) >> 12;
    for (r=0;r<3;r++) for (c=0;c<3;c++) {
        int64_t s=0; for (k=0;k<3;k++) s += (int64_t)nyaw[r*3+k]*(int64_t)np->rot[k*3+c];
        R[r*3+c] = (int32_t)(s>>12);
    }
    for (r=0;r<3;r++) {
        int64_t s=0; for (k=0;k<3;k++) s += (int64_t)nyaw[r*3+k]*(int64_t)np->trans[k];
        T[r] = (int32_t)(s>>12);
    }
    T[0]+=e->x; T[1]+=e->y; T[2]+=e->z;
}

static int mesh_stats(const re15_actor_t *e, int mesh, int kf,
                      int32_t lo[3], int32_t hi[3], const int32_t p[3], double *out_min)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int32_t R[9], T[3]; const re15_md1_mesh_t *m; int pass,i,r,n=0; double best=1e18;
    if (re15_skel_compute_pose(&s_bank.skel, kf, poses) != 0) return -1;
    if (mesh < 0 || mesh >= s_bank.md1.mesh_count) return -2;
    bone_matrix(e, &poses[mesh], R, T);
    m = &s_bank.md1.meshes[mesh];
    lo[0]=lo[1]=lo[2]=0x7fffffff; hi[0]=hi[1]=hi[2]=-0x7fffffff;
    for (pass=0; pass<2; pass++) {
        const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
        int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
        for (i=0;i<NV;i++) {
            int32_t v[3], w[3]; double dx,dy,dz,d;
            v[0]=V[i].x; v[1]=V[i].y; v[2]=V[i].z;
            for (r=0;r<3;r++) { int64_t s=0; int k;
                for (k=0;k<3;k++) s += (int64_t)R[r*3+k]*(int64_t)v[k];
                w[r] = (int32_t)(s>>12) + T[r]; }
            for (r=0;r<3;r++) { if (w[r]<lo[r]) lo[r]=w[r]; if (w[r]>hi[r]) hi[r]=w[r]; }
            dx=(double)w[0]-p[0]; dy=(double)w[1]-p[1]; dz=(double)w[2]-p[2];
            d=sqrt(dx*dx+dy*dy+dz*dz); if (d<best) best=d; n++;
        }
    }
    if (out_min) *out_min = best;
    return n ? 0 : -3;
}

static int gator_kf(const re15_actor_t *e)
{
    const re15_emd_clip_t *c = &s_bank.anim.clips[e->motion];
    int slot = (int)e->anim_frame % c->frame_count;
    int fi = c->first_frame + slot;
    int fend = c->first_frame + c->frame_count - 1;
    while ((s_bank.anim.frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(s_bank.anim.frames[fi] & 0xFFFu);
}

/* Leons GERENDERTE Wurzel - wie skeleton_common.c:255-256: kf_py + root_y_fix */
static void leon_root(const re15_actor_t *pl, int32_t w[3], int *out_kf, int *out_py, int *out_fix)
{
    int sl, fiv, fev, kfv; int16_t px=0,py=0,pz=0; int32_t tr[3]; int fix;
    const re15_emd_clip_t *cv = &s_bank.anim_victim.clips[pl->motion];
    sl  = (int)pl->anim_frame % cv->frame_count;
    fiv = cv->first_frame + sl; fev = cv->first_frame + cv->frame_count - 1;
    while ((s_bank.anim_victim.frames[fiv] & 0x8000u) && fiv < fev) fiv++;
    kfv = (int)(s_bank.anim_victim.frames[fiv] & 0xFFFu);
    re15_emd_get_keyframe_position(&s_bank.skel_victim, kfv, &px, &py, &pz);
    fix = re15_skel_root_y_fix(&s_bank.skel_victim, kfv);
    tr[0]=px; tr[1]=(int16_t)(py+fix); tr[2]=pz;
    re15_skel_bone_to_world(tr, (int16_t)pl->rot_y, pl->x, pl->y, pl->z, w);
    if (out_kf) *out_kf = kfv;
    if (out_py) *out_py = py;
    if (out_fix) *out_fix = fix;
}

static void bericht(const char *name, re15_actor_t *e, re15_actor_t *pl)
{
    int kf = gator_kf(e); int32_t p[3]; int mi, kfv=0, py=0, fix=0;
    leon_root(pl, p, &kfv, &py, &fix);
    printf("--- %s\n", name);
    printf("    Gator clip=%d f=%u kf=%d pos=(%d,%d,%d) | Leon clip=%d f=%u pos=(%d,%d,%d) kf=%d py=%d fix=%d -> Wurzel(%d,%d,%d)\n",
           (int)e->motion,(unsigned)e->anim_frame,kf,(int)e->x,(int)e->y,(int)e->z,
           (int)pl->motion,(unsigned)pl->anim_frame,(int)pl->x,(int)pl->y,(int)pl->z,
           kfv,py,fix,(int)p[0],(int)p[1],(int)p[2]);
    for (mi=6; mi<=7; mi++) {
        int32_t lo[3], hi[3]; double d=-1.0;
        if (mesh_stats(e, mi, kf, lo, hi, p, &d) == 0)
            printf("    Mesh %d AABB x[%d..%d] y[%d..%d] z[%d..%d] minDist=%.1f\n",
                   mi, lo[0],hi[0],lo[1],hi[1],lo[2],hi[2], d);
    }
}

int main(void)
{
    size_t sz=0; uint8_t *emd = slurp(RE15_ASSET_RE2_DIR "/EM23.EMD", &sz);
    re15_actor_t e, pl; int rc, f;
    if (!emd) { printf("EM23.EMD fehlt\n"); return 1; }
    memset(&s_bank,0,sizeof s_bank);
    rc = re2_emd_parse_bank(emd, sz, &s_bank);
    printf("parse=%d clips=%d victim_ok=%d root_y_fix(skel)=%p root_y_fix(victim)=%p\n",
           rc, s_bank.anim.clip_count, s_bank.victim_ok,
           (void*)s_bank.skel.root_y_fix, (void*)s_bank.skel_victim.root_y_fix);
    if (rc) return 1;
    s_bank.ok = 1;

    { int c; for (c=0;c<s_bank.anim.clip_count;c++)
        printf("   Gator Clip %2d: first=%d n=%d\n", c,
               s_bank.anim.clips[c].first_frame, s_bank.anim.clips[c].frame_count);
      for (c=0;c<s_bank.anim_victim.clip_count;c++)
        printf("   Opfer Clip %2d: first=%d n=%d\n", c,
               s_bank.anim_victim.clips[c].first_frame, s_bank.anim_victim.clips[c].frame_count);
    }

    memset(&e,0,sizeof e); memset(&pl,0,sizeof pl);
    e.type=0x23; e.rot_y=0; e.render_scale_q12=2731; e.x=0; e.y=-1200; e.z=0;
    e.motion=4; e.anim_frame=13;
    re15_clip_anchor_set_pub(&e, &s_bank.skel, &s_bank.anim, 4, 13);
    printf("Anker=(%d,%d)\n", (int)e.anchor_x, (int)e.anchor_z);
    pl.anchor_x=e.anchor_x; pl.anchor_z=e.anchor_z; pl.rot_y=e.rot_y;
    pl.motion=1; pl.anim_frame=119; pl.y=0;
    re15_clip_root_motion_abs_pub(&pl, &s_bank.skel_victim, &s_bank.anim_victim, 1, 119);

    e.motion=5; e.anim_frame=120;
    re15_clip_root_motion_abs_pub(&e, &s_bank.skel, &s_bank.anim, 5, 120);
    printf("\n>>> Nach P2 sf=120: Gator-Pos = (%d,%d,%d)\n", (int)e.x,(int)e.y,(int)e.z);

    for (f=0; f<30; f+=5) {
        char nm[128]; e.motion=11; e.anim_frame=(uint32_t)f;
        snprintf(nm,sizeof nm,"(IST) ECHTER PORT-P3: Clip11 f%d @ x=%d", f, (int)e.x);
        bericht(nm, &e, &pl);
    }

    e.motion=5; e.anim_frame=0;
    bericht("(IST-A) Befund-Variante A: Clip5 f0 @ x=0", &e, &pl);

    { re15_actor_t p2=pl, e2=e; p2.y=e.y; e2.motion=11; e2.anim_frame=0;
      bericht("(FIX-Y) echter P3 Clip11 f0 @ x=0 + Leon y=-1200", &e2, &p2); }

    { re15_actor_t e2=e; int g;
      for (g=0; g<30; g+=5) { char nm[128];
        e2.motion=11; e2.anim_frame=(uint32_t)g;
        re15_clip_root_motion_abs_pub(&e2,&s_bank.skel,&s_bank.anim,11,g);
        snprintf(nm,sizeof nm,"(FIX-P) Clip11 f%d + Positions-Fix (x=%d), Leon y=0", g,(int)e2.x);
        bericht(nm,&e2,&pl); } }

    { re15_actor_t e2=e, p2=pl; int g;
      for (g=0; g<30; g+=5) { char nm[128];
        e2.motion=11; e2.anim_frame=(uint32_t)g;
        re15_clip_root_motion_abs_pub(&e2,&s_bank.skel,&s_bank.anim,11,g);
        p2.y = e2.y;
        snprintf(nm,sizeof nm,"(FIX-BEIDE) Clip11 f%d x=%d + Leon y=%d",g,(int)e2.x,(int)p2.y);
        bericht(nm,&e2,&p2); } }


    /* ---- ECHTE Y-LEITUNG: sy aus den Keyframes (RE2 @0x80015c54-64 / @0x80015d6c-7c) ---- */
    {
        struct { const char *n; const re15_emd_skeleton_t *sk; const re15_emd_animation_t *an; int clip, fr; } t[] = {
            {"Gator C4 f13 ", &s_bank.skel, &s_bank.anim, 4, 13},
            {"Gator C5 f0  ", &s_bank.skel, &s_bank.anim, 5, 0},
            {"Gator C5 f119", &s_bank.skel, &s_bank.anim, 5, 119},
            {"Gator C11 f0 ", &s_bank.skel, &s_bank.anim, 11, 0},
            {"Gator C11 f15", &s_bank.skel, &s_bank.anim, 11, 15},
            {"Gator C11 f29", &s_bank.skel, &s_bank.anim, 11, 29},
            {"Opfer C1 f0  ", &s_bank.skel_victim, &s_bank.anim_victim, 1, 0},
            {"Opfer C1 f60 ", &s_bank.skel_victim, &s_bank.anim_victim, 1, 60},
            {"Opfer C1 f119", &s_bank.skel_victim, &s_bank.anim_victim, 1, 119},
            {"Opfer C0 f0  ", &s_bank.skel_victim, &s_bank.anim_victim, 0, 0},
        };
        unsigned ti;
        printf("\nKEYFRAME-SPEED (sx,sy,sz) + Wurzelpose (px,py,pz):\n");
        for (ti=0; ti<sizeof t/sizeof t[0]; ti++) {
            const re15_emd_clip_t *c = &t[ti].an->clips[t[ti].clip];
            int slot = t[ti].fr % c->frame_count;
            int fi = c->first_frame + slot;
            int fend = c->first_frame + c->frame_count - 1;
            int kf; int16_t sx=0,sy=0,sz=0,px=0,py=0,pz=0;
            while ((t[ti].an->frames[fi] & 0x8000u) && fi < fend) fi++;
            kf = (int)(t[ti].an->frames[fi] & 0xFFFu);
            re15_emd_get_keyframe_speed(t[ti].sk, kf, &sx, &sy, &sz);
            re15_emd_get_keyframe_position(t[ti].sk, kf, &px, &py, &pz);
            printf("   %s kf=%4d speed=(%6d,%6d,%6d) pose=(%6d,%6d,%6d)\n",
                   t[ti].n, kf, sx, sy, sz, px, py, pz);
        }
    }
    { re15_skel_pose_t a[RE15_EMD_MAX_BONES], b[RE15_EMD_MAX_BONES]; int i,k,diff=0;
      re15_skel_compute_pose(&s_bank.skel, 640, a);
      re15_skel_compute_pose(&s_bank.skel, 1094, b);
      for (i=0;i<s_bank.skel.bone_count;i++) {
        for (k=0;k<9;k++) if (a[i].rot[k]!=b[i].rot[k]) diff++;
        for (k=0;k<3;k++) if (a[i].trans[k]!=b[i].trans[k]) diff++; }
      printf("\nPose-Diff kf640 vs kf1094 ueber %d Bones: %d abweichende Komponenten\n",
             s_bank.skel.bone_count, diff); }
    free(emd); return 0;
}
