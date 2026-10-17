/*
 * RE1.5 Rebuilt — Runde 37 Spur Q "kistenregel": Benutzung nur mit dem Gegenstand im INVENTAR (RE2-Regel), wo die
 * RE2-Item-Box des Ports eine Original-Bit-Abfrage oeffnet. Herleitung und alle Belege: include/re15_kistenregel_r37.h
 * und analysis/befunde_runde37/Q_kistenregel.md (Zensus Abschnitt 3, Werkzeug tools/r37_kistenregel/
 * kistenregel_zensus.py). Haken: scd_vm.c op_ck (eine Zeile, "Runde 37 Spur Q").
 */
#include "re15_kistenregel_r37.h"

#include <stddef.h>

#include "re15_inventory.h"   /* re15_inv_find_item = RE1.5 FUN_8004dfec / RE2 FUN_800696CC (nur das Inventar) */
#include "re15_itembox.h"     /* g_itembox (nur fuer die Messzeile: liegt er in der Kiste?)                   */
#include "re15_scd.h"         /* re15_game_flag_get                                                         */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* DIE TABELLE (Dossier 3.1). Je Zeile: Raum, Datei-Offset des Ck-Satzes und seine Original-Bytes `21 bank bit wert`,
 * der Gegenstand, das "fertig"-Bit (gesetzt von der Benutzung selbst), Fall-Nummer. Die Offsets und Bytes prueft der
 * Riegel unit_r37_kistenregel_tabelle an den ausgelieferten RDTs; Elza-Raeume (ungerade) tragen dieselben Offsets.
 *
 *  K1  ROOM1090 sub00 @0x0230A Ck(3,0x85)==0 — Fire Extinguisher. Bit = ROOM1000 sub01 @0x00D0C (Port: Aufnahme in
 *      1030 seit Runde 38 Spur K, bis v0.8.25 1050; feuerloescher_r37.c). fertig (3,0x81) = sub06 @0x0271E. Ohne: Slot 2 sce 1 msg 7 @0x0230E.
 *  K2  ROOM10D0/10D1 sub20 @0x019C0/@0x019AA Ck(9,0x34)==1 — Blue Keycard (Taken-Bit 1110/1111 @0x00B2E).
 *      fertig (3,0x32) @0x0152A/@0x01514. Ohne: ELSE msg 8 "You have not the Blue Keycard / to operate it."
 *  K3  ROOM1230/1231 sub20 @0x01488 Ck(9,0x88)==1 — Red Keycard (1190/1191 sub13). fertig (3,0x89). Ohne: msg 8.
 *  K4  ROOM11E0/11E1 sub20 @0x01FA8 Ck(9,0x8A)==1 — Yellow Keycard (1011). fertig (3,0x8B) @0x01A8A. Ohne: msg 8.
 *      (Leon: ROOM1240 @0x0055A setzt (3,0x8B) beim neuen Spiel -> Leser tot; Elza: lebt.)
 *  K5  ROOM11E0/11E1 sub00 @0x01798 Ck(3,0x7F)==0 — Pliers (11B0 sub01 @0x011EA / 11B1 @0x01186). fertig (4,0x24)
 *      sub21 @0x01FC6. Ohne: Slot 6 sce 1 msg 10 "If I had some kind of pliers ..." @0x0179C.
 *  K6  ROOM1100/1101 sub00 @0x00AC8 Ck(3,0x67)==0 — Minidisc Player w/ Disc (1200/1201 sub01 @0x0095A). fertig
 *      (4,0xE8) sub02 @0x00C68. Ohne: Slot 2 sce 1 msg 1 @0x00AD6.
 *  K7  ROOM3010/3011 main00 @0x020AE Ck(3,0x3B)==0 — Green Keycard (3040/3041 sub01 @0x014DA). fertig (3,0x3C)
 *      sub02 @0x02302. Ohne: Slot 6 sce 1 msg 0 "It's locked.An ID card is required to open it." @0x020B2.
 *  K8  ROOM3050/3051 main00 @0x01F90 Ck(3,0xB2)==0 — Red Master Keycard (30A0/30A1 sub01 @0x0086C). fertig (3,0xB3)
 *      sub15 @0x0260E. Ohne: Slot 10 sce 1 msg 3 @0x01F94.
 *  K9  ROOM4000 main00 @0x01128 Ck(3,0x65)==0 + sub01 @0x01374 Ck(3,0x65)==1 — Blue Master Keycard (4010 sub01
 *      @0x006EC). fertig (3,0x20) sub02 @0x01450. Ohne: Slot 6 sce 1 msg 0 @0x0112C + Slot 2 weg @0x01140.
 *  K10 ROOM4030/4031 main00 @0x004FA Ck(3,0x57)==0 — Spark Plug (4040/4041 sub01 @0x01020). fertig (4,0x19) sub06
 *      @0x006D2. Ohne: Slot 6 sce 1 msg 0 @0x004FE.
 *  K11 ROOM1211 sub00 @0x01DEA Ck(4,0xEC)==0 — Prison Key 0x42 (Port-Aufnahme 10F1 setzt (4,0xEC),
 *      schluessel_r36.c; Original: Skript-Bit ROOM1011 sub06 @0x00E2C ohne Gegenstand). fertig (4,0xFC) sub02
 *      @0x01F76. Ohne: Slot 8 sce 1 msg 10 "It's locked.It seems that there is someone / inside..." @0x01DEE. */
