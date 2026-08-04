/* probe_marvin_render_pose.c — REGRESSION (ctest): Nutzer-Report #5 "Marvin schwebt am
 * ANFANG immer noch". Beweist das letzte Glied der Kette: die GERENDERTE Pose.
 *
 * Kette: State-Maschine (probe_marvin_10d0: byte-true Sequenz) -> Bank-Wahl (main.c
 * Render-Override state 4 subs {2,4,5,6,9} -> skel_own/anim_own, clip_override=motion)
 * -> re15_compute_actor_kf -> re15_skel_compute_pose -> Draw. main.c waehlt nur die
 * Baenke; kf-Aufloesung und Bone-Posen kommen KOMPLETT aus diesen Engine-Funktionen —
 * wenn die Posen hier ueber die Walk-Frames zyklieren, zeigt der Draw einen laufenden
 * Marvin (der Draw selbst ist der geteilte Pfad aller Figuren).
 *
 * MESSUNG: Marvin exakt im Zustand des Cutscene-Walks (state 4, sub 4/5, motion 5/0,
 * anim_frame 0..fc-1) -> pro Frame kf-Index + Bone-Posen; Assertions:
 *   (a) kf-Indizes wechseln jeden Frame (kein Freeze/kein %-Kollaps),
 *   (b) die Bein-/Arm-Bones bewegen sich substanziell (Gait-Varianz),
 *   (c) Wurzel-Y bleibt am Boden (kein Schwebe-Offset). */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_actor.h"
#include "re15_anim_select.h"
#include "re15_skeleton.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* Ein Walk-/Run-Clip durchposieren und die Bewegungs-Statistik pruefen. */
static void check_gait(const char *tag, const re15_emd_skeleton_t *sk,
                       const re15_emd_animation_t *an, int sub, int clip)
{
    re15_actor_t a; memset(&a, 0, sizeof a);
    a.active = 1; a.type = 0x40; a.state = 4;
    a.sub_state_1 = (uint8_t)sub; a.sub_state_2 = 2;      /* Phase 2 = WALK */
    a.motion = (int16_t)clip; a.anim_flags = 0;

    int fc = (clip < an->clip_count) ? an->clips[clip].frame_count : 0;
    CHECK(fc > 1, "(%s) Clip %d hat %d Frames", tag, clip, fc);
    if (fc <= 1) return;

    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    static int32_t prev_trans[RE15_EMD_MAX_BONES][3];
    int kf_prev = -1, kf_changes = 0;
    int64_t total_move = 0;                                /* Summe |delta trans| aller Bones */
    int32_t max_bone_move = 0;
    int32_t root_y_min = INT32_MAX, root_y_max = INT32_MIN;

    for (int t = 0; t < fc; t++) {
        a.anim_frame = (uint16_t)t;
        int kf = re15_compute_actor_kf(an, sk, &a, clip, (uint32_t)t);
        CHECK(kf >= 0 && kf < sk->keyframe_count,
              "(%s) f%d: kf %d im Pool (%d)", tag, t, kf, sk->keyframe_count);
        if (kf != kf_prev) kf_changes++;
        kf_prev = kf;
        if (kf < 0 || kf >= sk->keyframe_count) continue;
        CHECK(re15_skel_compute_pose(sk, kf, poses) == 0, "(%s) f%d: pose", tag, t);
        if (poses[0].trans[1] < root_y_min) root_y_min = poses[0].trans[1];
        if (poses[0].trans[1] > root_y_max) root_y_max = poses[0].trans[1];
        if (t > 0) {
            for (int b = 0; b < sk->bone_count; b++) {
                int32_t d = 0;
                for (int k = 0; k < 3; k++) {
                    int32_t dd = poses[b].trans[k] - prev_trans[b][k];
                    d += (dd < 0) ? -dd : dd;
                }
                total_move += d;
                if (d > max_bone_move) max_bone_move = d;
            }
        }
        for (int b = 0; b < sk->bone_count; b++)
            for (int k = 0; k < 3; k++) prev_trans[b][k] = poses[b].trans[k];
    }
    double avg = (double)total_move / (double)((fc - 1) * (sk->bone_count ? sk->bone_count : 1));
    printf("%s (sub %d, clip %d, %df): kf-Wechsel %d/%d, mittl. Bone-Bewegung %.1f units/Frame, "
           "max %d, Wurzel-Y [%d..%d]\n",
           tag, sub, clip, fc, kf_changes, fc, avg, max_bone_move,
           root_y_min, root_y_max);
    /* (a) jeder Frame ein neuer Keyframe (Bank-1-EDD: streng monotone kf-Ketten — probe_em40_gait) */
    CHECK(kf_changes == fc, "(%s) kf wechselt JEDEN Frame (Gait), got %d/%d", tag, kf_changes, fc);
    /* (b) substanzielle Gliedmassen-Bewegung: ein Gait bewegt Bones zweistellig pro Frame;
     * eine statische Pose ergaebe ~0 (Schwelle konservativ: > 5 units im Mittel). */
    CHECK(avg > 5.0, "(%s) Bones zyklieren (Gait-Varianz), got %.2f units/Frame", tag, avg);
    /* (c) die Wurzel bleibt in Koerperhoehe (kein Schwebe-Offset): EM040 steht ~ -1750
     * (Hueft-Hoehe, y negativ = oben); ein Fehler in der Pool-Geometrie laege GROB daneben. */
    CHECK(root_y_min > -4000 && root_y_max < 0,
          "(%s) Wurzel-Y plausibel [-4000..0], got [%d..%d]", tag, root_y_min, root_y_max);
}

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    char path[600]; size_t ems_size = 0;
    snprintf(path, sizeof path, "%s/EMD/CDEMD0.EMS", base);
    uint8_t *ems = read_file(path, &ems_size);
    if (!ems) { fprintf(stderr, "FAIL: EMS fehlt\n"); return 1; }
    int idx = re15_ems_index_for_type(0x40);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems, ems_size, idx, &off, &len) != 0) { fprintf(stderr, "FAIL idx\n"); return 1; }
    static uint8_t buf[0x80000];
    if (len > sizeof buf) { fprintf(stderr, "FAIL gross\n"); return 1; }
    memcpy(buf, ems + off, len);

    static re15_emd_skeleton_t sk_own; static re15_emd_animation_t an_own;
    CHECK(re15_emd_parse_own_bank(buf, len, &sk_own, &an_own) == 0, "own-Bank parst");
    printf("EM040 own-Bank: %d clips, %d bones, kf %d x %dB\n",
           an_own.clip_count, sk_own.bone_count, sk_own.keyframe_count, sk_own.keyframe_size_bytes);

    /* Die drei Gaits der Cutscene: Sub-4-Walk (Clip 5), Sub-5-Phase-2-Run (Clip 0),
     * Sub-5-Phase-1-Pivot (Clip 5). Dazu die Ankunfts-Geste (Sub 6 Clip 1). */
    check_gait("WALK",  &sk_own, &an_own, 4, 5);
    check_gait("RUN",   &sk_own, &an_own, 5, 0);
    check_gait("REACH", &sk_own, &an_own, 6, 1);

    free(ems);
    if (fails) { printf("MARVIN RENDER POSE: %d FAIL\n", fails); return 1; }
    printf("MARVIN RENDER POSE: all checks passed\n");
    return 0;
}
