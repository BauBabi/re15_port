/* =============================================================================================
 * RE1.5 Rebuilt — Android: Asset-Liste v2 lesen und gegen "zuletzt entpackt" abgleichen
 * (Runde 34a, N1, 2026-09-30). Regeln und Format: asset_abgleich.h. PORT-WAHL, kein
 * Originalverhalten (siehe Kopf dort). Reines C99 ohne SDL/Android - der PC-Unit-Test
 * tests/unit/test_r34a_asset_abgleich.c uebersetzt genau diese Datei.
 * ============================================================================================= */
/* POSIX-Dateifunktionen (opendir, lstat, unlink, rmdir) fuer re15_abgleich_waisen - wie asset_root_pc.c, weil
 * CMake mit -std=c11 ohne Erweiterungen uebersetzt (glibc blendet sie sonst aus). */
#if !defined(_WIN32) && !defined(_DEFAULT_SOURCE)
#  define _DEFAULT_SOURCE 1
#endif
#include "asset_abgleich.h"

#include <dirent.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#if defined(_WIN32)
#  include <direct.h>
#  include <io.h>
#  define re15_lstat stat                 /* mingw: kein lstat, keine Symlinks im Test */
#  define RE15_IST_LINK(m) 0
#else
#  define re15_lstat lstat
#  define RE15_IST_LINK(m) S_ISLNK(m)
#endif

/* =============================================================================== SHA-256 */
/* FIPS 180-4: 4.2.2 (K = erste 32 Bit der Nachkommastellen der Kubikwurzeln der ersten 64 Primzahlen),
 * 5.3.3 (H0 = dasselbe fuer die Quadratwurzeln der ersten 8 Primzahlen), 6.2.2 (Rechenschritte),
 * 5.1.1 (Auffuellen: 0x80, Nullen, Laenge in Bit als 64-Bit big-endian). */
static const uint32_t K256[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
    0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
    0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
    0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
    0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
    0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
    0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u, 0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
};
static const uint32_t H256[8] = {
    0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au, 0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u
};

#define ROTR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

