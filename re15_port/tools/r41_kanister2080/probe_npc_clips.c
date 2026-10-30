/* Runde 41 Spur G "kanister2080": Clip-Zensus eines NPC (CDEMD0.EMS Typ 0x40 Marvin / 0x49 John) - je Bank (0 = Loco
 * dir[1]/dir[2], 1 = Eigenbank dir[3]/dir[4], die Sub 2/4/5/6/9 spielen, re15_emd.h re15_emd_parse_own_bank) und Clip
 * die Lage der Haende (Bone 11 / Bone 14 = Enden der Armketten 0->9->10->11 und 0->12->13->14; 1->2->3->4 und 1->5->6->7 sind die Beine, Elternliste der EMR), des Kopfes
 * (Bone 8) und der Wurzel in Modellkoordinaten (Pose = re15_skel_compute_pose, dieselbe Rechnung wie der NPC-Zeichner).
 * Zweck: "beide Haende vorn zusammen" (Tragen) und "Haende von unten/hinten nach vorn oben" (Werfen) messbar machen.
 * Aufruf: probe_npc_clips <CDEMD0.EMS> <typ> */
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz; return b;
}
static int kf_of(const re15_emd_animation_t *an, int clip, int f)
{
    const re15_emd_clip_t *c = &an->clips[clip];
    int fi = c->first_frame + f;
    return (int)(an->frames[fi] & 0xfffu);
}
static void bank(const char *nm, const re15_emd_skeleton_t *sk, const re15_emd_animation_t *an)
{
    printf("== %s: %d Clips, %d Bones\n", nm, an->clip_count, sk->bone_count);
    for (int c = 0; c < an->clip_count; c++) {
        int n = an->clips[c].frame_count;
        if (n <= 0) continue;
        printf("  c%-2d %3df:", c, n);
        int fs[4] = { 0, n / 3, (2 * n) / 3, n - 1 };
        for (int k = 0; k < 4; k++) {
            re15_skel_pose_t p[RE15_EMD_MAX_BONES];
            if (re15_skel_compute_pose(sk, kf_of(an, c, fs[k]), p) != 0) { printf(" ?"); continue; }
            const int32_t *r = p[0].trans, *h4 = p[11].trans, *h7 = p[14].trans, *kp = p[8].trans;
            double dh = sqrt((double)(h4[0]-h7[0])*(h4[0]-h7[0]) + (double)(h4[1]-h7[1])*(h4[1]-h7[1]) + (double)(h4[2]-h7[2])*(h4[2]-h7[2]));
            printf(" |f%-2d W(%5d,%5d) HR(%5d,%5d,%5d) HL(%5d,%5d,%5d) d%4.0f K(%5d)", fs[k], r[0], r[1],
                   h4[0], h4[1], h4[2], h7[0], h7[1], h7[2], dh, kp[1]);
        }
        printf("\n");
    }
}
int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "Aufruf: %s CDEMD0.EMS typ\n", argv[0]); return 2; }
    size_t n = 0; uint8_t *ems = slurp(argv[1], &n); if (!ems) return 1;
    int typ = (int)strtol(argv[2], NULL, 0);
    size_t off = 0, len = 0;
    if (re15_ems_get_entry(ems, n, re15_ems_index_for_type((uint8_t)typ), &off, &len) != 0) { fprintf(stderr, "Blob fehlt\n"); return 1; }
    const uint8_t *emd = ems + off;
    re15_emd_skeleton_t sk0, sk1; re15_emd_animation_t an0, an1;
    if (re15_emd_parse_loco_bank(emd, len, &sk0, &an0) == 0) bank("Bank 0 (Loco dir[1]/dir[2])", &sk0, &an0);
    if (re15_emd_parse_own_bank(emd, len, &sk1, &an1) == 0) bank("Bank 1 (Eigen dir[3]/dir[4])", &sk1, &an1);
    printf("Elternliste:"); for (int b = 0; b < sk0.bone_count; b++) printf(" %d", (int)sk0.bone_parent[b]); printf("\n");
    return 0;
}
