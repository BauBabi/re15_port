/* Misst, ob die RE2-Kraehenbank (Typ 0x21) eine OPFER-Bank traegt — die Bedingung,
 * an der re15_re2z_victim_begin den Griff einrastet (enemy_ai_common.c:2372). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_enemy.h"
#include "re2_ems.h"
#define S(x) #x
#define X(x) S(x)
static uint8_t *rd(const char *p, size_t *n){FILE*f=fopen(p,"rb");if(!f)return NULL;
 fseek(f,0,SEEK_END);long s=ftell(f);fseek(f,0,SEEK_SET);uint8_t*b=malloc((size_t)s);
 if(fread(b,1,(size_t)s,f)!=(size_t)s){free(b);fclose(f);return NULL;}fclose(f);*n=(size_t)s;return b;}
int main(void){
    char p[600]; size_t n=0;
    snprintf(p,sizeof p,"%s/../RE2/CDEMD0.EMS", X(RE15_ASSETS_PATH));
    uint8_t *ems = rd(p,&n);
    if(!ems){ snprintf(p,sizeof p,"re15_port/shared_assets/RE2/CDEMD0.EMS"); ems=rd(p,&n); }
    if(!ems){ printf("CDEMD0.EMS nicht gefunden\n"); return 1; }
    printf("EMS %s (%zu Bytes)\n", p, n);
    for (int t = 0x20; t <= 0x22; t++) {
        static re15_enemy_bank_t eb; re15_tim_t tim = {0};
        memset(&eb,0,sizeof eb);
        int r = re2_ems_load_bank(ems, n, t, &eb, &tim);
        printf("  Typ 0x%02X: load=%d  loco_ok=%d own_ok=%d VICTIM_OK=%d\n",
               t, r, eb.loco_ok, eb.own_ok, eb.victim_ok);
    }
    return 0;
}
