/* Runde 41 Spur F "gatorszene", Nachbesserung 4 (Nachtrag 29): RE2-Sollwert "Abstand und Bilder bis zum ersten Angriff am Ende
 * des Alligator-Intros in ROOM40A0". Rechnet die RE2-Lagen aus den Belegen (keine Zahl nach Gefuehl):
 *   Gator:  ROOM40A0 sub11 `Pos_set 32 00 cd 11 f8 f8 c3 b8` @SCD-main 0x0736 = (4557,-1800,-18237), Gier 0x400 aus
 *           Sce_em_set @RDT 0x25A0; Intro P1 Clip 6 (FUN_8001a330(em,6,0,0) @0x801008ac-b8) bis zum Clip-Ende, dann
 *           z = -21212 (`addiu v1,zero,-21212 / sw v1,64(s0)` @0x801008d0-d4); P2 Clip 3 (@0x801008e4-f0) bis zum Ende, dann
 *           +0x238 = 110 (@0x8010091c-20), Routine 0x101 (@0x80100924-2c), Gier += 1024 (@0x80100938-48).
 *           Wurzelschritt FUN_80015e7c (RE2_Quellcode_V2/FUN_80015e7c.c): Bild 0 eines Clips (+0x14D == 0) setzt den
 *           Vorgaenger-Versatz auf 0, d.h. der erste Schritt ist off[0]; danach off[f] - off[f-1]. Drehung FUN_800152c8
 *           (RotMatrix Y = Gier, x += c*vx + s*vz, z += -s*vx + c*vz).
 *   Nach dem Intro: Sub 1 SCHWIMMEN = Clip 0 Blend 32 (@0x80100c34-40); Sub 1 -> Sub 2 nur bei +0x238 == 0 (@0x80100b5c-64)
 *           und +0x1F0 >= 0x65 (@0x80100b88/@0x80100ba4; +0x1F0 = SquareRoot0(dx*dx + dz*dz) @0x800265a4-e0 in Welteinheiten),
 *           +0x238 zaehlt je Takt ab (@0x801004a8-b8); Sub 2 PADDELN = Clip 1 (@0x80100cf8-d04), Clip-Ende -> Neustart
 *           (@0x80100d08-10). Der Koerper wird nie gelenkt (Root ruft nur FUN_80017d98 fuer den Hals, @0x8010036c-74): der
 *           Gator schwimmt geradeaus mit Gier 2048.
 *   Leon:   ROOM40A0 sub03 `Pos_set 32 00 80 da f8 f8 38 9b` @SCD-main 0x0250 = (-9600,-1800,-25800), danach (Warten auf
 *           Cut 1 @0x025E-6A) `Plc_dest 40 00 08 0a d0 d5 38 9b` @0x0286 -> (-10800,-25800), `Plc_dest 40 00 15 0a 00 08 80 00`
 *           @0x029E (Drehung auf 0x800), `Plc_ret` @0x02B6, Riegel auf `Set 22 02 07 00` @0x02B7.
 * Ausgabe: Wurzel/Kopf (Bone 6)/vorderster Knochen am Intro-Ende, Abstaende zu Leon, und je Bild danach die Lage, bis der
 * vorderste Knochen Leons x erreicht (Leon steht). Bau: gcc -std=c11 -DRE15_PLATFORM_PC=1 -I<port>/include ... gegen
 * libre15_engine.a + libre15_test_support.a (wie tools/r41_re2gator/probe_kamera_re2.c).
 *
 * Aufruf: probe_re2_intro_ende <CDEMD0.EMS> */
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
/* FUN_800152c8: x += c*vx + s*vz, z += -s*vx + c*vz (RotMatrix Y) */
static void schritt(int32_t *x, int32_t *z, int yaw, const int32_t d[3])
{
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    *x += (int32_t)(((int64_t)c * d[0] + (int64_t)s * d[2]) >> 12);
    *z += (int32_t)((-(int64_t)s * d[0] + (int64_t)c * d[2]) >> 12);
}
/* ein Clip-Bild wie FUN_8001a330 (RE2 PSX.EXE): Geschwindigkeit des Bildes f (FUN_80015e7c @0x8001a374; Bild 0: off[0] - 0),
 * Bildschritt FUN_8002959c (@0x8001a3a8); am Clip-Ende (`bne v0,zero,0x8001a3cc` @0x8001a3b0) wird der Schritt NICHT
 * angewandt, sonst FUN_800152c8 (@0x8001a3c0). */
