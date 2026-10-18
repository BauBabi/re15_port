/*
 * Runde 39 Spur A "stimmen" — AUSWERTE-WERKZEUG (kein add_test) fuer RE15_AUDIO_CAP_SYNC-Mitschnitte der echten exe.
 *
 *   fit <cap.raw> <state.log> <abschnitt> <bild|msg:ID> <wav> [<bild|msg:ID> <wav> ...]
 *       Mitschnitt = Stereo s16 44100 Hz, 1470 Frames je Spielbild (audio_pc.c RE15_AUDIO_CAP_SYNC). state.log
 *       (RE15_STATE_LOG) traegt je Bild eine Zeile "F<n> ... msg(a=<aktiv> fsm=<f> id=<id> ...)"; die Bildzaehlung
 *       beginnt in jedem Raum neu -> Raum-Abschnitte 0, 1, 2 ... Abschnitt k beginnt bei Takt
 *       (Takte - Summe der Bildzahlen der Abschnitte k, k+1, ...) (Muster probe_r38_elzaintro_auswertung: der letzte
 *       Abschnitt liegt bei Takte - (Endbild + 1)). "msg:ID" = erstes Bild des Abschnitts, in dem die Nachricht ID
 *       aktiv ist (a != 0).
 *       Je Zeile: die Aufnahme (PCM 16 bit, mono/stereo, beliebige Rate) wird wie der Lader linear auf 44100 Hz
 *       gerechnet und im Fenster ab Bild+lag (lag -2..+4) gegen das Mono-Mittel des Mitschnitts gefittet:
 *         korr      = normierte Kreuzkorrelation (1,0 = die Datei liegt unverfaelscht im Ton; BGM/SE senken sie)
 *         faktor    = dot(x,y)/dot(x,x) ueber den ganzen Clip (Pegelfaktor gegenueber der Datei; der Lader hebt
 *                     leise Aufnahmen bis 4x an, audio_pc.c "Level-floor")
 *         median    = Median der Pegelfaktoren je 0,5-s-Segment mit Stimmenergie
 *         konsistenz= Anteil der Segmente, deren Faktor im Band median +-50 % liegt (ein nicht gespielter Clip
 *                     streut um 0 mit wechselndem Vorzeichen; ein gespielter Clip haelt den Faktor ueber alle
 *                     Segmente, auch unter lauter BGM — gemessen Dossier A_stimmen.md §5)
 *       Ausgabe je Zeile: "zeile F<bild> <datei>: lag <k> korr <c> faktor <g> median <m> konsistenz <a>/<b> rms <r>
 *                            laenge <s> stimme <v> rest <q> klemm <z>"
 *         stimme    = Effektivwert der Stimme im Fenster, wie gespielt (|faktor| * rms der Datei)
 *         rest      = Effektivwert des Rests (BGM/SE) = sqrt(rms^2 - stimme^2) - Stimme/Rest = Hoerbarkeit (Nachbesserung 1)
 *         klemm     = Zahl der Abtastwerte (L+R) im Fenster, die am Anschlag +-32767 liegen (Uebersteuerung der Summe)"
 *   rms <cap.raw> <state.log> <abschnitt> <bild|msg:ID> <laenge>
 *       RMS des Mono-Mittels im Fenster [bild, bild+laenge): "fenster F<b>..F<e> rms <r>"
 *
 * Exit 0 bei auswertbarem Lauf (die Schwellen prueft das aufrufende Skript), 1 bei Lese-/Formatfehler, 2 bei Aufruf.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define T 1470
#define MAX_ABSCHNITTE 32

static long s_len[MAX_ABSCHNITTE];        /* Bilder je Raum-Abschnitt */
static int  s_n_abschnitte = 0;
static char *s_state = NULL;              /* state.log im Speicher */
static long s_state_n = 0;

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz + 1);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return NULL; }
    fclose(f); b[sz] = 0;
    if (n) *n = sz;
    return b;
}

