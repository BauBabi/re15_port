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

int main(void)
{
    const char *o = getenv("RE15_R42F_RE2");
    snprintf(s_re2, sizeof s_re2, "%s", (o && *o) ? o : RE15_XSTR(RE15_ASSETS_PATH) "/../RE2");
    printf("unit_r42_finale_assets: RE2-Ordner %s\n", s_re2);
    t_rdt();
    t_xa03();
    if (fails) { printf("unit_r42_finale_assets: %d Fehler\n", fails); return 1; }
    printf("unit_r42_finale_assets: OK (R702.ESP, 4 TIM, R702.EDT/.VH/.VB, R702_XA03_LEON.XAS bytegleich)\n");
    return 0;
}
