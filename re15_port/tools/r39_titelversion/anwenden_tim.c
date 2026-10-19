/* Runde 39 Spur T "titelversion", Nachbesserung 1 - Hilfsprogramm (nicht Teil des Spiels, nicht im CMake-Bau):
 * schickt TITLEU.TIM durch re15_titelversion_r39_anwenden (dieselbe Funktion, die re15_render_pc_show_title ueber
 * re15_titelversion_r39_bild aufruft) und schreibt das Ergebnis als TIM - fuer das Vergleichsbild
 * analysis/befunde_runde39/T_belege/titel_vorprojekt_vergleich.png (vorprojekt_titel.py --port-tim).
 * Bau (Git-Bash, im Repo, msys64 zuerst im PATH):
 *   PATH=/c/msys64/mingw64/bin:/usr/bin gcc -std=c11 -O1 -DRE15_PLATFORM_PC -Ire15_port/include
 *     -Ire15_port/engine/src re15_port/tools/r39_titelversion/anwenden_tim.c re15_port/engine/src/titelversion_r39.c
 *     -o anwenden_tim.exe
 * Aufruf: anwenden_tim <TITLEU.TIM> <version, z. B. v0.8.27> <aus.TIM> */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "re15_titelversion_r39.h"

int main(int argc, char **argv)
{
    FILE *f;
    long n;
    unsigned char *b;
    uint16_t *px;
    char t[RE15_R39T_TEXT_MAX];
    int r;
    if (argc != 4) { fprintf(stderr, "Aufruf: %s <TITLEU.TIM> <version> <aus.TIM>\n", argv[0]); return 2; }
    if (!(f = fopen(argv[1], "rb"))) return 3;
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n != 0x14 + 320 * 240 * 2) { fprintf(stderr, "Groesse %ld statt %d\n", n, 0x14 + 320 * 240 * 2); return 4; }
    b = (unsigned char *) malloc((size_t) n);
    px = (uint16_t *) malloc(320 * 240 * 2);
    if (!b || !px || fread(b, 1, (size_t) n, f) != (size_t) n) return 5;
    fclose(f);
    memcpy(px, b + 0x14, 320 * 240 * 2);              /* Pixel ab Datei-Byte 0x14 (Dossier 1.1) */
    r = re15_titelversion_r39_anwenden(px, 320, 240, argv[2]);
    re15_titelversion_r39_text(argv[2], t, (int) sizeof t);
    printf("anwenden(\"%s\"): %d Pixel entfernt, Anzeige \"%s\"\n", argv[2], r, t);
    memcpy(b + 0x14, px, 320 * 240 * 2);
    if (!(f = fopen(argv[3], "wb"))) return 6;
    fwrite(b, 1, (size_t) n, f);
    fclose(f);
    return r < 0;
}
