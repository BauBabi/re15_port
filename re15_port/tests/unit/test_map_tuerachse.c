/* Karte: die Achse eines Tuersymbols stimmt mit dem GEMALTEN Original ueberein.
 *
 * ⛔ NUTZER-BEFUND 2026-09-04 (fehler/error1.png): ein Pfeil auf das Tuersymbol zwischen
 * ROOM1130 und ROOM1150 mit dem Text "Wrong Rotation, Need 90 Degree turn". Es wurde
 * WAAGERECHT gezeichnet, obwohl beide Tuer-Datensaetze eine senkrechte Wand nennen.
 *
 * Der Zeichner (re15_inv_screen.c:1921) liest aus kind NUR die ACHSE:
 *     kind 0/2 (Nord/Sued) -> waagerechter 5x2-Balken
 *     kind 1/3 (Ost/West)  -> senkrechter  2x5-Balken
 * Das Vorzeichen ist auf dem Schirm nicht unterscheidbar und wird hier nicht geprueft.
 *
 * ⛔ DIESER TEST HAT SEIN MASS GEWECHSELT - UND ZWAR WEIL DAS ERSTE FALSCH WAR.
 * Die erste Fassung (2026-09-04, frueh) verglich die Achse mit der LAGE DER BEIDEN
 * RECHTECKE (schmale x-Ueberdeckung => senkrechte Wand). Das sah gut aus: 26 von 26.
 * Es war aber eine SELBSTBESTAETIGENDE Metrik - der Generator gewann die Achse zu dem
 * Zeitpunkt aus genau derselben Ueberdeckung. Gemessen gegen eine UNABHAENGIGE Wahrheit
 * kippte das Urteil:
 *
 *   Variante                          Original-Symbole   meine Ueberdeckungs-Schranke
 *   vor allen Fixes                     18 richtig / 8         20 / 6
 *   Achse aus der Ueberdeckung          20 richtig / 7         26 / 0   <- schmeichelt sich
 *   Achse aus dem Tuer-Rechteck         23 richtig / 4         20 / 6   <- ist besser
 *
 * Gemessen wird deshalb gegen die GEMALTEN Tuersymbole des Originals:
 * analysis/kartensymbole/symbolkatalog.csv, Spalte `ausrichtung` (senkrecht/waagerecht),
 * gewonnen aus den Pixeln von DATA/MAP0x.PIX. Das ist eine Quelle, die von unserer
 * Rechteck-Zuordnung, unserer Paarung und unserer Projektion voellig unabhaengig ist.
 * (Memory reai-v2-selbstbestaetigende-metrik.)
 *
 * ZUORDNUNG: je Katalog-Symbol die naechstgelegene Tuermarke auf DEMSELBEN Blatt mit
 * Manhattan-Abstand <= 8 px. Symbole ohne Marke in dieser Naehe werden nicht bewertet -
 * der Test sagt aus, wie viele er bewerten konnte.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

#define TOLERANZ 8

/* Spaltenindex einer Kopfzeile finden. */
static int spalte(const char *kopf, const char *name)
{
    int i = 0; const char *p = kopf;
    for (;;) {
        const char *k = strchr(p, ',');
        size_t n = k ? (size_t)(k - p) : strlen(p);
        if (n == strlen(name) && strncmp(p, name, n) == 0) return i;
        if (!k) return -1;
        p = k + 1; i++;
    }
}

static const char *feld(const char *zeile, int idx, char *puf, size_t n)
{
    const char *p = zeile; int i;
    for (i = 0; i < idx; i++) { p = strchr(p, ','); if (!p) return 0; p++; }
    {
        const char *k = strchr(p, ',');
        size_t len = k ? (size_t)(k - p) : strlen(p);
        while (len && (p[len-1] == '\n' || p[len-1] == '\r')) len--;
        if (len >= n) len = n - 1;
        memcpy(puf, p, len); puf[len] = 0;
    }
    return puf;
}

