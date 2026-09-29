/*
 * probe_r34_re2fx_knochen — Runde 34 Spur D, O-VB1 (bau_d.md §1.4): die GL-Waffenknochen-Basis B der
 * RE2-FX-Maschine (re2fx_gl_basis, engine/src/re2_fx.c) wird hier aus den RE2-Daten NACHGERECHNET:
 *
 *   Skelett   info/re2leon/PL0/PLD/PL01.PLD  (Claire, Verzeichnis dir[1] = EMR)
 *   Keyframes info/re2leon/PL0/PLD/PL01W09.PLW (dir[0] = EDD, dir[1] = keyframe-only EMR, Kopf @2 =
 *             Keyframe-Offset — dieselbe Kombination wie platform/pc/main.c:1822-1839 fuer RE1.5)
 *   Pose      re15_skel_compute_pose (Port-Zwilling FUN_8001f3bc), Teil 11 (a2 der Runde =
 *             *(+0x198) + 0x7AC `lw s0,408(s1) / addiu s0,s0,1964` @0x80044f78/@0x80044f90)
 *
 * Pruefungen:
 *   1..5   Dateien/Verzeichnisse lesbar, 15 Knochen, 16 Clips
 *   10     Clip 10 Bild 0 (Keyframe 271) Teil-11-Drehung == re2fx_gl_basis (exakt)
 *   11     Laufachse (Spalte y) Clip 10 waagrecht, Clip 12 hoch, Clip 14 tief (Zuordnung der Clips)
 *   12     lokal +x zeigt im waagrechten Clip nach Welt-oben (y < 0) -> acc.x -23 = abwaerts
 *   20     Negativ-Kontrolle: ein anderer Keyframe (Clip 12) ist NICHT gleich B
 * Rueckgabe 0 = gruen, sonst die Nummer.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_fx.h"
#include "re15_emd.h"
#include "re15_skeleton.h"


static uint8_t *lesen(const char *rel, long *n)
{
    char p[1024];
    snprintf(p, sizeof p, "%s/%s", RE15_REPO_ROOT, rel);   /* Zeichenkette aus r34_re2fx.cmake (wie r16_trefferhoehe) */
    FILE *f = fopen(p, "rb");
    if (!f) { printf("kann %s nicht oeffnen\n", p); return NULL; }
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)*n);
    if (b && fread(b, 1, (size_t)*n, f) != (size_t)*n) { free(b); b = NULL; }
    fclose(f);
    return b;
}
static uint32_t u32(const uint8_t *b, uint32_t o)
{ return (uint32_t)b[o] | ((uint32_t)b[o + 1] << 8) | ((uint32_t)b[o + 2] << 16) | ((uint32_t)b[o + 3] << 24); }

static re15_emd_skeleton_t  s_sk;
static re15_emd_animation_t s_an;

static int teil11(int clip, int bild, int32_t r[9])
{
    if (clip >= s_an.clip_count || bild >= s_an.clips[clip].frame_count) return -1;
    int kf = (int)(s_an.frames[s_an.clips[clip].first_frame + bild] & 0xfff);
    static re15_skel_pose_t po[RE15_EMD_MAX_BONES];
    g_anim_pose_actor = NULL;
    if (re15_skel_compute_pose(&s_sk, kf, po) != 0) return -1;
    memcpy(r, po[11].rot, sizeof po[11].rot);
    return kf;
}

int main(void)
{
    long npld = 0, nplw = 0;
    uint8_t *pld = lesen("info/re2leon/PL0/PLD/PL01.PLD", &npld);
    uint8_t *plw = lesen("info/re2leon/PL0/PLD/PL01W09.PLW", &nplw);
    if (!pld || !plw) { printf("FAIL 1: RE2-Dateien fehlen\n"); return 1; }
    uint32_t dp = u32(pld, 0), dw = u32(plw, 0);
    if (dp + 16 > (uint32_t)npld || dw + 16 > (uint32_t)nplw) { printf("FAIL 2: Verzeichnis\n"); return 2; }
    uint32_t pe1 = u32(pld, dp + 4), pe2 = u32(pld, dp + 8);
    uint32_t we0 = u32(plw, dw + 0), we1 = u32(plw, dw + 4), we2 = u32(plw, dw + 8);
    if (re15_emd_parse_skeleton(pld + pe1, pe2 - pe1, &s_sk) != 0 || s_sk.bone_count != 15)
    { printf("FAIL 3: PL01-Skelett (15 Knochen)\n"); return 3; }
    if (re15_emd_parse_animation(plw + we0, we1 - we0, &s_an) != 0 || s_an.clip_count != 16)
    { printf("FAIL 4: PL01W09-Clips (16)\n"); return 4; }
    const uint8_t *kb = plw + we1;
    int koff = kb[2] | (kb[3] << 8);
    int kfs  = s_sk.keyframe_size_bytes > 0 ? s_sk.keyframe_size_bytes : 80;
    s_sk.keyframe_data      = kb + koff;
    s_sk.keyframe_data_size = (size_t)((long)(we2 - we1) - koff);
    s_sk.keyframe_count     = (int)(((long)(we2 - we1) - koff) / kfs);
    if (s_sk.bone_parent[11] != 10 || s_sk.bone_parent[10] != 9 || s_sk.bone_parent[9] != 0)
    { printf("FAIL 5: Teil 11 nicht an 10/9/0\n"); return 5; }

    int16_t b[9]; re2fx_gl_basis(b);
    int32_t r[9];
    int kf = teil11(10, 0, r);
    printf("Clip 10 Bild 0 = Keyframe %d: [%d %d %d | %d %d %d | %d %d %d]\n", kf,
           r[0], r[1], r[2], r[3], r[4], r[5], r[6], r[7], r[8]);
    if (kf != 271) { printf("FAIL 10: Clip 10 Bild 0 ist nicht Keyframe 271\n"); return 10; }
    for (int k = 0; k < 9; k++)
        if (r[k] != b[k]) { printf("FAIL 10: B[%d] = %d, Daten %d\n", k, b[k], r[k]); return 10; }

    /* Laufachse = Spalte 1 (lokal y), Welt-y negativ = oben. */
    int32_t w[9], h[9], t[9];
    teil11(10, 0, w); teil11(12, 0, h); teil11(14, 0, t);
    int wy = w[4], hy = h[4], ty = t[4];
    printf("Laufachse y-Komponente: Clip 10 %d, Clip 12 %d, Clip 14 %d\n", wy, hy, ty);
    if (!(wy > -100 && wy < 100 && hy < -1000 && ty > 1000)) { printf("FAIL 11: Clip 10/12/14 nicht waagrecht/hoch/tief\n"); return 11; }
    if (!(w[3] < -4000)) { printf("FAIL 12: lokal +x zeigt im waagrechten Clip nicht nach oben\n"); return 12; }

    /* Negativ-Kontrolle: Clip 12 ist NICHT B. */
    int gleich = 1;
    for (int k = 0; k < 9; k++) if (h[k] != b[k]) gleich = 0;
    if (gleich) { printf("FAIL 20: Negativ-Kontrolle — Clip 12 gleich B\n"); return 20; }
    printf("probe_r34_re2fx_knochen: B aus PL01 + PL01W09 bestaetigt\n");
    return 0;
}
