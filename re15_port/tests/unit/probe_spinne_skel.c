/* probe_spinne_skel.c — MESSSONDE (kein ctest): EM025-Skelett (Eltern, Bind-Versatz) und Cliplaengen. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_emd.h"
#include "re15_ems.h"
int main(void)
{
    FILE *f = fopen(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", "rb"); if (!f) return 1;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    unsigned char *d = malloc((size_t)n); if (fread(d, 1, (size_t)n, f) != (size_t)n) return 1; fclose(f);
    size_t off = 0, len = 0; re15_ems_get_entry(d, (size_t)n, re15_ems_index_for_type(0x25), &off, &len);
    static re15_md1_t md1; static re15_emd_skeleton_t sk; static re15_emd_animation_t an; re15_tim_t tim;
    memset(&tim, 0, sizeof tim);
    re15_emd_parse_container(d + off, len, &md1, &sk, &an, &tim);
    for (int i = 0; i < sk.bone_count; i++)
        printf("bone %d parent %d rel %d %d %d\n", i, sk.bone_parent[i], sk.bone_relative_pos[i][0], sk.bone_relative_pos[i][1], sk.bone_relative_pos[i][2]);
    for (int c = 0; c < an.clip_count; c++) printf("clip %d frames %d\n", c, an.clips[c].frame_count);
    return 0;
}
