/*
 * Runde 42 Strang "namen", Stufe 6 — TESTWERKZEUG (kein Spielcode): baut aus einer vorhandenen Aufnahme eine "ersetzte" Datei fuer
 * die Riegel, ohne synchro/ anzufassen. Die Riegel legen das Ergebnis in eine eigene Ersatz-Wurzel neben eine Kopie der exe
 * (asset_root_pc.c nimmt das exe-Verzeichnis zuerst) und pruefen damit: eine Datei mit ANDEREM Fingerabdruck spielt.
 *
 *   werkzeug_r42_altaufnahme_ersatz anhang <quelle.wav> <ziel.wav>
 *       haengt einen leeren RIFF-Abschnitt "r42e" an und fuehrt die RIFF-Laenge nach: gueltige WAV, Ton Abtastwert fuer
 *       Abtastwert gleich (wav_find_data in audio_pc.c liest nur `fmt ` und `data`), 8 Byte laenger.
 *   werkzeug_r42_altaufnahme_ersatz bit <quelle.wav> <ziel.wav>
 *       kippt das unterste Bit des letzten Bytes: gleiche Groesse, ein Bit anders.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc != 4 || (strcmp(argv[1], "anhang") && strcmp(argv[1], "bit"))) {
        fprintf(stderr, "Aufruf: %s anhang|bit <quelle.wav> <ziel.wav>\n", argv[0]);
        return 2;
    }
    FILE *f = fopen(argv[2], "rb");
    if (!f) { fprintf(stderr, "Quelle nicht lesbar: %s\n", argv[2]); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz + 9);
    if (!b || sz < 44 || fread(b, 1, (size_t)sz, f) != (size_t)sz || memcmp(b, "RIFF", 4) || memcmp(b + 8, "WAVE", 4)) {
        fprintf(stderr, "keine WAV: %s\n", argv[2]); fclose(f); free(b); return 1;
    }
    fclose(f);
    long n = sz;
    if (!strcmp(argv[1], "anhang")) {
        if (n & 1) b[n++] = 0;                                   /* Abschnitte sind wortbuendig */
        memcpy(b + n, "r42e", 4); memset(b + n + 4, 0, 4); n += 8;
        const uint32_t riff = (uint32_t)(n - 8);
        b[4] = (uint8_t)riff; b[5] = (uint8_t)(riff >> 8); b[6] = (uint8_t)(riff >> 16); b[7] = (uint8_t)(riff >> 24);
    } else {
        b[n - 1] ^= 1;
    }
    FILE *o = fopen(argv[3], "wb");
    if (!o || fwrite(b, 1, (size_t)n, o) != (size_t)n) { fprintf(stderr, "Ziel nicht schreibbar: %s\n", argv[3]); if (o) fclose(o); free(b); return 1; }
    fclose(o); free(b);
    printf("%s: %s (%ld Byte) -> %s (%ld Byte)\n", argv[1], argv[2], sz, argv[3], n);
    return 0;
}
