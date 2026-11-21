/*
 * Runde 42 Strang "fabrikplan", Stufe 2 (Nachtrag 20.11.2026) - RIEGEL mit dem ECHTEN Lader (platform/pc/src/audio_pc.c
 * re15_voice_load_clip ueber re15_audio_stimme), RE15_AUDIO_CAP_SYNC ohne SDL-Geraet (Muster probe_r42_namen_altlader.c).
 *
 * Nutzer 20.11.2026 ~18:30 woertlich: "Ok, saemtliche Sprachaufnahmen liegen vor." (Commit ce5ec405f, 213 Dateien).
 * Liste: tests/unit/gen_r42_aufnahmen_v0831.inc (tools/r42_namen/aufnahmen_v0831_bauen.py aus FEHLENDE_AUFNAHMEN_2026-11-20.md).
 *
 * Je Spiel-Satz der Teile A (13, neuer Text, alte Aufnahme war stumm geschaltet), B (1) und C (61, ganz neue Saetze):
 *   re15_audio_stimme(Raum, Id) laeuft durch genau den Lade-Weg des Spiels - alle Umleitungen (birkinfilm_r42, elza_stimme_datei,
 *   stimmen_r39, rekorder_r38, dining_r39, johnruf_r39, elzaintro_r38) und die Stumm-Tabelle altaufnahme_r42. Geprueft:
 *     (1) die Datei synchro/STAGEn/roomXXXX/mainNN.wav liegt da und traegt NICHT den Abdruck der Stumm-Tabelle
 *         (re15_altaufnahme_r42_ist == 0),
 *     (2) der Sprachkanal laeuft (g_re15_voice_laeuft == 1, 3 Bilder nach dem Start),
 *     (3) im Mitschnitt des Mischers steckt GENAU diese Datei (normierte Kreuzkorrelation Datei gegen Mitschnitt >= 0,9) - eine
 *         Umleitung auf eine fremde Datei oder eine stumme Zeile faellt hier auf.
 * Ausgabe zusaetzlich als Tabelle (Teil, Raum, Id, Sprecher, Datei, korr, Ergebnis) fuer den Bericht.
 * INFO ohne Pruefung: ROOM1151 msg 2 (Elza/Irons, Teil G) und die 35 Saetze aus Teil D (v0.8.32) - ob v0.8.31 diese Ids ZEIGT,
 * entscheidet nicht der Lader, sondern das Raumskript (Bericht BERICHT_s2_r1.md, Punkt 3 und 4).
 * Gegenprobe: rot auf dem Stand vor dem Merge von master (1e5ab9f22, ohne die Aufnahmen): 61 Dateien aus C fehlen, 13 aus A
 * tragen den Abdruck der Stumm-Tabelle.
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

/* Symbole, die sonst main.c liefert (wie probe_r42_namen_altlader). */
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

#define CAP_DATEI "r42_aufnahmen_v0831.pcm"
#define T 1470

static int s_fail = 0, s_pass = 0;
#define CHECK(nr, cond, ...) do {                                                  \
        const int check_nr_ = (nr);                                                \
        if (cond) { s_pass++; printf("  PASS %3d: ", check_nr_); printf(__VA_ARGS__); printf("\n"); } \
        else { printf("  FAIL %3d: ", check_nr_); printf(__VA_ARGS__); printf("\n"); \
               if (!s_fail) s_fail = check_nr_; } } while (0)

static int s_bild = 0;
static void ticks(int n) { for (int i = 0; i < n; i++) { re15_audio_tick(); s_bild++; } }

