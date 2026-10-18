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
 *                 1171 3 -> main02; 1150 msg 29 hat keine Datei (Regel = main29, nicht vorhanden),
 *             (4) Sperre: ROOM11B0 msg 13 (sub03 @0x0126E/@0x01298 `2b 0d ff ff`) und 14 (main00 @0x0106C
 *                 `2c 07 01 31 ..`), ROOM1171 msg 2 bleiben stumm (-1), weil main13/14/02.wav anderen Zeilen gehoeren,
 *             (5) unberuehrt: die Ordnerregel fuer alle anderen (Raum, Nachricht), insbesondere ROOM1170 msg 3
 *                 (Leons Helipad) und ROOM11B0 msg 1..12 (alte Aufnahmen), Tabelle ohne Doppel.
 */
#include "re15_stimmen_r39.h"
#include "re15_adaruf.h"
#include "re15_irons_tod.h"
#include "re15_szenenleon_r36.h"

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

static int fails = 0, checks = 0;
#define CHECK(cond, ...) do { checks++; if (cond) { printf("  ok   "); } else { fails++; printf("  FAIL "); } \
                              printf(__VA_ARGS__); printf("\n"); } while (0)

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
    CHECK(n == 13, "Tabelle: %d Zeilen (soll 13 = 1050: 3 + 1150: 7 + 11B0: 2 + 1171: 1)", n);

    /* (3) Zuordnung je Punkt des Nutzers */
    static const struct { unsigned raum; int msg; int datei; } k_soll[] = {
        { 0x1050, 22,  9 }, { 0x1050, 23, 10 }, { 0x1050, 24, 11 },
        { 0x1150, 22, 15 }, { 0x1150, 23, 16 }, { 0x1150, 24, 17 }, { 0x1150, 25, 18 },
        { 0x1150, 26, 19 }, { 0x1150, 27, 20 }, { 0x1150, 28, 21 },
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
        { 0x1150, 29 }, { 0x1170, 3 }, { 0x1170, 0 }, { 0x1171, 0 }, { 0x1171, 1 }, { 0x1240, 0 }, { 0x1241, 3 },
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

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "tabelle";
    if (!strcmp(teil, "tabelle")) teil_tabelle();
    else { printf("Aufruf: test_r39_stimmen tabelle\n"); return 2; }
    printf("== test_r39_stimmen %s: %d Pruefungen, %d Fehler ==\n", teil, checks, fails);
    return fails ? 1 : 0;
}
