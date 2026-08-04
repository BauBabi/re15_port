/* probe_em40_gait.c — DIAGNOSE (Nutzer-Report #5: "Marvin schwebt am ANFANG immer noch"):
 * Ist EM040-BANK-1-Clip 5 (der NPC-Walk-Sub-Gait, +0x94=5 @0x800511dc) und Clip 0 (RUN-Gait
 * @0x8005157c) im KEYFRAME-INHALT wirklich ein Geh-/Lauf-Zyklus — oder eine statische Pose
 * (dann "gleitet" Marvin trotz korrekter State-Maschine)?
 * MESSUNG: pro Clip die referenzierten Keyframe-Indizes dumpen + die Byte-/Winkel-Varianz der
 * Bein-Bones ueber die Frames. Ein Gait MUSS periodische Aenderung zeigen; ein Einzel-Keyframe
 * oder identische Keyframes = statisch. Vergleich: Bank-0-Clips (dir[1]) derselben Datei. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_emd.h"
#include "re15_ems.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

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

static void dump_clip(const char *tag, const re15_emd_skeleton_t *sk,
                      const re15_emd_animation_t *an, int clip)
{
    if (clip >= an->clip_count) { printf("%s clip %d: FEHLT (%d clips)\n", tag, clip, an->clip_count); return; }
    int f0 = an->clips[clip].first_frame;
    int fc = an->clips[clip].frame_count;
    printf("%s clip %d: %d frames, kf-idx:", tag, clip, fc);
    int uniq = 0, last = -1;
    for (int f = 0; f < fc && f < 64; f++) {
        int kf = (int)(an->frames[f0 + f] & 0xFFF);
        printf(" %d", kf);
        if (kf != last) { uniq++; last = kf; }
    }
    printf("\n%s clip %d: %d Wechsel; kf_size=%d kf_count=%d\n",
           tag, clip, uniq, sk->keyframe_size_bytes, sk->keyframe_count);
    /* Byte-Differenz aufeinanderfolgender referenzierter Keyframes (0 = identische Pose) */
    long total_diff = 0; int cmp = 0;
    for (int f = 1; f < fc; f++) {
        int a = (int)(an->frames[f0 + f - 1] & 0xFFF);
        int b = (int)(an->frames[f0 + f] & 0xFFF);
        if (a >= sk->keyframe_count || b >= sk->keyframe_count) { printf("  !! kf-Index %d/%d OOB (pool %d)\n", a, b, sk->keyframe_count); continue; }
        const uint8_t *pa = sk->keyframe_data + (size_t)a * sk->keyframe_size_bytes;
        const uint8_t *pb = sk->keyframe_data + (size_t)b * sk->keyframe_size_bytes;
        int d = 0;
        for (int i = 0; i < sk->keyframe_size_bytes; i++) d += (pa[i] != pb[i]);
        total_diff += d; cmp++;
    }
    printf("%s clip %d: mittlere Byte-Differenz benachbarter KFs = %.1f/%d Bytes ueber %d Paare %s\n",
           tag, clip, cmp ? (double)total_diff / cmp : 0.0, sk->keyframe_size_bytes, cmp,
           (cmp && total_diff == 0) ? "<<< STATISCH!" : "");
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
    if (idx < 0 || re15_ems_get_entry(ems, ems_size, idx, &off, &len) != 0) { fprintf(stderr, "FAIL: idx\n"); return 1; }
    static uint8_t buf[0x80000];
    if (len > sizeof buf) { fprintf(stderr, "FAIL: gross\n"); return 1; }
    memcpy(buf, ems + off, len);

    static re15_emd_skeleton_t sk0, sk1;
    static re15_emd_animation_t an0, an1;
    static re15_md1_t md1; static re15_tim_t tim;
    if (re15_emd_parse_container(buf, len, &md1, &sk0, &an0, &tim) != 0) { fprintf(stderr, "FAIL c\n"); return 1; }
    int own = (re15_emd_parse_own_bank(buf, len, &sk1, &an1) == 0);
    printf("EM040: Bank0(container/largest) %d clips; own_ok=%d Bank1 %d clips\n",
           an0.clip_count, own, own ? an1.clip_count : -1);

    printf("\n== BANK 1 (dir[3]/dir[4] = +0x170/+0x174-Kanal, NPC-Walk-Subs) ==\n");
    dump_clip("B1", &sk1, &an1, 5);   /* Walk-Gait (Sub 4/5 INIT) */
    dump_clip("B1", &sk1, &an1, 0);   /* RUN-Gait (Sub-5 Phase 2) */
    dump_clip("B1", &sk1, &an1, 1);   /* Ankunfts-Geste */
    dump_clip("B1", &sk1, &an1, 2);   /* Idle */

    printf("\n== BANK 0 (dir[1]/dir[2], Vergleich) ==\n");
    dump_clip("B0", &sk0, &an0, 5);
    dump_clip("B0", &sk0, &an0, 0);
    dump_clip("B0", &sk0, &an0, 22);  /* 30f */
    dump_clip("B0", &sk0, &an0, 23);  /* 30f */

    free(ems);
    return 0;
}
