/*
 * bg_dump — dekodiert die MDEC-Raumhintergruende aus den Original-BSS-Dateien.
 *
 * WOZU: Die Werkzeugkette fuer die nachgezeichneten Vordergrundmasken braucht das
 * Hintergrundbild JEDES Kamera-Cuts. Bisher lagen die nur als BMP im Extraktionsbaum
 * des Java-Extraktors — und der deckt STAGE6 gar nicht und STAGE5 nur zu einem
 * Bruchteil ab. Ein Generator, der sich darauf stuetzt, waere fuer ein Sechstel des
 * Spiels blind. Dieses Werkzeug benutzt stattdessen die Engine-Dekoder
 * (bss_common.c / bss_vlc.c / bss_mdec.c), also dieselbe Kette, die im Spiel laeuft.
 *
 * QUELLE: STAGE<n>/ROOM<rrr>.BSS, Scheibe cut*0x10000 (die Schnittregel ist im Port
 * gemessen dokumentiert — 1688 Schnitte, alle byte-identisch mit der so gerechneten
 * Scheibe, bg_pc.c). Raeume mit Animation haben MEHR Chunks als Kamera-Cuts
 * (12 von 103 Raeumen); die zusaetzlichen Chunks haengen hinten an, die ersten nCut
 * Scheiben entsprechen also den Cuts.
 *
 * AUSGABE: <out>/ROOM<rrr><cc>.ppm (P6, 320x240) — bewusst PPM, damit das Werkzeug
 * ohne Bibliothek auskommt; die Auswertung liest es mit PIL.
 *
 * Aufruf:  bg_dump <cd-wurzel> <ausgabeverzeichnis> [stage]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_bss.h"

#define CHUNK 0x10000

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = sz;
    return b;
}

/* Ein Chunk -> 320x240 RGB. 0 = ok. */
static int decode_chunk(const uint8_t *chunk, uint8_t *rgb_out)
{
    re15_bss_chunk_t ch;
    if (!re15_bss_parse_chunk(chunk, CHUNK, &ch))  return -1;
    if (!re15_bss_chunk_has_video(&ch))            return -2;

    size_t cap = ((size_t)ch.run_length_words + 2) * 8 + 4096;
    int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
    if (!co) return -3;
    int n = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size,
                                ch.run_length_words, ch.quant, ch.version, co, cap);
    if (n < 0) { free(co); return -4; }
    int rv = re15_bss_mdec_decode(co, (size_t)n, RE15_BSS_FRAME_WIDTH,
                                  RE15_BSS_FRAME_HEIGHT, rgb_out);
    free(co);
    return rv == 0 ? 0 : -5;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Aufruf: bg_dump <cd-wurzel> <ausgabeverzeichnis> [stage]\n");
        return 2;
    }
    const char *cd  = argv[1];
    const char *out = argv[2];
    int only_stage  = (argc > 3) ? atoi(argv[3]) : 0;

    uint8_t *rgb = (uint8_t *)malloc((size_t)RE15_BSS_FRAME_WIDTH * RE15_BSS_FRAME_HEIGHT * 3);
    if (!rgb) return 1;

    int files = 0, written = 0, failed = 0;
    for (int stage = 1; stage <= 6; stage++) {
        if (only_stage && stage != only_stage) continue;
        for (int rr = 0; rr <= 0xFF; rr++) {
            char p[512];
            long sz = 0;
            snprintf(p, sizeof p, "%s/STAGE%d/ROOM%X%02X.BSS", cd, stage, stage, rr);
            uint8_t *bss = slurp(p, &sz);
            if (!bss) continue;
            files++;
            int chunks = (int)(sz / CHUNK);
            for (int c = 0; c < chunks; c++) {
                if (decode_chunk(bss + (long)c * CHUNK, rgb) != 0) { failed++; continue; }
                snprintf(p, sizeof p, "%s/ROOM%X%02X%02d.ppm", out, stage, rr, c);
                FILE *o = fopen(p, "wb");
                if (!o) { failed++; continue; }
                fprintf(o, "P6\n%d %d\n255\n", RE15_BSS_FRAME_WIDTH, RE15_BSS_FRAME_HEIGHT);
                fwrite(rgb, 1, (size_t)RE15_BSS_FRAME_WIDTH * RE15_BSS_FRAME_HEIGHT * 3, o);
                fclose(o);
                written++;
            }
            free(bss);
        }
    }
    free(rgb);
    printf("BSS-Dateien %d | Bilder geschrieben %d | Fehlschlaege %d\n", files, written, failed);
    return failed ? 1 : 0;
}
