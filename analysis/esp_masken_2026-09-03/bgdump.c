/* Standalone BG dumper: BSS chunk -> 320x240 RGB -> PPM.
 * Uses the port's own decoders (bss_common.c, bss_vlc.c, bss_mdec.c). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_bss.h"

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "usage: bgdump <in.BSS> <out.ppm>\n"); return 2; }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *buf = malloc((size_t)sz);
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { fprintf(stderr,"short read\n"); return 1; }
    fclose(f);

    re15_bss_chunk_t ch;
    if (!re15_bss_parse_chunk(buf, (size_t)sz, &ch)) { fprintf(stderr,"parse fail\n"); return 1; }
    fprintf(stderr, "%s: rl=%u id=0x%04X quant=%u ver=%u\n", argv[1],
            ch.run_length_words, ch.id, ch.quant, ch.version);
    if (!re15_bss_chunk_has_video(&ch)) { fprintf(stderr,"no video id\n"); return 1; }

    size_t cap = ((size_t)ch.run_length_words + 2) * 8 + 4096;
    int16_t *co = malloc(cap * sizeof(int16_t));
    int n = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size,
                                ch.run_length_words, ch.quant, ch.version, co, cap);
    if (n < 0) { fprintf(stderr,"vlc fail %d\n", n); return 1; }
    fprintf(stderr, "  vlc coeffs = %d\n", n);

    uint8_t *rgb = malloc(320*240*3);
    if (re15_bss_mdec_decode(co, (size_t)n, 320, 240, rgb) != 0) { fprintf(stderr,"mdec fail\n"); return 1; }

    FILE *o = fopen(argv[2], "wb");
    fprintf(o, "P6\n320 240\n255\n");
    fwrite(rgb, 1, 320*240*3, o);
    fclose(o);
    fprintf(stderr, "  wrote %s\n", argv[2]);
    return 0;
}
