/* Sonde p2 / pri-masken (2026-09-19): die ORIGINAL-Vordergrundatlanten (SLD-Trailer im
 * BSS-Chunk) ueber den ENGINE-Dekoder als TIM-Dateien ablegen — fuer die Kalibrierung
 * der Tiefenregel an den Kuenstler-Masken (tools/maske/kalib_geometrie.py).
 *
 * WARUM ueber die Engine: der Python-Nachbau des Dekompressors (tools/maske/original.py
 * sld_decompress) scheitert an ROOM1020/1030/1040/1070 ("bytearray index out of range"),
 * genau den 188 Cuts, fuer die der Java-Extraktor nie eine PRI##.TIM erzeugt hat. Der
 * Engine-Weg (re15_sld_atlas_from_chunk = FUN_800c47e8, re15_sld.h) dekodiert alle 359
 * Cuts mit 0 Fehlern (test_sld_atlas.c). Live statt Nachbildung.
 *
 * Aufruf: probe_p2_atlas_dump <ausgabeverzeichnis> [stage]   -> <out>/ROOM%04X_ORIG%02d.TIM
 * Kein add_test: reine Messsonde. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_sld.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#define CHUNK_SIZE 0x10000

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = sz;
    return b;
}

static uint8_t s_atlas[RE15_SLD_MAX_UNPACKED];

int main(int argc, char **argv)
{
    const char *out = argc > 1 ? argv[1] : "build/p2/atlas";
    int st_von = 1, st_bis = 6, stage, n_ok = 0, n_none = 0, n_fail = 0;
    if (argc > 2) { st_von = st_bis = atoi(argv[2]); }
    for (stage = st_von; stage <= st_bis; stage++) {
        char p[600]; long sbsz = 0;
        snprintf(p, sizeof p, "%s/BIN/STAGE%d.BIN", RE15_ASSET_PSX_DIR, stage);
        uint8_t *sb = slurp(p, &sbsz);
        if (!sb) { printf("STAGE%d.BIN nicht lesbar\n", stage); n_fail++; continue; }
        for (int rr = 0; rr <= 0xFF; rr++) {
            long bsz = 0;
            snprintf(p, sizeof p, "%s/STAGE%d/ROOM%X%02X.BSS", RE15_ASSET_PSX_DIR, stage, stage, rr);
            uint8_t *bss = slurp(p, &bsz);
            if (!bss) continue;
            int chunks = (int)(bsz / CHUNK_SIZE);
            if (chunks > RE15_SLD_TBL_COLS) chunks = RE15_SLD_TBL_COLS;
            for (int cut = 0; cut < chunks; cut++) {
                uint16_t L = 0; int len = 0;
                if (re15_sld_used_len(sb, (int)sbsz, stage, rr, cut, &L) != RE15_SLD_OK) { n_fail++; continue; }
                int rv = re15_sld_atlas_from_chunk(bss + (long)cut * CHUNK_SIZE, CHUNK_SIZE, L,
                                                   s_atlas, (int)sizeof s_atlas, &len);
                if (rv == RE15_SLD_NO_FOREGROUND) { n_none++; continue; }
                if (rv != RE15_SLD_OK) { printf("STAGE%d ROOM%X%02X cut%d: rv=%d\n", stage, stage, rr, cut, rv); n_fail++; continue; }
                snprintf(p, sizeof p, "%s/ROOM%X%02X0_ORIG%02d.TIM", out, stage, rr, cut);
                FILE *f = fopen(p, "wb");
                if (!f) { printf("nicht schreibbar: %s\n", p); n_fail++; continue; }
                fwrite(s_atlas, 1, (size_t)len, f); fclose(f); n_ok++;
            }
            free(bss);
        }
        free(sb);
    }
    printf("Atlanten geschrieben: %d | ohne Vordergrund: %d | Fehler: %d -> %s\n", n_ok, n_none, n_fail, out);
    return n_fail ? 1 : 0;
}
