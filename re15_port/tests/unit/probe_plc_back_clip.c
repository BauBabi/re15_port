/* probe_plc_back_clip.c — MESSSONDE zum Nutzer-Befund (2), 2026-08-22.
 *
 * "ROOM1090-Cutscene: zum Schluss rennt Leon noch ein bisschen komisch, fast auf der Stelle."
 *
 * Der Cutscene-Schwanz ist Plc_dest Mode 8 = RUECKWAERTSGEHEN (Handler @0x800311F0). Der
 * Handler schreibt `+0x94 = 0` (`sb zero,-13592(at)` = 0x800ACAE8 @0x8003122C) und spielt den
 * Clip aus dem PAAR (+0x84, +0x16C):
 *     801312a0/a4  lui a0,0x800b / lw a0,-13608(a0)   = 0x800ACAD8 = Spieler+0x84  (EMR)
 *     801312a8/ac  lui a1,0x800b / lw a1,-13376(a1)   = 0x800ACBC0 = Spieler+0x16C (EDD)
 *     801312b0/b4  jal 0x8001F314 (a2 = 0, a3 = 0x200)
 * Modus 7 identisch (@0x80031130-44). Die MODI 4/5/9 laden dagegen das ANDERE Paar:
 *     80030bec/f4  lw a0,0x800ACBC4 (+0x170) / lw a1,0x800ACBC8 (+0x174)   (Mode 4 WALK)
 *     80030ec0/c8  dieselben beiden                                        (Mode 5 RUN)
 *     80031488/90  dieselben beiden                                        (Mode 9 TURN)
 * Und die beiden Paare sind BELEGT VERSCHIEDENE BAENKE (der Port fuehrt beide bereits, siehe
 * anim_select_common.c Kletter-/Schiebe-Bloecke):
 *     0x800ACBC0 / 0x800ACAD8  -> EIN Schreiber: @0x8003154C / @0x80031578 = PLD-Directory
 *                                 = PL00.EDD / PL00.EMR      (die BASIS-Bank)
 *     0x800ACBC4 / 0x800ACBC8  -> EIN Schreiber: @0x80036BE4 / @0x80036C04 = PLW-Archiv
 *                                 = PL00W01.EDD / .EMR       (die WAFFEN-Bank)
 * Der Port bildet Mode 7/8 auf `RE15_MOTION_RUN` (Sentinel 100 = **W01 Clip 0 = RENNEN**) ab
 * (re15_to_re2.c:77) — also die falsche Bank UND die falsche Rolle.
 *
 * Diese Sonde misst BEIDE Clips aus den echten Dateien und zeigt, dass es zwei verschiedene
 * Animationen sind (Bildzahl + Wurzelstrecke + Armhaltung).
 */
#include "re15_emd.h"
#include "re15_skeleton.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, int *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (int)sz; return b;
}

static void dump_bank(const char *tag, const char *edd, const char *emr, int maxclip)
{
    char pe[600], pm[600]; int ne = 0, nm = 0;
    snprintf(pe, sizeof pe, "%s/%s", RE15_ASSET_PSX_DIR, edd);
    snprintf(pm, sizeof pm, "%s/%s", RE15_ASSET_PSX_DIR, emr);
    uint8_t *be = slurp(pe, &ne), *bm = slurp(pm, &nm);
    if (!be || !bm) { printf("%s: DATEI FEHLT (%s / %s)\n", tag, pe, pm); return; }
    re15_emd_animation_t an; memset(&an, 0, sizeof an);
    re15_emd_skeleton_t  sk; memset(&sk, 0, sizeof sk);
    if (re15_emd_parse_animation(be, ne, &an) != 0 ||
        re15_emd_parse_skeleton(bm, nm, &sk) != 0) { printf("%s: Parse fehlgeschlagen\n", tag); return; }
    printf("%s: %d Clips, %d Bones, %d Keyframes\n", tag, an.clip_count, sk.bone_count,
           sk.keyframe_count);
    for (int c = 0; c < an.clip_count && c < maxclip; c++) {
        const re15_emd_clip_t *cl = &an.clips[c];
        int n = cl->frame_count; if (n <= 0) { printf("  clip %2d: leer\n", c); continue; }
        int16_t sx0 = 0, sz0 = 0, sxN = 0, szN = 0;
        long long hand = 0; int nb = sk.bone_count;
        for (int i = 0; i < n; i++) {
            int kf = (int)(an.frames[cl->first_frame + i] & 0xFFFu);
            int16_t sx, sy, sz;
            re15_emd_get_keyframe_speed(&sk, kf, &sx, &sy, &sz);
            if (i == 0) { sx0 = sx; sz0 = sz; }
            sxN = sx; szN = sz;
            re15_skel_pose_t po[RE15_EMD_MAX_BONES];
            if (nb > 0 && re15_skel_compute_pose(&sk, kf, po) == 0) {
                int lo = 0;                       /* niedrigstes (= hoechstes) Bone-Y */
                for (int j = 0; j < nb; j++) if (po[j].trans[1] < lo) lo = po[j].trans[1];
                hand += lo;
            }
        }
        /* BEIN-SCHWUNG: pro Bone die Spanne (max-min) der lokalen X/Z-Weltposition ueber den
         * ganzen Clip. Ein Gang-Zyklus hat zwei grosse, gegenphasige Bein-Spannen; ein
         * Idle/Halte-Clip hat ueberall kleine Spannen. Objektives Kriterium statt "sieht aus wie". */
        int span[RE15_EMD_MAX_BONES]; int best1 = 0, best2 = 0, b1 = -1, b2 = -1;
        {
            int mn[RE15_EMD_MAX_BONES], mx[RE15_EMD_MAX_BONES];
            for (int j = 0; j < nb; j++) { mn[j] = 1 << 28; mx[j] = -(1 << 28); span[j] = 0; }
            for (int i = 0; i < n; i++) {
                int kf = (int)(an.frames[cl->first_frame + i] & 0xFFFu);
                re15_skel_pose_t po[RE15_EMD_MAX_BONES];
                if (re15_skel_compute_pose(&sk, kf, po) != 0) continue;
                for (int j = 0; j < nb; j++) {
                    int v = po[j].trans[0];
                    if (v < mn[j]) mn[j] = v;
                    if (v > mx[j]) mx[j] = v;
                }
            }
            for (int j = 0; j < nb; j++) {
                span[j] = mx[j] - mn[j];
                if (span[j] > best1) { best2 = best1; b2 = b1; best1 = span[j]; b1 = j; }
                else if (span[j] > best2) { best2 = span[j]; b2 = j; }
            }
        }
        printf("  clip %2d: %3d Bilder  sx %6d->%6d (%6d)  sz %6d->%6d (%6d)  hoechster Bone-Y "
               "Mittel %6lld  groesste X-Schwuenge: b%d=%d b%d=%d\n",
               c, n, (int)sx0, (int)sxN, (int)(sxN - sx0),
               (int)sz0, (int)szN, (int)(szN - sz0), hand / n, b1, best1, b2, best2);
    }
}

int main(void)
{
    printf("=== PL00 BASIS-Bank (0x800ACAD8/0x800ACBC0) — die Bank der Modi 7/8 ===\n");
    dump_bank("PL00", "PLD/PL00.EDD", "PLD/PL00.EMR", 8);
    printf("\n=== PL00W01 WAFFEN-Bank (0x800ACBC4/0x800ACBC8) — die Bank der Modi 4/5/9 ===\n");
    dump_bank("PL00W01", "PLD/PL00W01.EDD", "PLD/PL00W01.EMR", 8);
    return 0;
}
