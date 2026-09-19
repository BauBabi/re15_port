/* VERIFIKATIONS-SONDE (Gegenpruefung anker-y, 2026-09-14).
 * Misst die DRITTE Ankerkomponente y so, wie RE2 sie fuehrt:
 *   FUN_80015B94 @0x80015c54-64: anchor_y(+0x166) = entity_y(+0x3C) - off.y
 *   FUN_80015B94 @0x80015c88   : Kopie anchor_y -> Partner+0x166
 *   FUN_80015CB8 @0x80015d6c-7c: entity_y(+0x3C) = anchor_y(+0x166) + off.y
 *   Resolver FUN_80015db0 @0x80015e58-78: off = Keyframe +6/+8/+10 = SPEED-Feld
 * -> Leon_y = Gator_y - sy(Gator clip4 kf@f13) + sy(Opfer clip1 kf@f119)
 * Kein add_test. */
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

static uint8_t *slurp(const char *p, size_t *o)
{
    FILE *f = fopen(p, "rb"); long sz; uint8_t *b;
    if (!f) return NULL;
    fseek(f,0,SEEK_END); sz=ftell(f); fseek(f,0,SEEK_SET);
    b=(uint8_t*)malloc((size_t)sz);
    if(!b||fread(b,1,(size_t)sz,f)!=(size_t)sz){free(b);fclose(f);return NULL;}
    fclose(f); if(o)*o=(size_t)sz; return b;
}
static re15_enemy_bank_t s_bank;

static int kf_of(const re15_emd_animation_t *an, int clip, int fr)
{
    const re15_emd_clip_t *c = &an->clips[clip];
    int slot = fr % c->frame_count;
    int fi = c->first_frame + slot;
    int fend = c->first_frame + c->frame_count - 1;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(an->frames[fi] & 0xFFFu);
}
static void dump(const char *n, const re15_emd_skeleton_t *sk,
                 const re15_emd_animation_t *an, int clip, int fr)
{
    int kf = kf_of(an, clip, fr);
    int16_t sx=0,sy=0,sz=0,px=0,py=0,pz=0;
    re15_emd_get_keyframe_speed(sk, kf, &sx,&sy,&sz);
    re15_emd_get_keyframe_position(sk, kf, &px,&py,&pz);
    printf("  %-26s clip=%2d f=%3d kf=%4d  SPEED(+6/+8/+10)=(%6d,%6d,%6d)  POS(+0/+2/+4)=(%6d,%6d,%6d)\n",
           n, clip, fr, kf, (int)sx,(int)sy,(int)sz, (int)px,(int)py,(int)pz);
}

static void bone_matrix(const re15_actor_t *e, const re15_skel_pose_t *np,
                        int32_t R[9], int32_t T[3])
{
    int32_t fs = re15_sin_q12((int)e->rot_y), fc = re15_cos_q12((int)e->rot_y);
    int32_t nyaw[9]; int r,c,k;
    nyaw[0]=fc;nyaw[1]=0;nyaw[2]=fs; nyaw[3]=0;nyaw[4]=0x1000;nyaw[5]=0;
    nyaw[6]=-fs;nyaw[7]=0;nyaw[8]=fc;
    if (e->render_scale_q12) for(k=0;k<9;k++) nyaw[k]=(nyaw[k]*(int32_t)e->render_scale_q12)>>12;
    for(r=0;r<3;r++)for(c=0;c<3;c++){int64_t s=0;for(k=0;k<3;k++)s+=(int64_t)nyaw[r*3+k]*(int64_t)np->rot[k*3+c];R[r*3+c]=(int32_t)(s>>12);}
    for(r=0;r<3;r++){int64_t s=0;for(k=0;k<3;k++)s+=(int64_t)nyaw[r*3+k]*(int64_t)np->trans[k];T[r]=(int32_t)(s>>12);}
    T[0]+=e->x;T[1]+=e->y;T[2]+=e->z;
}
static double mindist(const re15_actor_t *e,int mesh,int kf,const int32_t p[3])
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES]; int32_t R[9],T[3];
    const re15_md1_mesh_t *m; int pass,i,r; double best=1e18;
    if (re15_skel_compute_pose(&s_bank.skel,kf,poses)!=0) return -1;
    if (mesh<0||mesh>=s_bank.md1.mesh_count) return -2;
    bone_matrix(e,&poses[mesh],R,T); m=&s_bank.md1.meshes[mesh];
    for(pass=0;pass<2;pass++){
        const re15_md1_vertex_t *V=pass?m->quad_vertices:m->tri_vertices;
        int NV=pass?m->quad_vertex_count:m->tri_vertex_count;
        for(i=0;i<NV;i++){int32_t v[3],w[3];double dx,dy,dz,d;
            v[0]=V[i].x;v[1]=V[i].y;v[2]=V[i].z;
            for(r=0;r<3;r++){int64_t s=0;int k;for(k=0;k<3;k++)s+=(int64_t)R[r*3+k]*(int64_t)v[k];w[r]=(int32_t)(s>>12)+T[r];}
            dx=(double)w[0]-p[0];dy=(double)w[1]-p[1];dz=(double)w[2]-p[2];
            d=sqrt(dx*dx+dy*dy+dz*dz); if(d<best)best=d;}
    }
    return best;
}
static int gkf(const re15_actor_t *e){ return kf_of(&s_bank.anim,(int)e->motion,(int)e->anim_frame); }

