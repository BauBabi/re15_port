/*
 * Runde 42 Strang "namen", Stufe 6 — RIEGEL mit dem ECHTEN Lader (platform/pc/src/audio_pc.c re15_voice_load_clip, Haken
 * "Runde 42 namen Stufe 6"), RE15_AUDIO_CAP_SYNC ohne SDL-Geraet (Muster probe_r39_stimmen_lader.c).
 *
 * PORT-WAHL (include/re15_altaufnahme_r42.h): 15 Saetze haben seit der Story v0.8.31 einen neuen Untertitel, ihre Datei
 * synchro/STAGEn/roomXXXX/mainNN.wav traegt noch die Aufnahme des alten Satzes. Eine Datei mit genau dem Fingerabdruck des Tags
 * v0.8.30 gilt wie KEINE Datei: die Zeile bleibt stumm. Jede andere Datei unter demselben Namen spielt.
 *
 * Gemessen wird am Mitschnitt des Mischers (44100 Hz, je Bild 1470 Abtastwerte):
 *   A  jede der 15 Dateien, die noch den Abdruck traegt (Groesse + FNV-1a-64, hier SELBST gerechnet): re15_audio_stimme(Raum,
 *      Nummer) -> Mitschnitt ueber die ganze Laenge der Aufnahme + 10 Bilder exakt 0, der Sprachkanal meldet "laeuft nicht".
 *      Traegt eine der 15 den Abdruck nicht mehr (der Nutzer hat sie ersetzt), muss sie SPIELEN (korr >= 0,9).
 *   B  NEGATIVKONTROLLE: jede andere Datei main00..main63 in denselben vier Ordnern (3020, 30E0, 6030, 6031) spielt weiter
 *      (normierte Kreuzkorrelation Datei gegen Mitschnitt >= 0,9, Sprachkanal "laeuft").
 * Aufruf `ersatz <Wurzel>`: die Probe liegt als Kopie in <Wurzel>, daneben <Wurzel>/synchro/... mit ERSETZTEN Dateien (der
 * Basis-Wurzelsucher asset_root_pc.c nimmt das exe-Verzeichnis zuerst; synchro/ im Repo wird nicht angefasst). Dann gilt A fuer
 * die Datei, die der Lader wirklich liest: ersetzte Dateien spielen, die uebrigen der 15 bleiben stumm.
 * Gegenprobe: rot auf dem Stand vor Stufe 6 (audio_pc.c ohne den Haken) - die alten Aufnahmen spielen dort (Teil A).
 */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE 1
#endif
#include "re15_audio.h"
#include "re15_engine.h"
#include "re15_altaufnahme_r42.h"

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

/* Symbole, die sonst main.c liefert (wie probe_r39_stimmen_lader). */
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
extern int g_re15_voice_laeuft;                               /* scd_vm.c, je Bild von re15_audio_tick gestempelt */

#define CAP_DATEI "r42_namen_altlader.pcm"
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

static uint8_t *lesen(const char *p, long *out_n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)(sz > 0 ? sz : 1));
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return NULL; }
    fclose(f);
    *out_n = sz;
    return b;
}

/* Datei (PCM 16 bit) auf 44100 Hz, Mono */
static double *pcm(const uint8_t *b, long sz, long *out_n)
{
    long o = 12; int tag = 0, ch = 0, rate = 0, bits = 0; const uint8_t *d = NULL; long dn = 0;
    if (sz < 44 || memcmp(b, "RIFF", 4) || memcmp(b + 8, "WAVE", 4)) return NULL;
    while (o + 8 <= sz) {
        uint32_t csz = (uint32_t)b[o + 4] | ((uint32_t)b[o + 5] << 8) | ((uint32_t)b[o + 6] << 16) | ((uint32_t)b[o + 7] << 24);
        if (!memcmp(b + o, "fmt ", 4)) {
            tag = b[o + 8] | (b[o + 9] << 8); ch = b[o + 10] | (b[o + 11] << 8);
            rate = (int)((uint32_t)b[o + 12] | ((uint32_t)b[o + 13] << 8) | ((uint32_t)b[o + 14] << 16) | ((uint32_t)b[o + 15] << 24));
            bits = b[o + 22] | (b[o + 23] << 8);
        } else if (!memcmp(b + o, "data", 4)) { d = b + o + 8; dn = (long)csz; if (o + 8 + dn > sz) dn = sz - o - 8; break; }
        o += 8 + (long)csz + (csz & 1);
    }
    if (!d || tag != 1 || bits != 16 || ch < 1 || rate <= 0) return NULL;
    long fr = dn / (2 * ch);
    long m = (long)((double)fr * 44100.0 / rate);
    double *x = (double *)malloc((size_t)(m > 0 ? m : 1) * sizeof(double));
    for (long i = 0; i < m; i++) {
        double pos = (double)i * rate / 44100.0; long j = (long)pos; double q = pos - j;
        long j0 = j < fr ? j : fr - 1, j1 = j + 1 < fr ? j + 1 : fr - 1; double a0 = 0, a1 = 0;
        for (int c = 0; c < ch; c++) {
            a0 += (int16_t)(d[(j0 * ch + c) * 2] | (d[(j0 * ch + c) * 2 + 1] << 8));
            a1 += (int16_t)(d[(j1 * ch + c) * 2] | (d[(j1 * ch + c) * 2 + 1] << 8));
        }
        x[i] = (a0 + (a1 - a0) * q) / ch;
    }
    *out_n = m;
    return x;
}

