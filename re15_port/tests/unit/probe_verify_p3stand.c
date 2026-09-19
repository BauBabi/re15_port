/* GEGENPRUEFUNG anker-y Teil 2: der ECHTE "heutige P3-Stand".
 * Faehrt die Zustandsfolge aus enemy_ai_boss_gator.c GBP_FRESSEN literal nach:
 *   P1  timer..: anchor_set(Clip4,f13) bei Gator (0,-1200,0)         (:1939)
 *   P2  sf=0..120: Gator root_motion_abs(5,sf); Leon nur fuer sf<120 (:1970/1974)
 *   P3  timer 221..340: e->motion=11, anim_frame=(t-221)%30 - und der Gator
 *       bekommt KEINE erneute Platzierung (:1993-1999). Er behaelt also die
 *       Position aus dem LETZTEN P2-Aufruf (sf=120 -> 120%120=0 -> sx=2803 -> x=0),
 *       aber seine POSE ist Clip 11, nicht Clip 5.
 * Genau diese Trennung (Position aus Clip5-f0, Pose aus Clip11) fehlt in
 * probe_gator_fress_anker.c Variante (A). Kein add_test. */
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
static uint8_t *slurp(const char *p,size_t *o){FILE*f=fopen(p,"rb");long sz;uint8_t*b;
 if(!f)return NULL;fseek(f,0,SEEK_END);sz=ftell(f);fseek(f,0,SEEK_SET);b=(uint8_t*)malloc((size_t)sz);
 if(!b||fread(b,1,(size_t)sz,f)!=(size_t)sz){free(b);fclose(f);return NULL;}fclose(f);if(o)*o=(size_t)sz;return b;}
static re15_enemy_bank_t B;
static int kf_of(const re15_emd_animation_t *an,int clip,int fr){
 const re15_emd_clip_t*c=&an->clips[clip];int slot=fr%c->frame_count;
 int fi=c->first_frame+slot,fe=c->first_frame+c->frame_count-1;
 while((an->frames[fi]&0x8000u)&&fi<fe)fi++;return (int)(an->frames[fi]&0xFFFu);}
static void bone_matrix(const re15_actor_t*e,const re15_skel_pose_t*np,int32_t R[9],int32_t T[3]){
 int32_t fs=re15_sin_q12((int)e->rot_y),fc=re15_cos_q12((int)e->rot_y);int32_t n[9];int r,c,k;
 n[0]=fc;n[1]=0;n[2]=fs;n[3]=0;n[4]=0x1000;n[5]=0;n[6]=-fs;n[7]=0;n[8]=fc;
 if(e->render_scale_q12)for(k=0;k<9;k++)n[k]=(n[k]*(int32_t)e->render_scale_q12)>>12;
 for(r=0;r<3;r++)for(c=0;c<3;c++){int64_t s=0;for(k=0;k<3;k++)s+=(int64_t)n[r*3+k]*(int64_t)np->rot[k*3+c];R[r*3+c]=(int32_t)(s>>12);}
 for(r=0;r<3;r++){int64_t s=0;for(k=0;k<3;k++)s+=(int64_t)n[r*3+k]*(int64_t)np->trans[k];T[r]=(int32_t)(s>>12);}
 T[0]+=e->x;T[1]+=e->y;T[2]+=e->z;}
static double mindist(const re15_actor_t*e,int mesh,int kf,const int32_t p[3],int32_t lo[3],int32_t hi[3]){
 re15_skel_pose_t P[RE15_EMD_MAX_BONES];int32_t R[9],T[3];const re15_md1_mesh_t*m;int pass,i,r;double best=1e18;
 if(re15_skel_compute_pose(&B.skel,kf,P)!=0)return -1;if(mesh<0||mesh>=B.md1.mesh_count)return -2;
 bone_matrix(e,&P[mesh],R,T);m=&B.md1.meshes[mesh];
 lo[0]=lo[1]=lo[2]=0x7fffffff;hi[0]=hi[1]=hi[2]=-0x7fffffff;
 for(pass=0;pass<2;pass++){const re15_md1_vertex_t*V=pass?m->quad_vertices:m->tri_vertices;
  int NV=pass?m->quad_vertex_count:m->tri_vertex_count;
  for(i=0;i<NV;i++){int32_t v[3],w[3];double dx,dy,dz,d;v[0]=V[i].x;v[1]=V[i].y;v[2]=V[i].z;
   for(r=0;r<3;r++){int64_t s=0;int k;for(k=0;k<3;k++)s+=(int64_t)R[r*3+k]*(int64_t)v[k];w[r]=(int32_t)(s>>12)+T[r];}
   for(r=0;r<3;r++){if(w[r]<lo[r])lo[r]=w[r];if(w[r]>hi[r])hi[r]=w[r];}
   dx=(double)w[0]-p[0];dy=(double)w[1]-p[1];dz=(double)w[2]-p[2];d=sqrt(dx*dx+dy*dy+dz*dz);if(d<best)best=d;}}
 return best;}