static void bericht(const char *nm, re15_actor_t *e, re15_actor_t *pl)
{
    int kf = gkf(e);
    int kfv = kf_of(&s_bank.anim_victim,(int)pl->motion,(int)pl->anim_frame);
    int16_t px=0,py=0,pz=0; int32_t tr[3],w[3];
    re15_emd_get_keyframe_position(&s_bank.skel_victim,kfv,&px,&py,&pz);
    tr[0]=px;tr[1]=py;tr[2]=pz;
    re15_skel_bone_to_world(tr,(int16_t)pl->rot_y,pl->x,pl->y,pl->z,w);
    printf("--- %s\n", nm);
    printf("    Gator pos=(%d,%d,%d) kf=%d | Leon pos=(%d,%d,%d) kfv=%d -> GERENDERT (%d,%d,%d)\n",
           (int)e->x,(int)e->y,(int)e->z,kf,(int)pl->x,(int)pl->y,(int)pl->z,kfv,
           (int)w[0],(int)w[1],(int)w[2]);
    printf("    Mesh6 minDist=%.1f   Mesh7 minDist=%.1f\n", mindist(e,6,kf,w), mindist(e,7,kf,w));
}

int main(void)
{
    size_t sz=0; uint8_t *emd=slurp(RE15_ASSET_RE2_DIR "/EM23.EMD",&sz);
    re15_actor_t e,pl; int rc;
    int16_t sgx=0,sgy=0,sgz=0,svx=0,svy=0,svz=0;
    int kf_g, kf_v;
    if(!emd){printf("EM23.EMD fehlt\n");return 1;}
    memset(&s_bank,0,sizeof s_bank);
    rc=re2_emd_parse_bank(emd,sz,&s_bank);
    printf("EM23.EMD %lu B parse=%d clips=%d victim_ok=%d victim_clips=%d meshes=%d bones=%d\n",
           (unsigned long)sz,rc,s_bank.anim.clip_count,s_bank.victim_ok,
           s_bank.anim_victim.clip_count,s_bank.md1.mesh_count,s_bank.skel.bone_count);
    if(rc!=0) return 1;
    s_bank.ok=1;
    printf("  kf_size gator=%d victim=%d  kf_count gator=%d victim=%d\n",
           s_bank.skel.keyframe_size_bytes, s_bank.skel_victim.keyframe_size_bytes,
           s_bank.skel.keyframe_count, s_bank.skel_victim.keyframe_count);

    printf("\nSPEED-FELDER (die RE2-Y-Leitung liest +8, FUN_80015db0 @0x80015e64):\n");
    dump("Gator Clip4 f13 (Anker)",&s_bank.skel,&s_bank.anim,4,13);
    dump("Gator Clip5 f0",&s_bank.skel,&s_bank.anim,5,0);
    dump("Gator Clip5 f119",&s_bank.skel,&s_bank.anim,5,119);
    dump("Gator Clip11 f0",&s_bank.skel,&s_bank.anim,11,0);
    dump("Opfer Clip1 f0",&s_bank.skel_victim,&s_bank.anim_victim,1,0);
    dump("Opfer Clip1 f60",&s_bank.skel_victim,&s_bank.anim_victim,1,60);
    dump("Opfer Clip1 f119",&s_bank.skel_victim,&s_bank.anim_victim,1,119);

    kf_g = kf_of(&s_bank.anim,4,13);
    kf_v = kf_of(&s_bank.anim_victim,1,119);
    re15_emd_get_keyframe_speed(&s_bank.skel,kf_g,&sgx,&sgy,&sgz);
    re15_emd_get_keyframe_speed(&s_bank.skel_victim,kf_v,&svx,&svy,&svz);
    printf("\nECHTE Y-LEITUNG (RE2-Formel):\n");
    printf("  anchor_y = Gator_y(-1200) - sy_g(%d) = %d\n", (int)sgy, -1200-(int)sgy);
    printf("  Leon_y   = anchor_y(%d) + sy_v(%d) = %d\n",
           -1200-(int)sgy, (int)svy, -1200-(int)sgy+(int)svy);
    printf("  Behauptung des Befunds: Leon_y = -1200 -> %s\n",
           (-1200-(int)sgy+(int)svy)==-1200 ? "STIMMT" : "FALSCH");

    /* Rahmen exakt wie die Ausgangs-Sonde */
    memset(&e,0,sizeof e); memset(&pl,0,sizeof pl);
    e.type=0x23; e.rot_y=0; e.render_scale_q12=2731;
    e.x=0; e.y=-1200; e.z=0; e.motion=4; e.anim_frame=13;
    re15_clip_anchor_set_pub(&e,&s_bank.skel,&s_bank.anim,4,13);
    pl.anchor_x=e.anchor_x; pl.anchor_z=e.anchor_z; pl.rot_y=e.rot_y;
    pl.motion=1; pl.anim_frame=119; pl.y=0;
    re15_clip_root_motion_abs_pub(&pl,&s_bank.skel_victim,&s_bank.anim_victim,1,119);
    e.motion=5; e.anim_frame=120;
    re15_clip_root_motion_abs_pub(&e,&s_bank.skel,&s_bank.anim,5,120);
    e.anim_frame=0;
    printf("\n");
    bericht("(A) heute, Leon y=0", &e, &pl);
    { re15_actor_t p=pl; p.y=e.y;
      bericht("(A2) Befunds-Modell: Leon y := Gator y (-1200)", &e, &p); }
    { re15_actor_t p=pl; p.y = (-1200-(int)sgy) + (int)svy;
      bericht("(A3) ECHTE RE2-Y-Leitung: y = (Gy - sy_g) + sy_v", &e, &p); }
    free(emd);
    return 0;
}