/* Korrelation der Datei mit dem Mitschnitt ab Bild b (lag 0..3) */
static double korr(const double *x, long n, int b)
{
    double xx = 0; for (long i = 0; i < n; i++) xx += x[i] * x[i];
    double best = -2;
    for (int lag = 0; lag <= 3; lag++) {
        long s0 = (long)(b + lag) * T; if (s0 + n > s_cap_n) break;
        double xy = 0, yy = 0;
        for (long i = 0; i < n; i++) { xy += x[i] * s_cap[s0 + i]; yy += s_cap[s0 + i] * s_cap[s0 + i]; }
        double c = (xx > 0 && yy > 0) ? xy / sqrt(xx * yy) : 0;
        if (c > best) best = c;
    }
    return best;
}
static double energie(int von, int bis)
{
    double e = 0; long s0 = (long)von * T, s1 = (long)bis * T; if (s1 > s_cap_n) s1 = s_cap_n;
    for (long i = s0; i < s1; i++) e += fabs(s_cap[i]);
    return e;
}

/* eigene FNV-1a-64 (unabhaengig von engine/src/altaufnahme_r42.c) */
static uint64_t fnv64(const uint8_t *b, long n)
{
    uint64_t h = 0xcbf29ce484222325ull;
    for (long i = 0; i < n; i++) { h ^= b[i]; h *= 0x100000001b3ull; }
    return h;
}

typedef struct {
    unsigned raum; int nr; int in_tabelle; int alt;   /* alt = Datei, die der Lader liest, traegt den Abdruck des Tags */
    int ersetzt_lokal;                                /* Datei kommt aus <Wurzel>/synchro (Aufruf ersatz)             */
    int bild, bilder, laeuft;
    double *x; long xn;
} lauf_t;

static int tabelle(unsigned raum, int nr)
{
    for (int i = 0; i < re15_altaufnahme_r42_anzahl(); i++) {
        const re15_altaufnahme_r42_t *e = re15_altaufnahme_r42_eintrag(i);
        if (e->raum == raum && e->datei == nr) return i;
    }
    return -1;
}