static void leonwurzel(const re15_actor_t*pl,int32_t w[3],int *kfv){
 int k=kf_of(&B.anim_victim,(int)pl->motion,(int)pl->anim_frame);int16_t px=0,py=0,pz=0;int32_t tr[3];
 re15_emd_get_keyframe_position(&B.skel_victim,k,&px,&py,&pz);tr[0]=px;tr[1]=py;tr[2]=pz;
 re15_skel_bone_to_world(tr,(int16_t)pl->rot_y,pl->x,pl->y,pl->z,w);if(kfv)*kfv=k;}
static void row(const char*nm,const re15_actor_t*e,int posekf,const re15_actor_t*pl){
 int32_t w[3],lo[3],hi[3];int kfv=0;double d6,d7;
 leonwurzel(pl,w,&kfv);
 d6=mindist(e,6,posekf,w,lo,hi);d7=mindist(e,7,posekf,w,lo,hi);
 printf("  %-46s Gator(%d,%d,%d) poseKF=%4d | Leon(%d,%d,%d) kfv=%d -> W(%d,%d,%d) | Mesh6=%8.1f Mesh7=%8.1f\n",
   nm,(int)e->x,(int)e->y,(int)e->z,posekf,(int)pl->x,(int)pl->y,(int)pl->z,kfv,
   (int)w[0],(int)w[1],(int)w[2],d6,d7);}
int main(void){
 size_t sz=0;uint8_t*emd=slurp(RE15_ASSET_RE2_DIR "/EM23.EMD",&sz);
 re15_actor_t e,pl;int rc,sf,t;
 if(!emd){printf("EM23.EMD fehlt\n");return 1;}
 memset(&B,0,sizeof B);rc=re2_emd_parse_bank(emd,sz,&B);
 if(rc!=0){printf("parse=%d\n",rc);return 1;}B.ok=1;
 printf("EM23 clips=%d victim_ok=%d  Clip11 frames=%d  Clip5 frames=%d\n",
   B.anim.clip_count,B.victim_ok,B.anim.clips[11].frame_count,B.anim.clips[5].frame_count);

 memset(&e,0,sizeof e);memset(&pl,0,sizeof pl);
 e.type=0x23;e.rot_y=0;e.render_scale_q12=2731;e.x=0;e.y=-1200;e.z=0;
 pl.rot_y=0;pl.y=0;
 /* P1 @:1939 */
 e.motion=4;e.anim_frame=13;
 re15_clip_anchor_set_pub(&e,&B.skel,&B.anim,4,13);
 pl.anchor_x=e.anchor_x;pl.anchor_z=e.anchor_z;
 printf("Anker=(%d,%d)\n",(int)e.anchor_x,(int)e.anchor_z);
 /* P2 @:1970/1974 - die ECHTE Schleife, timer 100..220 */
 for(sf=0;sf<=120;sf++){
   e.motion=5;e.anim_frame=(uint32_t)sf;
   re15_clip_root_motion_abs_pub(&e,&B.skel,&B.anim,5,sf);
   if(sf<120){pl.rot_y=e.rot_y;pl.motion=1;pl.anim_frame=(uint32_t)sf;
     re15_clip_root_motion_abs_pub(&pl,&B.skel_victim,&B.anim_victim,1,sf);}
 }
 printf("\nNACH P2 (sf=120, letzter Aufruf): Gator=(%d,%d,%d) anim=Clip%d f%u | Leon=(%d,%d,%d) Clip%d f%u\n",
   (int)e.x,(int)e.y,(int)e.z,(int)e.motion,(unsigned)e.anim_frame,
   (int)pl.x,(int)pl.y,(int)pl.z,(int)pl.motion,(unsigned)pl.anim_frame);

 printf("\nWAS Variante (A) der Ausgangs-Sonde MISST (Pose Clip5 f0):\n");
 row("(A)  Leon y=0      Pose=Clip5 f0",&e,kf_of(&B.anim,5,0),&pl);
 {re15_actor_t p=pl;p.y=e.y;row("(A2) Leon y=-1200  Pose=Clip5 f0",&e,kf_of(&B.anim,5,0),&p);}

 printf("\nDER ECHTE P3-STAND (:1993-1999: motion=11, KEINE Neu-Platzierung des Gators):\n");
 for(t=221;t<=250;t+=1){
   int af=(t-221)%30;int pk=kf_of(&B.anim,11,af);char nm[80];
   re15_actor_t p2;
   /* Leon wird in P3 jeden Frame neu platziert (Clip1 f119) */
   pl.motion=1;pl.anim_frame=119;
   re15_clip_root_motion_abs_pub(&pl,&B.skel_victim,&B.anim_victim,1,119);
   snprintf(nm,sizeof nm,"P3 t=%d Clip11 f%-2d  Leon y=0",t,af);
   row(nm,&e,pk,&pl);
   p2=pl;p2.y=e.y;
   snprintf(nm,sizeof nm,"P3 t=%d Clip11 f%-2d  Leon y=-1200 (Y-Leitung)",t,af);
   row(nm,&e,pk,&p2);
 }
 free(emd);return 0;}
