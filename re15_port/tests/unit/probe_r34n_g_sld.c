/*
 * Spur G2 (Runde 34 Nacht) — Mess-Sonde, KEIN Verhalten, kein add_test.
 *
 * Zieht den sprite.pri-Vordergrundatlas eines BSS-Cuts GENAU so, wie ihn der Port im Spiel
 * zieht (bg_pc.c re15_pri_cache_from_chunk: re15_sld_used_len aus BIN/STAGE<n>.BIN, dann
 * re15_sld_atlas_from_chunk auf den 64-KB-Chunk), und schreibt den entpackten Sony-TIM in eine
 * Datei. Zweck: den Port-Atlas von ROOM1150 Cut 2 (Masken der Blink-Gruppen 6..11) mit dem
 * Python-Auszug (tools/maske/original.py) und der Alt-Datei BSS/ROOM1150/PRI02.TIM vergleichen.
 *
 *   probe_r34n_g_sld <STAGEn.BIN> <ROOMxxx.BSS> <stage> <raum-index-hex> <cut> <aus.tim>
 *   z.B. probe_r34n_g_sld STAGE1.BIN ROOM115.BSS 1 15 2 atlas.tim
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "re15_sld.h"

static uint8_t *lies(const char *p, int *sz)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    *sz = (int)n;
    return b;
}

int main(int argc, char **argv)
{
    if (argc < 7) {
        fprintf(stderr, "usage: %s <STAGEn.BIN> <ROOMxxx.BSS> <stage> <raum-index-hex> <cut> <aus.tim>\n",
                argv[0]);
        return 2;
    }
    int sbsz = 0, bsz = 0;
    uint8_t *sb = lies(argv[1], &sbsz);
    uint8_t *bss = lies(argv[2], &bsz);
    if (!sb || !bss) { fprintf(stderr, "Datei fehlt\n"); return 1; }
    int stage = atoi(argv[3]);
    int ri = (int)strtol(argv[4], NULL, 16);
    int cut = atoi(argv[5]);
    uint16_t used = 0;
    int rv = re15_sld_used_len(sb, sbsz, stage, ri, cut, &used);
    printf("used_len rv=%d L=0x%X\n", rv, used);
    if (rv != RE15_SLD_OK) return 1;
    if ((cut + 1) * 0x10000 > bsz) { fprintf(stderr, "Cut ausserhalb der BSS\n"); return 1; }
    static uint8_t out[RE15_SLD_MAX_UNPACKED];
    int len = 0;
    rv = re15_sld_atlas_from_chunk(bss + (size_t)cut * 0x10000, 0x10000, used, out, (int)sizeof out, &len);
    printf("atlas rv=%d len=%d\n", rv, len);
    if (rv != RE15_SLD_OK) return 1;
    FILE *o = fopen(argv[6], "wb");
    if (!o) return 1;
    fwrite(out, 1, (size_t)len, o);
    fclose(o);
    free(sb);
    free(bss);
    return 0;
}