static void sha256_block(uint32_t h[8], const unsigned char *p)
{
    uint32_t w[64];
    for (int i = 0; i < 16; i++)
        w[i] = ((uint32_t)p[4 * i] << 24) | ((uint32_t)p[4 * i + 1] << 16) | ((uint32_t)p[4 * i + 2] << 8) | p[4 * i + 3];
    for (int i = 16; i < 64; i++) {
        uint32_t s0 = ROTR32(w[i - 15], 7) ^ ROTR32(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = ROTR32(w[i - 2], 17) ^ ROTR32(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    uint32_t a = h[0], b = h[1], c = h[2], d = h[3], e = h[4], f = h[5], g = h[6], hh = h[7];
    for (int i = 0; i < 64; i++) {
        uint32_t S1 = ROTR32(e, 6) ^ ROTR32(e, 11) ^ ROTR32(e, 25);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t t1 = hh + S1 + ch + K256[i] + w[i];
        uint32_t S0 = ROTR32(a, 2) ^ ROTR32(a, 13) ^ ROTR32(a, 22);
        uint32_t mj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t t2 = S0 + mj;
        hh = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
}

void re15_sha256_start(re15_sha256_t *c)
{
    memcpy(c->h, H256, sizeof H256);
    c->bytes = 0;
    c->fill = 0;
}

void re15_sha256_dazu(re15_sha256_t *c, const void *daten, size_t n)
{
    const unsigned char *p = (const unsigned char *)daten;
    c->bytes += (uint64_t)n;
    if (c->fill) {
        size_t k = 64 - c->fill;
        if (k > n) k = n;
        memcpy(c->blk + c->fill, p, k);
        c->fill += k; p += k; n -= k;
        if (c->fill < 64) return;
        sha256_block(c->h, c->blk);
        c->fill = 0;
    }
    while (n >= 64) { sha256_block(c->h, p); p += 64; n -= 64; }
    if (n) { memcpy(c->blk, p, n); c->fill = n; }
}

void re15_sha256_ende(re15_sha256_t *c, char hex[65])
{
    static const char ziffer[] = "0123456789abcdef";
    uint64_t bits = c->bytes * 8u;
    c->blk[c->fill++] = 0x80;
    if (c->fill > 56) {
        memset(c->blk + c->fill, 0, 64 - c->fill);
        sha256_block(c->h, c->blk);
        c->fill = 0;
    }
    memset(c->blk + c->fill, 0, 56 - c->fill);
    for (int i = 0; i < 8; i++) c->blk[56 + i] = (unsigned char)(bits >> (56 - 8 * i));
    sha256_block(c->h, c->blk);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 4; j++) {
            unsigned v = (unsigned)(c->h[i] >> (24 - 8 * j)) & 0xffu;
            hex[8 * i + 2 * j]     = ziffer[v >> 4];
            hex[8 * i + 2 * j + 1] = ziffer[v & 15u];
        }
    hex[64] = '\0';
    c->fill = 0;
}

int re15_sha256_datei(const char *pfad, char hex[65], long long *groesse)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return -1;
    static unsigned char puffer[1u << 16];   /* nur vom Entpack-Thread benutzt */
    re15_sha256_t c;
    re15_sha256_start(&c);
    long long n = 0;
    for (;;) {
        size_t k = fread(puffer, 1, sizeof puffer, f);
        if (k == 0) break;
        re15_sha256_dazu(&c, puffer, k);
        n += (long long)k;
    }
    int fehler = ferror(f);
    fclose(f);
    if (fehler) return -1;
    re15_sha256_ende(&c, hex);
    if (groesse) *groesse = n;
    return 0;
}

/* =============================================================================== Liste */
static void fehler_setzen(char *fehler, size_t fehler_n, const char *fmt, ...)
{
    if (!fehler || fehler_n == 0) return;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(fehler, fehler_n, fmt, ap);
    va_end(ap);
}

static int ist_ziffer(char c) { return c >= '0' && c <= '9'; }

/* 1..18 ASCII-Ziffern ab s (vor e); Zeiger hinter die Zahl oder NULL. */
static const char *zahl_lesen(const char *s, const char *e, long long *wert)
{
    long long v = 0;
    int n = 0;
    while (s < e && ist_ziffer(*s)) {
        if (++n > RE15_ABGLEICH_ZAHL_MAX) return NULL;
        v = v * 10 + (*s - '0');
        s++;
    }
    if (n == 0) return NULL;
    *wert = v;
    return s;
}

/* Kopfzeile (ohne Zeilenende): 0 = v2, ALTES_FORMAT = v1-Kopf, sonst UNGUELTIG. */
static int kopf_lesen(const char *z, size_t L, long long *anzahl, long long *bytes)
{
    const char *e = z + L;
    size_t k = sizeof RE15_ABGLEICH_KOPF - 1;
    if (L >= k && memcmp(z, RE15_ABGLEICH_KOPF, k) == 0) {
        const char *p = zahl_lesen(z + k, e, anzahl);
        if (!p || p >= e || *p != ' ') return RE15_ABGLEICH_UNGUELTIG;
        p = zahl_lesen(p + 1, e, bytes);
        return (p && p == e) ? 0 : RE15_ABGLEICH_UNGUELTIG;
    }
    k = sizeof RE15_ABGLEICH_KOPF_V1 - 1;                  /* v1: "# re15 assets <ziffern> <ziffern>" */
    if (L > k && memcmp(z, RE15_ABGLEICH_KOPF_V1, k) == 0) {
        const char *p = z + k, *q;
        for (q = p; q < e && ist_ziffer(*q); q++) {}
        if (q > p && q < e && *q == ' ') {
            p = ++q;
            for (; q < e && ist_ziffer(*q); q++) {}
            if (q > p && q == e) return RE15_ABGLEICH_ALTES_FORMAT;
        }
    }
    return RE15_ABGLEICH_UNGUELTIG;
}

static char ascii_klein(char c) { return (c >= 'A' && c <= 'Z') ? (char)(c - 'A' + 'a') : c; }

/* Regeln im Kopf von asset_abgleich.h. Nachbesserung R4-1 (Gegenpruefung H5/U3): NUR druckbares ASCII 0x20-0x7e
 * (bis dahin jedes wohlgeformte UTF-8 - der App-Speicher faltet Unicode, eine Dublettenregel mit ASCII-Faltung liess
 * Kelvin-Zeichen/K durch) und jedes Segment <= RE15_ABGLEICH_SEGMENT_MAX (Namen > 255 B legt das Geraet nicht an). */
int re15_abgleich_pfad_ok(const char *p, size_t n)
{
    if (!p || n == 0 || n > RE15_ABGLEICH_PFAD_MAX) return 0;
    int schraeg = 0;
    size_t seg = 0;                                        /* Anfang des laufenden Segments */
    for (size_t i = 0; i <= n; i++) {
        if (i == n || p[i] == '/') {
            size_t l = i - seg;
            if (l == 0) return 0;                          /* fuehrendes '/', '//', '/' am Ende */
            if (l > RE15_ABGLEICH_SEGMENT_MAX) return 0;
            if (l == 1 && p[seg] == '.') return 0;
            if (l == 2 && p[seg] == '.' && p[seg + 1] == '.') return 0;
            if (i < n) schraeg = 1;
            seg = i + 1;
            continue;
        }
        unsigned char c = (unsigned char)p[i];
        if (c < 0x20 || c > 0x7e || c == '\\') return 0;  /* Steuerzeichen, DEL, Nicht-ASCII, '\' */
    }
    if (!schraeg) return 0;
    size_t k = sizeof RE15_ABGLEICH_NEU_ENDUNG - 1;
    if (n >= k) {
        size_t j;
        for (j = 0; j < k && ascii_klein(p[n - k + j]) == RE15_ABGLEICH_NEU_ENDUNG[j]; j++) {}
        if (j == k) return 0;
    }
    return 1;
}

static int pfad_vergleich(const void *a, const void *b)
{
    return strcmp(((const re15_abgleich_eintrag_t *)a)->pfad, ((const re15_abgleich_eintrag_t *)b)->pfad);
}

static int pfad_vergleich_klein(const void *a, const void *b)
{
    const unsigned char *x = (const unsigned char *)*(const char *const *)a;
    const unsigned char *y = (const unsigned char *)*(const char *const *)b;
    for (;; x++, y++) {
        unsigned char cx = (unsigned char)ascii_klein((char)*x), cy = (unsigned char)ascii_klein((char)*y);
        if (cx != cy) return cx < cy ? -1 : 1;
        if (!cx) return 0;
    }
}

void re15_abgleich_frei(re15_abgleich_liste_t *l)
{
    if (!l) return;
    free(l->puffer);
    free(l->e);
    memset(l, 0, sizeof *l);
}

int re15_abgleich_lesen(re15_abgleich_liste_t *l, const char *text, size_t len,
                        char *fehler, size_t fehler_n)
{
    memset(l, 0, sizeof *l);
    if (fehler && fehler_n) fehler[0] = '\0';
    if (!text) { fehler_setzen(fehler, fehler_n, "keine Liste"); return RE15_ABGLEICH_UNGUELTIG; }
    if (len > RE15_ABGLEICH_LISTE_MAX) {
        fehler_setzen(fehler, fehler_n, "Liste %zu B > %u B (64 MiB)", len, (unsigned)RE15_ABGLEICH_LISTE_MAX);
        return RE15_ABGLEICH_UNGUELTIG;
    }
    const char *nul = (const char *)memchr(text, '\0', len);
    if (nul) {
        fehler_setzen(fehler, fehler_n, "NUL-Byte an Stelle %zu", (size_t)(nul - text));
        return RE15_ABGLEICH_UNGUELTIG;
    }

    char *b = (char *)malloc(len + 1);
    if (!b) { fehler_setzen(fehler, fehler_n, "kein Speicher"); return RE15_ABGLEICH_KEIN_SPEICHER; }
    memcpy(b, text, len);
    b[len] = '\0';
    /* Eintraege wachsen mit (nicht je '\n' vorab: 64 MiB Leerzeilen waeren sonst ~6 GB) */
    size_t kap = 1024;
    re15_abgleich_eintrag_t *e = (re15_abgleich_eintrag_t *)malloc(kap * sizeof *e);
    const char **klein = NULL;
    if (!e) { free(b); fehler_setzen(fehler, fehler_n, "kein Speicher"); return RE15_ABGLEICH_KEIN_SPEICHER; }

    int rc = RE15_ABGLEICH_UNGUELTIG;
    long long kopf_n = -1, kopf_b = -1, summe = 0;
    size_t n = 0, nr = 0;
    for (char *z = b; z; ) {
        char *nl = strchr(z, '\n');
        if (nl) *nl = '\0';
        nr++;
        size_t L = strlen(z);
        while (L > 0 && z[L - 1] == '\r') z[--L] = '\0';
        if (nr == 1) {
            int k = kopf_lesen(z, L, &kopf_n, &kopf_b);
            if (k == RE15_ABGLEICH_ALTES_FORMAT) {
                fehler_setzen(fehler, fehler_n, "Zeile 1: Kopfzeile im alten Format v1 (bis v0.8.19, ohne Pruefsumme)");
                rc = RE15_ABGLEICH_ALTES_FORMAT;
                goto raus;
            }
            if (k != 0) {
                fehler_setzen(fehler, fehler_n, "Zeile 1: Kopfzeile fehlt/unlesbar (erwartet '%s<anzahl> <bytes>')",
                              RE15_ABGLEICH_KOPF);
                goto raus;
            }
        } else if (L == 0) {
            /* leere Zeile: uebersprungen */
        } else if (z[0] == '#') {
            fehler_setzen(fehler, fehler_n, "Zeile %zu: unerwartete Kommentarzeile", nr);
            goto raus;
        } else {
            const char *ende = z + L;
            long long g = 0;
            const char *p = zahl_lesen(z, ende, &g);
            if (!p || p >= ende || *p != '\t') {
                fehler_setzen(fehler, fehler_n, "Zeile %zu: Groesse (1-18 Ziffern) + Tab erwartet", nr);
                goto raus;
            }
            p++;
            if (ende - p < 65 || p[64] != '\t') {
                fehler_setzen(fehler, fehler_n, "Zeile %zu: SHA-256 (64 Zeichen 0-9a-f) + Tab erwartet", nr);
                goto raus;
            }
            for (int i = 0; i < 64; i++)
                if (!ist_ziffer(p[i]) && !(p[i] >= 'a' && p[i] <= 'f')) {
                    fehler_setzen(fehler, fehler_n, "Zeile %zu: SHA-256 (64 Zeichen 0-9a-f) + Tab erwartet", nr);
                    goto raus;
                }
            const char *pfad = p + 65;
            if (!re15_abgleich_pfad_ok(pfad, (size_t)(ende - pfad))) {
                fehler_setzen(fehler, fehler_n, "Zeile %zu: unzulaessiger Pfad", nr);
                goto raus;
            }
            if (summe > LLONG_MAX - g) {
                fehler_setzen(fehler, fehler_n, "Zeile %zu: Summe der Groessen laeuft ueber", nr);
                goto raus;
            }
            if (n == kap) {
                re15_abgleich_eintrag_t *mehr = (re15_abgleich_eintrag_t *)realloc(e, 2 * kap * sizeof *e);
                if (!mehr) { fehler_setzen(fehler, fehler_n, "kein Speicher"); rc = RE15_ABGLEICH_KEIN_SPEICHER; goto raus; }
                e = mehr;
                kap *= 2;
            }
            e[n].groesse = g;
            memcpy(e[n].sha, p, 64);
            e[n].sha[64] = '\0';
            e[n].pfad = pfad;
            n++;
            summe += g;
        }
        z = nl ? nl + 1 : NULL;
    }
    if (n == 0) {
        fehler_setzen(fehler, fehler_n, "Liste ohne Dateien");
        goto raus;
    }
    if (kopf_n != (long long)n || kopf_b != summe) {
        fehler_setzen(fehler, fehler_n, "Kopfzeile nennt %lld Dateien / %lld Bytes, die Zeilen ergeben %zu / %lld",
                      kopf_n, kopf_b, n, summe);
        goto raus;
    }
    qsort(e, n, sizeof *e, pfad_vergleich);
    klein = (const char **)malloc(n * sizeof *klein);
    if (!klein) { fehler_setzen(fehler, fehler_n, "kein Speicher"); rc = RE15_ABGLEICH_KEIN_SPEICHER; goto raus; }
    for (size_t i = 0; i < n; i++) klein[i] = e[i].pfad;
    qsort(klein, n, sizeof *klein, pfad_vergleich_klein);
    for (size_t i = 1; i < n; i++)
        if (pfad_vergleich_klein(&klein[i - 1], &klein[i]) == 0) {
            if (strcmp(klein[i - 1], klein[i]) == 0)
                fehler_setzen(fehler, fehler_n, "Pfad doppelt: %s", klein[i]);
            else
                fehler_setzen(fehler, fehler_n, "Pfade nur in Gross/klein verschieden: %s / %s", klein[i - 1], klein[i]);
            goto raus;
        }
    free(klein);
    l->puffer = b;
    l->e = e;
    l->n = n;
    l->summe = summe;
    return 0;

raus:
    free(klein);
    free(e);
    free(b);
    return rc;
}

const re15_abgleich_eintrag_t *re15_abgleich_suchen(const re15_abgleich_liste_t *l, const char *pfad)
{
    if (!l || !l->e || l->n == 0 || !pfad) return NULL;
    re15_abgleich_eintrag_t schluessel;
    memset(&schluessel, 0, sizeof schluessel);
    schluessel.pfad = pfad;
    return (const re15_abgleich_eintrag_t *)bsearch(&schluessel, l->e, l->n, sizeof *l->e, pfad_vergleich);
}

/* =============================================================================== Abgleich */
void re15_abgleich_plan_frei(re15_abgleich_plan_t *p)
{
    if (!p) return;
    free(p->aktion);
    free((void *)p->weg);
    memset(p, 0, sizeof *p);
}

int re15_abgleich_planen(re15_abgleich_plan_t *p, const re15_abgleich_liste_t *neu,
                         const re15_abgleich_liste_t *alt)
{
    memset(p, 0, sizeof *p);
    p->aktion = (unsigned char *)malloc(neu->n ? neu->n : 1);
    if (!p->aktion) return -1;
    for (size_t i = 0; i < neu->n; i++) {
        const re15_abgleich_eintrag_t *e = &neu->e[i];
        unsigned char a;
        if (!alt) {
            a = RE15_ABGLEICH_PRUEFEN;
        } else {
            const re15_abgleich_eintrag_t *v = re15_abgleich_suchen(alt, e->pfad);
            if (!v) a = RE15_ABGLEICH_NEU;
            else if (v->groesse == e->groesse && strcmp(v->sha, e->sha) == 0) a = RE15_ABGLEICH_BEHALTEN;
            else a = RE15_ABGLEICH_GEAENDERT;
        }
        p->aktion[i] = a;
        switch (a) {
        case RE15_ABGLEICH_BEHALTEN:  p->n_behalten++;  break;
        case RE15_ABGLEICH_GEAENDERT: p->n_geaendert++; break;
        case RE15_ABGLEICH_NEU:       p->n_neu++;       break;
        default:                      p->n_pruefen++;   break;
        }
    }
    if (alt && alt->n) {
        p->weg = (const char **)malloc(alt->n * sizeof *p->weg);
        if (!p->weg) { re15_abgleich_plan_frei(p); return -1; }
        for (size_t j = 0; j < alt->n; j++)               /* alt->e ist sortiert -> weg auch */
            if (!re15_abgleich_suchen(neu, alt->e[j].pfad)) p->weg[p->n_weg++] = alt->e[j].pfad;
    }
    return 0;
}

int re15_abgleich_tun(int aktion, long long groesse_ist, long long groesse_soll)
{
    switch (aktion) {
    case RE15_ABGLEICH_BEHALTEN:
        return groesse_ist == groesse_soll ? RE15_TUN_NICHTS : RE15_TUN_ENTPACKEN;
    case RE15_ABGLEICH_PRUEFEN:
        return groesse_ist == groesse_soll ? RE15_TUN_SUMME_PRUEFEN : RE15_TUN_ENTPACKEN;
    default:                                               /* GEAENDERT, NEU, Unbekanntes */
        return RE15_TUN_ENTPACKEN;
    }
}

/* =============================================================================== Waisen (R4-1, U2/E1) */
#define WAISEN_PFAD  4096                                  /* <wurzel>/<rel> (Android PATH_MAX) */
#define WAISEN_TIEFE 64                                    /* Ordnertiefe (die Baeume haben 4) */

typedef struct {
    const char *wurzel;
    const re15_abgleich_liste_t *l;
    void (*melde)(void *ctx, const char *rel, int ok);
    void *ctx;
    long n_weg, n_fehler;
} waisen_t;

/* Alle Namen eines Ordners (ohne "." und "..") ZUERST einlesen, dann erst loeschen - kein unlink waehrend readdir.
 * 0 = ok (namen und n gesetzt, auch leer), -1 = Ordner unlesbar oder kein Speicher (namen enthaelt, was gelesen wurde). */
static int namen_lesen(const char *ordner, char ***namen, size_t *n)
{
    *namen = NULL;
    *n = 0;
    DIR *d = opendir(ordner);
    if (!d) return -1;
    size_t kap = 0;
    int rc = 0;
    struct dirent *de;
    while ((de = readdir(d)) != NULL) {
        const char *s = de->d_name;
        if (strcmp(s, ".") == 0 || strcmp(s, "..") == 0) continue;
        if (*n == kap) {
            size_t k2 = kap ? 2 * kap : 64;
            char **m = (char **)realloc(*namen, k2 * sizeof *m);
            if (!m) { rc = -1; break; }
            *namen = m;
            kap = k2;
        }
        size_t k = strlen(s);
        char *c = (char *)malloc(k + 1);
        if (!c) { rc = -1; break; }
        memcpy(c, s, k + 1);
        (*namen)[(*n)++] = c;
    }
    closedir(d);
    return rc;
}

/* rel = Pfad relativ zu w->wurzel (Puffer WAISEN_PFAD, len Zeichen belegt) */
static void waisen_ordner(waisen_t *w, char *rel, size_t len, int tiefe)
{
    char voll[WAISEN_PFAD];
    int k0 = snprintf(voll, sizeof voll, "%s/%s", w->wurzel, rel);
    if (k0 < 0 || (size_t)k0 >= sizeof voll) { w->n_fehler++; return; }
    char **namen = NULL;
    size_t n = 0;
    if (namen_lesen(voll, &namen, &n) != 0) w->n_fehler++;   /* was gelesen wurde, wird trotzdem bearbeitet */
    for (size_t i = 0; i < n; i++) {
        size_t k = strlen(namen[i]);
        if (len + 1 + k >= WAISEN_PFAD) { w->n_fehler++; free(namen[i]); continue; }
        rel[len] = '/';
        memcpy(rel + len + 1, namen[i], k + 1);
        free(namen[i]);
        int k1 = snprintf(voll, sizeof voll, "%s/%s", w->wurzel, rel);
        struct stat sb;
        if (k1 < 0 || (size_t)k1 >= sizeof voll) {
            w->n_fehler++;
        } else if (re15_lstat(voll, &sb) != 0) {
            /* zwischen readdir und lstat verschwunden - nichts zu tun */
        } else if (S_ISDIR(sb.st_mode) && !RE15_IST_LINK(sb.st_mode)) {
            if (tiefe < WAISEN_TIEFE) waisen_ordner(w, rel, len + 1 + k, tiefe + 1);
            else w->n_fehler++;
            (void)rmdir(voll);                             /* nur wenn leer; sonst ENOTEMPTY - gewollt */
        } else if (!re15_abgleich_suchen(w->l, rel)) {     /* Datei, Symlink, Rest: nicht in der Liste -> weg */
            int ok = unlink(voll) == 0;
            if (ok) w->n_weg++;
            else w->n_fehler++;
            if (w->melde) w->melde(w->ctx, rel, ok);
        }
        rel[len] = '\0';
    }
    free(namen);
}

long re15_abgleich_waisen(const char *wurzel, const char *const *baeume, size_t n_baeume,
                          const re15_abgleich_liste_t *l,
                          void (*melde)(void *ctx, const char *rel, int ok), void *ctx, long *n_fehler)
{
    waisen_t w;
    memset(&w, 0, sizeof w);
    w.wurzel = wurzel;
    w.l = l;
    w.melde = melde;
    w.ctx = ctx;
    char rel[WAISEN_PFAD];
    char voll[WAISEN_PFAD];
    for (size_t b = 0; wurzel && l && baeume && b < n_baeume; b++) {
        const char *baum = baeume[b];
        size_t k = baum ? strlen(baum) : 0;
        /* nur ein einfacher Ordnername direkt unter der Wurzel (nie "", ".", "..", nie mit '/') */
        if (k == 0 || k >= sizeof rel || strchr(baum, '/') || strcmp(baum, ".") == 0 || strcmp(baum, "..") == 0) {
            w.n_fehler++;
            continue;
        }
        int kv = snprintf(voll, sizeof voll, "%s/%s", wurzel, baum);
        struct stat sb;
        if (kv < 0 || (size_t)kv >= sizeof voll) { w.n_fehler++; continue; }
        if (re15_lstat(voll, &sb) != 0 || !S_ISDIR(sb.st_mode) || RE15_IST_LINK(sb.st_mode)) continue;   /* kein Baum */
        memcpy(rel, baum, k + 1);
        waisen_ordner(&w, rel, k, 0);
    }
    if (n_fehler) *n_fehler = w.n_fehler;
    return w.n_weg;
}