static double *s_cap = NULL; static long s_cap_n = 0;
static int cap_lesen(void)
{
    FILE *f = fopen(CAP_DATEI, "rb"); if (!f) return 0;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    long n = sz / 4; int16_t *s = (int16_t *)malloc((size_t)(n > 0 ? n : 1) * 4);
    if (!s || fread(s, 4, (size_t)n, f) != (size_t)n) { fclose(f); free(s); return 0; }
    fclose(f);
    s_cap = (double *)malloc((size_t)(n > 0 ? n : 1) * sizeof(double)); s_cap_n = n;
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

/* Datei (PCM 16 bit) auf 44100 Hz, Mono - wie probe_r42_namen_altlader */
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

static const struct { char teil; unsigned raum; int nr; const char *sprecher; const char *text; } k_satz[] = {
#include "gen_r42_aufnahmen_v0831.inc"
};
#define SATZ_N ((int)(sizeof k_satz / sizeof k_satz[0]))

typedef struct {
    int i;                          /* Index in k_satz; -1 = ROOM1151 msg 2 (INFO) */
    unsigned raum; int nr; char teil;
    int da, abdruck, bild, bilder, laeuft;
    double *x; long xn;
} lauf_t;

int main(void)
{
    remove(CAP_DATEI);
    static char capenv[] = "RE15_AUDIO_CAP_SYNC=" CAP_DATEI;
    static char noint[] = "RE15_NO_INTRO=1";
    static char voice[] = "RE15_VOICE=1";
    putenv(capenv); putenv(noint); putenv(voice);
    re15_audio_init();
    ticks(5);

    static lauf_t L[SATZ_N + 1]; int nl = 0;
    int n_teil[4] = { 0 };
    for (int i = 0; i <= SATZ_N; i++) {
        lauf_t *l = &L[nl];
        memset(l, 0, sizeof *l);
        if (i < SATZ_N) {
            if (k_satz[i].teil < 'A' || k_satz[i].teil > 'D') continue;
            n_teil[k_satz[i].teil - 'A']++;
            l->i = i; l->raum = k_satz[i].raum; l->nr = k_satz[i].nr; l->teil = k_satz[i].teil;
        } else {
            l->i = -1; l->raum = 0x1151u; l->nr = 2; l->teil = 'G';       /* Irons in Elzas Buero (Teil G) */
        }
        char p[600], rel[80]; long sz = 0;
        snprintf(rel, sizeof rel, "synchro/STAGE%u/room%04X/main%02d.wav", l->raum >> 12, l->raum, l->nr);
        snprintf(p, sizeof p, "%s/STAGE%u/room%04X/main%02d.wav", RE15_SYNCHRO_DIR, l->raum >> 12, l->raum, l->nr);
        uint8_t *b = lesen(p, &sz);
        l->da = b != NULL;
        if (b) {
            l->abdruck = re15_altaufnahme_r42_ist(rel, b, (size_t)sz);
            l->x = pcm(b, sz, &l->xn);
            free(b);
        }
        nl++;
        if (l->teil == 'D') continue;                                     /* v0.8.32: nicht abspielen, nur INFO */
        l->bilder = l->x ? (int)((l->xn + T - 1) / T) : 120;
        l->bild = s_bild;
        re15_audio_stimme((uint16_t)l->raum, l->nr);
        ticks(3);
        l->laeuft = g_re15_voice_laeuft;
        ticks(l->bilder + 10 - 3);
    }
    ticks(20);
    re15_audio_shutdown();
    if (!cap_lesen()) { printf("FEHLER: Mitschnitt %s nicht lesbar\n", CAP_DATEI); return 1; }
    printf("Mitschnitt: %ld Takte; Liste: A %d, B %d, C %d, D %d Saetze\n", s_cap_n / T, n_teil[0], n_teil[1], n_teil[2], n_teil[3]);

    int nr = 1, spielt[3] = { 0 }, n_abc = 0;
    printf("\n| Teil | Raum | Id | Sprecher | Datei | Abdruck Stumm-Tabelle | Sprachkanal | korr | Ergebnis |\n"
           "|---|---|---|---|---|---|---|---|---|\n");
    for (int k = 0; k < nl; k++) {
        lauf_t *l = &L[k];
        if (l->teil < 'A' || l->teil > 'C') continue;
        n_abc++;
        const double c = l->x ? korr(l->x, l->xn, l->bild) : -9;
        const int ok = l->da && !l->abdruck && l->laeuft == 1 && c >= 0.9;
        spielt[l->teil - 'A'] += ok;
        printf("| %c | %04X | %d | %s | main%02d.wav%s | %s | %s | %.3f | %s |\n", l->teil, l->raum, l->nr, k_satz[l->i].sprecher,
               l->nr, l->da ? "" : " FEHLT", l->abdruck ? "JA (stumm)" : "nein", l->laeuft ? "laeuft" : "still", c,
               ok ? "spielt" : "STUMM/FALSCH");
    }
    printf("\n");
    for (int k = 0; k < nl; k++) {
        lauf_t *l = &L[k];
        if (l->teil < 'A' || l->teil > 'C') continue;
        const double c = l->x ? korr(l->x, l->xn, l->bild) : -9;
        CHECK(nr++, l->da && !l->abdruck && l->laeuft == 1 && c >= 0.9,
              "%c ROOM%04X msg %2d (%s: \"%s\"): main%02d.wav %s, Abdruck der Stumm-Tabelle %d, Sprachkanal laeuft %d, korr %.3f -> %s",
              l->teil, l->raum, l->nr, k_satz[l->i].sprecher, k_satz[l->i].text, l->nr, l->da ? "da" : "FEHLT", l->abdruck, l->laeuft, c,
              (l->da && !l->abdruck && l->laeuft == 1 && c >= 0.9) ? "SPIELT mit der Nutzer-Datei" : "NICHT");
    }
    CHECK(nr++, n_teil[0] == 13 && n_teil[1] == 1 && n_teil[2] == 61 && n_teil[3] == 35,
          "Liste vollstaendig: A %d (13), B %d (1), C %d (61), D %d (35)", n_teil[0], n_teil[1], n_teil[2], n_teil[3]);
    CHECK(nr++, spielt[0] == 13 && spielt[1] == 1 && spielt[2] == 61,
          "alle 75 Spiel-Saetze v0.8.31 spielen ihre Nutzer-Datei: A %d/13, B %d/1, C %d/61", spielt[0], spielt[1], spielt[2]);

    /* INFO: 1151 msg 2 und Teil D */
    for (int k = 0; k < nl; k++) {
        lauf_t *l = &L[k];
        if (l->teil == 'G') {
            const double e = energie(l->bild, l->bild + l->bilder + 10);
            printf("  INFO: ROOM1151 msg 2 (Irons, Teil G): eigene Datei %s, Sprachkanal %s, Energie %.0f - keine Umleitung auf "
                   "room1150/main02.wav (ROOM1151 sub03 @0x00EFA `2b 02 00 00` wird seit Runde 38 Spur Q durch Text 20 ersetzt, "
                   "kleinkram_r38.c A_IRONS_TEXT; msg 2 erscheint im Port nie)\n", l->da ? "da" : "fehlt",
                   l->laeuft ? "laeuft" : "still", e);
        }
    }
    int d_da = 0;
    for (int k = 0; k < nl; k++) d_da += (L[k].teil == 'D' && L[k].da);
    printf("  INFO: Teil D (v0.8.32): %d von %d Dateien liegen unter synchro/ - nicht abgespielt; dass v0.8.31 diese Ids nicht zeigt, "
           "belegt der Bericht (RDT-Nachrichten, Message_on im SCD, Port-Programme)\n", d_da, n_teil[3]);
    for (int k = 0; k < nl; k++) free(L[k].x);

    free(s_cap);
    remove(CAP_DATEI);
    printf("=== %s: %d bestanden, erste Verletzung %d ===\n", s_fail ? "ROT" : "GRUEN", s_pass, s_fail);
    return s_fail;
}
