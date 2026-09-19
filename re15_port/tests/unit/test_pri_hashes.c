/* Hash-Riegel fuer die Freistellungen des Nutzers (pri/STAGE1/**.png).
 *
 * ⛔ WOZU (Audit 2026-09-19, pri-masken-audit.md §1.5): drei Nutzer-PNGs waren in Git
 * veraendert, zwei ersetzt/zerteilt — von MIR, unter dem unveraenderten Eintrag in
 * auswahl.json (10D0/06.png: 30x46 -> 65x120 bei stehender Lage x=34,y=172 = "Boden ueber
 * Leons Bein"). Kein Test hat das gesehen. Dieser Riegel pinnt den JETZT festgelegten Stand
 * jeder Datei (Gegenpruefung Punkt 1: nicht "erste Git-Fassung" — der Nutzer liefert
 * mehrfach, 1130/03.png dreimal): der Git-Blob-Hash (sha1 ueber "blob <n>\0" + Inhalt,
 * dieselbe Zahl wie `git hash-object`) muss fuer jede gelistete Datei stimmen, und die
 * Menge der Dateien unter pri/STAGE1 muss genau die Liste sein. Aendert der Nutzer eine
 * Freistellung bewusst, wird die Liste mit tools/maske/pri_hashes.py neu erzeugt — und
 * der Commit sagt es.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

typedef struct { const char *pfad, *sha, *herkunft; } eintrag_t;
static const eintrag_t LISTE[] = {
#include "pri_stage1_hashes.inc"
};

/* ---- SHA-1 (FIPS 180-1), kompakt ------------------------------------------------ */
typedef struct { uint32_t h[5]; uint64_t len; uint8_t buf[64]; size_t n; } sha1_t;
static uint32_t rol(uint32_t x, int s) { return (x << s) | (x >> (32 - s)); }
static void sha1_block(sha1_t *s, const uint8_t *p)
{
    uint32_t w[80], a, b, c, d, e, i;
    for (i = 0; i < 16; i++)
        w[i] = ((uint32_t)p[i*4] << 24) | ((uint32_t)p[i*4+1] << 16) | ((uint32_t)p[i*4+2] << 8) | p[i*4+3];
    for (i = 16; i < 80; i++) w[i] = rol(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
    a = s->h[0]; b = s->h[1]; c = s->h[2]; d = s->h[3]; e = s->h[4];
    for (i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20)      { f = (b & c) | (~b & d);           k = 0x5A827999u; }
        else if (i < 40) { f = b ^ c ^ d;                    k = 0x6ED9EBA1u; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d);  k = 0x8F1BBCDCu; }
        else             { f = b ^ c ^ d;                    k = 0xCA62C1D6u; }
        uint32_t t = rol(a, 5) + f + e + k + w[i];
        e = d; d = c; c = rol(b, 30); b = a; a = t;
    }
    s->h[0] += a; s->h[1] += b; s->h[2] += c; s->h[3] += d; s->h[4] += e;
}
static void sha1_init(sha1_t *s)
{
    s->h[0] = 0x67452301u; s->h[1] = 0xEFCDAB89u; s->h[2] = 0x98BADCFEu; s->h[3] = 0x10325476u; s->h[4] = 0xC3D2E1F0u;
    s->len = 0; s->n = 0;
}
static void sha1_update(sha1_t *s, const uint8_t *p, size_t n)
{
    s->len += n;
    while (n) {
        size_t k = 64 - s->n; if (k > n) k = n;
        memcpy(s->buf + s->n, p, k); s->n += k; p += k; n -= k;
        if (s->n == 64) { sha1_block(s, s->buf); s->n = 0; }
    }
}
static void sha1_final(sha1_t *s, char hex[41])
{
    uint8_t pad[72]; size_t i; uint64_t bits = s->len * 8;
    memset(pad, 0, sizeof pad); pad[0] = 0x80;
    size_t padlen = (s->n < 56) ? (56 - s->n) : (120 - s->n);
    sha1_update(s, pad, padlen);
    for (i = 0; i < 8; i++) pad[i] = (uint8_t)(bits >> (56 - 8 * i));
    sha1_update(s, pad, 8);
    for (i = 0; i < 5; i++) sprintf(hex + i * 8, "%08x", s->h[i]);
    hex[40] = 0;
}