int main(int argc, char **argv)
{
    const char *ersatz = (argc > 2 && !strcmp(argv[1], "ersatz")) ? argv[2] : NULL;
    remove(CAP_DATEI);
    static char capenv[] = "RE15_AUDIO_CAP_SYNC=" CAP_DATEI;
    static char noint[] = "RE15_NO_INTRO=1";
    static char voice[] = "RE15_VOICE=1";
    putenv(capenv); putenv(noint); putenv(voice);
    re15_audio_init();
    ticks(5);

    static const unsigned raeume[4] = { 0x3020, 0x30E0, 0x6030, 0x6031 };
    static lauf_t L[128]; int nl = 0;
    for (int r = 0; r < 4; r++) {
        for (int nr = 0; nr < 64 && nl < 128; nr++) {
            char p[900]; long sz = 0; uint8_t *b = NULL; int lokal = 0;
            if (ersatz) {
                snprintf(p, sizeof p, "%s/synchro/STAGE%u/room%04X/main%02d.wav", ersatz, raeume[r] >> 12, raeume[r], nr);
                b = lesen(p, &sz); lokal = b != NULL;
            }
            if (!b) {
                snprintf(p, sizeof p, "%s/STAGE%u/room%04X/main%02d.wav", RE15_SYNCHRO_DIR, raeume[r] >> 12, raeume[r], nr);
                b = lesen(p, &sz);
            }
            if (!b) continue;
            lauf_t *l = &L[nl++];
            memset(l, 0, sizeof *l);
            l->raum = raeume[r]; l->nr = nr; l->ersetzt_lokal = lokal;
            const int k = tabelle(raeume[r], nr);
            l->in_tabelle = k >= 0;
            if (k >= 0) {
                const re15_altaufnahme_r42_t *e = re15_altaufnahme_r42_eintrag(k);
                l->alt = ((long)e->groesse == sz && e->fnv == fnv64(b, sz));
            }
            l->x = pcm(b, sz, &l->xn);
            free(b);
            l->bilder = l->x ? (int)((l->xn + T - 1) / T) : 300;
            l->bild = s_bild;
            re15_audio_stimme((uint16_t)l->raum, l->nr);
            ticks(3);
            l->laeuft = g_re15_voice_laeuft;
            ticks(l->bilder + 10 - 3);
        }
    }
    ticks(20);                                                          /* Schreibpuffer leeren */
    re15_audio_shutdown();
    if (!cap_lesen()) { printf("FEHLER: Mitschnitt %s nicht lesbar\n", CAP_DATEI); return 1; }
    printf("Mitschnitt: %ld Takte, %d Dateien angefordert%s%s\n", s_cap_n / T, nl, ersatz ? ", Ersatz-Wurzel " : "", ersatz ? ersatz : "");

    int nr = 1, n_alt = 0, n_neu = 0, n_andere = 0, n_lokal = 0;
    for (int i = 0; i < nl; i++) {
        lauf_t *l = &L[i];
        if (l->in_tabelle && l->alt) {
            n_alt++;
            const double e = energie(l->bild, l->bild + l->bilder + 10);
            const double c = l->x ? korr(l->x, l->xn, l->bild) : 0;
            CHECK(nr++, e == 0 && l->laeuft == 0,
                  "A  ROOM%04X msg %2d: main%02d.wav traegt den Abdruck des Tags (alte Aufnahme) -> STUMM ueber %d Bilder "
                  "(Energie %.0f, Sprachkanal laeuft %d, korr %.3f)", l->raum, l->nr, l->nr, l->bilder + 10, e, l->laeuft, c);
        } else if (l->in_tabelle) {
            n_neu++; n_lokal += l->ersetzt_lokal;
            const double c = l->x ? korr(l->x, l->xn, l->bild) : -9;
            CHECK(nr++, c >= 0.9 && l->laeuft == 1,
                  "A  ROOM%04X msg %2d: main%02d.wav ist ERSETZT (%s, anderer Abdruck) -> SPIELT (korr %.3f, Sprachkanal laeuft %d)",
                  l->raum, l->nr, l->nr, l->ersetzt_lokal ? "Ersatz-Wurzel" : "Bestand", c, l->laeuft);
        } else {
            n_andere++;
            const double c = l->x ? korr(l->x, l->xn, l->bild) : -9;
            CHECK(nr++, c >= 0.9 && l->laeuft == 1,
                  "B  ROOM%04X msg %2d: main%02d.wav (nicht in der Liste) spielt weiter (korr %.3f, Sprachkanal laeuft %d)", l->raum,
                  l->nr, l->nr, c, l->laeuft);
        }
        free(l->x);
    }
    printf("  von den 15: %d stumm (alte Aufnahme), %d spielen (ersetzt, davon %d aus der Ersatz-Wurzel); %d andere Dateien spielen\n",
           n_alt, n_neu, n_lokal, n_andere);
    CHECK(nr++, n_alt + n_neu == 15, "alle 15 Saetze der Liste gemessen (%d stumm + %d ersetzt)", n_alt, n_neu);
    CHECK(nr++, n_andere >= 30, "Negativkontrolle: %d andere Dateien in 3020/30E0/6030/6031 gemessen", n_andere);
    if (ersatz) {
        int soll = argc > 3 ? atoi(argv[3]) : 1;
        CHECK(nr++, n_lokal == soll && n_neu >= soll, "Ersatz-Lauf: %d ersetzte Dateien aus der Ersatz-Wurzel gespielt (Soll %d)", n_lokal, soll);
    }

    free(s_cap);
    remove(CAP_DATEI);
    printf("=== %s: %d bestanden, erste Verletzung %d ===\n", s_fail ? "ROT" : "GRUEN", s_pass, s_fail);
    return s_fail;
}
