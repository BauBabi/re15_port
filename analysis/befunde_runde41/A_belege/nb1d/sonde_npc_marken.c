/* Sonde (Spur A, Nachbesserung 1, dritte Fortsetzung): Aufsetzer-Marken (Bildwort-Bit 0x4000) ALLER NPC-Typen 0x40..0x4f
 * in beiden Archiven, je Bank (Container dir[1], OWN +0x170/+0x174, LOCO +0x84/+0x16c): Clips mit Marke und ihre Bilder. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_roy_r37.h"
static uint8_t *slurp(const char *p, size_t *n){ FILE *f=fopen(p,"rb"); if(!f) return NULL; fseek(f,0,SEEK_END); long sz=ftell(f); fseek(f,0,SEEK_SET); uint8_t *b=malloc(sz); if(fread(b,1,sz,f)!=(size_t)sz){free(b);b=NULL;} fclose(f); if(b)*n=sz; return b; }
static int bank(const char *name, const re15_emd_animation_t *an)
{
    int clips_mit = 0;
    printf("  %-4s Clips %2d:", name, an->clip_count);
    for (int c = 0; c < an->clip_count; c++) {
        int fc = an->clips[c].frame_count, any = 0;
        for (int f = 0; f < fc; f++) {
            uint32_t w = re15_emd_get_frame_entry(an, c, f);
            if (w & 0x4000u) { if (!any) printf(" c%d(%d)[", c, fc); else printf(","); printf("%d:%x", f, (unsigned)(w >> 12)); any = 1; }
        }
        if (any) { printf("]"); clips_mit++; }
    }
    if (!clips_mit) printf(" KEINE Marke");
    printf("\n");
    return clips_mit;
}
int main(void)
{
    for (int arch = 0; arch < 2; arch++) {
        char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(arch));
        size_t n = 0; uint8_t *ems = slurp(p, &n);
        if (!ems) { printf("%s nicht lesbar\n", p); continue; }
        for (int t = 0x40; t <= 0x4f; t++) {
            uint32_t sek = 0, laen = 0;
            if (!re15_ems_toc(arch, (uint8_t)t, &sek, &laen)) continue;
            size_t off = (size_t)sek * 2048u;
            if (off + laen > n) { printf("%s Typ 0x%02x ausserhalb\n", re15_ems_archiv_datei(arch), t); continue; }
            uint8_t *blob = malloc(laen); memcpy(blob, ems + off, laen);
            static re15_md1_t md1; static re15_emd_skeleton_t sk, sko, skl; static re15_emd_animation_t an, ano, anl;
            re15_tim_t tim = (re15_tim_t){0};
            memset(&md1,0,sizeof md1); memset(&sk,0,sizeof sk); memset(&an,0,sizeof an);
            memset(&sko,0,sizeof sko); memset(&ano,0,sizeof ano); memset(&skl,0,sizeof skl); memset(&anl,0,sizeof anl);
            int ok = re15_emd_parse_container(blob, laen, &md1, &sk, &an, &tim) == 0;
            int own = re15_emd_parse_own_bank(blob, laen, &sko, &ano) == 0;
            int loco = re15_emd_parse_loco_bank(blob, laen, &skl, &anl) == 0;
            printf("%s Typ 0x%02x (Sektor %u, %u B): Container %s, OWN %s, LOCO %s\n", re15_ems_archiv_datei(arch), t, sek, laen,
                   ok ? "ok" : "-", own ? "ok" : "-", loco ? "ok" : "-");
            int m = 0;
            if (ok)   m += bank("CONT", &an);
            if (own)  m += bank("OWN", &ano);
            if (loco) m += bank("LOCO", &anl);
            printf("  => Typ 0x%02x %s: %s\n", t, re15_ems_archiv_datei(arch), m ? "MARKIERT" : "OHNE MARKE (PORT-WAHL)");
        }
    }
    return 0;
}