static const re15_kr37_fall_t k_faelle[] = {
    /* raum    ck_off  bank  bit  wert  item  fertig      K */
    { 0x1090, 0x0230A, 3, 0x85, 0, 0x31, 3, 0x81,  1 },
    { 0x10D0, 0x019C0, 9, 0x34, 1, 0x38, 3, 0x32,  2 },
    { 0x10D1, 0x019AA, 9, 0x34, 1, 0x38, 3, 0x32,  2 },
    { 0x1230, 0x01488, 9, 0x88, 1, 0x37, 3, 0x89,  3 },
    { 0x1231, 0x01488, 9, 0x88, 1, 0x37, 3, 0x89,  3 },
    { 0x11E0, 0x01FA8, 9, 0x8A, 1, 0x39, 3, 0x8B,  4 },
    { 0x11E1, 0x01FA8, 9, 0x8A, 1, 0x39, 3, 0x8B,  4 },
    { 0x11E0, 0x01798, 3, 0x7F, 0, 0x30, 4, 0x24,  5 },
    { 0x11E1, 0x01798, 3, 0x7F, 0, 0x30, 4, 0x24,  5 },
    { 0x1100, 0x00AC8, 3, 0x67, 0, 0x44, 4, 0xE8,  6 },
    { 0x1101, 0x00AC8, 3, 0x67, 0, 0x44, 4, 0xE8,  6 },
    { 0x3010, 0x020AE, 3, 0x3B, 0, 0x36, 3, 0x3C,  7 },
    { 0x3011, 0x020AE, 3, 0x3B, 0, 0x36, 3, 0x3C,  7 },
    { 0x3050, 0x01F90, 3, 0xB2, 0, 0x46, 3, 0xB3,  8 },
    { 0x3051, 0x01F90, 3, 0xB2, 0, 0x46, 3, 0xB3,  8 },
    { 0x4000, 0x01128, 3, 0x65, 0, 0x47, 3, 0x20,  9 },
    { 0x4000, 0x01374, 3, 0x65, 1, 0x47, 3, 0x20,  9 },
    { 0x4030, 0x004FA, 3, 0x57, 0, 0x41, 4, 0x19, 10 },
    { 0x4031, 0x004FA, 3, 0x57, 0, 0x41, 4, 0x19, 10 },
    { 0x1211, 0x01DEA, 4, 0xEC, 0, 0x42, 4, 0xFC, 11 },
};
#define N_FAELLE ((int)(sizeof k_faelle / sizeof k_faelle[0]))

int g_re15_kr37_gesperrt_zaehler = 0;

int re15_kistenregel_r37_anzahl(void) { return N_FAELLE; }

const re15_kr37_fall_t *re15_kistenregel_r37_zeile(int i)
{
    return (i >= 0 && i < N_FAELLE) ? &k_faelle[i] : NULL;
}

#ifdef RE15_PLATFORM_PC
/* Messzeile je Fall nur bei einem WECHSEL der Entscheidung (sub01-Stellen wie K9 @0x01374 laufen je Bild). */
static signed char s_letzt[N_FAELLE];
static uint16_t    s_letzt_raum = 0xFFFFu;

static int kisten_platz(uint8_t id)
{
    for (int i = 0; i < RE15_BOX_SLOTS; i++)
        if (g_itembox.slots[i].id == id) return i;
    return -1;
}
#endif

int re15_kistenregel_r37_ck(uint16_t raum, uint32_t off, uint8_t bank, uint8_t bit, int got)
{
    for (int k = 0; k < N_FAELLE; k++) {
        const re15_kr37_fall_t *f = &k_faelle[k];
        if (f->raum != raum || f->ck_off != off || f->bank != bank || f->bit != bit) continue;
        /* (a) schon benutzt -> das Original-Bit gilt (RE2 ROOM6150 sub02 @0x02D3C `21 04 9f 01` VOR `5e 3f`);
         * Bit nicht gesetzt -> nichts zu verbieten. */
        if (!got || re15_game_flag_get(f->fertig_bank, f->fertig_bit)) return got;
        /* (b) im INVENTAR? (RE2 Keep_Item_ck @0x8005850c `jal 0x800696cc` = nur 0x800d4a3c + i*4) */
        int im_inventar = re15_inv_find_item(f->item) >= 0;
#ifdef RE15_PLATFORM_PC
        if (s_letzt_raum != raum) {
            for (int j = 0; j < N_FAELLE; j++) s_letzt[j] = -1;
            s_letzt_raum = raum;
        }
        if (s_letzt[k] != (signed char)im_inventar) {
            s_letzt[k] = (signed char)im_inventar;
            if (im_inventar)
                fprintf(stderr, "[kistenregel] ROOM%04X @0x%05X Ck(%u,0x%02X) K%u: Item 0x%02X im Inventar -> wie "
                                "Original\n", (unsigned)raum, (unsigned)off, (unsigned)bank, (unsigned)bit,
                        (unsigned)f->fall, (unsigned)f->item);
            else {
                int p = kisten_platz(f->item);
                if (p >= 0)
                    fprintf(stderr, "[kistenregel] ROOM%04X @0x%05X Ck(%u,0x%02X) K%u: Item 0x%02X NICHT im Inventar "
                                    "(Item-Box Platz %d) -> Besitz 0, Zweig ohne Gegenstand\n", (unsigned)raum,
                            (unsigned)off, (unsigned)bank, (unsigned)bit, (unsigned)f->fall, (unsigned)f->item, p);
                else
                    fprintf(stderr, "[kistenregel] ROOM%04X @0x%05X Ck(%u,0x%02X) K%u: Item 0x%02X NICHT im Inventar "
                                    "(weder Inventar noch Item-Box) -> Besitz 0, Zweig ohne Gegenstand\n",
                            (unsigned)raum, (unsigned)off, (unsigned)bank, (unsigned)bit, (unsigned)f->fall,
                            (unsigned)f->item);
            }
        }
#endif
        if (im_inventar) return got;
        g_re15_kr37_gesperrt_zaehler++;
        return 0;
    }
    return got;
}
