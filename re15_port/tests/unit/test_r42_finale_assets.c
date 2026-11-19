/* test_r42_finale_assets.c — Runde 42 Strang finale, Plan S1 (Pruefbericht s1 r2 R2): jede RE2-Datei des Finales ist bytegleich mit
 * ihrer Quelle.
 *
 *   ROOM7020 = info/re2leon/PL0/RDT/ROOM7020.RDT (Werkzeug tools/r42_finale/re2_r702_esp.py):
 *     R702.ESP        = RDT[0x2788 : 0x2E34]   Kopf-Offset [18] @RDT+0x50 = 0x2788, [19] + 4 = 0x2E34; Ids `16 27 15 1a`
 *     R702_16.TIM     = RDT @0x1F55C, R702_27.TIM @0x205FC, R702_15.TIM @0x2167C, R702_1A.TIM @0x226FC (Kopf [20] = 0x1F55C + Tafel
 *                       rueckwaerts vor [21] = 0 / 0x10A0 / 0x2120 / 0x31A0); Laenge aus dem TIM-Kopf
 *     R702.EDT        = RDT[0x5C90 : 0x5D50]   (RDT-Kopfwort +0x08), Satz 0x0F @0x05CCC `00 00 c3 20` (Einschlag, Se_on 0x020F0001
 *                       @0x80103b54-5c), Satz 0x0E @0x05CC8 (Landung, Stufe 3)
 *     R702.VH         = RDT[0x5D50 : 0x6B70]   (+0x0C), "pBAV"
 *     R702.VB         = RDT[0x6B70 : 0x6B70 + (VAB-Gesamtgroesse VH+0x0C - 0xE20)]   (+0x10)
 *   VOICE07.XAS = info/re2leon/PL0/VOICE/VOICE07.XAS:
 *     R702_XA03_LEON.XAS = Kanal-5-Sektoren im Lesefenster [5, 5 + 1026) Schritt 16 (STAGE7.BIN @0x156 `05 00 00 00 42 00`)
 *   Stufe 4 (Plan S9, Elza 5091): R702_XA03_CLAIRE.XAS = Kanal-5-Sektoren von VOICE17.XAS der Claire-PAL-Disc (SLES-10972, LBA 96572,
 *     2288 Sektoren) im Lesefenster [5, 5 + 1234) Schritt 16 (Claire-STAGE7.BIN @0x156 `05 00 00 00 4f 00`; Werkzeug
 *     tools/r42_finale/re2_xa_finale.py --claire). Die Disc liegt nicht im Repo (Dossier B 1.3: info/re2clair VOICE11.XAS ist nur
 *     Nullbytes, VOICE17.XAS fehlt) - geprueft werden darum Groesse (78 Sektoren), jeder Subheader `01 05 64 00` doppelt und die md5
 *     des Schnitts aus dem Werkzeuglauf.
 *
 * Die Paketliste (release/make_package.sh) fuehrt dieselben Dateien. Gegenprobe: RE15_R42F_RE2=<Ordner> liest die Dateien aus einem
 * anderen shared_assets/RE2-Stand (aaea5f8c2: rot, Dateien fehlen; ein gekipptes Byte: rot).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_finale_r42.h"

#define RE15_XSTR_(x) #x
#define RE15_XSTR(x)  RE15_XSTR_(x)

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, size_t *out)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)n + 1);
    if (b && fread(b, 1, (size_t)n, f) != (size_t)n) { free(b); b = NULL; }
    fclose(f);
    if (b) *out = (size_t)n;
    return b;
}
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }

static char s_re2[1024];
static uint8_t *datei(const char *name, size_t *n)
{
    char p[1200];
    snprintf(p, sizeof p, "%s/%s", s_re2, name);
    return slurp(p, n);
}

/* Datei == RDT[von : von + laenge]; laenge 0 = Dateilaenge */
static void gleich(const uint8_t *rdt, size_t nr, const char *name, uint32_t von, size_t soll)
{
    size_t n = 0;
    uint8_t *d = datei(name, &n);
    CHECK(d != NULL, "%s fehlt in %s", name, s_re2);
    if (!d) return;
    if (soll) CHECK(n == soll, "%s: %u Byte statt %u", name, (unsigned)n, (unsigned)soll);
    CHECK(von + n <= nr && memcmp(d, rdt + von, n) == 0, "%s != ROOM7020.RDT @0x%05X (%u Byte)", name, von, (unsigned)n);
    free(d);
}

