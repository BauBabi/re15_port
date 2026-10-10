/* Nachbesserung 1 Spur C: Wurzel-Speeds (e7c-Quelle, FUN_80015E7C) der Stagger-Clips 3/4 der RE2-Bank 0x10. */
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re2_ems.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
extern re15_enemy_bank_t *re15_enemy_alloc(uint8_t type);
static uint8_t *slurp(const char *p, size_t *n){FILE*f=fopen(p,"rb");if(!f)return NULL;fseek(f,0,SEEK_END);long sz=ftell(f);fseek(f,0,SEEK_SET);uint8_t*b=malloc(sz);fread(b,1,sz,f);fclose(f);*n=sz;return b;}
int main(void){
  size_t n=0; uint8_t *ems=slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS",&n); if(!ems){puts("FAIL ems");return 1;}
  re15_enemy_reset();
  re15_enemy_bank_t *eb=re15_enemy_alloc(0x10);
  if(re2_ems_load_bank(ems,n,0x10,eb,NULL)!=0){puts("FAIL bank");return 1;} eb->ok=1;
  int clips[]={3,4,21,8,9};
  for(int ci=0;ci<5;ci++){int c=clips[ci]; if(c>=eb->anim.clip_count)continue; const re15_emd_clip_t*cl=&eb->anim.clips[c];
    printf("clip %d frames %d:",c,cl->frame_count);
    for(int f=0;f<cl->frame_count;f++){int kf=(int)(eb->anim.frames[cl->first_frame+f]&0xFFFu); int16_t sx,sy,sz; if(!re15_emd_get_keyframe_speed(&eb->skel,kf,&sx,&sy,&sz)){printf(" [%d:-]",f);continue;} printf(" [%d:%d,%d,%d]",f,sx,sy,sz);} printf("\n");}
  return 0; }
