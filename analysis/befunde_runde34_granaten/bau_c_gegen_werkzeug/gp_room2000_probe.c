/* Uebersetzen (Git-Bash, gegen das Bauverzeichnis des Arbeitsbaums; KEIN Projekt-Bau):
 *   WT=/c/workspace/git/reAi_v2/.claude/worktrees/r34g_c/re15_port
 *   PATH=/c/msys64/mingw64/bin:$PATH gcc -std=c11 -DRE15_PLATFORM_PC=1 -I$WT/include -I$WT/platform/pc/src \
 *     gp_room2000_probe.c $WT/platform/pc/src/fx_plattform_pc.c -L$WT/build_r34_c/tests -L$WT/build_r34_c/engine \
 *     -lre15_test_support -lre15_engine -lre15_test_support -lm -o gp_room2000_probe.exe
 * Ergebnis 2026-09-30 (Stand 75d73484): 6 Plaetze, A=41 cursor 0 flags 03 t=0..27, defW neu 1 / master 4096. */
/* Gegenpruefung r34g/c: ROOM2000 main00 Sce_espr_on id 0x0b sub 0 (x3 @0x174C/5C/6C) -> was zeichnet
 * C2 (re15_pc_esp_defwh) gegen den master-Stand (defW/defH nur bei Routine 17/18, sonst 0x1000)? */
#include "fx_plattform_pc.h"
#include "re15_esp.h"
#include "re15_scd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint8_t *slurp(const char *p, size_t *n){FILE*f=fopen(p,"rb");if(!f)return NULL;fseek(f,0,SEEK_END);long s=ftell(f);fseek(f,0,SEEK_SET);uint8_t*b=malloc(s);fread(b,1,s,f);fclose(f);*n=s;return b;}
static uint32_t u32(const uint8_t*p){return p[0]|(p[1]<<8)|(p[2]<<16)|((uint32_t)p[3]<<24);}
void re15_audio_arms_zusatz_se(int a, int s){(void)a;(void)s;}
int main(void){
  const char *A = "C:/workspace/git/reAi_v2/.claude/worktrees/r34g_c/re15_port/shared_assets/PSX";
  char p[512]; size_t n=0, nc=0;
  snprintf(p,sizeof p,"%s/DATA/CORE00.ESP",A); uint8_t *core=slurp(p,&nc);
  static re15_esp_t g, r; re15_esp_parse_global(core,nc,&g); re15_esp_set_global_bank(&g);
  snprintf(p,sizeof p,"%s/STAGE2/ROOM2000.RDT",A); uint8_t *rdt=slurp(p,&n);
  int rc = re15_esp_parse(rdt,n,u32(rdt+0x4c),u32(rdt+0x50),u32(rdt+0x54),u32(rdt+0x58),&r);
  re15_esp_set_room_bank(&r);
  printf("ROOM2000 ESP rc=%d ids=%d\n", rc, r.id_count);
  re15_esp_fx_reset(); g_re15_pauseflags = 0;
  /* Sce_espr_on 3a 00 0b 00 00 00 00 19 a4 a2 4c eb c0 2b 00 04 (ROOM2000 main00 +0x12E) */
  int s = re15_esp_fx_spawn_rows(&r, 0x0b, 0, 0x1900, -23900, -5300, 11200, -5300, 0x400);
  printf("spawn_rows -> %d Plaetze\n", s);
  for (int t = 0; t <= 400; t++) {
    if (1) {
      for (int i = 0; i < RE15_ESP_FX_MAX; i++) {
        const re15_esp_fx_t *f = re15_esp_fx_get(i); if (!f) continue;
        int32_t w=0,h=0; re15_pc_esp_defwh(f,&w,&h);
        unsigned selA = f->row[0] | (f->row[1]<<8);
        int master_w = (selA==17||selA==18) ? (f->row[4]|(f->row[5]<<8)) : 0x1000;
        printf("  t=%2d slot %2d id=%02x A=%u cursor=%d flags=%02x sichtbar(neu)=%d sichtbar(master)=%d defW neu=%d master=%d\n",
               t, i, f->effect_id, selA, f->row_cursor, f->flags, re15_pc_esp_sichtbar(f), re15_esp_fx_visible(f), w, master_w);
      }
    }
    re15_esp_fx_tick(&r);
  }
  return 0;
}