static void t_rdt(void)
{
    size_t nr = 0;
    uint8_t *rdt = slurp(RE15_REPO_ROOT "/info/re2leon/PL0/RDT/ROOM7020.RDT", &nr);
    CHECK(rdt && nr > 0x24F9C, "info/re2leon/PL0/RDT/ROOM7020.RDT fehlt");
    if (!rdt || nr <= 0x24F9C) { free(rdt); return; }
    /* Kopf: +0x08/+0x0C/+0x10 = Tonbank (FUN_80059e54), Offsettafel ab +0x08 + 4*i: [18] ESP, [19] ESP-Ende - 4, [20] TIM-Block */
    CHECK(rd32(rdt + 0x08) == 0x5C90 && rd32(rdt + 0x0C) == 0x5D50 && rd32(rdt + 0x10) == 0x6B70, "Tonbank-Kopf %x %x %x",
          rd32(rdt + 0x08), rd32(rdt + 0x0C), rd32(rdt + 0x10));
    CHECK(rd32(rdt + 8 + 4 * 18) == 0x2788 && rd32(rdt + 8 + 4 * 19) + 4 == 0x2E34 && rd32(rdt + 8 + 4 * 20) == 0x1F55C,
          "ESP-/TIM-Kopf");
    gleich(rdt, nr, "R702.ESP", 0x2788, 0x2E34 - 0x2788);
    CHECK(memcmp(rdt + 0x2788, "\x16\x27\x15\x1a\xff\xff\xff\xff", 8) == 0, "ESP-Ids");
    static const struct { const char *n; uint32_t von; } tims[] = {
        { "R702_16.TIM", 0x1F55C }, { "R702_27.TIM", 0x205FC }, { "R702_15.TIM", 0x2167C }, { "R702_1A.TIM", 0x226FC } };
    for (size_t i = 0; i < sizeof tims / sizeof tims[0]; i++) {
        const uint8_t *t = rdt + tims[i].von;
        const uint32_t cl = rd32(t + 8), im = rd32(t + 8 + cl);
        CHECK(rd32(t) == 0x10 && rd32(t + 4) == 8, "%s: kein 4-bpp-TIM im RDT", tims[i].n);
        gleich(rdt, nr, tims[i].n, tims[i].von, 8 + cl + im);
    }
    /* Tonbank */
    const uint32_t gesamt = rd32(rdt + 0x5D50 + 12);
    CHECK(memcmp(rdt + 0x5D50, "pBAV", 4) == 0 && gesamt == 104448, "VAB-Kopf (pBAV, Gesamt %u)", gesamt);
    gleich(rdt, nr, RE15_FI_TONBANK ".EDT", 0x5C90, 0x5D50 - 0x5C90);
    gleich(rdt, nr, RE15_FI_TONBANK ".VH", 0x5D50, 0x6B70 - 0x5D50);
    gleich(rdt, nr, RE15_FI_TONBANK ".VB", 0x6B70, gesamt - (0x6B70 - 0x5D50));
    CHECK(memcmp(rdt + 0x5C90 + 4 * RE15_FI_SE_EINSCHLAG, "\x00\x00\xc3\x20", 4) == 0, "Satz 0x0F @0x05CCC");
    CHECK(0x5C90 + 4 * RE15_FI_SE_EINSCHLAG == 0x5CCC && 0x5C90 + 4 * RE15_FI_SE_LANDUNG == 0x5CC8, "Satznummern");
    free(rdt);
}

static void t_xa03(void)
{
    size_t nx = 0, na = 0;
    uint8_t *x = slurp(RE15_REPO_ROOT "/info/re2leon/PL0/VOICE/VOICE07.XAS", &nx);
    uint8_t *a = datei(RE15_FI_XA03_DATEI, &na);
    CHECK(x && a, "VOICE07.XAS oder %s fehlt", RE15_FI_XA03_DATEI);
    if (x && a) {
        const size_t sek = 2336, n = (RE15_FI_XA03_LESE + 15) / 16;
        CHECK(na == n * sek, "%s: %u Byte statt %u", RE15_FI_XA03_DATEI, (unsigned)na, (unsigned)(n * sek));
        for (size_t i = 0; i < n && na == n * sek && (5 + 16 * i + 1) * sek <= nx; i++) {
            const uint8_t *q = x + (5 + 16 * i) * sek;
            CHECK(memcmp(a + i * sek, q, sek) == 0 && q[1] == 5 && q[2] == 0x64, "%s Sektor %u", RE15_FI_XA03_DATEI, (unsigned)i);
        }
    }
    free(x); free(a);
}

