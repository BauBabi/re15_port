/*
 * Runde 42 Strang "namen", Stufe 7 - TESTWERKZEUG: steckt eine Aufnahme im Tonmitschnitt eines Laufs?
 *
 *   werkzeug_r42_tonmess <mitschnitt.raw> <SPIELT|STUMM>[@<Tick>]=<datei.wav> ...
 *
 * <mitschnitt.raw> = Mitschnitt des Mischers aus dem echten Spiel (RE15_AUDIO_CAP oder RE15_AUDIO_CAP_SYNC: s16le, Stereo,
 * 44100 Hz; mit CAP_SYNC je Spielbild genau 1470 Stereo-Abtastwerte, audio_pc.c re15_audio_tick). Je Aufnahme wird der GANZE
 * Mitschnitt durchsucht - auch Musik, Raumton und die anderen Stimmen liegen darin.
 *
 * MASS (wie das Pruefer-Werkzeug der Stufe 6, pruef_s6_r1/ton_mess.py, hier in C ohne Fremdbibliothek): beide Signale Mono auf
 * 11025 Hz; normierte Kreuzkorrelation der ganzen Aufnahme gegen jeden Ausschnitt gleicher Laenge (schnelle Fourier-
 * Transformation). Spitze 1,0 = an dieser Stelle ist nur diese Aufnahme zu hoeren. Dazu der ABSTAND = Spitze / (99,9-%-Wert der
 * Korrelation ausserhalb von +-1 s um die Spitze).
 * URTEIL "spielt": Spitze >= 0,25 UND Abstand >= 4. Die zwei Schwellen sind Schwellen dieses MESSWERKZEUGS (kein Spielverhalten),
 * uebernommen vom Pruefer der Stufe 6; gemessen dort und hier: eine Aufnahme, die spielt, liegt bei 0,37 bis 0,97 mit Abstand
 * >= 5, eine, die nicht spielt, bei hoechstens 0,22 mit Abstand < 3,5.
 * @<Tick> (nur SPIELT, nur CAP_SYNC): die Spitze muss hoechstens 0,5 s neben Tick/30 s liegen - der Tick steht im Log der
 * Sprachausgabe ("[voice] Start ROOM.. Id .. capTick N"), die Aufnahme spielt also an IHREM Untertitel.
 *
 * Ausgabe je Datei eine Zeile; Rueckgabe 0 = jede Erwartung erfuellt, 1 = mindestens eine verletzt, 2 = Eingabe nicht lesbar.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FS 11025
#define SCHWELLE_SPITZE 0.25
#define SCHWELLE_ABSTAND 4.0

static uint8_t *lesen(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)(sz > 0 ? sz : 1));
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return NULL; }
    fclose(f);
    *n = sz;
    return b;
}

/* WAV (PCM 16 bit, beliebig viele Kanaele und Rate) -> Mono FS: gleitendes Mittel ueber rate/FS Werte, dann linear abgetastet */
static double *wav_mono(const char *p, long *out_n)
{
    long sz = 0; uint8_t *b = lesen(p, &sz);
    if (!b) return NULL;
    long o = 12; int tag = 0, ch = 0, rate = 0, bits = 0; const uint8_t *d = NULL; long dn = 0;
    if (sz < 44 || memcmp(b, "RIFF", 4) || memcmp(b + 8, "WAVE", 4)) { free(b); return NULL; }
    while (o + 8 <= sz) {
        const uint32_t csz = (uint32_t)b[o + 4] | ((uint32_t)b[o + 5] << 8) | ((uint32_t)b[o + 6] << 16) | ((uint32_t)b[o + 7] << 24);
        if (!memcmp(b + o, "fmt ", 4)) {
            tag = b[o + 8] | (b[o + 9] << 8); ch = b[o + 10] | (b[o + 11] << 8);
            rate = (int)((uint32_t)b[o + 12] | ((uint32_t)b[o + 13] << 8) | ((uint32_t)b[o + 14] << 16) | ((uint32_t)b[o + 15] << 24));
            bits = b[o + 22] | (b[o + 23] << 8);
        } else if (!memcmp(b + o, "data", 4)) { d = b + o + 8; dn = (long)csz; if (o + 8 + dn > sz) dn = sz - o - 8; break; }
        o += 8 + (long)csz + (long)(csz & 1);
    }
    if (!d || tag != 1 || bits != 16 || ch < 1 || rate <= 0) { free(b); return NULL; }
    const long fr = dn / (2 * ch);
    double *v = (double *)malloc((size_t)(fr > 0 ? fr : 1) * sizeof(double));
    for (long i = 0; i < fr; i++) {
        double a = 0;
        for (int c = 0; c < ch; c++) a += (int16_t)(d[(i * ch + c) * 2] | (d[(i * ch + c) * 2 + 1] << 8));
        v[i] = a / ch;
    }
    free(b);
    int k = rate / FS; if (k < 1) k = 1;
    if (k > 1) {                                        /* gleitendes Mittel, zentriert */
        double *g = (double *)malloc((size_t)(fr > 0 ? fr : 1) * sizeof(double));
        double s = 0; const long h0 = k / 2, h1 = k - 1 - h0;
        for (long i = -h1; i < fr; i++) {
            const long rein = i + h1, raus = i - h0 - 1;
            if (rein < fr) s += v[rein];
            if (raus >= 0) s -= v[raus];
            if (i >= 0) g[i] = s / k;
        }
        free(v); v = g;
    }
    const long m = (long)((double)fr * FS / rate);
    double *x = (double *)malloc((size_t)(m > 0 ? m : 1) * sizeof(double));
    for (long i = 0; i < m; i++) {
        const double pos = (double)i * rate / FS; long j = (long)pos; const double q = pos - (double)j;
        const long j0 = j < fr ? j : fr - 1, j1 = j + 1 < fr ? j + 1 : fr - 1;
        x[i] = v[j0] + (v[j1] - v[j0]) * q;
    }
    free(v);
    *out_n = m;
    return x;
}

