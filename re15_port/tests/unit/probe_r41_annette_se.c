/*
 * probe_r41_annette_se.c — Runde 41 Spur AN, Nachbesserung 1: die Mini-Bank shared_assets/RE2/ANNETTE_SE.VBS
 * (tools/r41_annette/an_se_cut.py, Belege analysis/befunde_runde41/AN_belege/an_se.md). Teile (je ein ctest,
 * registriert in tests/unit/probes/r41_annette_se.cmake):
 *
 *   bank    Datei: Groesse/FNV aus gen/re2_annette_se_bank.inc, Trailer-VH-Offset 0x10, VH parsebar, 4 Wellen,
 *           4 Saetze = RE15_AN_SE_* (include/re15_annette_r41.h).
 *   toene   je Satz: EDT-Bytes 0/3 und Prioritaets-Nibble = RE2-Quellsatz (KRACH ENEMSE-Bank 64 0x0B `00 00 d2 04`,
 *           STURZ ROOM6141 0x0A `00 01 23 20`, BRUELL ROOM6141 0x0C `00 00 c3 01`, ZUG_AN ROOM7020 0x0C `00 00 93 00`),
 *           Programm 0, Ton-Lage 0/1/3/4; die 32 Ton-Bytes jeder Lage = Quell-Ton ausser +0x14..+0x17 (Programm,
 *           VAG-Index); Tonhoehe note2pitch2 = 0x5F3 / 0x400 / 0x1C4 / 0x23A (gegen die echte RE2-Aufnahme belegt, an_se.md).
 *   lagen   Lagen je Satz 1/2/1/1 (STURZ: Satz-Byte 3 = 0x20 -> eine Zusatzlage, beide auf derselben Welle wie in
 *           ROOM6141 Ton 2/3 -> VAG 10), Wellen dekodierbar, Laenge bis zum Endblock wie im Werkzeug, nicht stumm.
 */
#include "re15_vab.h"
#include "re15_annette_r41.h"
#define RE2_ANSE_MIT_QUELLEN 1          /* re2_anse_quell_ton / re2_anse_quell_edt */
#include "../../engine/src/gen/re2_annette_se_bank.inc"

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fehler = 0;
#define PRUEF(c, ...) do { if (!(c)) { g_fehler++; printf("FEHLER: " __VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }
static uint32_t fnv1a(const uint8_t *d, size_t n)
{
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < n; i++) { h ^= d[i]; h *= 16777619u; }
    return h;
}

static uint8_t   *s_d = NULL;
static size_t     s_n = 0;
static uint32_t   s_vh = 0;
static re15_vab_t s_vab;

static int bank_laden(void)
{
    char p[600];
    snprintf(p, sizeof p, "%s/ANNETTE_SE.VBS", RE15_ASSET_RE2_DIR);
    s_d = slurp(p, &s_n);
    PRUEF(s_d && s_n == RE2_ANSE_DATEI_GROESSE && fnv1a(s_d, s_n) == RE2_ANSE_DATEI_FNV,
          "ANNETTE_SE.VBS fehlt/weicht ab (%zu B, Soll %u B FNV 0x%08X aus gen/re2_annette_se_bank.inc)",
          s_n, (unsigned)RE2_ANSE_DATEI_GROESSE, (unsigned)RE2_ANSE_DATEI_FNV);
    if (!s_d || s_n != RE2_ANSE_DATEI_GROESSE) return -1;
    s_vh = rd32(s_d + RE2_ANSE_EDT_SIZE - 8);
    PRUEF(s_vh == 0x10u, "Trailer-VH-Offset 0x%X statt 0x10 (4 Saetze x 4 B)", (unsigned)s_vh);
    PRUEF(memcmp(s_d + s_vh, "pBAV", 4) == 0, "kein pBAV am VH-Offset");
    PRUEF(re15_vab_parse(s_d + s_vh, RE2_ANSE_EDT_SIZE - s_vh, &s_vab) == 0, "VH nicht parsebar");
    return 0;
}