/* md5 (RFC 1321), nur fuer die Pruefsumme des Claire-Schnitts (die Quelle liegt nicht im Repo). */
static void md5(const uint8_t *m, size_t n, uint8_t out[16])
{
    static const uint32_t K[64] = {
        0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,0x698098d8,0x8b44f7af,0xffff5bb1,
        0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,
        0xd8a1e681,0xe7d3fbc8,0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,0xfffa3942,
        0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
        0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,
        0x85845dd1,0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391 };
    static const uint8_t R[64] = { 7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
                                   4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21 };
    uint32_t h[4] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };
    const size_t L = ((n + 8) / 64 + 1) * 64;
    uint8_t *b = (uint8_t *)calloc(L, 1);
    if (!b) { memset(out, 0, 16); return; }
    memcpy(b, m, n); b[n] = 0x80;
    const uint64_t bits = (uint64_t)n * 8;
    for (int i = 0; i < 8; i++) b[L - 8 + i] = (uint8_t)(bits >> (8 * i));
    for (size_t o = 0; o < L; o += 64) {
        uint32_t w[16];
        for (int i = 0; i < 16; i++) w[i] = (uint32_t)b[o+4*i] | (uint32_t)b[o+4*i+1] << 8 | (uint32_t)b[o+4*i+2] << 16 | (uint32_t)b[o+4*i+3] << 24;
        uint32_t a = h[0], bb = h[1], c = h[2], d = h[3];
        for (int i = 0; i < 64; i++) {
            uint32_t f; int g;
            if (i < 16)      { f = (bb & c) | (~bb & d); g = i; }
            else if (i < 32) { f = (d & bb) | (~d & c); g = (5 * i + 1) % 16; }
            else if (i < 48) { f = bb ^ c ^ d;          g = (3 * i + 5) % 16; }
            else             { f = c ^ (bb | ~d);       g = (7 * i) % 16; }
            const uint32_t t = d; d = c; c = bb;
            const uint32_t x = a + f + K[i] + w[g];
            bb = bb + ((x << R[i]) | (x >> (32 - R[i]))); a = t;
        }
        h[0] += a; h[1] += bb; h[2] += c; h[3] += d;
    }
    free(b);
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) out[4*i+j] = (uint8_t)(h[i] >> (8 * j));
}

static void t_xa03_claire(void)
{
    size_t na = 0;
    uint8_t *a = datei(RE15_FI_XA03_DATEI_ELZA, &na);
    CHECK(a != NULL, "%s fehlt (Elza 5091 waere beim Einschlag stumm)", RE15_FI_XA03_DATEI_ELZA);
    if (a) {
        const size_t sek = 2336, n = (RE15_FI_XA03_LESE_ELZA + 15) / 16;   /* 1234 -> 78 Kanalsektoren */
        CHECK(n == 78 && na == n * sek, "%s: %u Byte statt %u", RE15_FI_XA03_DATEI_ELZA, (unsigned)na, (unsigned)(n * sek));
        for (size_t i = 0; i < n && na == n * sek; i++)
            CHECK(memcmp(a + i * sek, "\x01\x05\x64\x00\x01\x05\x64\x00", 8) == 0, "%s Sektor %u Subheader", RE15_FI_XA03_DATEI_ELZA, (unsigned)i);
        uint8_t h[16];
        md5(a, na, h);
        static const uint8_t soll[16] = { 0xdf,0x88,0xfe,0x57,0x7d,0x84,0xdc,0xe0,0x67,0x16,0x68,0xc4,0xf9,0x68,0x08,0xbc };
        CHECK(memcmp(h, soll, 16) == 0, "%s: md5 weicht vom Schnitt df88fe577d84dce0671668c4f96808bc ab", RE15_FI_XA03_DATEI_ELZA);
    }
    free(a);
}

int main(void)
{
    const char *o = getenv("RE15_R42F_RE2");
    snprintf(s_re2, sizeof s_re2, "%s", (o && *o) ? o : RE15_XSTR(RE15_ASSETS_PATH) "/../RE2");
    printf("unit_r42_finale_assets: RE2-Ordner %s\n", s_re2);
    t_rdt();
    t_xa03();
    t_xa03_claire();
    if (fails) { printf("unit_r42_finale_assets: %d Fehler\n", fails); return 1; }
    printf("unit_r42_finale_assets: OK (R702.ESP, 4 TIM, R702.EDT/.VH/.VB, R702_XA03_LEON.XAS bytegleich, R702_XA03_CLAIRE.XAS md5)\n");
    return 0;
}