/* Mitschnitt s16le Stereo 44100 -> Mono FS (Mittel ueber 4 Abtastwerte) */
static double *cap_mono(const char *p, long *out_n)
{
    long sz = 0; uint8_t *b = lesen(p, &sz);
    if (!b) return NULL;
    const long fr = sz / 4, m = fr / 4;
    double *x = (double *)malloc((size_t)(m > 0 ? m : 1) * sizeof(double));
    for (long i = 0; i < m; i++) {
        double a = 0;
        for (int q = 0; q < 4; q++) {
            const uint8_t *s = b + (i * 4 + q) * 4;
            a += ((int16_t)(s[0] | (s[1] << 8)) + (int16_t)(s[2] | (s[3] << 8))) * 0.5;
        }
        x[i] = a * 0.25;
    }
    free(b);
    *out_n = m;
    return x;
}

/* komplexe FFT, Laenge n = Zweierpotenz, richtung +1 = hin, -1 = zurueck (ohne 1/n) */
static double *s_cos = NULL, *s_sin = NULL; static long s_tw_n = 0;
static void fft(double *re, double *im, long n, int richtung)
{
    if (s_tw_n != n) {
        free(s_cos); free(s_sin);
        s_cos = (double *)malloc((size_t)(n / 2) * sizeof(double)); s_sin = (double *)malloc((size_t)(n / 2) * sizeof(double));
        for (long i = 0; i < n / 2; i++) { const double w = -2.0 * 3.14159265358979323846 * (double)i / (double)n; s_cos[i] = cos(w); s_sin[i] = sin(w); }
        s_tw_n = n;
    }
    for (long i = 1, j = 0; i < n; i++) {
        long bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) { double t = re[i]; re[i] = re[j]; re[j] = t; t = im[i]; im[i] = im[j]; im[j] = t; }
    }
    for (long len = 2; len <= n; len <<= 1) {
        const long halb = len >> 1, schritt = n / len;
        for (long i = 0; i < n; i += len)
            for (long k = 0; k < halb; k++) {
                const double wr = s_cos[k * schritt], wi = richtung > 0 ? s_sin[k * schritt] : -s_sin[k * schritt];
                const double ur = re[i + k], ui = im[i + k];
                const double vr = re[i + k + halb] * wr - im[i + k + halb] * wi, vi = re[i + k + halb] * wi + im[i + k + halb] * wr;
                re[i + k] = ur + vr; im[i + k] = ui + vi;
                re[i + k + halb] = ur - vr; im[i + k + halb] = ui - vi;
            }
    }
}

static int dbl_cmp(const void *a, const void *b)
{
    const double x = *(const double *)a, y = *(const double *)b;
    return x < y ? -1 : (x > y ? 1 : 0);
}