static void teil_bank(void)
{
    if (bank_laden() != 0) return;
    printf("ANNETTE_SE.VBS %zu B FNV 0x%08X, VH @0x%X: Programme %d, Toene %d, Wellen %d\n", s_n, fnv1a(s_d, s_n),
           (unsigned)s_vh, s_vab.program_count, s_vab.tone_count, s_vab.vag_count);
    PRUEF(s_vab.vag_count == RE2_ANSE_SE_COUNT, "%d Wellen statt %d", s_vab.vag_count, RE2_ANSE_SE_COUNT);
    PRUEF(RE2_ANSE_SE_COUNT == RE15_AN_SE_N && RE2_ANSE_SE_KRACH == RE15_AN_SE_KRACH && RE2_ANSE_SE_STURZ == RE15_AN_SE_STURZ &&
          RE2_ANSE_SE_BRUELL == RE15_AN_SE_BRUELL && RE2_ANSE_SE_ZUG_AN == RE15_AN_SE_ZUG_AN,
          "Satznummern Bank != RE15_AN_SE_*");
    PRUEF(RE2_ANSE_VBD_OFF + RE2_ANSE_VBD_SIZE == RE2_ANSE_DATEI_GROESSE, "VBD endet nicht am Dateiende");
    uint32_t summe = 0;
    for (int i = 0; i < s_vab.vag_count; i++) {
        PRUEF(s_vab.samples[i].offset == summe, "Welle %d @%u statt @%u", i, (unsigned)s_vab.samples[i].offset, (unsigned)summe);
        summe += s_vab.samples[i].size;
    }
    PRUEF(summe == RE2_ANSE_VBD_SIZE, "Wellen %u B statt VBD %u B", (unsigned)summe, (unsigned)RE2_ANSE_VBD_SIZE);
    /* VH-Groessenfeld +0x0C = VH + VBD (wie im Werkzeug gesetzt) */
    PRUEF(rd32(s_d + s_vh + 12) == (RE2_ANSE_EDT_SIZE - s_vh - 8) + RE2_ANSE_VBD_SIZE, "VH-Groessenfeld");
}

static void teil_toene(void)
{
    if (bank_laden() != 0) return;
    static const int lage_soll[RE2_ANSE_SE_COUNT] = { 0, 1, 3, 4 };
    static const uint16_t pitch_soll[RE2_ANSE_SE_COUNT] = {
        RE2_ANSE_PITCH_KRACH, RE2_ANSE_PITCH_STURZ, RE2_ANSE_PITCH_BRUELL, RE2_ANSE_PITCH_ZUG_AN };
    static const int lagen[RE2_ANSE_SE_COUNT] = {
        RE2_ANSE_LAGEN_KRACH, RE2_ANSE_LAGEN_STURZ, RE2_ANSE_LAGEN_BRUELL, RE2_ANSE_LAGEN_ZUG_AN };
    PRUEF(RE2_ANSE_PITCH_KRACH == 0x5F3 && RE2_ANSE_PITCH_STURZ == 0x400 && RE2_ANSE_PITCH_BRUELL == 0x1C4 &&
          RE2_ANSE_PITCH_ZUG_AN == 0x23A, "Tonhoehen in der .inc weichen von den belegten Werten ab");
    for (int s = 0; s < RE2_ANSE_SE_COUNT; s++) {
        const uint8_t *e = s_d + s * 4, *q = re2_anse_quell_edt[s];
        printf("Satz %d: EDT %02x %02x %02x %02x (Quelle %02x %02x %02x %02x)\n", s, e[0], e[1], e[2], e[3], q[0], q[1], q[2], q[3]);
        PRUEF(e[0] == q[0] && e[3] == q[3] && (e[2] & 0x0F) == (q[2] & 0x0F), "Satz %d: Byte 0/3/Prio weicht von der Quelle ab", s);
        PRUEF(e[1] == 0 && (e[2] >> 4) == lage_soll[s], "Satz %d: Programm %d / Ton-Lage %d statt 0 / %d", s, e[1], e[2] >> 4,
              lage_soll[s]);
        PRUEF((q[3] >> 5) + 1 == lagen[s], "Satz %d: Quelle hat %d Lagen, .inc %d", s, (q[3] >> 5) + 1, lagen[s]);
        for (int k = 0; k < lagen[s]; k++) {
            const uint8_t *t = s_d + s_vh + 0x20 + 0x800 + (lage_soll[s] + k) * 0x20;
            const uint8_t *qt = re2_anse_quell_ton[s];
            PRUEF(memcmp(t, qt, 0x14) == 0 && memcmp(t + 0x18, qt + 0x18, 8) == 0,
                  "Satz %d Lage %d: Ton-Bytes weichen ausser +0x14..+0x17 von der Quelle ab", s, k);
            PRUEF(t[0x14] == 0 && t[0x15] == 0 && t[0x16] == s + 1 && t[0x17] == 0,
                  "Satz %d Lage %d: Programm/VAG-Index %02x%02x/%02x%02x", s, k, t[0x15], t[0x14], t[0x17], t[0x16]);
            const re15_vab_tone_t *vt = &s_vab.tones[lage_soll[s] + k];
            uint16_t p = re15_vab_note2pitch2(vt->min_note, vt->pitch_shift, vt->center_note, vt->pitch_shift);
            printf("  Lage %d: Ton %d note %u center %u shift %u vol %u pan %u -> pitch 0x%03X = %.1f Hz\n", k,
                   lage_soll[s] + k, vt->min_note, vt->center_note, vt->pitch_shift, vt->vol, vt->pan, p, 44100.0 * p / 4096.0);
            PRUEF(p == pitch_soll[s], "Satz %d Lage %d: pitch 0x%X statt 0x%X", s, k, p, pitch_soll[s]);
        }
    }
}