/* Abschnitte aus state.log: jede Zeile "F<n> ..."; faellt n, beginnt ein neuer Raum-Abschnitt. */
static int abschnitte_lesen(const char *pfad)
{
    s_state = (char *)slurp(pfad, &s_state_n);
    if (!s_state) { printf("FEHLER: state.log %s\n", pfad); return 0; }
    long prev = -1; long maxf = -1;
    const char *p = s_state;
    while (*p) {
        const char *e = strchr(p, '\n'); if (!e) e = p + strlen(p);
        if (*p == 'F' && p[1] >= '0' && p[1] <= '9') {
            long f = strtol(p + 1, NULL, 10);
            if (f < prev) {
                if (s_n_abschnitte < MAX_ABSCHNITTE) s_len[s_n_abschnitte++] = maxf + 1;
                maxf = -1;
            }
            if (f > maxf) maxf = f;
            prev = f;
        }
        p = (*e) ? e + 1 : e;
    }
    if (maxf >= 0 && s_n_abschnitte < MAX_ABSCHNITTE) s_len[s_n_abschnitte++] = maxf + 1;
    return s_n_abschnitte > 0;
}

/* Erstes Bild im Abschnitt k, in dem msg(a=.. id=ID) aktiv ist; -1 wenn nie. */
static long msg_bild(int k, int id)
{
    int akt = 0; long prev = -1;
    const char *p = s_state;
    while (*p) {
        const char *e = strchr(p, '\n'); if (!e) e = p + strlen(p);
        if (*p == 'F' && p[1] >= '0' && p[1] <= '9') {
            long f = strtol(p + 1, NULL, 10);
            if (f < prev) akt++;
            prev = f;
            if (akt == k) {
                const char *m = strstr(p, "msg(a=");
                if (m && m < e) {
                    int a = atoi(m + 6);
                    const char *i = strstr(m, " id=");
                    if (a != 0 && i && i < e && atoi(i + 4) == id) return f;
                }
            } else if (akt > k) break;
        }
        p = (*e) ? e + 1 : e;
    }
    return -1;
}

static long bild_parsen(const char *arg, int k)
{
    if (!strncmp(arg, "msg:", 4)) {
        long b = msg_bild(k, atoi(arg + 4));
        if (b < 0) printf("FEHLER: Nachricht %s im Abschnitt %d nie aktiv\n", arg + 4, k);
        return b;
    }
    return strtol(arg, NULL, 10);
}

/* WAV (RIFF, fmt tag 1, 16 bit) -> Mono double, Rate. */
static double *wav_lesen(const char *pfad, long *out_n, int *out_rate)
{
    long n = 0; uint8_t *b = slurp(pfad, &n);
    if (!b || n < 12 || memcmp(b, "RIFF", 4) || memcmp(b + 8, "WAVE", 4)) { printf("FEHLER: WAV %s\n", pfad); free(b); return NULL; }
    long o = 12; int tag = 0, ch = 0, rate = 0, bits = 0; const uint8_t *data = NULL; long dn = 0;
    while (o + 8 <= n) {
        uint32_t sz = (uint32_t)b[o + 4] | ((uint32_t)b[o + 5] << 8) | ((uint32_t)b[o + 6] << 16) | ((uint32_t)b[o + 7] << 24);
        if (!memcmp(b + o, "fmt ", 4) && o + 8 + 16 <= n) {
            tag = b[o + 8] | (b[o + 9] << 8); ch = b[o + 10] | (b[o + 11] << 8);
            rate = (int)((uint32_t)b[o + 12] | ((uint32_t)b[o + 13] << 8) | ((uint32_t)b[o + 14] << 16) | ((uint32_t)b[o + 15] << 24));
            bits = b[o + 22] | (b[o + 23] << 8);
            if (tag == 0xFFFE && sz >= 26) tag = b[o + 8 + 24] | (b[o + 8 + 25] << 8);   /* WAVE_FORMAT_EXTENSIBLE */
        } else if (!memcmp(b + o, "data", 4)) {
            data = b + o + 8; dn = (long)sz; if (o + 8 + dn > n) dn = n - o - 8;
        }
        o += 8 + (long)sz + (sz & 1);
    }
    if (!data || tag != 1 || bits != 16 || ch < 1 || rate <= 0) {
        printf("FEHLER: %s: tag=%d bits=%d ch=%d rate=%d (nur PCM 16 bit)\n", pfad, tag, bits, ch, rate); free(b); return NULL;
    }
    long frames = dn / (2 * ch);
    double *m = (double *)malloc((size_t)frames * sizeof(double));
    for (long i = 0; i < frames; i++) {
        double s = 0;
        for (int c = 0; c < ch; c++) s += (int16_t)(data[(i * ch + c) * 2] | (data[(i * ch + c) * 2 + 1] << 8));
        m[i] = s / ch;
    }
    free(b);
    *out_n = frames; *out_rate = rate;
    return m;
}

