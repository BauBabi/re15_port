/* probe_tempo_gegenpruefung.c - UNABHAENGIGE Nachrechnung des Tempo-Bit-Befunds.
 * Dumpt ALLE Keyframe-sx je Bild (Clip 0/2 der RE2-Loco-Bank EM010) und rechnet
 * beide Modelle ein zweites Mal, ohne die Zwischenwerte der Erst-Sonde zu benutzen.
 * Belege: FUN_80015E7C @0x80015FCC-FE4 (subu/sh +0x144), Wrap-Null @0x80015F14/F18/F1C
 * (a3==0), Normal-Advance @0x80029B28-4C, Tempo-Weiche @0x80101D00-2C (0xAAAAAAAB),
 * Extra-Advance FUN_8002A9C8 @0x8002A9C8-AA1C, Gate @0x80101CD8-CFC.
 */
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re2_ems.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

int main(void)
{
    size_t n = 0;
    uint8_t *raw = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!raw) { printf("SKIP: CDEMD0.EMS fehlt\n"); return 0; }
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_enemy_reset();
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x10);
    if (!eb || re2_ems_load_bank(raw, n, 0x10, eb, NULL) != 0) { printf("SKIP: Bank\n"); return 0; }
    eb->buf = NULL; eb->ok = 1;
    if (!eb->loco_ok) { printf("SKIP: keine Loco-Bank\n"); return 0; }
    const re15_emd_animation_t *A = &eb->anim_loco;
    const re15_emd_skeleton_t  *S = &eb->skel_loco;

    for (int clip = 0; clip <= 2; clip += 2) {
        if (clip >= A->clip_count) continue;
        const re15_emd_clip_t *c = &A->clips[clip];
        int fc = c->frame_count;
        int32_t *sx = (int32_t *)calloc((size_t)fc, sizeof(int32_t));
        printf("\n=== CLIP %d  frame_count=%d  first_frame=%d ===\nsx:", clip, fc, c->first_frame);
        for (int f = 0; f < fc; f++) {
            int kf = (int)(A->frames[c->first_frame + f] & 0xFFFu);
            int16_t a = 0, b = 0, d = 0;
            re15_emd_get_keyframe_speed(S, kf, &a, &b, &d);
            sx[f] = a;
            printf(" %d", (int)a);
        }
        printf("\n");

        for (int bit = 0; bit <= 1; bit++) {
            /* MODELL ORIGINAL: Vorstand = POSITION, Wrap auf Bild 0 -> Vorstand 0. */
            long long org = 0; int32_t prev = 0; int f = 0; int extras = 0; int32_t omax = 0;
            /* MODELL PORT: Delta nur bei fr_now == fr_prev+1 (enemy_ai_common.c:700/706). */
            long long prt = 0; int fp = 0; int prev_kf = -1; int zero = 0; int32_t pmax = 0;
            for (int t = 0; t < 100; t++) {
                f = (f + 1) % fc;
                fp = (fp + 1) % fc;
                if (bit && (f % 3) == 2) { f = (f + 1) % fc; extras++; }
                if (bit && (fp % 3) == 2) { fp = (fp + 1) % fc; }
                {   int32_t base = (f == 0) ? 0 : prev;
                    int32_t dx = sx[f] - base; org += dx; if (dx > omax) omax = dx; prev = sx[f]; }
                {   int32_t dx = 0;
                    if (prev_kf >= 0 && fp == prev_kf + 1) dx = sx[fp] - sx[prev_kf];
                    if (dx == 0) zero++; if (dx > pmax) pmax = dx;
                    prt += dx; prev_kf = fp; }
            }
            printf("  Bit %s: PORT %6lld (Null %2d, max %3ld) | ORIG %6lld (Extras %2d, max %3ld)\n",
                   bit ? "AN " : "AUS", prt, zero, (long)pmax, org, extras, (long)omax);
        }
        free(sx);
    }
    printf("\nGEGENPRUEFUNG-OK\n");
    return 0;
}