static void teil_lagen(void)
{
    if (bank_laden() != 0) return;
    static const int lagen[RE2_ANSE_SE_COUNT] = {
        RE2_ANSE_LAGEN_KRACH, RE2_ANSE_LAGEN_STURZ, RE2_ANSE_LAGEN_BRUELL, RE2_ANSE_LAGEN_ZUG_AN };
    static const int abtast[RE2_ANSE_SE_COUNT] = {
        RE2_ANSE_ABTAST_KRACH, RE2_ANSE_ABTAST_STURZ, RE2_ANSE_ABTAST_BRUELL, RE2_ANSE_ABTAST_ZUG_AN };
    static const uint16_t pitch_soll[RE2_ANSE_SE_COUNT] = {
        RE2_ANSE_PITCH_KRACH, RE2_ANSE_PITCH_STURZ, RE2_ANSE_PITCH_BRUELL, RE2_ANSE_PITCH_ZUG_AN };
    static const char *namen[RE2_ANSE_SE_COUNT] = { "KRACH", "STURZ", "BRUELL", "ZUG_AN" };
    static int16_t pcm[40000];
    for (int s = 0; s < RE2_ANSE_SE_COUNT; s++) {
        int vags[8], tones[8];
        int n = re15_edt_resolve_layers_ex(s_d, &s_vab, s, vags, tones, 8);
        PRUEF(n == lagen[s], "Satz %d: %d Lagen statt %d", s, n, lagen[s]);
        for (int k = 0; k < n; k++)
            PRUEF(vags[k] == s, "Satz %d Lage %d: Welle %d statt %d", s, k, vags[k], s);
        if (n <= 0) continue;
        uint32_t off = s_vab.samples[vags[0]].offset, sz = s_vab.samples[vags[0]].size;
        const uint8_t *w = s_d + RE2_ANSE_VBD_OFF + off;
        int np = re15_vag_adpcm_decode(w, sz, pcm, sizeof pcm / sizeof pcm[0]);
        /* Endblock (Flag 1 ohne Wiederholung) wie im Werkzeug */
        int ende = -1;
        for (uint32_t b = 0; b < sz; b += 16) if ((w[b + 1] & 1) && !(w[b + 1] & 2)) { ende = (int)(b / 16 + 1) * 28; break; }
        double q = 0; int spitze = 0;
        for (int i = 0; i < ende && i < np; i++) { q += (double)pcm[i] * pcm[i]; if (abs(pcm[i]) > spitze) spitze = abs(pcm[i]); }
        double rms = (ende > 0) ? sqrt(q / ende) : 0.0;
        printf("Satz %d %-6s: %d Lage(n), Welle %d %u B, dekodiert %d Abtastwerte, Endblock bei %d = %.3f s, RMS %.0f, Spitze %d\n",
               s, namen[s], n, vags[0], (unsigned)sz, np, ende, ende / (44100.0 * pitch_soll[s] / 4096.0), rms, spitze);
        PRUEF(ende == abtast[s], "Satz %d: Endblock bei %d statt %d Abtastwerten", s, ende, abtast[s]);
        PRUEF(np >= ende, "Satz %d: nur %d Abtastwerte dekodiert", s, np);
        PRUEF(spitze > 4000 && rms > 300.0, "Satz %d: Welle (fast) stumm (RMS %.0f, Spitze %d)", s, rms, spitze);
    }
}

int main(int argc, char **argv)
{
    const char *teil = (argc > 1) ? argv[1] : "bank";
    if (!strcmp(teil, "bank"))       teil_bank();
    else if (!strcmp(teil, "toene")) teil_toene();
    else if (!strcmp(teil, "lagen")) teil_lagen();
    else { printf("unbekannter Teil %s (bank|toene|lagen)\n", teil); return 2; }
    free(s_d);
    printf("%s: %s (%d Fehler)\n", teil, g_fehler ? "ROT" : "GRUEN", g_fehler);
    return g_fehler ? 1 : 0;
}