/* linear auf 44100 Hz (wie audio_pc.c re15_voice_load_clip). */
static double *auf_44100(const double *a, long n, int rate, long *out_n)
{
    long m = (long)((double)n * 44100.0 / rate);
    double *x = (double *)malloc((size_t)m * sizeof(double));
    for (long i = 0; i < m; i++) {
        double pos = (double)i * rate / 44100.0; long j = (long)pos; double fr = pos - j;
        double a0 = a[j < n ? j : n - 1], a1 = a[j + 1 < n ? j + 1 : n - 1];
        x[i] = a0 + (a1 - a0) * fr;
    }
    *out_n = m;
    return x;
}

static int cmp_d(const void *a, const void *b) { double x = *(const double *)a, y = *(const double *)b; return x < y ? -1 : x > y; }

int main(int argc, char **argv)
{
    if (argc < 6) { printf("Aufruf: fit|rms <cap.raw> <state.log> <abschnitt> ...\n"); return 2; }
    long cn = 0; uint8_t *cb = slurp(argv[2], &cn);
    if (!cb) { printf("FEHLER: Mitschnitt %s\n", argv[2]); return 1; }
    long takte = cn / (T * 4);
    double *cap = (double *)malloc((size_t)takte * T * sizeof(double));
    long *klemm = (long *)calloc((size_t)(takte > 0 ? takte : 1), sizeof(long));   /* Anschlag-Werte je Takt */
    for (long i = 0; i < takte * T; i++) {
        int16_t l = (int16_t)(cb[i * 4] | (cb[i * 4 + 1] << 8)), r = (int16_t)(cb[i * 4 + 2] | (cb[i * 4 + 3] << 8));
        cap[i] = ((double)l + r) * 0.5;
        if (l >= 32767 || l <= -32768) klemm[i / T]++;
        if (r >= 32767 || r <= -32768) klemm[i / T]++;
    }
    free(cb);
    if (!abschnitte_lesen(argv[3])) return 1;
    int k = atoi(argv[4]);
    if (k < 0 || k >= s_n_abschnitte) { printf("FEHLER: Abschnitt %d (es gibt %d)\n", k, s_n_abschnitte); return 1; }
    long nach = 0; for (int j = k; j < s_n_abschnitte; j++) nach += s_len[j];
    long off = takte - nach;
    printf("takte %ld abschnitte %d abschnitt %d bilder %ld takt0 %ld\n", takte, s_n_abschnitte, k, s_len[k], off);
    if (off < 0) { printf("FEHLER: Mitschnitt kuerzer als die Abschnitte\n"); return 1; }

    if (!strcmp(argv[1], "rms")) {
        if (argc < 7) return 2;
        long b = bild_parsen(argv[5], k); if (b < 0) return 1;
        long ln = strtol(argv[6], NULL, 10);
        long s0 = (off + b) * T, s1 = (off + b + ln) * T; if (s1 > takte * T) s1 = takte * T;
        double e = 0; for (long i = s0; i < s1; i++) e += cap[i] * cap[i];
        printf("fenster F%ld..F%ld rms %.0f\n", b, b + ln - 1, s1 > s0 ? sqrt(e / (s1 - s0)) : 0.0);
        return 0;
    }
    if (strcmp(argv[1], "fit")) return 2;
    for (int a = 5; a + 1 < argc; a += 2) {
        long b = bild_parsen(argv[a], k); if (b < 0) return 1;
        long wn = 0; int rate = 0; double *w = wav_lesen(argv[a + 1], &wn, &rate); if (!w) return 1;
        long n = 0; double *x = auf_44100(w, wn, rate, &n); free(w);
        double xx = 0; for (long i = 0; i < n; i++) xx += x[i] * x[i];
        int best_k = 0; double best_c = -2, best_g = 0, best_rms = 0;
        for (int lag = -2; lag <= 4; lag++) {
            long s0 = (off + b + lag) * T;
            if (s0 < 0 || s0 + n > takte * T) continue;
            double xy = 0, yy = 0;
            for (long i = 0; i < n; i++) { xy += x[i] * cap[s0 + i]; yy += cap[s0 + i] * cap[s0 + i]; }
            double c = (xx > 0 && yy > 0) ? xy / sqrt(xx * yy) : 0;
            if (c > best_c) { best_c = c; best_k = lag; best_g = xx > 0 ? xy / xx : 0; best_rms = sqrt(yy / n); }
        }
        /* Segmentfaktoren (0,5 s = 15 Bilder) */
        long s0 = (off + b + best_k) * T, seg = 15L * T;
        double fak[4096]; int nf = 0;
        for (long s = 0; s + seg <= n && nf < 4096; s += seg) {
            double sxx = 0, sxy = 0;
            for (long i = s; i < s + seg; i++) { sxx += x[i] * x[i]; sxy += x[i] * cap[s0 + i]; }
            if (sxx >= 0.02 * xx * seg / n) fak[nf++] = sxy / sxx;
        }
        double med = 0; int kons = 0;
        if (nf) {
            double srt[4096]; memcpy(srt, fak, (size_t)nf * sizeof(double)); qsort(srt, (size_t)nf, sizeof(double), cmp_d);
            med = srt[nf / 2];
            for (int i = 0; i < nf; i++) if (fabs(fak[i] - med) <= 0.5 * fabs(med)) kons++;
        }
        const char *base = strrchr(argv[a + 1], '/'); base = base ? base + 1 : argv[a + 1];
        { const char *bs = strrchr(base, '\\'); if (bs) base = bs + 1; }
        /* Stimme/Rest im Fenster (Nachbesserung 1): Stimme = |faktor| * rms(Datei), Rest = Fenster ohne Stimme */
        double stimme = fabs(best_g) * sqrt(xx / (n > 0 ? n : 1));
        double rest = best_rms * best_rms > stimme * stimme ? sqrt(best_rms * best_rms - stimme * stimme) : 0.0;
        long kl = 0; { long t0 = off + b + best_k, t1 = t0 + (n + T - 1) / T; if (t0 < 0) t0 = 0; if (t1 > takte) t1 = takte;
                       for (long tt = t0; tt < t1; tt++) kl += klemm[tt]; }
        printf("zeile F%ld %s: lag %d korr %.3f faktor %.2f median %.2f konsistenz %d/%d rms %.0f laenge %.1f stimme %.0f rest %.0f klemm %ld\n",
               b, base, best_k, best_c, best_g, med, kons, nf, best_rms, (double)n / T, stimme, rest, kl);
        free(x);
    }
    free(cap); free(klemm); free(s_state);
    return 0;
}
