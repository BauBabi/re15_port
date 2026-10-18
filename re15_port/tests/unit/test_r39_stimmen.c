/*
 * Runde 39 Spur A "stimmen" — RIEGEL (Unit) fuer die Zuordnung (Raum, Nachricht) -> Nutzer-Aufnahme
 * (include/re15_stimmen_r39.h, engine/src/stimmen_r39.c). Dossier analysis/befunde_runde39/A_stimmen.md.
 *
 *   tabelle   (1) jede Zeile der Tabelle zeigt auf eine vorhandene RIFF/WAVE-Datei des Nutzers,
 *             (2) die Message_on-Stellen der Szenen tragen genau die Nachrichten-Ids der Tabelle:
 *                 ROOM1050 adaruf k_ruf +0x1C/+0x50/+0x80 `2b 16|17|18 00 00`, ROOM1150 irons_tod Programm 0
 *                 OP_MSG(22..29) in Szenenreihenfolge, ROOM11B0 szl-Programm +0x0008 `2b 14` / +0x010A `2b 15`,
 *                 ROOM1171.RDT sub02 @0x014BA `2b 00` / @0x0151E `2b 01` / @0x01586 `2b 03`,
 *             (3) Zuordnung: 1050 22/23/24 -> main09/10/11, 1150 22..28 -> main15..21, 11B0 20/21 -> main13/14,
 *                 1171 3 -> main02; 1150 29 -> main22 (Nutzer-Nachtrag 2, 2026-10-18),
 *             (4) Sperre: ROOM11B0 msg 13 (sub03 @0x0126E/@0x01298 `2b 0d ff ff`) und 14 (main00 @0x0106C
 *                 `2c 07 01 31 ..`), ROOM1171 msg 2 bleiben stumm (-1), weil main13/14/02.wav anderen Zeilen gehoeren,
 *             (5) unberuehrt: die Ordnerregel fuer alle anderen (Raum, Nachricht), insbesondere ROOM1170 msg 3
 *                 (Leons Helipad) und ROOM11B0 msg 1..12 (alte Aufnahmen), Tabelle ohne Doppel.
 *   pegel     Stufe 2 des Pegel-Bodens (include/re15_stimmen_pegel_r39.h, Nachbesserung 1, NUTZER-VORGABE "ruhig lauter
 *             anheben") am GANZEN Bestand synchro/STAGE*/room*/main*.wav (Liste aus CMake GLOB): nach der Spitzen-Stufe
 *             des Laders (audio_pc.c "Level-floor", Ziel 14745, Deckel 4x - hier nachgebildet) greift Stufe 2 genau bei den
 *             Dateien unter T (= den 10 Irons-Dateien room1150, alle am 4x-Deckel), hebt sie auf R oder bis zur Spitze C
 *             (kein Abtastwert ueber C), ist idempotent, und laesst alle anderen Dateien bitgleich (Leon 1150 main15/16/
 *             17/22 ausdruecklich). Herleitung T/R/C wird am Bestand mitgezaehlt (min/median rms, max Spitze).
 */
#include "re15_stimmen_r39.h"
#include "re15_stimmen_pegel_r39.h"
#include "re15_adaruf.h"
#include "re15_irons_tod.h"
#include "re15_szenenleon_r36.h"
#include "re15_rdt.h"
#include "re15_msg.h"

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
#ifndef RE15_R39_BESTAND
#define RE15_R39_BESTAND "r39_stimmen_bestand.txt"
#endif

static int fails = 0, checks = 0;
#define CHECK(cond, ...) do { checks++; if (cond) { printf("  ok   "); } else { fails++; printf("  FAIL "); } \
                              printf(__VA_ARGS__); printf("\n"); } while (0)
/* leise Pruefung (nur Verletzungen drucken - am Bestand sind es Hunderte) */
#define CHECKQ(cond, ...) do { checks++; if (!(cond)) { fails++; printf("  FAIL "); printf(__VA_ARGS__); printf("\n"); } } while (0)

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)(sz > 0 ? sz : 1));
    if (!b || sz <= 0 || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return NULL; }
    fclose(f);
    if (n) *n = sz;
    return b;
}

