/* Sonde (Spur A, Nachbesserung 1, dritte Fortsetzung): Roy (Typ 0x47, CDEMD1) OWN-Clip 5 ohne Marke - ist es ein
 * Geh-Zyklus? Je Bild: Keyframe, Wurzel (x,y,z) und Fusshoehen der Teile 4/7 (PSX-y: groesser = tiefer); zum Vergleich
 * Elliot (0x47, CDEMD0) OWN-Clip 5 (30 Bilder, Marken 7/22). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_roy_r37.h"
#include "re15_skeleton.h"
static uint8_t *slurp(const char *p, size_t *n){ FILE *f=fopen(p,"rb"); if(!f) return NULL; fseek(f,0,SEEK_END); long sz=ftell(f); fseek(f,0,SEEK_SET); uint8_t *b=malloc(sz); if(fread(b,1,sz,f)!=(size_t)sz){free(b);b=NULL;} fclose(f); if(b)*n=sz; return b; }
int main(void)
{
    for (int arch = 0; arch < 2; arch++) {
        char p[512]; snprintf(p, sizeof p, "%s/%s", RE15_ASSET_PSX_DIR, re15_ems_archiv_datei(arch));
        size_t n = 0; uint8_t *ems = slurp(p, &n);
        uint32_t sek = 0, laen = 0;
        if (!ems || !re15_ems_toc(arch, 0x47, &sek, &laen)) { printf("Typ 0x47 %d fehlt\n", arch); continue; }
        uint8_t *blob = malloc(laen); memcpy(blob, ems + (size_t)sek * 2048u, laen);
        static re15_emd_skeleton_t sko; static re15_emd_animation_t ano;
        memset(&sko, 0, sizeof sko); memset(&ano, 0, sizeof ano);
        if (re15_emd_parse_own_bank(blob, laen, &sko, &ano) != 0) { printf("OWN fehlt\n"); continue; }
        printf("=== %s Typ 0x47 OWN: Clips %d\n", re15_ems_archiv_datei(arch), ano.clip_count);
        for (int c = 0; c < ano.clip_count; c++) printf("  Clip %d: %d Bilder\n", c, ano.clips[c].frame_count);
        int c = 5, fc = ano.clips[c].frame_count;
        printf("  Clip 5 je Bild: kf | Wurzel x y z | Fuss y4 y7 | Wort>>12\n");
        for (int f = 0; f < fc; f++) {
            uint32_t w = re15_emd_get_frame_entry(&ano, c, f);
            int kf = (int)(w & 0xfffu); if (kf >= sko.keyframe_count) kf = sko.keyframe_count - 1;
            re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
            void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
            int ok = re15_skel_compute_pose(&sko, kf, poses);
            g_anim_pose_actor = save;
            if (ok == 0) printf("   %2d: %4d | %6d %6d %6d | %6d %6d | %x\n", f, kf, poses[0].trans[0], poses[0].trans[1], poses[0].trans[2],
                                poses[4].trans[1], poses[7].trans[1], (unsigned)(w >> 12));
        }
    }
    return 0;
}
