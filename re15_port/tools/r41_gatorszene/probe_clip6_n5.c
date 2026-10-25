/* Runde 41 Spur F "gatorszene", Nachbesserung 5: Clip 6 (Durchbruch, RE2 Intro P1 @0x801008ac-b8) des RE2-EM23 je Bild -
 * kumulierter Wurzelversatz (EMR-Keyframe-Bytes 6..11 roh, FUN_80015e7c), Kopf (Bone 6) im Rig (roh), vorderster Knochen, Rest-Weg
 * ab Bild f mit dem Port-Massstab 2731/4096 (bildweise Schritte wie gatorszene_r41.c ga_schritt). Belegt GSZ_DURCHBRUCH_BIS:
 * Bild 0..15 steigt der Kopf (Rig-y +1611 -> -1392), Bild 15..37 der Vorstoss, danach Ausgleiten.
 * Bau wie probe_re2_band_n5.c; Aufruf: probe_clip6_n5 <shared_assets/RE2/CDEMD0.EMS> [clip] > clip6.txt
 * (leiche_lage_n5.py --kandidaten liest die Datei). */
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re2_ems.h"
#include "re15_skeleton.h"
#include "re15_actor.h"
#include "re15_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz; return b;
}
static re15_enemy_bank_t *bk;
static int kf_of(int clip, int frame)
{
    const re15_emd_animation_t *an = &bk->anim;
    const re15_emd_clip_t *c = &an->clips[clip];
    int fi = c->first_frame + frame, fend = c->first_frame + c->frame_count;
    while ((an->frames[fi] & 0x8000u) && fi + 1 < fend) fi++;
    return (int)(an->frames[fi] & 0xfffu);
}
static void off(int clip, int f, int32_t o[3])
{
    int16_t a[3]; re15_emd_get_keyframe_speed(&bk->skel, kf_of(clip, f), &a[0], &a[1], &a[2]);
    o[0] = a[0]; o[1] = a[1]; o[2] = a[2];
}
int main(int argc, char **argv)
{
    size_t n = 0;
    uint8_t *ems = slurp(argv[1], &n);
    re15_enemy_reset();
    bk = re15_enemy_alloc(0x23u);
    if (!bk || re2_ems_load_bank(ems, n, 0x23, bk, NULL) != 0) { printf("Bank fehlt\n"); return 1; }
    bk->ok = 1;
    int clip = argc > 2 ? atoi(argv[2]) : 6;
    const int nf = bk->anim.clips[clip].frame_count;
    int32_t o[400][3];
    for (int f = 0; f < nf; f++) off(clip, f, o[f]);
    for (int f = 0; f < nf; f++) {
        re15_skel_pose_t p[RE15_EMD_MAX_BONES];
        re15_skel_compute_pose(&bk->skel, kf_of(clip, f), p);
        double vorn = -1e9; int vb = -1;
        for (int b = 0; b < bk->skel.bone_count; b++) if (p[b].trans[0] > vorn) { vorn = p[b].trans[0]; vb = b; }
        /* Rest-Weg ab f (Schritte f+1..nf-2, Port 2/3) */
        double rx = 0, rz = 0;
        for (int g = f + 1; g < nf - 1; g++) { rx += (o[g][0] - o[g - 1][0]) * 2731.0 / 4096.0; rz += (o[g][2] - o[g - 1][2]) * 2731.0 / 4096.0; }
        printf("f %3d off (%6d,%6d) kopf6 rig (%6d,%6d,%6d) vorn bone %2d x %6.0f | Wurzel+Kopf roh %6.0f | Rest ab f (2/3) (%7.0f,%6.0f)\n",
               f, o[f][0], o[f][2], p[6].trans[0], p[6].trans[1], p[6].trans[2], vb, vorn, o[f][0] + (double)p[6].trans[0], rx, rz);
    }
    return 0;
}