int main(int argc, char **argv)
{
    if (argc < 3) { fprintf(stderr, "Aufruf: %s <mitschnitt.raw> <SPIELT|STUMM>[@Tick]=<datei.wav> ...\n", argv[0]); return 2; }
    long nc = 0; double *cap = cap_mono(argv[1], &nc);
    if (!cap || nc < FS) { printf("FEHLER: Mitschnitt %s nicht lesbar oder kuerzer als 1 s\n", argv[1]); return 2; }
    double rms = 0; for (long i = 0; i < nc; i++) rms += cap[i] * cap[i];
    printf("Mitschnitt %s: %.1f s, Effektivwert %.0f\n", argv[1], (double)nc / FS, sqrt(rms / (double)nc));
    long n = 1; while (n < nc + 60L * FS) n <<= 1;       /* Platz fuer Aufnahmen bis 60 s */
    double *cre = (double *)calloc((size_t)n, sizeof(double)), *cim = (double *)calloc((size_t)n, sizeof(double));
    double *vre = (double *)malloc((size_t)n * sizeof(double)), *vim = (double *)malloc((size_t)n * sizeof(double));
    double *cs = (double *)malloc((size_t)(nc + 1) * sizeof(double)), *e = (double *)malloc((size_t)nc * sizeof(double));
    double *sorte = (double *)malloc((size_t)nc * sizeof(double));
    if (!cre || !cim || !vre || !vim || !cs || !e || !sorte) { printf("FEHLER: Speicher\n"); return 2; }
    memcpy(cre, cap, (size_t)nc * sizeof(double));
    fft(cre, cim, n, +1);
    cs[0] = 0; for (long i = 0; i < nc; i++) cs[i + 1] = cs[i] + cap[i] * cap[i];

    int verletzt = 0, gemessen = 0;
    for (int a = 2; a < argc; a++) {
        char *gl = strchr(argv[a], '=');
        if (!gl) { printf("FEHLER: Argument '%s' ohne '='\n", argv[a]); return 2; }
        const int soll_spielt = !strncmp(argv[a], "SPIELT", 6);
        if (!soll_spielt && strncmp(argv[a], "STUMM", 5)) { printf("FEHLER: Argument '%s' (SPIELT= oder STUMM=)\n", argv[a]); return 2; }
        long tick = -1;
        { const char *at = strchr(argv[a], '@'); if (at && at < gl) tick = atol(at + 1); }
        const char *pfad = gl + 1;
        long nv = 0; double *v = wav_mono(pfad, &nv);
        if (!v || nv < 16) { printf("  FEHLER: %s nicht lesbar (PCM 16 bit erwartet)\n", pfad); verletzt++; free(v); continue; }
        if (nv >= nc || nv > 60L * FS) { printf("  FEHLER: %s laenger als der Mitschnitt oder als 60 s\n", pfad); verletzt++; free(v); continue; }
        double vv = 0; for (long i = 0; i < nv; i++) vv += v[i] * v[i];
        memset(vre, 0, (size_t)n * sizeof(double)); memset(vim, 0, (size_t)n * sizeof(double));
        memcpy(vre, v, (size_t)nv * sizeof(double));
        fft(vre, vim, n, +1);
        for (long i = 0; i < n; i++) {                   /* cap * konj(v) */
            const double r = cre[i] * vre[i] + cim[i] * vim[i], m = cim[i] * vre[i] - cre[i] * vim[i];
            vre[i] = r; vim[i] = m;
        }
        fft(vre, vim, n, -1);
        const long nl = nc - nv;                        /* gueltige Lagen */
        long npos = 0;
        for (long i = 0; i < nl; i++) { e[i] = cs[i + nv] - cs[i]; if (e[i] > 0) sorte[npos++] = e[i]; }
        double boden = 1.0;
        if (npos > 0) { qsort(sorte, (size_t)npos, sizeof(double), dbl_cmp); boden = 1e-9 + 0.05 * sorte[npos / 2]; }
        long k = 0; double best = -2;
        for (long i = 0; i < nl; i++) {
            const double en = e[i] > boden ? e[i] : boden;
            const double c = (vre[i] / (double)n) / sqrt(en * vv);
            e[i] = c;
            if (c > best) { best = c; k = i; }
        }
        long nr = 0;
        for (long i = 0; i < nl; i++) if (i < k - FS || i >= k + FS) sorte[nr++] = fabs(e[i]);
        double streu = 1e-9;
        if (nr > 0) { qsort(sorte, (size_t)nr, sizeof(double), dbl_cmp); streu = sorte[(long)((double)(nr - 1) * 0.999)]; if (streu < 1e-9) streu = 1e-9; }
        const double abstand = best / streu, zeit = (double)k / FS;
        const int spielt = best >= SCHWELLE_SPITZE && abstand >= SCHWELLE_ABSTAND;
        int ok = spielt == soll_spielt;
        char lage[80] = "";
        if (soll_spielt && tick >= 0) {
            const double soll_zeit = (double)tick / 30.0;
            const int lok = fabs(zeit - soll_zeit) <= 0.5;
            snprintf(lage, sizeof lage, ", Start laut Log bei %.2f s%s", soll_zeit, lok ? "" : " <- NICHT an dieser Stelle");
            ok = ok && lok;
        }
        const char *name = pfad; { const char *q = strstr(pfad, "room"); if (q) name = q; }
        printf("  %s %-26s %-12s Spitze %.3f bei %7.2f s, Abstand %5.1f (Soll %s%s)\n", ok ? "PASS" : "FAIL", name,
               spielt ? "SPIELT" : "spielt nicht", best, zeit, abstand, soll_spielt ? "SPIELT" : "STUMM", lage);
        verletzt += !ok; gemessen++;
        free(v);
    }
    printf("=== %s: %d Aufnahmen gemessen, %d Erwartungen verletzt ===\n", verletzt ? "ROT" : "GRUEN", gemessen, verletzt);
    return verletzt ? 1 : 0;
}
