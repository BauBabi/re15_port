/* probe_r38_elzatoene_kreuz.c — AUSWERTE-WERKZEUG Spur H (Runde 38), kein add_test.
 *
 * Misst, OB und WANN ein bestimmter Ton im RE15_AUDIO_CAP_SYNC-Mitschnitt der echten exe erklingt: normierte
 * Kreuzkorrelation des Tons (so dekodiert und getaktet wie der Mischer in platform/pc/src/audio_pc.c: ADPCM ueber
 * re15_vag_adpcm_decode, Tonhoehe re15_vab_note2pitch2(min, shift, center, shift) wie se_play_layers, Schritt =
 * pitch << 4 gegen 44100 Hz, Positionsfortschritt pos_frac/pos wie audio_callback) mit dem Mono-Mittel des Mitschnitts.
 *
 * Aufruf: probe_r38_elzatoene_kreuz <cap.raw> <exit-raumbild> <quelle> <bild> [<bild> ...]
 *   quelle  vbs:<ELZASE.VBS>:<satz>      Satz der Mini-Bank (Aufbau gen/re2_elza_bank.inc)
 *           rdt:<ROOMxxxx.RDT>:<bank>:<se> Satz der Raumbank (RDT-Kopf +0x08 + bank*12 = EDT/VH/VB)
 *   Mitschnitt: Stereo s16, 1470 Frames je Spielbild; Raum-Bild f liegt bei Takt (Takte - 1 - exit) + f
 *   (dieselbe Zuordnung wie tests/unit/probe_r36_elza_auswertung.c).
 * Ausgabe je Bild: "bild B korrelation K versatz V" — das Maximum ueber Versatz -2..+6 Bilder ab B (V in Bildern,
 * auf 1/1470 genau), dazu dieselbe Suche 60 Bilder frueher als Gegenprobe ("gegenprobe G").
 */
#include "re15_vab.h"
#include "../../engine/src/gen/re2_elza_bank.inc"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (n) *n = (size_t)sz;
    return b;
}
static uint32_t le32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

/* Ton -> Vorlage bei 44100 Hz (Mischer-Takt). Rueckgabe Laenge, 0 = Fehler. */
static int vorlage(const char *quelle, double **out)
{
    char q[1024]; snprintf(q, sizeof q, "%s", quelle);
    const uint8_t *edt = NULL, *vh = NULL, *vb = NULL; size_t vhsz = 0, vbsz = 0; int se = -1;
    size_t n = 0; uint8_t *d = NULL;
    if (!strncmp(q, "vbs:", 4)) {
        char *a = q + 4, *c = strrchr(a, ':'); if (!c) return 0; *c = 0; se = atoi(c + 1);
        d = slurp(a, &n); if (!d || n < RE2_ELZA_VBD_OFF + RE2_ELZA_VBD_SIZE) return 0;
        edt = d + RE2_ELZA_EDT_OFF;
        uint32_t vo = le32(d + RE2_ELZA_EDT_SIZE - 8);
        vh = d + vo; vhsz = RE2_ELZA_EDT_SIZE - vo; vb = d + RE2_ELZA_VBD_OFF; vbsz = RE2_ELZA_VBD_SIZE;
    } else if (!strncmp(q, "rdt:", 4)) {
        char *a = q + 4, *c2 = strrchr(a, ':'); if (!c2) return 0; *c2 = 0; se = atoi(c2 + 1);
        char *c1 = strrchr(a, ':'); if (!c1) return 0; *c1 = 0; int bank = atoi(c1 + 1);
        d = slurp(a, &n); if (!d || n < 0x60) return 0;
        uint32_t e = le32(d + 0x08 + bank * 12), h = le32(d + 0x0C + bank * 12), b = le32(d + 0x10 + bank * 12);
        if (!e || !h || !b || h <= e || b <= h || b >= n) return 0;
        edt = d + e; vh = d + h; vhsz = b - h; vb = d + b; vbsz = n - b;
    } else return 0;
    re15_vab_t vab;
    if (re15_vab_parse(vh, vhsz, &vab) != 0) return 0;
    int vags[8], tones[8];
    int nl = re15_edt_resolve_layers_ex(edt, &vab, se, vags, tones, 8);
    if (nl <= 0) return 0;
    const re15_vab_sample_t *s = &vab.samples[vags[0]];
    if (s->offset + s->size > vbsz) return 0;
    size_t cap = (s->size / 16) * 28;
    int16_t *pcm = (int16_t *)malloc(cap * sizeof(int16_t));
    int len = re15_vag_adpcm_decode(vb + s->offset, s->size, pcm, cap);
    const re15_vab_tone_t *t = &vab.tones[tones[0]];
    uint16_t pitch = re15_vab_note2pitch2(t->min_note, t->pitch_shift, t->center_note, t->pitch_shift);
    uint32_t step = (uint32_t)pitch << 4, frac = 0; int pos = 0, m = 0;
    double *v = (double *)malloc(sizeof(double) * (size_t)(len * 8 + 16));
    while (pos < len) { v[m++] = pcm[pos]; frac += step; pos += (int)(frac >> 16); frac &= 0xffffu; }
    fprintf(stderr, "vorlage %s: satz %d vag %d %u B, pitch 0x%03x (%d Hz), %d Samples @44100\n", quelle, se, vags[0],
            (unsigned)s->size, pitch, (int)((44100u * pitch) >> 12), m);
    free(pcm); free(d);
    *out = v;
    return m;
}

