/* PROBE (Messung, kein Test): dekodiert ROOM1090 BG13.BSS byte-true ueber die
 * Engine-Kette re15_bss_parse_chunk -> re15_bss_vlc_decode -> re15_bss_mdec_decode
 * und schreibt das reine HINTERGRUNDBILD als PPM. Zweck: beweisen, dass die vom
 * Nutzer gemeldeten schwarzen Dreiecke NICHT im vorgerenderten Hintergrund stecken,
 * sondern gezeichnete Geometrie sind. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_bss.h"

int main(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1]
        : "C:/workspace/git/reAi_v2/re15_port/shared_assets/PSX/BSS/ROOM1090/BG13.BSS";
    const char *out  = (argc > 2) ? argv[2]
        : "C:/workspace/git/reAi_v2/shots/bg13_1090.ppm";
    FILE *f = fopen(path, "rb");
    if (!f) { printf("FAIL open %s\n", path); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (fread(buf, 1, (size_t)sz, f) != (size_t)sz) { printf("FAIL read\n"); return 1; }
    fclose(f);

    re15_bss_chunk_t chunk;
    if (!re15_bss_parse_chunk(buf, (size_t)sz, &chunk)) { printf("FAIL parse\n"); return 1; }
    if (!re15_bss_chunk_has_video(&chunk))              { printf("FAIL novideo\n"); return 1; }
    size_t cap = ((size_t)chunk.run_length_words + 2) * 4;
    int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
    int n = re15_bss_vlc_decode(chunk.vlc_payload, chunk.vlc_payload_size,
                                chunk.run_length_words, chunk.quant, chunk.version, co, cap);
    if (n < 0) { printf("FAIL vlc %d\n", n); return 1; }
    uint8_t *rgb = (uint8_t *)malloc(320 * 240 * 3);
    if (re15_bss_mdec_decode(co, (size_t)n, 320, 240, rgb) != 0) { printf("FAIL mdec\n"); return 1; }
    FILE *o = fopen(out, "wb");
    if (!o) { printf("FAIL out %s\n", out); return 1; }
    fprintf(o, "P6\n320 240\n255\n");
    fwrite(rgb, 1, 320 * 240 * 3, o);
    fclose(o);
    printf("OK %s -> %s (%ld B, vlc %d coeffs)\n", path, out, sz, n);
    return 0;
}
