/*
 * Runde 39 Spur A "stimmen" — RIEGEL mit dem ECHTEN Lader (platform/pc/src/audio_pc.c re15_voice_load_clip, Haken
 * "Runde 39 Spur A"), RE15_AUDIO_CAP_SYNC ohne SDL-Geraet (Muster probe_r38_werkstatt11b0_ton).
 *
 * Gemessen wird der MECHANISMUS je Nutzer-Punkt: re15_audio_stimme(Raum, Nachricht) -> der Mitschnitt der naechsten
 * Bilder traegt die Abtastwerte der NUTZER-DATEI, die laut Tabelle zu dieser Nachricht gehoert (normierte
 * Kreuzkorrelation der auf 44100 Hz gerechneten Datei gegen das Mono-Mittel des Mitschnitts, lag 0..3 Bilder; der
 * Lader hebt leise Dateien an, der Pegelfaktor aendert die Korrelation nicht):
 *   1  ROOM1050 msg 22/23/24 -> main09/10/11.wav      (Punkt 1, vorher stumm: main22..24.wav gibt es nicht)
 *   2  ROOM1150 msg 22..29   -> main15..22.wav        (Punkt 2; 29 <- main22 seit Nutzer-Nachtrag 2), Irons' Zeilen
 *                                                     25..28 ueber den alten 4x-Deckel hinaus angehoben (Stufe 2,
 *                                                     re15_stimmen_pegel_r39.h: Faktor 6..16), Leon hoechstens 2,1x
 *   3  ROOM11B0 msg 20/21    -> main13/14.wav         (Punkt 3), msg 13/14 (Original-Untersuchungstexte) stumm,
 *                                                     msg 1 weiter main01.wav (Ordnerregel unberuehrt)
 *   4  ROOM1171 msg 3        -> main02.wav            (Punkt 4), msg 0/1 weiter main00/01.wav, msg 2 stumm
 */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_stimmen_r39.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif
#ifndef RE15_SYNCHRO_DIR
#define RE15_SYNCHRO_DIR "../synchro"
#endif

/* Symbole, die sonst main.c liefert (wie probe_r38_werkstatt11b0_ton). */
re15_engine_state_t g_engine;
void re15_debug_text(int x, int y, int z, const char *text) { (void)x; (void)y; (void)z; (void)text; }
unsigned char *re15_asset_read_file(const char *path, int *out_size)
{
    char buf[512];
    const char *pfx = "shared_assets/PSX/";
    if (strncmp(path, pfx, strlen(pfx)) == 0)
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path + strlen(pfx));
    else
        snprintf(buf, sizeof buf, "%s/%s", RE15_ASSET_PSX_DIR, path);
    FILE *f = fopen(buf, "rb");
    if (!f) { snprintf(buf, sizeof buf, "%s", path); f = fopen(buf, "rb"); }
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    unsigned char *b = (unsigned char *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && out_size) *out_size = (int)sz;
    return b;
}
extern void re15_audio_stimme(uint16_t room, int voice_id);   /* audio_pc.c (Runde 38 Spur F) = re15_voice_play */

#define CAP_DATEI "r39_stimmen_lader.pcm"
#define T 1470

static int s_fail = 0, s_pass = 0;
#define CHECK(nr, cond, ...) do {                                                  \
        if (cond) { s_pass++; printf("  PASS %2d: ", nr); printf(__VA_ARGS__); printf("\n"); } \
        else { printf("  FAIL %2d: ", nr); printf(__VA_ARGS__); printf("\n");      \
               if (!s_fail) s_fail = nr; } } while (0)

static int s_bild = 0;
static void ticks(int n) { for (int i = 0; i < n; i++) { re15_audio_tick(); s_bild++; } }

static double *s_cap = NULL; static long s_cap_n = 0;   /* Mono-Mittel des Mitschnitts, je Takt T Werte */
static int cap_lesen(void)
{
    FILE *f = fopen(CAP_DATEI, "rb"); if (!f) return 0;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    long n = sz / 4; int16_t *s = (int16_t *)malloc((size_t)n * 4);
    if (!s || fread(s, 4, (size_t)n, f) != (size_t)n) { fclose(f); free(s); return 0; }
    fclose(f);
    s_cap = (double *)malloc((size_t)n * sizeof(double)); s_cap_n = n;
    for (long i = 0; i < n; i++) s_cap[i] = ((double)s[2 * i] + s[2 * i + 1]) * 0.5;
    free(s);
    return 1;
}

