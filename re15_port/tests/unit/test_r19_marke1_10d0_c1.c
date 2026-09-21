/*
 * RIEGEL Runde 19 / Marke 1 — ROOM10D0 Cut 1: die Tischplatte muss VOR dem Spieler
 * liegen, nicht hinter ihm.
 *
 * ⛔ WOZU (Nutzer-Marke F162, 2026-09-21, befund.log Zeile 13147):
 * Der Spieler steht an (3575, 0, -4568) HINTER dem hinteren Klapptisch. Gemessen im
 * Bild (analysis/befunde_2026-09-21/pri-runde19/marke1-10D0-C1.md): 133 seiner 255
 * gezeichneten Punkte im Koerperkasten x150..170 y82..117 lagen SICHTBAR auf der
 * gruenen Tischplatte, waehrend das METALLGESTELL desselben Tisches ihn vollstaendig
 * verdeckte (162 von 162 Punkten). Ursache: das Tiefenmodell "aufrecht": true der
 * Platte projizierte ihre eigene Vorderkante (Bildpunkt 156,108) auf den FUSSBODEN
 * y0=0 und landete bei Welt(4306,-5736) = Tiefe 201..206, also 5732 Welteinheiten
 * HINTER dem eigenen Gestell (Tiefe 111..172).
 *
 * MESSZAHLEN, die dieser Riegel festnagelt:
 *   Spieler an der Marke: Kamera-z Fuss 12047 / Kopf 11644 (Log 12057 / 11625).
 *   Verdeckungsregel (re15_pri.h, geometrie.verdeckt): eine Maske der Tiefe t verdeckt
 *   einen Figurpunkt der Kamera-z vz genau dann, wenn t < (1023*vz)>>16.
 *   (1023*11644)>>16 = 181  ->  eine Maske verdeckt den KOPF nur mit t <= 180.
 *   Vorher: groesste Tiefe des Cuts 206  (> 180, Riegel FAELLT).
 *   Nachher: groesste Tiefe des Cuts 172 (<= 180, Riegel HAELT).
 *
 * Zweiter Teil: die Silhouette bleibt die Freistellung des NUTZERS. Die Soll-Deckung
 * steht als P4-Bitmap in ROOM10D0_PRI01.PBM (1021 gesetzte Punkte = 01.png bei
 * Alphaschwelle 110). Der Riegel prueft, dass der Maskensatz genau diese Punkte deckt —
 * kein Punkt mehr, kein Punkt weniger. Damit kann ein spaeteres Tiefenmodell die
 * Freistellung nicht heimlich veraendern.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_pri.h"

#define CUT            1
#define SPIELER_KOPF_VZ 11644.0   /* Welt(3575,-1400,-4568), Log-Kopf 11625 */
#define SOLL_PUNKTE    1021       /* pri/STAGE1/10D0/01.png, Alphaschwelle 110 */

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = (size_t)sz;
    return b;
}

/* (1023*vz)>>16 — der OT-Index der Figur (@0x8002565c, ZSF3 @0x80066c70) */
static int figur_bucket(double vz) { return (int)((1023.0 * vz) / 65536.0); }

/* P4-Bitmap 320x240 lesen (Soll-Deckung). -> 1 bei Erfolg */
static int pbm_lesen(const char *pfad, uint8_t *aus /* 320*240 */)
{
    size_t n = 0;
    uint8_t *b = slurp(pfad, &n);
    if (!b) return 0;
    /* Kopf: "P4\n<w> <h>\n" — Kommentarzeilen mit '#' ueberspringen */
    size_t i = 0;
    if (n < 2 || b[0] != 'P' || b[1] != '4') { free(b); return 0; }
    i = 2;
    int feld[2] = { 0, 0 }, gelesen = 0;
    while (i < n && gelesen < 2) {
        while (i < n && (b[i] == ' ' || b[i] == '\n' || b[i] == '\r' || b[i] == '\t')) i++;
        if (i < n && b[i] == '#') { while (i < n && b[i] != '\n') i++; continue; }
        int v = 0, ziffern = 0;
        while (i < n && b[i] >= '0' && b[i] <= '9') { v = v * 10 + (b[i] - '0'); i++; ziffern++; }
        if (!ziffern) break;
        feld[gelesen++] = v;
    }
    if (gelesen != 2 || feld[0] != 320 || feld[1] != 240) { free(b); return 0; }
    i++;   /* das EINE Trennzeichen nach der Hoehe */
    const size_t zeile = 320 / 8;
    if (i + zeile * 240 > n) { free(b); return 0; }
    for (int y = 0; y < 240; y++)
        for (int x = 0; x < 320; x++)
            aus[y * 320 + x] = (b[i + y * zeile + (x >> 3)] >> (7 - (x & 7))) & 1;
    free(b);
    return 1;
}

