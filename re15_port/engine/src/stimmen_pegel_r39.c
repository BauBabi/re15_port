/*
 * RE1.5 Rebuilt — Lautheits-Boden der Sprachaufnahmen, Stufe 2 (Runde 39 Spur A "stimmen", Nachbesserung 1).
 *
 * Herleitung der Konstanten T/R/C aus dem Bestand und die Belege: include/re15_stimmen_pegel_r39.h und
 * analysis/befunde_runde39/A_stimmen.md §9. Aufgerufen aus audio_pc.c re15_voice_load_clip direkt nach der
 * Spitzen-Stufe ("Level-floor", Ziel 14745, Deckel 4x). Ganzzahlig (kein libm), PSX-neutral (fprintf nur PC).
 */
#include "re15_stimmen_pegel_r39.h"

#include <stdio.h>

/* floor(sqrt(v)) fuer v >= 0 (bitweise, ohne libm) */
static int isqrt64(int64_t v)
{
    if (v <= 0) return 0;
    int64_t r = 0, bit = (int64_t)1 << 62;
    while (bit > v) bit >>= 2;
    while (bit) {
        if (v >= r + bit) { v -= r + bit; r = (r >> 1) + bit; }
        else r >>= 1;
        bit >>= 2;
    }
    return (int)r;
}

static void messen(const int16_t *pcm, long n, int *rms, int *spitze)
{
    int64_t sq = 0; int pk = 1;
    for (long i = 0; i < n; i++) {
        int32_t a = pcm[i];
        sq += (int64_t)a * a;
        if (a < 0) a = -a;
        if (a > pk) pk = a;
    }
    *rms = isqrt64(sq / n);
    *spitze = pk;
}

int re15_stimmen_pegel_r39(int16_t *pcm, long n, const char *stamm, re15_stimmen_pegel_t *info)
{
    re15_stimmen_pegel_t t = { 0, 0, 0, 0, 256, 0 };
    if (!pcm || n <= 0) { if (info) *info = t; return 256; }
    messen(pcm, n, &t.rms_vorher, &t.spitze_vorher);
    t.rms_nachher = t.rms_vorher; t.spitze_nachher = t.spitze_vorher;
    if (t.rms_vorher <= 0 || t.rms_vorher >= RE15_STIMMEN_PEGEL_T) { if (info) *info = t; return 256; }

    /* Faktor Q8: auf R anheben, aber die Spitze nie ueber C (kein Clipping) */
    int g   = RE15_STIMMEN_PEGEL_R * 256 / t.rms_vorher;
    int lim = RE15_STIMMEN_PEGEL_C * 256 / t.spitze_vorher;
    if (lim < g) { g = lim; t.begrenzt = 1; }
    if (g <= 256) { t.begrenzt = 0; if (info) *info = t; return 256; }   /* Spitze schon an C: nichts zu tun */

    for (long i = 0; i < n; i++) {
        int32_t s = (int32_t)(((int64_t)pcm[i] * g) >> 8);
        if (s > 32767) s = 32767; else if (s < -32768) s = -32768;      /* unerreichbar: |pcm|*g/256 <= C */
        pcm[i] = (int16_t)s;
    }
    messen(pcm, n, &t.rms_nachher, &t.spitze_nachher);
    t.faktor_q8 = g;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[voice] pegel %s.wav: rms %d -> %d (x%d.%02d), spitze %d -> %d%s (Runde 39 Spur A Stufe 2: T=%d R=%d C=%d)\n",
            stamm ? stamm : "?", t.rms_vorher, t.rms_nachher, g / 256, (g % 256) * 100 / 256,
            t.spitze_vorher, t.spitze_nachher, t.begrenzt ? " (Spitzenbegrenzung C)" : "",
            RE15_STIMMEN_PEGEL_T, RE15_STIMMEN_PEGEL_R, RE15_STIMMEN_PEGEL_C);
#endif
    if (info) *info = t;
    return g;
}
