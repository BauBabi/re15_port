/**
 * @file probe_verify_sperre_dauer.c
 * @brief GEGENPRUEFUNG (nur lesend) des Befundes "Sperre = P0+P1, Fall-Clip 2 = 60 Bilder".
 *
 * Misst SELBST aus shared_assets/RE2/CDEMD0.EMS:
 *   - alle vier EDD-Baenke (anim / anim_loco / anim_own / anim_victim) fuer kind 0x10/0x11/0x16
 *   - die Bildzahlen der im Befund zitierten Clips 1,2 (Sturz) und 8,9 (Liegen) sowie 0x16/0x17
 *   - und rechnet die Sperrdauer aus dem Original-Mechanismus nach:
 *       Startbild = side*5+10 (@0x80103310-1C), Advance +1/Bild, Wrap bei clip_len
 *       (@0x80029B28-4C, s3 = u16 EDD[clip*4] @0x80029680)
 */
#include "re15_enemy.h"
#include "re15_emd.h"
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

static void dump_anim(const char *name, const re15_emd_animation_t *a, int ok)
{
    if (!ok || !a || a->clip_count <= 0) { printf("    %-12s: (leer)\n", name); return; }
    printf("    %-12s: clips=%d  ", name, a->clip_count);
    for (int i = 0; i < a->clip_count && i < 34; i++)
        printf("[%d]=%d ", i, (int)a->clips[i].frame_count);
    printf("\n");
}

int main(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) { printf("FAIL: CDEMD0.EMS nicht lesbar\n"); return 1; }
    printf("CDEMD0.EMS: %zu Bytes\n", n);

    const int kinds[3] = { 0x10, 0x11, 0x16 };
    for (int k = 0; k < 3; k++) {
        re15_enemy_bank_t eb;
        memset(&eb, 0, sizeof eb);
        if (re2_ems_load_bank(ems, n, kinds[k], &eb, NULL) != 0) {
            printf("kind 0x%02X: LADEN FEHLGESCHLAGEN\n", kinds[k]);
            continue;
        }
        printf("kind 0x%02X:\n", kinds[k]);
        dump_anim("anim(Aktion)", &eb.anim, 1);
        dump_anim("anim_loco",    &eb.anim_loco, eb.loco_ok);
        dump_anim("anim_own",     &eb.anim_own,  eb.own_ok);
        dump_anim("anim_victim",  &eb.anim_victim, eb.victim_ok);

        /* Die Rechnung des Befundes, mit den GEMESSENEN Laengen der AKTIONS-Bank. */
        for (int side = 0; side < 2; side++) {
            int clip = (side == 0) ? 1 : 2;
            int start = side * 5 + 10;
            int len = (clip < eb.anim.clip_count) ? (int)eb.anim.clips[clip].frame_count : -1;
            int adv = (len > start) ? (len - start) : -1;
            printf("    SPERRE side=%d clip=%d start=%d len=%d -> %d Advances\n",
                   side, clip, start, len, adv);
        }
        for (int side = 0; side < 2; side++) {
            int c = (side == 0) ? 8 : 9;
            int len = (c < eb.anim.clip_count) ? (int)eb.anim.clips[c].frame_count : -1;
            printf("    LIEGECLIP side=%d clip=%d len=%d\n", side, c, len);
        }
        for (int side = 0; side < 2; side++) {
            int c = (side == 0) ? 0x17 : 0x16;
            int len = (c < eb.anim.clip_count) ? (int)eb.anim.clips[c].frame_count : -1;
            printf("    BODENPOSE side=%d clip=0x%02X len=%d\n", side, c, len);
        }
    }
    free(ems);
    return 0;
}
