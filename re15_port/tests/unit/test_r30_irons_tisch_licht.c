/* test_r30_irons_tisch_licht.c — Runde 30 Nachschliff, Spur tischlicht.
 *
 * Riegel auf die WAHL des Lichtsatzes fuer Irons Diary (obj 5) und Memory Card (obj 6)
 * (include/re15_irons_tisch.h "LICHTSATZ DER ZWEI PROPS", PORT-WAHL, keine Original-Adresse):
 *   A  re15_irons_tisch_licht_cut liefert RE15_IRONS_LICHT_CUT (= 2) NUR fuer obj 5/6 in
 *      ROOM1150/1151 und -1 (= Lichtsatz des aktiven Cuts, byte-true) fuer jedes andere
 *      Objekt und jeden anderen Raum.
 *   B  Der gewaehlte Satz ist ein VORHANDENER Satz: beide RDTs, Kopf @0x2C = 0x398, nCut 9,
 *      @0x003E8 genau die im Kopf zitierten 40 Byte; Lichtblock 1150 == 1151 byte-gleich;
 *      re15_light_parse liest daraus ambient (110,90,84).
 *   C  Der bisher in Cut 6 benutzte Satz @0x00488 ist wirklich der dunkle (ambient 40,40,24)
 *      - der Befund, gegen den die Wahl steht.
 * Das SICHTBARE Ergebnis haelt integration_r30_irons_tisch_licht fest.
 */
#include "re15_irons_tisch.h"
#include "re15_light.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int s_fehler = 0;
#define PRUEF(bed, ...) do { if (!(bed)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fehler++; } } while (0)

static unsigned char *lies(const char *pfad, long *n)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    *n = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *b = (unsigned char *)malloc((size_t)*n);
    if (b && fread(b, 1, (size_t)*n, f) != (size_t)*n) { free(b); b = NULL; }
    fclose(f);
    return b;
}

int main(void)
{
    /* ---- A: die Wahl gilt NUR fuer obj 5/6 in 1150/1151 ------------------------------ */
    const uint16_t raeume[] = { 0x1150, 0x1151, 0x1140, 0x1141, 0x1100, 0x1110, 0x1170, 0x1160,
                                0x2090, 0x5090, 0x0000, 0x1152 };
    int n_ok = 0;
    for (size_t r = 0; r < sizeof raeume / sizeof raeume[0]; r++) {
        for (int oid = -1; oid <= 32; oid++) {
            int erw = ((raeume[r] == 0x1150 || raeume[r] == 0x1151) &&
                       (oid == RE15_IRONS_DIARY_OBJ_ID || oid == RE15_IRONS_KARTE_OBJ_ID))
                      ? RE15_IRONS_LICHT_CUT : -1;
            int ist = re15_irons_tisch_licht_cut(raeume[r], oid);
            PRUEF(ist == erw, "A raum %04X obj %d: licht_cut %d, erwartet %d", raeume[r], oid, ist, erw);
            if (ist == erw) n_ok++;
        }
    }
    PRUEF(RE15_IRONS_LICHT_CUT == 2, "A RE15_IRONS_LICHT_CUT = %d, erwartet 2", RE15_IRONS_LICHT_CUT);
    printf("A: %d Faelle geprueft, Wahl nur fuer obj 5/6 in 1150/1151\n", n_ok);

    /* ---- B/C: der Satz ist vorhanden, byte-gleich in beiden RDTs ---------------------- */
    static const unsigned char k_satz2[40] = {
        0x01, 0x00, 0x00, 0x00, 0x91, 0x91, 0x6e, 0xff, 0x87, 0x6e, 0x80, 0x80, 0x80, 0x6e,
        0x5a, 0x54, 0xb1, 0x97, 0x30, 0xf8, 0xb3, 0xa0, 0xb8, 0x8e, 0x30, 0xf8, 0x20, 0xb8,
        0xd0, 0x07, 0xd0, 0x07, 0xd0, 0x07, 0x20, 0x4e, 0xc0, 0x5d, 0x20, 0x4e };
    unsigned char *blk[2] = { NULL, NULL };
    const char *namen[2] = { RE15_ASSET_PSX_DIR "/STAGE1/ROOM1150.RDT",
                             RE15_ASSET_PSX_DIR "/STAGE1/ROOM1151.RDT" };
    for (int i = 0; i < 2; i++) {
        long n = 0;
        unsigned char *d = lies(namen[i], &n);
        PRUEF(d != NULL, "B %s nicht lesbar", namen[i]);
        if (!d) continue;
        unsigned ls = (unsigned)d[0x2C] | ((unsigned)d[0x2D] << 8) |
                      ((unsigned)d[0x2E] << 16) | ((unsigned)d[0x2F] << 24);
        PRUEF(ls == 0x398, "B %s lightStart @0x2C = 0x%X, erwartet 0x398", namen[i], ls);
        PRUEF(d[1] == 9, "B %s nCut = %u, erwartet 9", namen[i], d[1]);
        PRUEF(n >= 0x398 + 9 * 40, "B %s zu kurz", namen[i]);
        if (ls == 0x398 && d[1] == 9 && n >= 0x398 + 9 * 40) {
            PRUEF(memcmp(d + 0x3E8, k_satz2, 40) == 0, "B %s @0x3E8 ist nicht der zitierte Satz", namen[i]);
            re15_light_set_t ls_set;
            PRUEF(re15_light_parse(d + 0x398, 9 * 40, &ls_set) == 0, "B %s re15_light_parse", namen[i]);
            const re15_light_cut_t *c2 = &ls_set.cuts[RE15_IRONS_LICHT_CUT];
            PRUEF(c2->ambient[0] == 110 && c2->ambient[1] == 90 && c2->ambient[2] == 84,
                  "B %s Satz %d ambient (%u,%u,%u), erwartet (110,90,84)", namen[i],
                  RE15_IRONS_LICHT_CUT, c2->ambient[0], c2->ambient[1], c2->ambient[2]);
            const re15_light_cut_t *c6 = &ls_set.cuts[6];
            PRUEF(c6->ambient[0] == 40 && c6->ambient[1] == 40 && c6->ambient[2] == 24,
                  "C %s Satz 6 (@0x488) ambient (%u,%u,%u), erwartet (40,40,24)", namen[i],
                  c6->ambient[0], c6->ambient[1], c6->ambient[2]);
            blk[i] = (unsigned char *)malloc(9 * 40);
            if (blk[i]) memcpy(blk[i], d + 0x398, 9 * 40);
        }
        free(d);
    }
    if (blk[0] && blk[1])
        PRUEF(memcmp(blk[0], blk[1], 9 * 40) == 0, "B Lichtblock ROOM1150 != ROOM1151");
    free(blk[0]); free(blk[1]);

    if (s_fehler) { printf("unit_r30_irons_tisch_licht: %d FEHLER\n", s_fehler); return 1; }
    printf("unit_r30_irons_tisch_licht: OK\n");
    return 0;
}
