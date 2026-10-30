/* Runde 41 Spur F "gatorszene", Nachbesserung 5 (Abnahme 4 M1/M4 = Entscheid F-E2): RE2-SOLLBAND des Kampfbeginns in ROOM40A0
 * im SELBEN Mass wie der Port. Erweitert tools/r41_gatorszene/probe_re2_intro_ende.c (Rechnung und Adressen dort) um:
 *   (1) KONTAKT-MASS (gleich auf beiden Seiten): erstes Bild, in dem ein Knochen des Gators die Spielfigur LAENGS der Schwimm-
 *       richtung erreicht: max_b <P_b - L, h> >= 0, h = Vorwaerts der Gier (cos, -sin) (FUN_800152c8-Konvention, Gier 0 = +X).
 *       RE2 schwimmt nach dem Intro geradeaus mit Gier 2048 (Koerper nie gelenkt, Root nur Hals FUN_80017d98 @0x8010036c-74):
 *       h = (-1, 0) -> das Mass ist dort GENAU "vorderster Knochen auf Leons x" der Nachbesserung 4 (121 Bilder).
 *   (2) ZONENBEZIEHUNG: Zonenbyte +0x21A (538) = 0 aus INIT (`sb zero,538(s0)` @0x80100654), Wechsel nur im Sub-1-Pre:
 *       0 -> 1 bei x < -17877 (`lw v0,56(s0) / slti v0,v0,-17877` @0x80100990-98, `sb v0,538(s0)` @0x801009a8, Routine 0x301 =
 *       LUNGE @0x801009ac-b0), 1 -> 2 bei z >= -7322 (@0x801009d8-ec). Die Pruefung liest NUR die Gator-Lage (+0x38/+0x40),
 *       keine Spielerlage. Ausgabe: Zone beim Kontakt und ob vor dem Kontakt ein Zonenwechsel (= Dash) liegt.
 *   (3) FENSTER DES KAMPFBEGINNS fuer Leon: Ende der Zwischensequenz = sub03 `Plc_ret 42` @SCD 0x02B6 + `Set 22 02 07 00`
 *       @0x02B7 (Riegel auf) - dasselbe Ereignis wie der Port-Kampfbeginn (Programm 3: Plc_ret + Riegel auf, Folge C). Fruehestens
 *       nach Cut 1: Warten auf Cut 1 `While .. Cmp 23 00 1a 05 01 00` @0x025E-6A (>= 1 Bild), `Sleep 09 0a 3c 00` @0x0276 (60),
 *       `Sleep 09 0a 32 00` @0x027E (50), `Plc_dest` @0x0286 + `While .. Ck 21 05 0a 00` @0x028E-98 (>= 1 Bild), `Plc_dest`
 *       @0x029E + `While` @0x02A6-B0 (>= 1 Bild) -> Plc_ret fruehestens Cut 1 + 113. Der Gator: Clip-6-Ende -> Cut 1
 *       (`jal 0x8002c7ac` / `addiu a0,zero,1` @0x801008c4-c8), Clip 3 ab dem naechsten Takt (P2 @0x801008e4-ec), 150 Bilder,
 *       Intro-Ende (Sub 1, Gier += 1024 @0x80100928-48) = Cut 1 + 150. Ausgabe: Abstand und Kontaktbilder, wenn Leon die
 *       Steuerung in Cut 1 + k bekommt (k = 113 .. 150): obere Grenze (fruehester Kampfbeginn) .. untere Grenze (Intro-Ende).
 *   (4) MASSSTAB DES PORTS: derselbe RE2-Ablauf mit dem Zeichen-Massstab des Port-Gators (GB_SCALE_Q12 2731/4096,
 *       enemy_ai_boss_gator.c; Wurzelschritt und Knochen mal s wie gator_re2_r41.c gr2_bone_welt/gr2_wurzel_schritt), die Lagen
 *       am Intro-Ende/im Fenster wie RE2 (Weltlage): Kontaktbilder ab Intro-Ende bzw. ab dem fruehesten Kampfbeginn.
 * Bau (Git-Bash, PATH mit C:/msys64/mingw64/bin zuerst): gcc -std=c11 -DRE15_PLATFORM_PC=1 -I re15_port/include
 *   re15_port/tools/r41_gatorszene/probe_re2_band_n5.c re15_port/build/engine/libre15_engine.a
 *   re15_port/build/tests/libre15_test_support.a -lm -o probe_re2_band_n5.exe
 * Aufruf: probe_re2_band_n5 <shared_assets/RE2/CDEMD0.EMS> */
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
static int32_t g_s = 4096;                 /* Zeichen-Massstab Q12 (RE2 1x = 4096, Port 2731) */
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
static void clip_bild(int clip, int f, int32_t *x, int32_t *z, int yaw)
{
    int32_t a[3] = { 0, 0, 0 }, b[3];
    if (f >= bk->anim.clips[clip].frame_count - 1) return;           /* Clip-Ende: kein Schritt (@0x8001a3b0) */
    off(clip, f, b);
    if (f > 0) off(clip, f - 1, a);
    int32_t dx = (int32_t)(((int64_t)(b[0] - a[0]) * g_s) >> 12), dz = (int32_t)(((int64_t)(b[2] - a[2]) * g_s) >> 12);
    int32_t c = re15_cos_q12(yaw), s = re15_sin_q12(yaw);
    *x += (int32_t)(((int64_t)c * dx + (int64_t)s * dz) >> 12);
    *z += (int32_t)((-(int64_t)s * dx + (int64_t)c * dz) >> 12);
}
/* Kontakt-Mass: max_b <P_b - L, h>, h = Vorwaerts der Gier hy (cos, -sin) */
static double vorne(int clip, int f, int32_t x, int32_t z, int yaw, int hy, double lx, double lz)
{
    re15_skel_pose_t p[RE15_EMD_MAX_BONES];
    re15_skel_compute_pose(&bk->skel, kf_of(clip, f), p);
    double hx = re15_cos_q12(hy) / 4096.0, hz = -re15_sin_q12(hy) / 4096.0, best = -1e18;
    for (int b = 0; b < bk->skel.bone_count; b++) {
        int32_t tr[3] = { (int32_t)(((int64_t)p[b].trans[0] * g_s) >> 12), (int32_t)(((int64_t)p[b].trans[1] * g_s) >> 12),
                          (int32_t)(((int64_t)p[b].trans[2] * g_s) >> 12) };
        int32_t w[3]; re15_skel_bone_to_world(tr, (int16_t)yaw, x, -1800, z, w);
        double v = (w[0] - lx) * hx + (w[2] - lz) * hz;
        if (v > best) best = v;
    }
    return best;
}
typedef struct { int32_t x, z; int yaw, phase, f3; } re2_t;   /* phase 3 = in Clip 3 (Bild f3 gezeigt), 1 = Intro vorbei */
static int zone_von(int zone, int32_t x, int32_t z)
{
    if (zone == 0 && x < -17877) return 1;                         /* @0x80100990-a8 */
    if (zone == 1 && z >= -7322) return 2;                         /* @0x801009d8-ec */
    return zone;
}
/* ab Zustand st (Leon steht bei (lx,lz)): Bilder bis zum Kontakt; *zw = Bild des ersten Zonenwechsels (-1 keiner) */
static int lauf(re2_t st, double lx, double lz, int *zw, int *zone_k)
{
    const int n3 = bk->anim.clips[3].frame_count, nc0 = bk->anim.clips[0].frame_count, nc1 = bk->anim.clips[1].frame_count;
    int32_t x = st.x, z = st.z; int yaw = st.yaw, f3 = st.f3, phase = st.phase;
    int sperre = (phase == 1) ? 110 : 0, sub = 1, f0 = 0, f1 = 0, zone = 0;
    *zw = -1;
    for (int t = 1; t <= 900; t++) {
        int clip, fr, hy;
        if (phase == 3) {
            f3++;
            clip_bild(3, f3, &x, &z, yaw);
            clip = 3; fr = f3; hy = (yaw + 1024) & 0xfff;          /* Root-ry-Rampe: Blick = Schwimmrichtung nach dem Knick */
            if (f3 >= n3 - 1) { phase = 1; yaw = (yaw + 1024) & 0xfff; sperre = 110; }   /* @0x8010091c-48 */
        } else {
            int nz = zone_von(zone, x, z);
            if (nz != zone) { zone = nz; if (*zw < 0) *zw = t; }    /* Dash-Ausloeser (Pre vor der Hysterese @0x80100b5c) */
            if (sub == 1) { clip = 0; fr = f0; clip_bild(0, f0, &x, &z, yaw); f0 = (f0 + 1) % nc0; }
            else          { clip = 1; fr = f1; clip_bild(1, f1, &x, &z, yaw); f1 = (f1 + 1) % nc1; }
            if (sperre > 0) sperre--;                              /* @0x801004a8-b8 */
            double d = hypot(x - lx, z - lz);
            if (sub == 1 && sperre == 0 && d >= 0x65) sub = 2;     /* @0x80100b5c-64 / @0x80100b88 */
            hy = yaw;
        }
        if (vorne(clip, fr, x, z, yaw, hy, lx, lz) >= 0) { *zone_k = zone; return t; }
    }
    *zone_k = zone;
    return -1;
}
int main(int argc, char **argv)
{
    size_t n = 0;
    if (argc < 2) { printf("probe_re2_band_n5 <CDEMD0.EMS>\n"); return 1; }
    uint8_t *ems = slurp(argv[1], &n);
    if (!ems) { printf("CDEMD0.EMS fehlt\n"); return 1; }
    re15_enemy_reset();
    bk = re15_enemy_alloc(0x23u);
    if (!bk || re2_ems_load_bank(ems, n, 0x23, bk, NULL) != 0) { printf("Bank 0x23 nicht ladbar\n"); return 1; }
    bk->ok = 1;
    const int n6 = bk->anim.clips[6].frame_count, n3 = bk->anim.clips[3].frame_count;
    const double lx = -10800, lz = -25800;                          /* sub03 Plc_dest @0x0286 */
    printf("EM23: Clip 6 %d Bilder, Clip 3 %d, Clip 0 %d, Clip 1 %d\n", n6, n3, bk->anim.clips[0].frame_count,
           bk->anim.clips[1].frame_count);
    for (int sv = 0; sv < 2; sv++) {
        const int32_t s_lauf = sv ? 2731 : 4096;
        printf("== Massstab %d/4096 (%s) ==\n", s_lauf, sv ? "Port GB_SCALE_Q12" : "RE2 1x");
        g_s = 4096;                                                 /* Intro-Weltlagen IMMER RE2 1x */
        int32_t x = 4557, z = -18237; int yaw = 0x400;
        for (int f = 0; f < n6; f++) clip_bild(6, f, &x, &z, yaw);
        z = -21212;
        static re2_t pos3[400];
        for (int f = 0; f < n3; f++) { clip_bild(3, f, &x, &z, yaw); pos3[f].x = x; pos3[f].z = z; pos3[f].yaw = yaw; pos3[f].phase = 3; pos3[f].f3 = f; }
        g_s = s_lauf;
        re2_t ende = { x, z, (yaw + 1024) & 0xfff, 1, n3 - 1 };
        int zw, zk;
        int t_ende = lauf(ende, lx, lz, &zw, &zk);
        printf("Intro-Ende (Cut 1 + %d): Wurzel (%d,%d) Gier %d, Abstand %.1f (SquareRoot0 %u) -> Kontakt nach %d Bildern, Zone beim "
               "Kontakt %d, Zonenwechsel vor dem Kontakt: %s\n", n3, x, z, ende.yaw, hypot(x - lx, z - lz),
               re15_squareroot0((uint32_t)((int64_t)(x - lx) * (x - lx) + (int64_t)(z - lz) * (z - lz))), t_ende, zk,
               (zw >= 0 && zw < t_ende) ? "JA" : "nein");
        for (int k = 113; k <= n3; k++) {
            if (k != 113 && k != 120 && k != 130 && k != 140 && k != 145 && k != n3) continue;
            re2_t st = (k >= n3) ? ende : pos3[k - 1];
            int t = lauf(st, lx, lz, &zw, &zk);
            printf("  Leon-Steuerung Cut 1 + %3d: Gator (%d,%d) %s, Abstand %.1f (SquareRoot0 %u) -> Kontakt nach %d Bildern%s\n", k,
                   st.x, st.z, st.phase == 3 ? "in Clip 3" : "Intro vorbei", hypot(st.x - lx, st.z - lz),
                   re15_squareroot0((uint32_t)((int64_t)(st.x - (int32_t)lx) * (st.x - (int32_t)lx) + (int64_t)(st.z - (int32_t)lz) * (st.z - (int32_t)lz))),
                   t, (zw >= 0 && zw < t) ? " (Zonenwechsel davor!)" : "");
        }
    }
    return 0;
}