static void clip_bild(int clip, int f, int32_t *x, int32_t *z, int yaw)
{
    int32_t a[3] = { 0, 0, 0 }, b[3], d[3];
    if (f >= bk->anim.clips[clip].frame_count - 1) return;           /* Clip-Ende: kein Schritt */
    off(clip, f, b);
    if (f > 0) off(clip, f - 1, a);
    d[0] = b[0] - a[0]; d[1] = 0; d[2] = b[2] - a[2];
    schritt(x, z, yaw, d);
}
static int g_nb;
/* Weltlage aller Knochen einer Pose; vorderster = groesstes Skalarprodukt mit der Richtung (fx,fz) */
static void knochen(int clip, int f, int32_t x, int32_t z, int yaw, int32_t kopf[3], int32_t vorn[3], double fx, double fz)
{
    re15_skel_pose_t p[RE15_EMD_MAX_BONES];
    re15_skel_compute_pose(&bk->skel, kf_of(clip, f), p);
    double best = -1e18;
    for (int b = 0; b < g_nb; b++) {
        int32_t w[3]; re15_skel_bone_to_world(p[b].trans, (int16_t)yaw, x, -1800, z, w);
        if (b == 6) memcpy(kopf, w, sizeof w);
        double s = fx * w[0] + fz * w[2];
        if (s > best) { best = s; memcpy(vorn, w, sizeof w); }
    }
}
int main(int argc, char **argv)
{
    size_t n = 0;
    if (argc < 2) { printf("probe_re2_intro_ende <CDEMD0.EMS>\n"); return 1; }
    uint8_t *ems = slurp(argv[1], &n);
    if (!ems) { printf("CDEMD0.EMS fehlt\n"); return 1; }
    re15_enemy_reset();
    bk = re15_enemy_alloc(0x23u);
    if (!bk || re2_ems_load_bank(ems, n, 0x23, bk, NULL) != 0) { printf("Bank 0x23 nicht ladbar\n"); return 1; }
    bk->ok = 1;
    g_nb = bk->skel.bone_count;
    printf("EM23: %d Knochen; Clip 6 %d Bilder, Clip 3 %d, Clip 0 %d, Clip 1 %d\n", g_nb, bk->anim.clips[6].frame_count,
           bk->anim.clips[3].frame_count, bk->anim.clips[0].frame_count, bk->anim.clips[1].frame_count);
    int32_t x = 4557, z = -18237; int yaw = 0x400;                     /* Pos_set @SCD 0x0736, Gier Sce_em_set @0x25A0 */
    const int n6 = bk->anim.clips[6].frame_count, n3 = bk->anim.clips[3].frame_count;
    for (int f = 0; f < n6; f++) clip_bild(6, f, &x, &z, yaw);
    printf("Clip-6-Ende: Wurzel (%d,%d) -> z = -21212 (@0x801008d0-d4)\n", x, z);
    z = -21212;
    for (int f = 0; f < n3; f++) clip_bild(3, f, &x, &z, yaw);
    const int32_t leon_x = -10800, leon_z = -25800;                     /* sub03 Plc_dest @0x0286 */
    int32_t kopf[3], vorn[3];
    knochen(3, n3 - 1, x, z, yaw, kopf, vorn, -1.0, 0.0);
    printf("Clip-3-Ende (Intro P2 Ende, Gier %d -> %d): Wurzel (%d,%d), Kopf Bone 6 (%d,%d), vorderster Knochen (x min) (%d,%d)\n",
           yaw, (yaw + 1024) & 0xfff, x, z, kopf[0], kopf[2], vorn[0], vorn[2]);
    yaw = (yaw + 1024) & 0xfff;                                         /* @0x80100938-48 */
    double dw = hypot(x - leon_x, z - leon_z), dk = hypot(kopf[0] - leon_x, kopf[2] - leon_z);
    printf("Leon (Plc_dest @0x0286) (%d,%d): Abstand Wurzel %.0f, Kopf %.0f, vorderster Knochen %.0f; Laengsabstand Wurzel %d, "
           "Querabstand %d\n", leon_x, leon_z, dw, dk, hypot(vorn[0] - leon_x, vorn[2] - leon_z), x - leon_x, leon_z - z);
    /* nach dem Intro: Sub 1 Clip 0 (110 Takte Sperre), dann Sub 2 Clip 1; Leon steht */
    int sperre = 110, sub = 1, f0 = 0, f1 = 0, gemeldet = 0;
    const int nc0 = bk->anim.clips[0].frame_count, nc1 = bk->anim.clips[1].frame_count;
    for (int t = 1; t <= 400; t++) {
        int clip, fr;
        if (sub == 1) { clip = 0; fr = f0; clip_bild(0, f0, &x, &z, yaw); f0 = (f0 + 1) % nc0; }
        else          { clip = 1; fr = f1; clip_bild(1, f1, &x, &z, yaw); f1 = (f1 + 1) % nc1; }
        knochen(clip, fr, x, z, yaw, kopf, vorn, -1.0, 0.0);
        if (sperre > 0) sperre--;                                       /* @0x801004a8-b8 (Ende des Root-Takts) */
        double d = hypot(x - leon_x, z - leon_z);
        if (sub == 1 && sperre == 0 && d >= 0x65) sub = 2;              /* @0x80100b5c-64, @0x80100b88 */
        if (t == 1 || t % 10 == 0 || (!gemeldet && vorn[0] <= leon_x))
            printf("  t=%3d Sub %d Clip %d Bild %3d: Wurzel (%6d,%6d) Kopf (%6d,%6d) vorn x %6d | Wurzel-Leon %6.0f Kopf-Leon %6.0f\n",
                   t, sub, clip, fr, x, z, kopf[0], kopf[2], vorn[0], d, hypot(kopf[0] - leon_x, kopf[2] - leon_z));
        if (!gemeldet && vorn[0] <= leon_x) {
            printf("ERSTER MOEGLICHER KONTAKT (vorderster Knochen auf Leons x) nach %d Takten, Querabstand Kopf-Leon %d\n",
                   t, leon_z - kopf[2]);
            gemeldet = 1;
        }
        if (gemeldet && x <= leon_x) { printf("Wurzel auf Leons x nach %d Takten\n", t); break; }
    }
    return 0;
}
