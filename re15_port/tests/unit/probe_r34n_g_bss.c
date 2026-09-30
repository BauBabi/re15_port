/*
 * Spur G (Runde 34 Nacht) — Mess-Sonde, KEIN Verhalten, kein add_test.
 *
 * Dekodiert JEDEN 64-KB-Chunk einer Original-BSS (Schnittregel bg_pc.c: Cut n =
 * Datei[n*0x10000], Laenge 0x10000) mit denselben Engine-Funktionen, die der Port
 * im Spiel benutzt (re15_bss_parse_chunk / re15_bss_vlc_decode / re15_bss_mdec_decode),
 * und schreibt je Cut ein PPM. Zweck: die 13 Hintergruende von ROOM1170 sehen und die
 * Schrift "MAGAZINE CLUB" pixelgenau mit dem Savestate-RAM (0x80198000) vergleichen.
 *
 *   probe_r34n_g_bss <datei.bss> <ausgabe-praefix>
 *   -> <praefix>_cutNN.ppm
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "re15_bss.h"

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s <bss> <out-prefix>\n", argv[0]);
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { fprintf(stderr, "open %s failed\n", argv[1]); return 1; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf || fread(buf, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); return 1; }
    fclose(f);

    int n = (int)(sz / RE15_BSS_CHUNK_SIZE);
    for (int c = 0; c < n; c++) {
        const uint8_t *chunk = buf + (size_t)c * RE15_BSS_CHUNK_SIZE;
        re15_bss_chunk_t ch;
        if (!re15_bss_parse_chunk(chunk, RE15_BSS_CHUNK_SIZE, &ch) || !re15_bss_chunk_has_video(&ch)) {
            printf("cut %02d: kein Video (id=0x%04x)\n", c, ch.id);
            continue;
        }
        size_t cap = ((size_t)ch.run_length_words + 2) * 4;
        int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
        int w = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words,
                                    ch.quant, ch.version, co, cap);
        uint8_t *rgb = (uint8_t *)malloc(320 * 240 * 3);
        int rv = (w > 0) ? re15_bss_mdec_decode(co, (size_t)w, 320, 240, rgb) : -99;
        printf("cut %02d: rl=%u q=%u v=%u vlc=%d mdec=%d\n", c, ch.run_length_words, ch.quant,
               ch.version, w, rv);
        if (rv == 0) {
            char p[512];
            snprintf(p, sizeof p, "%s_cut%02d.ppm", argv[2], c);
            FILE *o = fopen(p, "wb");
            if (o) {
                fprintf(o, "P6\n320 240\n255\n");
                fwrite(rgb, 1, 320 * 240 * 3, o);
                fclose(o);
            }
        }
        free(rgb);
        free(co);
    }
    free(buf);
    return 0;
}