static int blob_hash(const char *pfad, char hex[41])
{
    FILE *f = fopen(pfad, "rb"); long len; uint8_t *b; char kopf[40]; sha1_t s;
    if (!f) return 0;
    fseek(f, 0, SEEK_END); len = ftell(f); fseek(f, 0, SEEK_SET);
    if (len < 0) { fclose(f); return 0; }
    b = (uint8_t *)malloc((size_t)len + 1);
    if (!b || (len > 0 && fread(b, 1, (size_t)len, f) != (size_t)len)) { free(b); fclose(f); return 0; }
    fclose(f);
    sha1_init(&s);
    sprintf(kopf, "blob %ld", len);
    sha1_update(&s, (const uint8_t *)kopf, strlen(kopf) + 1);   /* inkl. NUL */
    sha1_update(&s, b, (size_t)len);
    sha1_final(&s, hex);
    free(b);
    return 1;
}

/* Alle *.png unter pri/STAGE1/<raum>/ (eine Ebene tief, wie die Liste). */
static int gelistet(const char *rel)
{
    size_t i;
    for (i = 0; i < sizeof LISTE / sizeof LISTE[0]; i++)
        if (strcmp(LISTE[i].pfad, rel) == 0) return 1;
    return 0;
}

int main(void)
{
    char wurzel[600], pfad[800], hex[41];
    size_t i, n = sizeof LISTE / sizeof LISTE[0];
    int falsch = 0, fehlt = 0, ungelistet = 0, gefunden = 0;
    snprintf(wurzel, sizeof wurzel, "%s/..", RE15_PORT_SRC_DIR);
    printf("== Hash-Riegel pri/STAGE1 (%u Dateien in der Liste) ==\n", (unsigned)n);
    for (i = 0; i < n; i++) {
        snprintf(pfad, sizeof pfad, "%s/%s", wurzel, LISTE[i].pfad);
        if (!blob_hash(pfad, hex)) { printf("  FEHLT: %s\n", LISTE[i].pfad); fehlt++; continue; }
        if (strcmp(hex, LISTE[i].sha) != 0) {
            printf("  VERAENDERT: %s ist %s, festgelegt %s (Herkunft %s)\n", LISTE[i].pfad, hex, LISTE[i].sha, LISTE[i].herkunft);
            falsch++;
        }
    }
    /* Gegenrichtung: keine Datei ohne Eintrag. */
    {
        DIR *d1; struct dirent *e1;
        snprintf(pfad, sizeof pfad, "%s/pri/STAGE1", wurzel);
        d1 = opendir(pfad);
        if (d1) {
            while ((e1 = readdir(d1)) != NULL) {
                char sub[800]; DIR *d2; struct dirent *e2;
                if (e1->d_name[0] == '.') continue;
                snprintf(sub, sizeof sub, "%s/pri/STAGE1/%s", wurzel, e1->d_name);
                d2 = opendir(sub);
                if (!d2) continue;
                while ((e2 = readdir(d2)) != NULL) {
                    char rel[300]; size_t L = strlen(e2->d_name);
                    if (L < 4 || strcmp(e2->d_name + L - 4, ".png") != 0) continue;
                    snprintf(rel, sizeof rel, "pri/STAGE1/%s/%s", e1->d_name, e2->d_name);
                    gefunden++;
                    if (!gelistet(rel)) { printf("  UNGELISTET: %s\n", rel); ungelistet++; }
                }
                closedir(d2);
            }
            closedir(d1);
        }
    }
    printf("  %d Dateien im Baum, %d gelistet, %d veraendert, %d fehlen, %d ungelistet\n",
           gefunden, (int)n, falsch, fehlt, ungelistet);
    CHECK("die Liste ist nicht leer und der Baum wurde gelesen", n > 0 && gefunden > 0);
    CHECK("jede gelistete Freistellung hat genau den festgelegten Git-Blob", falsch == 0 && fehlt == 0);
    CHECK("keine Freistellung ohne Eintrag (Liste mit tools/maske/pri_hashes.py neu erzeugen)", ungelistet == 0);
    return g_fail;
}