int main(void)
{
    char pfad[600], zeile[4096], kopf[4096], puf[128];
    FILE *f;
    int c_seite, c_typ, c_x, c_y, c_aus;
    int k, n, bewertet = 0, passt = 0, falsch = 0, ohne_marke = 0, zeilen = 0;

    printf("=== Karte: Tuersymbol-Achse gegen die GEMALTEN Original-Symbole ===\n");

    re15_map_visited_reset();
    for (k = 0; k < 13; k++) re15_map_debug_reveal_page((unsigned)k);
    n = re15_map_mark_count();

    snprintf(pfad, sizeof pfad, "%s/../analysis/kartensymbole/symbolkatalog.csv",
             RE15_PORT_SRC_DIR);
    f = fopen(pfad, "rb");
    if (!f) { printf("  FAIL: Symbolkatalog nicht lesbar: %s\n", pfad); return 1; }
    if (!fgets(kopf, sizeof kopf, f)) { printf("  FAIL: Katalog leer\n"); fclose(f); return 1; }
    c_seite = spalte(kopf, "seite");  c_typ = spalte(kopf, "typ");
    c_x = spalte(kopf, "screen_x");   c_y   = spalte(kopf, "screen_y");
    c_aus = spalte(kopf, "ausrichtung");
    if (c_seite < 0 || c_typ < 0 || c_x < 0 || c_y < 0 || c_aus < 0) {
        printf("  FAIL: Kopfzeile ohne die erwarteten Spalten\n"); fclose(f); return 1; }

    while (fgets(zeile, sizeof zeile, f)) {
        int pg, sx, sy, senk_soll, bester = -1, bd = 0;
        zeilen++;
        if (!feld(zeile, c_typ, puf, sizeof puf) || strcmp(puf, "TUER") != 0) continue;
        if (!feld(zeile, c_aus, puf, sizeof puf)) continue;
        if      (strncmp(puf, "senk", 4) == 0) senk_soll = 1;
        else if (strncmp(puf, "waag", 4) == 0) senk_soll = 0;
        else continue;                                  /* '-' = keine Aussage */
        feld(zeile, c_seite, puf, sizeof puf); pg = atoi(puf);
        feld(zeile, c_x, puf, sizeof puf);     sx = atoi(puf);
        feld(zeile, c_y, puf, sizeof puf);     sy = atoi(puf);

        for (k = 0; k < n; k++) {
            int mp, mr, mx, my, kind, d;
            if (!re15_map_mark_get(k, &mp, &mr, &mx, &my, &kind)) continue;
            if (mp != pg || kind > 3) continue;
            d = abs(mx - sx) + abs(my - sy);
            if (d <= TOLERANZ && (bester < 0 || d < bd)) { bester = k; bd = d; }
        }
        if (bester < 0) { ohne_marke++; continue; }
        {
            int mp, mr, mx, my, kind;
            re15_map_mark_get(bester, &mp, &mr, &mx, &my, &kind);
            bewertet++;
            if ((kind == 1 || kind == 3) == (senk_soll != 0)) passt++;
            else {
                falsch++;
                printf("  [Abweichung] Blatt %d Original-Symbol (%d,%d) ist %s, "
                       "unsere Marke (%d,%d) kind %d ist %s (Abstand %d px)\n",
                       pg, sx, sy, senk_soll ? "senkrecht" : "waagerecht",
                       mx, my, kind, (kind == 1 || kind == 3) ? "senkrecht" : "waagerecht",
                       bd);
            }
        }
    }
    fclose(f);

    printf("  [Abdeckung] %d Katalogzeilen, %d Tuersymbole bewertet "
           "(%d richtig, %d falsch), %d ohne Marke im Umkreis von %d px\n",
           zeilen, bewertet, passt, falsch, ohne_marke, TOLERANZ);

    /* ⛔ DIE MENGE DARF NICHT LEER LAUFEN (Memory reai-v2-schiene-abdeckung). */
    CHECK("die Pruefmenge ist nicht leer (>= 20 bewertete Symbole)", bewertet >= 20);
    /* ⛔ DIE SCHRANKE IST DER GEMESSENE STAND, NICHT EINE GEFUEHLTE ZAHL.
     * Endstand 2026-09-04: 23 richtig, 4 falsch. Die vier Reste sind Symbole, deren
     * Marke aus einem Tuer-Datensatz mit QUADRATISCHEM Trigger-Rechteck stammt (dort
     * sagt die Streckung nichts) oder deren Zuordnung ueber ein geteiltes Rechteck
     * laeuft. Sie stehen oben namentlich. Wer die Regel aendert, muss diese Zahlen
     * verbessern - schlechter werden darf sie nicht. */
    CHECK("mindestens 23 Achsen stimmen mit dem gemalten Original ueberein", passt >= 23);
    CHECK("hoechstens 4 Achsen weichen ab", falsch <= 4);

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
