/* Runde 41 Spur G "kanister2080", Nachbesserung 1 (Abnahme 0, M1): JEDES Bild eines NPC-Clips (CDEMD0.EMS Typ 0x40 Marvin /
 * 0x49 John), nicht nur vier Stichbilder wie probe_npc_clips.c - die Stichbilder haben Loco c2 (= PL00-Klettern, Aufstieg)
 * als "Buecken + Aufheben" erscheinen lassen. Je Bild: Wurzel (Bone 0), Haende (Bone 11/14), Kopf (Bone 8) und die Fuesse
 * (Bone 4/7 = Enden der Beinketten 1->2->3->4 / 1->5->6->7, Elternliste der EMR) in Modellkoordinaten (+x vorn, y < 0 oben,
 * y 0 = Boden), Pose = re15_skel_compute_pose (dieselbe Rechnung wie der NPC-Zeichner).
 * Aufruf: probe_npc_clip_bilder <CDEMD0.EMS> <typ> <bank 0|1> <clip> */
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz; return b;
}
int main(int argc, char **argv)
{
    if (argc < 5) { fprintf(stderr, "Aufruf: %s CDEMD0.EMS typ bank clip\n", argv[0]); return 2; }
    size_t n = 0; uint8_t *ems = slurp(argv[1], &n); if (!ems) return 1;
    const int typ = (int)strtol(argv[2], NULL, 0), bank = atoi(argv[3]), clip = atoi(argv[4]);
    size_t off = 0, len = 0;
    if (re15_ems_get_entry(ems, n, re15_ems_index_for_type((uint8_t)typ), &off, &len) != 0) { fprintf(stderr, "Blob fehlt\n"); return 1; }
    re15_emd_skeleton_t sk; re15_emd_animation_t an;
    const int rc = bank ? re15_emd_parse_own_bank(ems + off, len, &sk, &an) : re15_emd_parse_loco_bank(ems + off, len, &sk, &an);
    if (rc != 0 || clip < 0 || clip >= an.clip_count) { fprintf(stderr, "Bank/Clip fehlt\n"); return 1; }
    const re15_emd_clip_t *c = &an.clips[clip];
    printf("Typ 0x%02X Bank %d (%s) Clip %d: %d Bilder, erstes Schluesselbild %d\n", typ, bank, bank ? "Eigen dir[3]/dir[4]" : "Loco dir[1]/dir[2]",
           clip, c->frame_count, (int)(an.frames[c->first_frame] & 0xfffu));
    printf(" Bild  kf   Wurzel(x,y)    HandR(x,y,z)        HandL(x,y,z)        Kopf-y  FussR-y FussL-y\n");
    for (int f = 0; f < c->frame_count; f++) {
        const int kf = (int)(an.frames[c->first_frame + f] & 0xfffu);
        re15_skel_pose_t p[RE15_EMD_MAX_BONES];
        if (re15_skel_compute_pose(&sk, kf, p) != 0) { printf(" %3d ?\n", f); continue; }
        printf(" %3d %4d (%5d,%5d) (%5d,%5d,%5d) (%5d,%5d,%5d) %6d %6d %6d\n", f, kf, p[0].trans[0], p[0].trans[1],
               p[11].trans[0], p[11].trans[1], p[11].trans[2], p[14].trans[0], p[14].trans[1], p[14].trans[2],
               p[8].trans[1], p[4].trans[1], p[7].trans[1]);
    }
    return 0;
}