/* Nutzer-Datei (PCM 16 bit) auf 44100 Hz, Mono */
static double *datei(unsigned raum, int nr, long *out_n)
{
    char p[600]; snprintf(p, sizeof p, "%s/STAGE%u/room%04X/main%02d.wav", RE15_SYNCHRO_DIR, raum >> 12, raum, nr);
    FILE *f = fopen(p, "rb"); if (!f) { printf("  (Datei fehlt: %s)\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz); if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return NULL; }
    fclose(f);
    long o = 12; int tag = 0, ch = 0, rate = 0, bits = 0; const uint8_t *d = NULL; long dn = 0;
    while (o + 8 <= sz) {
        uint32_t csz = (uint32_t)b[o + 4] | ((uint32_t)b[o + 5] << 8) | ((uint32_t)b[o + 6] << 16) | ((uint32_t)b[o + 7] << 24);
        if (!memcmp(b + o, "fmt ", 4)) {
            tag = b[o + 8] | (b[o + 9] << 8); ch = b[o + 10] | (b[o + 11] << 8);
            rate = (int)((uint32_t)b[o + 12] | ((uint32_t)b[o + 13] << 8) | ((uint32_t)b[o + 14] << 16) | ((uint32_t)b[o + 15] << 24));
            bits = b[o + 22] | (b[o + 23] << 8);
        } else if (!memcmp(b + o, "data", 4)) { d = b + o + 8; dn = (long)csz; if (o + 8 + dn > sz) dn = sz - o - 8; }
        o += 8 + (long)csz + (csz & 1);
    }
    if (!d || tag != 1 || bits != 16 || ch < 1 || rate <= 0) { printf("  (Format %s: tag %d bits %d)\n", p, tag, bits); free(b); return NULL; }
    long fr = dn / (2 * ch);
    long m = (long)((double)fr * 44100.0 / rate);
    double *x = (double *)malloc((size_t)m * sizeof(double));
    for (long i = 0; i < m; i++) {
        double pos = (double)i * rate / 44100.0; long j = (long)pos; double q = pos - j;
        long j0 = j < fr ? j : fr - 1, j1 = j + 1 < fr ? j + 1 : fr - 1; double a0 = 0, a1 = 0;
        for (int c = 0; c < ch; c++) {
            a0 += (int16_t)(d[(j0 * ch + c) * 2] | (d[(j0 * ch + c) * 2 + 1] << 8));
            a1 += (int16_t)(d[(j1 * ch + c) * 2] | (d[(j1 * ch + c) * 2 + 1] << 8));
        }
        x[i] = (a0 + (a1 - a0) * q) / ch;
    }
    free(b);
    *out_n = m;
    return x;
}

/* Korrelation der Datei mit dem Mitschnitt ab Bild b (lag 0..3), zusaetzlich Pegelfaktor */
static double korr(unsigned raum, int nr, int b, double *faktor, long *bilder)
{
    long n = 0; double *x = datei(raum, nr, &n); if (!x) return -9;
    if (bilder) *bilder = (n + T - 1) / T;
    double xx = 0; for (long i = 0; i < n; i++) xx += x[i] * x[i];
    double best = -2; if (faktor) *faktor = 0;
    for (int lag = 0; lag <= 3; lag++) {
        long s0 = (long)(b + lag) * T; if (s0 + n > s_cap_n) break;
        double xy = 0, yy = 0;
        for (long i = 0; i < n; i++) { xy += x[i] * s_cap[s0 + i]; yy += s_cap[s0 + i] * s_cap[s0 + i]; }
        double c = (xx > 0 && yy > 0) ? xy / sqrt(xx * yy) : 0;
        if (c > best) { best = c; if (faktor) *faktor = xx > 0 ? xy / xx : 0; }
    }
    free(x);
    return best;
}
static double energie(int von, int bis)
{
    double e = 0; long s0 = (long)von * T, s1 = (long)bis * T; if (s1 > s_cap_n) s1 = s_cap_n;
    for (long i = s0; i < s1; i++) e += fabs(s_cap[i]);
    return e;
}

typedef struct { unsigned raum; int msg; int datei; int bild; long bilder; } lauf_t;

int main(void)
{
    remove(CAP_DATEI);
    static char capenv[] = "RE15_AUDIO_CAP_SYNC=" CAP_DATEI;
    static char noint[] = "RE15_NO_INTRO=1";
    putenv(capenv); putenv(noint);
    re15_audio_init();
    ticks(5);

    /* Laeufe: je Zeile Stimme anfordern, Clip-Laenge + 10 Bilder Ruhe */
    lauf_t L[32]; int nl = 0;
    const int n = re15_stimmen_r39_anzahl();
    for (int i = 0; i < n; i++) {
        const re15_stimmen_r39_t *e = re15_stimmen_r39_eintrag(i);
        long nn = 0; double *x = datei(e->raum, e->datei, &nn); free(x);
        L[nl].raum = e->raum; L[nl].msg = e->nachricht; L[nl].datei = e->datei; L[nl].bild = s_bild;
        L[nl].bilder = nn > 0 ? (nn + T - 1) / T : 0; nl++;
        re15_audio_stimme((uint16_t)e->raum, e->nachricht);
        ticks((int)((nn + T - 1) / T) + 10);
    }
    /* Sperren und unberuehrte Regel */
    int b_11b0_13 = s_bild; re15_audio_stimme(0x11B0, 13); ticks(70);
    int b_11b0_14 = s_bild; re15_audio_stimme(0x11B0, 14); ticks(130);
    int b_11b0_1  = s_bild; re15_audio_stimme(0x11B0, 1);  ticks(160);
    int b_1171_2  = s_bild; re15_audio_stimme(0x1171, 2);  ticks(80);
    int b_1171_0  = s_bild; re15_audio_stimme(0x1171, 0);  ticks(70);
    int b_1170_3  = s_bild; re15_audio_stimme(0x1170, 3);  ticks(10);
    long n1170 = 0; { double *x = datei(0x1170, 3, &n1170); free(x); }
    ticks((int)(n1170 / T) + 10);
    ticks(20);                                                          /* Schreibpuffer leeren */
    re15_audio_shutdown();
    if (!cap_lesen()) { printf("FEHLER: Mitschnitt %s nicht lesbar\n", CAP_DATEI); return 1; }
    printf("Mitschnitt: %ld Takte\n", s_cap_n / T);

    int nr = 1;
    for (int i = 0; i < nl; i++) {
        double f = 0; double c = korr(L[i].raum, L[i].datei, L[i].bild, &f, NULL);
        /* Irons (1150 msg 25..28): Stufe 2 ueber den alten 4x-Deckel (Zensus: 7,4..12,6x); alle anderen: nur Stufe 1
         * (Zensus: hoechstens 2,1x) — "Leon unveraendert" */
        int irons = (L[i].raum == 0x1150 && L[i].msg >= 25 && L[i].msg <= 28);
        CHECK(nr++, c >= 0.9 && (irons ? (f >= 6.0 && f <= 16.0) : (f >= 0.9 && f <= 2.1)),
              "ROOM%04X msg %d -> main%02d.wav im Mitschnitt ab Bild %d: korr %.3f faktor %.2f (%ld Bilder)%s",
              L[i].raum, L[i].msg, L[i].datei, L[i].bild, c, f, L[i].bilder, irons ? " [Irons: Stufe 2 >= 6x]" : "");
    }
    CHECK(nr++, energie(b_11b0_13, b_11b0_13 + 70) == 0, "ROOM11B0 msg 13 (Untersuchungstext) stumm: main13.wav gehoert zu msg 20");
    CHECK(nr++, energie(b_11b0_14, b_11b0_14 + 130) == 0, "ROOM11B0 msg 14 (Rolltor-Text) stumm: main14.wav gehoert zu msg 21");
    { double f = 0; double c = korr(0x11B0, 1, b_11b0_1, &f, NULL);
      CHECK(nr++, c >= 0.9, "ROOM11B0 msg 1 weiter main01.wav (Ordnerregel): korr %.3f", c); }
    CHECK(nr++, energie(b_1171_2, b_1171_2 + 80) == 0, "ROOM1171 msg 2 'C'mon, give me a hand!' stumm: main02.wav gehoert zu msg 3");
    { double f = 0; double c = korr(0x1171, 0, b_1171_0, &f, NULL);
      CHECK(nr++, c >= 0.9, "ROOM1171 msg 0 weiter main00.wav (Ordnerregel): korr %.3f", c); }
    { double f = 0; double c = korr(0x1170, 3, b_1170_3, &f, NULL);
      CHECK(nr++, c >= 0.9, "ROOM1170 (Leon) msg 3 weiter main03.wav, von der 1171-Zeile unberuehrt: korr %.3f", c); }
    /* Gegenprobe des Masses: die 1171-Datei main02 liegt NICHT dort, wo msg 2 angefordert wurde */
    { double f = 0; double c = korr(0x1171, 2, b_1171_2, &f, NULL);
      CHECK(nr++, c < 0.2, "Gegenprobe: main02.wav nicht im msg-2-Fenster (korr %.3f)", c); }

    free(s_cap);
    remove(CAP_DATEI);
    printf("=== %s: %d bestanden, erste Verletzung %d ===\n", s_fail ? "ROT" : "GRUEN", s_pass, s_fail);
    return s_fail;
}
