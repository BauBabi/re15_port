/* lautstaerke_r41.c — Runde 41 Spur D: Lautstaerke-Option (Zustand + Datei). Belege: include/re15_lautstaerke_r41.h,
 * analysis/befunde_runde41/D_kleinkram_a.md §1. Kein SDL: die Endstufe liest nur re15_lautstaerke_r41_q15(). */
#include "re15_lautstaerke_r41.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Vor dem ersten laden() (Unit-Proben ohne main.c) unveraendert 100 %; der Programmstart ruft laden() vor dem ersten Ton
 * (main.c re15_lautstaerke_r41_pc_start). */
static int s_arbeit      = RE15_LAUT_R41_MAX;   /* S[7]-Analog: Arbeitskopie, sofort angewendet   */
static int s_gespeichert = RE15_LAUT_R41_MAX;   /* DAT_800b0fce-Analog: gespeicherter Stand       */
static int s_stereo      = 1;                   /* gespeicherter Sound-Modus (1 = Stereo)          */

static int klemmen(int p)
{
    if (p < 0) return 0;
    if (p > RE15_LAUT_R41_MAX) return RE15_LAUT_R41_MAX;
    return p;
}

/* Voreinstellung ohne Datei: 50 (Spur N, N-E2). MESSHAKEN: ein Mitschnitt (RE15_AUDIO_CAP / RE15_AUDIO_CAP_SYNC) OHNE
 * RE15_OPTIONS_FILE behaelt 100 %, damit die Pegel-Riegel der Suite die volle Mischung messen; wer die Endstufe messen will,
 * setzt RE15_OPTIONS_FILE (auch auf eine fehlende Datei -> 50). Kein Spielweg setzt diese Variablen. */
int re15_lautstaerke_r41_voreinstellung(void)
{
    const char *o = getenv(RE15_LAUT_R41_ENV);
    const char *c1 = getenv("RE15_AUDIO_CAP"), *c2 = getenv("RE15_AUDIO_CAP_SYNC");
    if (!(o && *o) && ((c1 && *c1) || (c2 && *c2))) return RE15_LAUT_R41_MAX;
    return RE15_LAUT_R41_STANDARD;
}

const char *re15_lautstaerke_r41_pfad(void)
{
    const char *e = getenv(RE15_LAUT_R41_ENV);
    return (e && *e) ? e : RE15_LAUT_R41_DATEI;
}

int re15_lautstaerke_r41_parse(const char *text, int *prozent, int *stereo)
{
    if (!text) return 0;
    int n = 0;
    const char *p = text;
    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (!*p) break;
        if (!strncmp(p, "volume=", 7)) {
            int v = atoi(p + 7);
            if (prozent) *prozent = klemmen(v);
            n++;
        } else if (!strncmp(p, "sound=", 6)) {
            const char *w = p + 6;
            if (!strncmp(w, "stereo", 6)) { if (stereo) *stereo = 1; n++; }
            else if (!strncmp(w, "mono", 4)) { if (stereo) *stereo = 0; n++; }
        }
        while (*p && *p != '\n') p++;
    }
    return n;
}

int re15_lautstaerke_r41_format(char *buf, size_t n, int prozent, int stereo)
{
    if (!buf || n == 0) return -1;
    int w = snprintf(buf, n, "volume=%d\nsound=%s\n", klemmen(prozent), stereo ? "stereo" : "mono");
    return (w > 0 && (size_t)w < n) ? w : -1;
}

void re15_lautstaerke_r41_reset(void)
{
    s_arbeit = s_gespeichert = re15_lautstaerke_r41_voreinstellung();
    s_stereo = 1;
}

void re15_lautstaerke_r41_laden(void)
{
    re15_lautstaerke_r41_reset();
    const char *pfad = re15_lautstaerke_r41_pfad();
    FILE *f = fopen(pfad, "rb");
    if (!f) {
        fprintf(stderr, "[lautstaerke] %s fehlt -> %d %%, Stereo\n", pfad, s_arbeit);
        return;
    }
    char text[1024];
    size_t got = fread(text, 1, sizeof text - 1, f);
    fclose(f);
    text[got] = 0;
    int p = re15_lautstaerke_r41_voreinstellung(), st = 1;
    int n = re15_lautstaerke_r41_parse(text, &p, &st);
    s_gespeichert = s_arbeit = klemmen(p);
    s_stereo = st ? 1 : 0;
    fprintf(stderr, "[lautstaerke] %s gelesen (%d Schluessel): %d %%, %s\n", pfad, n, s_arbeit,
            s_stereo ? "Stereo" : "Mono");
}

int re15_lautstaerke_r41_speichern(void)
{
    const char *pfad = re15_lautstaerke_r41_pfad();
    char text[64];
    if (re15_lautstaerke_r41_format(text, sizeof text, s_gespeichert, s_stereo) < 0) return -1;
    FILE *f = fopen(pfad, "wb");
    if (!f) {
        fprintf(stderr, "[lautstaerke] %s nicht schreibbar\n", pfad);
        return -1;
    }
    fputs(text, f);
    fclose(f);
    fprintf(stderr, "[lautstaerke] %s geschrieben: %d %%, %s\n", pfad, s_gespeichert, s_stereo ? "Stereo" : "Mono");
    return 0;
}

int  re15_lautstaerke_r41_prozent(void)      { return s_arbeit; }
/* N-E2: (Wert/100)^2 in Q15 = doppelte Absenkung in dB bei gleicher Reglerstellung. Hoechstwert 100 * 100 * 0x8000 passt in 32 Bit. */
int  re15_lautstaerke_r41_q15(void)          { return s_arbeit * s_arbeit * RE15_LAUT_R41_Q15_VOLL / (RE15_LAUT_R41_MAX * RE15_LAUT_R41_MAX); }
int  re15_lautstaerke_r41_gespeichert(void)  { return s_gespeichert; }
int  re15_lautstaerke_r41_stereo(void)       { return s_stereo; }
void re15_lautstaerke_r41_setzen(int prozent){ s_arbeit = klemmen(prozent); }

void re15_lautstaerke_r41_schritt(int richtung)
{
    if (richtung == 0) return;
    s_arbeit = klemmen(s_arbeit + (richtung > 0 ? RE15_LAUT_R41_SCHRITT : -RE15_LAUT_R41_SCHRITT));
}

int re15_lautstaerke_r41_eintritt(void)
{
    s_arbeit = s_gespeichert;                    /* @0x8002f244-4c: S[7] := DAT_800b0fce */
    return s_stereo;
}

int re15_lautstaerke_r41_bestaetigen(int stereo)
{
    s_gespeichert = s_arbeit;                    /* @0x8002f2c0: DAT_800b0fce := S[7] */
    s_stereo = stereo ? 1 : 0;
    return re15_lautstaerke_r41_speichern();
}

void re15_lautstaerke_r41_verwerfen(void)
{
    s_arbeit = s_gespeichert;                    /* @0x8002f27c: zurueck ohne Speichern */
}
