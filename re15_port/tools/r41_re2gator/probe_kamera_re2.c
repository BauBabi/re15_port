/* Nachtrag 4 (Spur B r41): Blickbeziehung der RE2-Gator-Schnitte in ROOM40A0.
 * Gator-Lage je Schnitt aus den RE2-Belegen (Intro: Pos_set sub11 @0x0736 (4557,-1800,-18237), yaw 0x400 aus
 * Sce_em_set @0x25A0, Wurzel(f) = Pos + R*off6[f] (FUN_80015e7c/152c8), Clip-6-Ende z = -21212 @0x801008d0;
 * R1: Bein 2 (x = -23078), Kanister (-22816,-9500) im Fenster 8601..9000 vor der Wurzel @0x80100ae4-ec;
 * FRESSEN P0: x = -23078 @0x80101048, Leon (x+10643, -915) @0x8010106c-84) und die RID-Saetze des RDT.
 * Kopf = Bone 6 (Kette 1->5->6), Weltlage wie gr2_bone_welt (Scale 1, RE2 zeichnet 1x). */
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re2_ems.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_camera.h"
#include "re15_actor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
static void off(int clip, int f, int32_t o[3]) { int16_t a[3]; re15_emd_get_keyframe_speed(&bk->skel, kf_of(clip, f), &a[0], &a[1], &a[2]); o[0]=a[0]; o[1]=a[1]; o[2]=a[2]; }
static double g_face[2];   /* Blickachse des Kopfes (Bone 1 -> Bone 6, xz, normiert) */
static void kopf(int clip, int f, int32_t ex, int32_t ey, int32_t ez, int yaw, int bone, int32_t out[3])
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    re15_skel_compute_pose(&bk->skel, kf_of(clip, f), poses);
    re15_skel_bone_to_world(poses[bone].trans, (int16_t)yaw, ex, ey, ez, out);
    int32_t b1[3]; re15_skel_bone_to_world(poses[1].trans, (int16_t)yaw, ex, ey, ez, b1);
    double ax = out[0] - b1[0], az = out[2] - b1[2], l = sqrt(ax*ax + az*az);
    g_face[0] = l > 0 ? ax / l : 0; g_face[1] = l > 0 ? az / l : 0;
}
static void rig(int yaw, const int32_t v[3], int32_t *dx, int32_t *dz)
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    *dx = (int32_t)(((int64_t)c * v[0] + (int64_t)s * v[2]) >> 12);
    *dz = (int32_t)((-(int64_t)s * v[0] + (int64_t)c * v[2]) >> 12);
}
static const re15_camera_cut_t *cuts; static int ncut;
static void bez(const char *was, int cut, const int32_t k[3], int yaw, const int32_t *zweit)
{
    re15_camera_view_t v; re15_camera_build_view(&cuts[cut], &v);
    int64_t px = k[0], py = k[1], pz = k[2];
    int32_t vx = (int32_t)((v.rot[0]*px + v.rot[1]*py + v.rot[2]*pz) >> 12) + v.trans[0];
    int32_t vy = (int32_t)((v.rot[3]*px + v.rot[4]*py + v.rot[5]*pz) >> 12) + v.trans[1];
    int32_t vz = (int32_t)((v.rot[6]*px + v.rot[7]*py + v.rot[8]*pz) >> 12) + v.trans[2];
    int sx = vz > 0 ? 160 + (int)((int64_t)vx * v.fov_screen_dist / vz) : -9999;
    int sy = vz > 0 ? 120 + (int)((int64_t)vy * v.fov_screen_dist / vz) : -9999;
    double fx = cos(yaw * 2 * M_PI / 4096.0), fz = -sin(yaw * 2 * M_PI / 4096.0);
    double cx = cuts[cut].pos_x - k[0], cz = cuts[cut].pos_z - k[2], d = sqrt(cx*cx + cz*cz);
    double fr = (fx * cx + fz * cz) / (d > 0 ? d : 1);
    double fk = (g_face[0] * cx + g_face[1] * cz) / (d > 0 ? d : 1);
    printf("%-34s cut %2d: Kopf=(%6d,%6d,%6d) yaw=%4d  Bild=(%5d,%5d) tiefe=%6d  dist_xz=%6.0f  frontal(yaw)=%+.2f  frontal(Kopfachse)=%+.2f (%3.0f Grad)",
           was, cut, k[0], k[1], k[2], yaw, sx, sy, vz, d, fr, fk, acos(fk > 1 ? 1 : fk < -1 ? -1 : fk) * 180 / M_PI);
    if (zweit) {
        int64_t qx = zweit[0], qy = zweit[1], qz = zweit[2];
        int32_t wx = (int32_t)((v.rot[0]*qx + v.rot[1]*qy + v.rot[2]*qz) >> 12) + v.trans[0];
        int32_t wy = (int32_t)((v.rot[3]*qx + v.rot[4]*qy + v.rot[5]*qz) >> 12) + v.trans[1];
        int32_t wz = (int32_t)((v.rot[6]*qx + v.rot[7]*qy + v.rot[8]*qz) >> 12) + v.trans[2];
        printf("  Leon-Bild=(%d,%d) tiefe=%d", wz > 0 ? 160 + (int)((int64_t)wx * v.fov_screen_dist / wz) : -9999,
               wz > 0 ? 120 + (int)((int64_t)wy * v.fov_screen_dist / wz) : -9999, wz);
    }
    printf("\n");
}
int main(int argc, char **argv)
{
    size_t n = 0, rn = 0;
    if (argc < 3) { printf("probe <CDEMD0.EMS> <ROOM40A0.RDT>\n"); return 1; }
    uint8_t *ems = slurp(argv[1], &n), *rdt = slurp(argv[2], &rn);
    if (!ems || !rdt) { printf("Dateien fehlen\n"); return 1; }
    re15_enemy_reset();
    bk = re15_enemy_alloc(0x23u);
    if (!bk || re2_ems_load_bank(ems, n, 0x23, bk, NULL) != 0) { printf("Bank\n"); return 1; }
    bk->ok = 1;
    ncut = rdt[1];
    uint32_t rid; memcpy(&rid, rdt + 8 + 4 * 7, 4);
    cuts = (const re15_camera_cut_t *)(rdt + rid);
    printf("RE2 ROOM40A0: %d Cuts, RID @0x%x; Clip 6 %d Bilder, Clip 3 %d, Clip 9 %d, Clip 4 %d, Clip 5 %d\n", ncut, rid,
           bk->anim.clips[6].frame_count, bk->anim.clips[3].frame_count, bk->anim.clips[9].frame_count,
           bk->anim.clips[4].frame_count, bk->anim.clips[5].frame_count);
    int32_t P[3] = { 4557, -1800, -18237 }, o[3], k[3], dx, dz; int yaw = 0x400;
    int f6[] = { 0, 30, 50, 69, 70, 100, 124 };
    for (unsigned i = 0; i < sizeof f6 / sizeof *f6; i++) {
        int f = f6[i]; off(6, f, o); rig(yaw, o, &dx, &dz);
        int32_t ex = P[0] + dx, ez = P[2] + dz;
        kopf(6, f, ex, P[1], ez, yaw, 6, k);
        char b[64]; snprintf(b, sizeof b, "Intro Clip6 f%d (Wurzel %d,%d)", f, ex, ez);
        if (f < 70) bez(b, 10, k, yaw, NULL); else if (f < 124) bez(b, 0, k, yaw, NULL); else bez(b, 1, k, yaw, NULL);
        if (f == 69 || f == 70) { bez("   (Vergleich Cut 10)", 10, k, yaw, NULL); bez("   (Vergleich Cut 0)", 0, k, yaw, NULL); bez("   (Vergleich Cut 1)", 1, k, yaw, NULL); }
    }
    /* Clip 3: Anker (x_ende, -21212) */
    off(6, bk->anim.clips[6].frame_count - 1, o); rig(yaw, o, &dx, &dz);
    int32_t ax = P[0] + dx, az = -21212;
    int f3[] = { 0, 40, 75, 110, 148, 149 };
    for (unsigned i = 0; i < sizeof f3 / sizeof *f3; i++) {
        int f = f3[i]; off(3, f, o); rig(yaw, o, &dx, &dz);
        int y2 = (f == 149) ? yaw + 1024 : yaw;
        int32_t ex = ax + dx, ez = az + dz;
        kopf(3, f, ex, P[1], ez, yaw, 6, k);
        char b[64]; snprintf(b, sizeof b, "Intro Clip3 f%d (Wurzel %d,%d)", f, ex, ez);
        bez(b, 1, k, y2, NULL);
    }
    /* R1: Bein 2 nach Norden, Wurzel z so, dass der Kanister (-22816,-9500) im Fenster liegt */
    {
        int y = 0xC00; int32_t ex = -23078, ez = -9500 - 8800;
        kopf(9, 0, ex, -1800, ez, y, 6, k);
        bez("R1 Clip9 f0 Bein2 Nord", 5, k, y, NULL);
        bez("   (Vergleich Cut 6)", 6, k, y, NULL);
        bez("   (Vergleich Cut 4)", 4, k, y, NULL);
        bez("   (Vergleich Cut 3)", 3, k, y, NULL);
        ez = -13302; kopf(9, 0, ex, -1800, ez, y, 6, k);
        bez("R1 Grenzfall z=-13302", 5, k, y, NULL);
        kopf(9, 36, ex, -1800, -18300, y, 6, k);
        bez("R1 Clip9 f36", 5, k, y, NULL);
    }
    /* FRESSEN P0: x = -23078, Leon (x+10643, -915), Gator nach Osten (yaw 0), Clip 4 f13 */
    {
        int y = 0; int32_t ex = -23078, ez = -915; int32_t leon[3] = { -23078 + 10643, -1800, -915 };
        kopf(4, 13, ex, -1800, ez, y, 6, k);
        bez("FRESSEN P0 Clip4 f13", 7, k, y, leon);
        bez("   (Vergleich Cut 9)", 9, k, y, leon);
        bez("   (Vergleich Cut 8)", 8, k, y, leon);
        bez("   (Vergleich Cut 6)", 6, k, y, leon);
        kopf(5, 60, ex, -1800, ez, y, 6, k);
        bez("FRESSEN P2 Clip5 f60", 7, k, y, leon);
    }
    for (int c = 0; c < ncut; c++)
        printf("RID %2d pos=(%6d,%6d,%6d) ziel=(%6d,%6d,%6d) fov=%u H=%d\n", c, cuts[c].pos_x, cuts[c].pos_y, cuts[c].pos_z,
               cuts[c].target_x, cuts[c].target_y, cuts[c].target_z, cuts[c].fov, cuts[c].fov >> 7);
    return 0;
}