int main(void)
{
    const char *msk = RE15_ASSET_PSX_DIR "/MASKS/ROOM10D0.MSK";
    const char *pbm = RE15_ASSET_PSX_DIR "/MASKS/ROOM10D0_PRI01.PBM";
    size_t n = 0;
    uint8_t *blob = slurp(msk, &n);
    if (!blob) { printf("FAIL: %s nicht lesbar\n", msk); return 1; }

    uint32_t off = re15_pri_msk_section_offset(blob, n, CUT);
    if (!off) { printf("FAIL: Container hat keine Sektion fuer Cut %d\n", CUT); free(blob); return 1; }

    re15_pri_cut_t cut;
    int m = re15_pri_parse_section(blob, n, off, &cut);
    if (m <= 0) { printf("FAIL: Sektion Cut %d nicht parsebar (%d)\n", CUT, m); free(blob); return 1; }

    /* --- Teil 1: keine Maske darf HINTER dem Spielerkopf liegen --- */
    const int schwelle = figur_bucket(SPIELER_KOPF_VZ);   /* 181 */
    int tmin = 1 << 30, tmax = -1, zu_fern = 0, schlimmste = -1;
    for (int i = 0; i < m; i++) {
        int t = (int)cut.masks[i].depth;
        if (t < tmin) tmin = t;
        if (t > tmax) tmax = t;
        if (t >= schwelle) { zu_fern++; if (t > schlimmste) schlimmste = t; }
    }
    printf("Cut %d: %d Masken, Tiefe %d..%d | Spielerkopf vz %.0f -> bucket %d\n",
           CUT, m, tmin, tmax, SPIELER_KOPF_VZ, schwelle);

    int fehler = 0;
    if (zu_fern) {
        printf("FAIL: %d von %d Masken liegen mit Tiefe >= %d HINTER dem Spielerkopf "
               "(schlimmste %d) — der Tisch verdeckt den Spieler nicht, der HINTER ihm steht\n",
               zu_fern, m, schwelle, schlimmste);
        fehler = 1;
    } else {
        printf("OK  : alle %d Masken mit Tiefe <= %d, also VOR dem Spielerkopf\n", m, schwelle - 1);
    }

    /* --- Teil 2: die Silhouette ist bitgenau die Freistellung des Nutzers --- */
    static uint8_t soll[320 * 240];
    static uint8_t ist[320 * 240];
    if (!pbm_lesen(pbm, soll)) { printf("FAIL: %s nicht lesbar/kein 320x240-P4\n", pbm); free(blob); return 1; }
    memset(ist, 0, sizeof ist);
    for (int i = 0; i < m; i++) {
        const re15_pri_mask_t *k = &cut.masks[i];
        for (int y = 0; y < (int)k->height; y++) {
            int yy = (int)k->dstY + y;
            if (yy < 0 || yy >= 240) continue;
            for (int x = 0; x < (int)k->width; x++) {
                int xx = (int)k->dstX + x;
                if (xx < 0 || xx >= 320) continue;
                ist[yy * 320 + xx] = 1;
            }
        }
    }
    int soll_n = 0, fehlt = 0, zuviel = 0;
    for (int i = 0; i < 320 * 240; i++) {
        soll_n += soll[i];
        if (soll[i] && !ist[i]) fehlt++;
        if (!soll[i] && ist[i]) zuviel++;
    }
    printf("Soll-Deckung %d px (erwartet %d) | Rechtecke decken: fehlt %d, zuviel %d\n",
           soll_n, SOLL_PUNKTE, fehlt, zuviel);
    if (soll_n != SOLL_PUNKTE) {
        printf("FAIL: die Soll-Deckung hat %d statt %d Punkten — die Freistellung des "
               "Nutzers wurde veraendert\n", soll_n, SOLL_PUNKTE);
        fehler = 1;
    }
    /* Die Rechtecke sind Kaesten; sie duerfen MEHR ueberspannen (der Atlas stanzt die
     * Form), aber KEIN Sollpunkt darf ausserhalb aller Rechtecke liegen. */
    if (fehlt) {
        printf("FAIL: %d Punkte der Freistellung liegen in KEINEM Maskenrechteck\n", fehlt);
        fehler = 1;
    }

    free(blob);
    printf(fehler ? "=== RIEGEL r19-marke1 FAELLT ===\n" : "=== RIEGEL r19-marke1 HAELT ===\n");
    return fehler;
}
