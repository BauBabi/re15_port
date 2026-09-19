/* GEGENPRUEFUNG anker-y Teil 3: welchen Keyframe posiert der RENDERER wirklich?
 * Die Ausgangs-Sonde rechnet in gator_kf() `anim_frame %% frame_count`. Der Renderer
 * ruft re15_compute_actor_kf (main.c:8210) und nimmt den Modulo NUR, wenn
 * clip_override >= 0 ist - und clip_override = (anim_flags & 0x04) ? motion : -1
 * (anim_select_common.c:280). Ohne das Bit laeuft HOLD-LAST (anim_select_common.c:101-112).
 * Kein add_test. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_ems.h"
#include "re15_emd.h"
#include "re15_enemy.h"
#include "re15_actor.h"
#include "re15_skeleton.h"
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif
int re15_compute_actor_kf(const re15_emd_animation_t*,const re15_emd_skeleton_t*,
                          const re15_actor_t*,int,uint32_t);
static uint8_t*slurp(const char*p,size_t*o){FILE*f=fopen(p,"rb");long s;uint8_t*b;
 if(!f)return NULL;fseek(f,0,SEEK_END);s=ftell(f);fseek(f,0,SEEK_SET);b=(uint8_t*)malloc((size_t)s);
 if(!b||fread(b,1,(size_t)s,f)!=(size_t)s){free(b);fclose(f);return NULL;}fclose(f);if(o)*o=(size_t)s;return b;}
int main(void){
 size_t sz=0;uint8_t*e=slurp(RE15_ASSET_RE2_DIR "/EM23.EMD",&sz);re15_enemy_bank_t B;re15_actor_t a;
 int mod_kf,fi,fe;
 if(!e){printf("EM23.EMD fehlt\n");return 1;}
 memset(&B,0,sizeof B);
 if(re2_emd_parse_bank(e,sz,&B)!=0){printf("parse fail\n");return 1;}
 memset(&a,0,sizeof a);a.type=0x23;a.motion=5;a.anim_frame=120;
 /* Modulo-Weg der Ausgangs-Sonde */
 {const re15_emd_clip_t*c=&B.anim.clips[5];int slot=120%c->frame_count;
  fi=c->first_frame+slot;fe=c->first_frame+c->frame_count-1;
  while((B.anim.frames[fi]&0x8000u)&&fi<fe)fi++;mod_kf=(int)(B.anim.frames[fi]&0xFFFu);}
 printf("Clip5 frame_count=%d\n",B.anim.clips[5].frame_count);
 printf("  Ausgangs-Sonde gator_kf() (120%%120=0)            -> kf %d\n",mod_kf);
 a.anim_flags=0x04;
 printf("  Renderer mit anim_flags 0x04 (clip_override=5)   -> kf %d\n",
   re15_compute_actor_kf(&B.anim,&B.skel,&a,(int)a.motion,120u));
 a.anim_flags=0x00;
 printf("  Renderer mit anim_flags 0    (clip_override=-1)  -> kf %d   <= Gator 0x23: KEIN\n",
   re15_compute_actor_kf(&B.anim,&B.skel,&a,-1,120u));
 printf("     anim_flags-Schreiber in enemy_ai_boss_gator.c (grep: 0 Treffer) -> Bit 0x04 bleibt 0\n");
 printf("\nP3-Pose (Clip11, anim_frame 0..29 < frame_count %d): beide Wege identisch\n",
        B.anim.clips[11].frame_count);
 {int f;for(f=0;f<30;f+=10){a.motion=11;a.anim_flags=0;
   printf("   f%-2d HOLD-LAST-Weg kf=%d | LOOP-Weg kf=%d\n",f,
     re15_compute_actor_kf(&B.anim,&B.skel,&a,-1,(uint32_t)f),
     re15_compute_actor_kf(&B.anim,&B.skel,&a,11,(uint32_t)f));}}
 free(e);return 0;}