static uint8_t *rdt(unsigned id, long *n)
{
    char p[600];
    snprintf(p, sizeof p, "%s/STAGE%u/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, id >> 12, id);
    return slurp(p, n);
}

static int bytes(const uint8_t *b, long n, long off, const char *hex)
{
    long i = 0;
    for (const char *h = hex; *h; h += 2, i++) {
        while (*h == ' ') h++;
        if (!*h) break;
        unsigned v = 0; sscanf(h, "%2x", &v);
        if (off + i >= n || b[off + i] != (uint8_t)v) return 0;
    }
    return 1;
}

static int teil_tabelle(void)
{
    const int n = re15_stimmen_r39_anzahl();
    CHECK(n == 14, "Tabelle: %d Zeilen (soll 14 = 1050: 3 + 1150: 8 + 11B0: 2 + 1171: 1)", n);

    /* (3) Zuordnung je Punkt des Nutzers */
    static const struct { unsigned raum; int msg; int datei; } k_soll[] = {
        { 0x1050, 22,  9 }, { 0x1050, 23, 10 }, { 0x1050, 24, 11 },
        { 0x1150, 22, 15 }, { 0x1150, 23, 16 }, { 0x1150, 24, 17 }, { 0x1150, 25, 18 },
        { 0x1150, 26, 19 }, { 0x1150, 27, 20 }, { 0x1150, 28, 21 }, { 0x1150, 29, 22 },   /* 29: Nachtrag 2 */
        { 0x11B0, 20, 13 }, { 0x11B0, 21, 14 },
        { 0x1171,  3,  2 },
    };
    for (unsigned i = 0; i < sizeof k_soll / sizeof k_soll[0]; i++) {
        char st[24] = "main99";
        int r = re15_stimmen_r39_stamm(k_soll[i].raum, k_soll[i].msg, st, sizeof st);
        char soll[24]; snprintf(soll, sizeof soll, "main%02d", k_soll[i].datei);
        CHECK(r == 1 && !strcmp(st, soll), "ROOM%04X msg %d -> %s (r=%d, '%s')", k_soll[i].raum, k_soll[i].msg, soll, r, st);
    }

    /* (1) jede Tabellenzeile zeigt auf eine vorhandene Nutzer-Datei; keine Doppel */
    for (int i = 0; i < n; i++) {
        const re15_stimmen_r39_t *e = re15_stimmen_r39_eintrag(i);
        if (!e) { CHECK(0, "Eintrag %d fehlt", i); continue; }
        char q[600]; long sz = 0;
        snprintf(q, sizeof q, "%s/STAGE%u/room%04X/main%02u.wav", RE15_SYNCHRO_DIR, e->raum >> 12, e->raum, e->datei);
        uint8_t *w = slurp(q, &sz);
        CHECK(w && sz > 44 && !memcmp(w, "RIFF", 4) && !memcmp(w + 8, "WAVE", 4),
              "Datei %s (%ld Byte, RIFF/WAVE)", q, sz);
        free(w);
        for (int j = 0; j < i; j++) {
            const re15_stimmen_r39_t *f = re15_stimmen_r39_eintrag(j);
            CHECK(!(f->raum == e->raum && (f->nachricht == e->nachricht || f->datei == e->datei)),
                  "kein Doppel: Zeile %d (%04X,%u->%u) gegen %d (%04X,%u->%u)", i, e->raum, e->nachricht, e->datei,
                  j, f->raum, f->nachricht, f->datei);
        }
    }
    CHECK(re15_stimmen_r39_eintrag(n) == NULL && re15_stimmen_r39_eintrag(-1) == NULL, "Eintrag ausserhalb = NULL");

    /* (2) Message_on-Stellen der Szenen */
    {
        int len = 0; const uint8_t *p = re15_adaruf_programm(&len);
        CHECK(p && len > 0x84 && bytes(p, len, 0x1C, "2b160000") && bytes(p, len, 0x50, "2b170000") &&
              bytes(p, len, 0x80, "2b180000"), "ROOM1050 k_ruf +0x1C/+0x50/+0x80 = Message_on 22/23/24 (%d Byte)", len);
        CHECK(RE15_ADARUF_MSG_RUF == 22 && RE15_ADARUF_MSG_LEON_A == 23 && RE15_ADARUF_MSG_LEON_B == 24,
              "re15_adaruf.h Ids 22/23/24");
        int l22 = 0, l24 = 0;
        CHECK(re15_adaruf_meldung(22, &l22) && l22 > 0 && re15_adaruf_meldung(24, &l24) && l24 > 0,
              "ROOM1050 Port-Texte 22 (%d B) / 24 (%d B) vorhanden", l22, l24);
    }
    {
        int len = 0; const uint8_t *p = re15_irons_tod_programm(0, &len);
        int folge[8] = { 0 }; int nf = 0, ok = p != NULL;
        for (long i = 0; p && i + 3 < len; i++)
            if (p[i] == 0x2b && p[i + 1] >= 22 && p[i + 1] <= 29 && p[i + 2] == 0 && p[i + 3] == 0) {
                if (nf < 8) folge[nf] = p[i + 1];
                nf++; i += 3;
            }
        for (int i = 0; i < 8; i++) ok &= (nf == 8 && folge[i] == 22 + i);
        CHECK(ok, "ROOM1150 Programm 0 (Szene): Message_on 22..29 je einmal in Szenenreihenfolge (%d Treffer: %d %d %d %d %d %d %d %d)",
              nf, folge[0], folge[1], folge[2], folge[3], folge[4], folge[5], folge[6], folge[7]);
        int l29 = 0;
        CHECK(re15_irons_tod_meldung(0x1150, 29, &l29) && l29 > 0, "ROOM1150 Port-Text 29 vorhanden (%d B)", l29);
    }
    {
        int len = 0; const uint8_t *p = re15_szl_programm(&len);
        CHECK(p && len > 0x10E && bytes(p, len, 0x0008, "2b140000") && bytes(p, len, 0x010A, "2b150000"),
              "ROOM11B0 szl-Programm +0x0008 / +0x010A = Message_on 20 / 21 (%d Byte)", len);
        CHECK(RE15_SZL_MSG_HOLD == 20 && RE15_SZL_MSG_FOLLOW == 21, "re15_szenenleon_r36.h Ids 20/21");
    }
    {
        long n1 = 0; uint8_t *b = rdt(0x1171, &n1);
        CHECK(b && bytes(b, n1, 0x14BA, "2b000000") && bytes(b, n1, 0x151E, "2b010000") && bytes(b, n1, 0x1586, "2b030000"),
              "ROOM1171.RDT sub02 @0x014BA/@0x0151E/@0x01586 = Message_on 0/1/3");
        free(b);
        long n2 = 0; uint8_t *c = rdt(0x11B0, &n2);
        CHECK(c && bytes(c, n2, 0x126E, "2b0dffff") && bytes(c, n2, 0x1298, "2b0dffff") && bytes(c, n2, 0x106C, "2c070131"),
              "ROOM11B0.RDT sub03 @0x0126E/@0x01298 = Message_on 13, main00 @0x0106C Aot_set Platz 7 (msg 14)");
        free(c);
    }
    /* Wortlaut der Original-Nachrichten, dekodiert wie im Spiel (re15_msg_load_room_block + re15_msg_get_text):
     * die gesperrten Ids sind Untersuchungstexte / eine nie gezeigte Zeile, die Zieldatei spricht etwas anderes. */
    {
        static const struct { unsigned raum; int id; const char *text; } k_text[] = {
            { 0x1171, 3, "John: Alright, let's get down from here!" },
            { 0x1171, 2, "John: C'mon, give me a hand!" },
            { 0x11B0, 13, "It doesn't seem like you could move this on your own." },
            { 0x11B0, 14, "The shutter is tightly sealed in place." },
        };
        for (unsigned i = 0; i < sizeof k_text / sizeof k_text[0]; i++) {
            long rn = 0; uint8_t *rb = rdt(k_text[i].raum, &rn);
            static re15_rdt_t rd; memset(&rd, 0, sizeof rd);
            const char *t = NULL;
            if (rb && re15_rdt_parse(rb, (size_t)rn, &rd) >= 0 && rd.messages) {
                re15_msg_load_room_block(rd.messages, rd.messages_size);
                t = re15_msg_get_text(k_text[i].id);
            }
            CHECK(t && !strcmp(t, k_text[i].text), "ROOM%04X msg %d = \"%s\" (ist \"%s\")", k_text[i].raum, k_text[i].id,
                  k_text[i].text, t ? t : "(null)");
            free(rb);
        }
    }

    /* (4) Sperre: die Datei gehoert einer anderen Zeile -> stumm */
    static const struct { unsigned raum; int msg; } k_sperre[] = {
        { 0x11B0, 13 }, { 0x11B0, 14 }, { 0x1171, 2 },
        { 0x1050, 9 }, { 0x1050, 11 }, { 0x1150, 15 }, { 0x1150, 21 },
    };
    for (unsigned i = 0; i < sizeof k_sperre / sizeof k_sperre[0]; i++) {
        char st[24] = "main77";
        int r = re15_stimmen_r39_stamm(k_sperre[i].raum, k_sperre[i].msg, st, sizeof st);
        CHECK(r == -1 && !strcmp(st, "main77"), "Sperre ROOM%04X msg %d (r=%d, stamm unveraendert '%s')",
              k_sperre[i].raum, k_sperre[i].msg, r, st);
    }

    /* (5) unberuehrt */
    static const struct { unsigned raum; int msg; } k_frei[] = {
        { 0x11B0, 1 }, { 0x11B0, 12 }, { 0x11B0, 22 }, { 0x1050, 8 }, { 0x1050, 25 }, { 0x1150, 2 }, { 0x1150, 14 },
        { 0x1150, 30 }, { 0x1170, 3 }, { 0x1170, 0 }, { 0x1171, 0 }, { 0x1171, 1 }, { 0x1240, 0 }, { 0x1241, 3 },
        { 0x1011, 16 }, { 0x1141, 42 }, { 0x1100, 40 }, { 0x1051, 22 }, { 0x1151, 22 }, { 0x11B1, 20 },
    };
    for (unsigned i = 0; i < sizeof k_frei / sizeof k_frei[0]; i++) {
        char st[24] = "main55";
        int r = re15_stimmen_r39_stamm(k_frei[i].raum, k_frei[i].msg, st, sizeof st);
        CHECK(r == 0 && !strcmp(st, "main55"), "Regel bleibt: ROOM%04X msg %d (r=%d)", k_frei[i].raum, k_frei[i].msg, r);
    }
    CHECK(re15_stimmen_r39_stamm(0x1050, 22, NULL, 0) == 1 && re15_stimmen_r39_stamm(0x1050, 256, NULL, 0) == 0 &&
          re15_stimmen_r39_stamm(0x1050, -1, NULL, 0) == 0, "stamm == NULL nur Ergebnis; Ids ausserhalb 0..255 = 0");
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------
 * pegel: Stufe 2 (re15_stimmen_pegel_r39) am ganzen Bestand
 * ------------------------------------------------------------------------------------------------------------- */

/* WAV -> int16 mono (PCM 16/24/32 bit, auch WAVE_FORMAT_EXTENSIBLE; Stereo gemittelt, wie der Lader) */
static int16_t *wav_mono16(const char *p, long *out_n, int *out_rate, int *out_bits, int *out_ch)
{
    long n = 0; uint8_t *b = slurp(p, &n);
    if (!b || n < 12 || memcmp(b, "RIFF", 4) || memcmp(b + 8, "WAVE", 4)) { free(b); return NULL; }
    long o = 12; int tag = 0, ch = 0, rate = 0, bits = 0; const uint8_t *d = NULL; long dn = 0;
    while (o + 8 <= n) {
        uint32_t sz = (uint32_t)b[o + 4] | ((uint32_t)b[o + 5] << 8) | ((uint32_t)b[o + 6] << 16) | ((uint32_t)b[o + 7] << 24);
        if (!memcmp(b + o, "fmt ", 4) && o + 24 <= n) {
            tag = b[o + 8] | (b[o + 9] << 8); ch = b[o + 10] | (b[o + 11] << 8);
            rate = (int)((uint32_t)b[o + 12] | ((uint32_t)b[o + 13] << 8) | ((uint32_t)b[o + 14] << 16) | ((uint32_t)b[o + 15] << 24));
            bits = b[o + 22] | (b[o + 23] << 8);
            if (tag == 0xFFFE && sz >= 26) tag = b[o + 8 + 24] | (b[o + 8 + 25] << 8);
        } else if (!memcmp(b + o, "data", 4)) { d = b + o + 8; dn = (long)sz; if (o + 8 + dn > n) dn = n - o - 8; }
        o += 8 + (long)sz + (sz & 1);
    }
    if (!d || tag != 1 || ch < 1 || (bits != 16 && bits != 24 && bits != 32)) { free(b); return NULL; }
    int bps = bits / 8; long frames = dn / (bps * ch);
    int16_t *m = (int16_t *)malloc((size_t)(frames > 0 ? frames : 1) * sizeof(int16_t));
    for (long i = 0; i < frames; i++) {
        long acc = 0;
        for (int c = 0; c < ch; c++) {
            const uint8_t *s = d + (i * ch + c) * bps; int32_t v;
            if (bits == 16) v = (int16_t)(s[0] | (s[1] << 8));
            else if (bits == 24) { v = (int32_t)(((uint32_t)s[0] << 8) | ((uint32_t)s[1] << 16) | ((uint32_t)s[2] << 24)); v >>= 16; }
            else { v = (int32_t)((uint32_t)s[0] | ((uint32_t)s[1] << 8) | ((uint32_t)s[2] << 16) | ((uint32_t)s[3] << 24)); v >>= 16; }
            acc += v;
        }
        m[i] = (int16_t)(acc / ch);
    }
    free(b);
    *out_n = frames; *out_rate = rate; *out_bits = bits; *out_ch = ch;
    return m;
}

/* Spitzen-Stufe des Laders, nachgebildet (audio_pc.c re15_voice_load_clip "Level-floor": Ziel 14745 = ~45 % FS,
 * Deckel 4x = 1024 Q8; seit aa346af1 2026-06-28). Liefert den Faktor Q8. */
static int stufe1(int16_t *pcm, long n)
{
    int32_t peak = 1;
    for (long i = 0; i < n; i++) { int32_t a = pcm[i] < 0 ? -pcm[i] : pcm[i]; if (a > peak) peak = a; }
    if (peak >= 14745) return 256;
    int g = 14745 * 256 / peak; if (g > 4 * 256) g = 4 * 256;
    for (long i = 0; i < n; i++) {
        int32_t s = (pcm[i] * g) >> 8;
        if (s > 32767) s = 32767; else if (s < -32768) s = -32768;
        pcm[i] = (int16_t)s;
    }
    return g;
}

static int cmp_int(const void *a, const void *b) { int x = *(const int *)a, y = *(const int *)b; return x < y ? -1 : x > y; }

static int teil_pegel(void)
{
    static const char *k_irons[10] = { "02", "05", "07", "09", "12", "13", "18", "19", "20", "21" };   /* Zensus Dossier §9.1 */
    static const char *k_leon1150[4] = { "15", "16", "17", "22" };
    FILE *lf = fopen(RE15_R39_BESTAND, "r");
    CHECK(lf != NULL, "Bestandsliste %s (CMake GLOB synchro/STAGE*/room*/main*.wav)", RE15_R39_BESTAND);
    if (!lf) return 1;
    char zeile[1024]; int dateien = 0, unber = 0, gehoben = 0, unlesbar = 0, fremd = 0;
    int irons_da[10] = { 0 }, leon_da[4] = { 0 };
    static int rms_unber[2048]; int n_unber = 0; int max_spitze = 0; char max_name[96] = "";
    while (fgets(zeile, sizeof zeile, lf)) {
        size_t L = strlen(zeile);
        while (L && (zeile[L - 1] == '\n' || zeile[L - 1] == '\r' || zeile[L - 1] == ' ')) zeile[--L] = 0;
        if (!L) continue;
        dateien++;
        long n = 0; int rate = 0, bits = 0, ch = 0;
        int16_t *pcm = wav_mono16(zeile, &n, &rate, &bits, &ch);
        if (!pcm || n <= 0) { unlesbar++; printf("  (nicht lesbar: %s)\n", zeile); free(pcm); continue; }
        /* Kurzname "roomXXXX/mainNN.wav" */
        const char *kurz = zeile;
        { const char *q = strrchr(zeile, '/'); if (q) { const char *r = q; while (r > zeile && *(r - 1) != '/') r--; kurz = r; } }
        int g1 = stufe1(pcm, n);
        int16_t *kopie = (int16_t *)malloc((size_t)n * sizeof(int16_t)); memcpy(kopie, pcm, (size_t)n * sizeof(int16_t));
        re15_stimmen_pegel_t inf; int g2 = re15_stimmen_pegel_r39(kopie, n, NULL, &inf);
        const char *nr = strstr(kurz, "/main"); nr = nr ? nr + 5 : "";
        int ist1150 = strstr(kurz, "room1150/") != NULL;
        if (inf.rms_vorher >= RE15_STIMMEN_PEGEL_T) {
            unber++;
            CHECKQ(g2 == 256 && inf.faktor_q8 == 256 && !memcmp(kopie, pcm, (size_t)n * sizeof(int16_t)),
                   "%s: rms %d >= T=%d -> muss bitgleich bleiben (faktor %d)", kurz, inf.rms_vorher, RE15_STIMMEN_PEGEL_T, g2);
            if (n_unber < 2048) rms_unber[n_unber++] = inf.rms_vorher;
            if (inf.spitze_vorher > max_spitze) { max_spitze = inf.spitze_vorher; snprintf(max_name, sizeof max_name, "%s", kurz); }
            if (ist1150) for (int k = 0; k < 4; k++) if (!strncmp(nr, k_leon1150[k], 2) && nr[2] == '.') leon_da[k] = 1;
        } else {
            gehoben++;
            CHECKQ(g2 > 256 && inf.faktor_q8 == g2, "%s: rms %d < T -> muss angehoben werden (faktor %d)", kurz, inf.rms_vorher, g2);
            CHECKQ(inf.spitze_nachher <= RE15_STIMMEN_PEGEL_C, "%s: Spitze nach Stufe 2 %d > C=%d (Clipping-Gefahr)", kurz, inf.spitze_nachher, RE15_STIMMEN_PEGEL_C);
            CHECKQ(inf.begrenzt ? inf.spitze_nachher >= RE15_STIMMEN_PEGEL_C * 99 / 100 : inf.rms_nachher >= RE15_STIMMEN_PEGEL_R * 98 / 100,
                   "%s: weder R=%d erreicht (rms %d) noch an C begrenzt (spitze %d)", kurz, RE15_STIMMEN_PEGEL_R, inf.rms_nachher, inf.spitze_nachher);
            CHECKQ(g1 == 1024, "%s: Stufe 2 greift, obwohl Stufe 1 nicht am 4x-Deckel war (g1 %d Q8)", kurz, g1);
            re15_stimmen_pegel_t inf2; int g3 = re15_stimmen_pegel_r39(kopie, n, NULL, &inf2);
            CHECKQ(g3 == 256, "%s: zweite Anwendung aendert noch (faktor %d) - Stufe 2 muss ein Boden sein", kurz, g3);
            int gesamt = (int)(((int64_t)g1 * g2) >> 8);
            printf("  gehoben %-24s %5d Hz %2d bit %dch  Stufe1 x%d.%02d  Stufe2 x%d.%02d%s  gesamt x%d.%02d  rms %d -> %d (%.4f FS)  spitze %d -> %d\n",
                   kurz, rate, bits, ch, g1 / 256, (g1 % 256) * 100 / 256, g2 / 256, (g2 % 256) * 100 / 256,
                   inf.begrenzt ? " (C)" : "", gesamt / 256, (gesamt % 256) * 100 / 256,
                   inf.rms_vorher, inf.rms_nachher, inf.rms_nachher / 32768.0, inf.spitze_vorher, inf.spitze_nachher);
            int bekannt = 0;
            if (ist1150) for (int k = 0; k < 10; k++) if (!strncmp(nr, k_irons[k], 2) && nr[2] == '.') { irons_da[k] = 1; bekannt = 1; }
            if (!bekannt) { fremd++; printf("  (angehobene Datei ausserhalb der Irons-Klasse: %s)\n", kurz); }
            CHECKQ(gesamt >= 6 * 256 + 128, "%s: Gesamtfaktor x%d.%02d < 6,5 (Zensus: 6,9..12,6)", kurz, gesamt / 256, (gesamt % 256) * 100 / 256);
            CHECKQ(gesamt <= 13 * 256, "%s: Gesamtfaktor x%d.%02d > 13 (Zensus: hoechstens 12,6)", kurz, gesamt / 256, (gesamt % 256) * 100 / 256);
        }
        free(kopie); free(pcm);
    }
    fclose(lf);
    CHECK(dateien >= 200 && unlesbar == 0, "%d Dateien des Bestands gelesen, %d nicht lesbar", dateien, unlesbar);
    CHECK(gehoben == 10 && fremd == 0, "genau die 10 Dateien der Irons-Klasse angehoben (ist %d, davon %d fremde)", gehoben, fremd);
    { int alle = 1; for (int k = 0; k < 10; k++) alle &= irons_da[k];
      CHECK(alle, "room1150 main02/05/07/09/12/13/18/19/20/21 alle angehoben (%d%d%d%d%d%d%d%d%d%d)", irons_da[0], irons_da[1],
            irons_da[2], irons_da[3], irons_da[4], irons_da[5], irons_da[6], irons_da[7], irons_da[8], irons_da[9]); }
    { int alle = 1; for (int k = 0; k < 4; k++) alle &= leon_da[k];
      CHECK(alle, "Leon ROOM1150 main15/16/17/22 bitgleich (unberuehrt: %d%d%d%d)", leon_da[0], leon_da[1], leon_da[2], leon_da[3]); }
    CHECK(unber == dateien - gehoben, "%d Dateien bitgleich (Stufe 1 wie bisher)", unber);
    if (n_unber) {
        qsort(rms_unber, (size_t)n_unber, sizeof(int), cmp_int);
        printf("  Herleitung am Bestand: %d Dateien auf Pegel, rms min %d (T=%d) median %d (R=%d), groesste Spitze %d = %s (C=%d)\n",
               n_unber, rms_unber[0], RE15_STIMMEN_PEGEL_T, rms_unber[n_unber / 2], RE15_STIMMEN_PEGEL_R, max_spitze, max_name, RE15_STIMMEN_PEGEL_C);
        if (max_spitze > RE15_STIMMEN_PEGEL_C)
            printf("  HINWEIS: eine gelieferte Datei (%s) ist in der Spitze lauter als C - C neu aus dem Bestand ableiten\n", max_name);
    }
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "tabelle";
    if (!strcmp(teil, "tabelle")) teil_tabelle();
    else if (!strcmp(teil, "pegel")) teil_pegel();
    else { printf("Aufruf: test_r39_stimmen tabelle|pegel\n"); return 2; }
    printf("== test_r39_stimmen %s: %d Pruefungen, %d Fehler ==\n", teil, checks, fails);
    return fails ? 1 : 0;
}