static double korr(const double *x, long nx, const double *t, int nt, long lag)
{
    if (lag < 0 || lag + nt > nx) return 0.0;
    double sxt = 0, sxx = 0, stt = 0;
    for (int i = 0; i < nt; i++) { double a = x[lag + i], b = t[i]; sxt += a * b; sxx += a * a; stt += b * b; }
    return (sxx > 0 && stt > 0) ? sxt / sqrt(sxx * stt) : 0.0;
}

static void suche(const double *x, long nx, long off, const double *t, int nt, int bild, double *best, double *versatz)
{
    *best = -2; *versatz = 0;
    long l0 = (off + bild - 2) * 1470L, l1 = (off + bild + 6) * 1470L;
    for (long lag = l0; lag <= l1; lag++) {
        double c = korr(x, nx, t, nt, lag);
        if (c > *best) { *best = c; *versatz = (double)(lag - (off + bild) * 1470L) / 1470.0; }
    }
}

int main(int argc, char **argv)
{
    if (argc < 5) { printf("Aufruf: <cap.raw> <exit-bild> <quelle> <bild> ...\n"); return 2; }
    size_t n = 0; uint8_t *c = slurp(argv[1], &n);
    if (!c) { printf("FEHLER: %s\n", argv[1]); return 1; }
    long nx = (long)(n / 4); long takte = nx / 1470; int exitb = atoi(argv[2]);
    long off = takte - 1 - exitb;
    if (off < 0) { printf("FEHLER: Mitschnitt kuerzer als der Raum\n"); return 1; }
    double *x = (double *)malloc(sizeof(double) * (size_t)nx);
    for (long i = 0; i < nx; i++) {
        int16_t l = (int16_t)(c[i * 4] | (c[i * 4 + 1] << 8)), r = (int16_t)(c[i * 4 + 2] | (c[i * 4 + 3] << 8));
        x[i] = ((double)l + (double)r) * 0.5;
    }
    double *t = NULL; int nt = vorlage(argv[3], &t);
    if (nt <= 0) { printf("FEHLER: Quelle %s\n", argv[3]); return 1; }
    if (nt > 17640) nt = 17640;                         /* 0,4 s: Anschlag + Hauptteil */
    for (int a = 4; a < argc; a++) {
        int b = atoi(argv[a]); double k, v, kg, vg;
        suche(x, nx, off, t, nt, b, &k, &v);
        suche(x, nx, off, t, nt, b - 60, &kg, &vg);
        printf("bild %d korrelation %.3f versatz %.2f gegenprobe %.3f\n", b, k, v, kg);
    }
    free(x); free(t); free(c);
    return 0;
}
