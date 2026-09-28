/* Nachtrag K: die Zeilen (Row-VM) des Granaten-Projektils 0x040D1000 = CORE00.ESP Effekt 4,
 * sub 0x0D (Tabelle sub&7 = 5; sub>>3 = 1 verschiebt nur die CLUT, re15_esp.c:170-183).
 * Spawn im Original: FSM @0x800336bc-0x800337a4 `lui a0,0x40d / ori a0,a0,0x1000` + jal 0x80019700.
 * Reines Messwerkzeug, gelinkt gegen libre15_engine.a (Parser re15_esp_parse_global). */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "re15_esp.h"
int main(int argc, char **argv)
{
    const char *p = argc > 1 ? argv[1] : "re15_port/shared_assets/PSX/DATA/CORE00.ESP";
    int sub = argc > 2 ? (int)strtol(argv[2], NULL, 0) : 0x0d;
    int eid = argc > 3 ? (int)strtol(argv[3], NULL, 0) : 4;
    FILE *f = fopen(p, "rb"); if (!f) { perror(p); return 1; }
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = malloc((size_t)n); fread(b, 1, (size_t)n, f); fclose(f);
    static re15_esp_t e;
    if (re15_esp_parse_global(b, (size_t)n, &e) != 0) { puts("parse fail"); return 1; }
    int ei = re15_esp_find_id(&e, (uint8_t)eid);
    int st = re15_esp_row_streams(&e, ei, sub);
    printf("CORE00.ESP (%ld B): Effekt %d = Index %d, eff_end @0x%X, sub 0x%02X -> Streams %d\n",
           n, eid, ei, e.eff[ei].eff_end, sub, st);
    for (int s = 0; s < st; s++) {
        int nr = 0; const uint8_t *r = re15_esp_row_stream(&e, ei, sub, s, &nr);
        printf(" Stream %d: %d Zeilen @Datei 0x%X\n", s, nr, (unsigned)(r - b));
        for (int i = 0; i < nr; i++) {
            const uint8_t *z = r + i * 40;
            #define H(o) ((int16_t)(z[o] | (z[(o)+1] << 8)))
            printf("  Z%02d @0x%05X A=%d B=%d wh=%d,%d acc=(%d,%d,%d) p0e=%d vel=(%d,%d,%d) p16=%d ang=(%d,%d,%d) p1e=%d eul=(%d,%d,%d) gate=%d\n",
                   i, (unsigned)(z - b), H(0), H(2), H(4), H(6), H(8), H(10), H(12), H(14), H(16), H(18), H(20),
                   H(22), H(24), H(26), H(28), H(30), H(32), H(34), H(36), H(38));
        }
    }
    return 0;
}
