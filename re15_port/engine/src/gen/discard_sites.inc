/* ERZEUGT von tools/gen_discard_sites.py - NICHT von Hand aendern.
 *
 * Die Benutzungsstellen der Schluessel-Gegenstaende, aus den ausgelieferten Daten
 * ABGELEITET (Aufnahmebedingungen A/B/C/D/E siehe Generator-Kopf), nicht gewaehlt.
 *
 * ABDECKUNG DIESES LAUFS:
 *   206 RDTs mit Header gelesen (+34 Stummel <0x48 B), 0 Desync-Stopps
 *   164 Item_aot_set-Records insgesamt
 *   20 Nachrichten "...used the X..." in 18 Raeumen
 *   19 davon namentlich aufgeloest, 1 nicht (You've used the Latch Key.)
 *   14 Eintraege aufgenommen = 8 Gegenstaende x ihre Raumvarianten
 *   VERWORFEN ROOM2060 msg 5  0x40 Fuse                     B: keine Item_aot_set-Ausgabe
 *   VERWORFEN ROOM2061 msg 5  0x40 Fuse                     B: keine Item_aot_set-Ausgabe
 *   VERWORFEN ROOM4001 msg 2  0x47 Blue Master Keycard      D: kein Message_on 2 im SCD (nur [3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
 *   VERWORFEN ROOM11E0 msg 9  0x39 Yellow Keycard           E5: flag(3,139) wird auch ausserhalb gesetzt (ROOM1240 @0x0055A) - kein Marker dieses Panels
 *   VERWORFEN ROOM11E1 msg 9  0x39 Yellow Keycard           E5: flag(3,139) wird auch ausserhalb gesetzt (ROOM1240 @0x0055A) - kein Marker dieses Panels
 *   VERWORFEN ROOM3091 msg 32  (A: kein Name der 102er-Tabelle)  "You've used the Latch Key."
 *
 * AUFGENOMMENE GEGENSTAENDE (Id, Name, Ausgaben, Mengen je Ausgabe):
 *   0x30 Pliers                   2 Item_aot_set, Menge [1]
 *   0x31 Fire Extinguisher        1 Item_aot_set, Menge [1]
 *   0x36 Green Keycard            2 Item_aot_set, Menge [1]
 *   0x37 Red Keycard              4 Item_aot_set, Menge [1]
 *   0x38 Blue Keycard             2 Item_aot_set, Menge [1]
 *   0x44 Minidisc Player w/ Disc  2 Item_aot_set, Menge [1]
 *   0x46 Red Master Keycard       2 Item_aot_set, Menge [1]
 *   0x47 Blue Master Keycard      1 Item_aot_set, Menge [1]
 *
 * (E) ZIFFERNCODE-PANELS: 6 Raeume tragen das Muster `21 05 0d 01 ... 21 05 10 01`
 *     (Ck(5,13..16,1), die vier Ziffern-Riegel). Bei ihnen faellt die
 *     "used the"-Zeile, BEVOR das Tastenfeld aufgeht - deshalb umgehaengt:
 *     ROOM10D0 msg 9 -> msg 5  (Ausloeser @0x0199E, Erfolgs-Sub 19), Gate flag(3,50)==1 (`Set` @0x0152A)
 *     ROOM10D1 msg 9 -> msg 5  (Ausloeser @0x01988, Erfolgs-Sub 19), Gate flag(3,50)==1 (`Set` @0x01514)
 *     ROOM1230 msg 9 -> msg 5  (Ausloeser @0x01466, Erfolgs-Sub 19), Gate flag(3,137)==1 (`Set` @0x00FF2)
 *     ROOM1231 msg 9 -> msg 5  (Ausloeser @0x01466, Erfolgs-Sub 19), Gate flag(3,137)==1 (`Set` @0x00FF2)
 *     ROOM11E0 GESTRICHEN: E5: flag(3,139) wird auch ausserhalb gesetzt (ROOM1240 @0x0055A) - kein Marker dieses Panels
 *     ROOM11E1 GESTRICHEN: E5: flag(3,139) wird auch ausserhalb gesetzt (ROOM1240 @0x0055A) - kein Marker dieses Panels
 */

typedef struct { uint16_t room; uint8_t msg; uint8_t item;
                 uint8_t gate_zone; uint8_t gate_bit; } re15_discard_site_t;

static const re15_discard_site_t re15_discard_sites[] = {
    { 0x1090,  9, 0x31, 0,   0 },   /* ROOM1090: "You've used the Fire Extinguisher." */
    { 0x10D0,  5, 0x38, 3,  50 },   /* ROOM10D0: "You've opened the lock." nach flag(3,50) @0x0152A */
    { 0x10D1,  5, 0x38, 3,  50 },   /* ROOM10D1: "You've opened the lock." nach flag(3,50) @0x01514 */
    { 0x1100,  4, 0x44, 0,   0 },   /* ROOM1100: "You've used the Minidisc Player w/ Disc." */
    { 0x1101,  4, 0x44, 0,   0 },   /* ROOM1101: "You've used the Minidisc Player w/ Disc." */
    { 0x11E0, 12, 0x30, 0,   0 },   /* ROOM11E0: "You've used the Pliers." */
    { 0x11E1, 12, 0x30, 0,   0 },   /* ROOM11E1: "You've used the Pliers." */
    { 0x1230,  5, 0x37, 3, 137 },   /* ROOM1230: "You've opened the lock." nach flag(3,137) @0x00FF2 */
    { 0x1231,  5, 0x37, 3, 137 },   /* ROOM1231: "You've opened the lock." nach flag(3,137) @0x00FF2 */
    { 0x3010,  1, 0x36, 0,   0 },   /* ROOM3010: "You've used the Green Keycard." */
    { 0x3011,  1, 0x36, 0,   0 },   /* ROOM3011: "You've used the Green Keycard." */
    { 0x3050,  5, 0x46, 0,   0 },   /* ROOM3050: "You've used the Red Master Keycard." */
    { 0x3051,  5, 0x46, 0,   0 },   /* ROOM3051: "You've used the Red Master Keycard." */
    { 0x4000,  2, 0x47, 0,   0 },   /* ROOM4000: "You've used the Blue Master Keycard." */
};
#define RE15_DISCARD_SITE_COUNT 14

/* NUR FUER DEN RIEGEL (tests/unit/r21_discard_wegwerfen.c TEIL R): die gemessenen
 * Zifferncode-Panels mitsamt der VORZEITIGEN Zeile, die frueher die Abfrage
 * ausloeste, dem Leser-Sub und dem Taken-Bit, das der Leser prueft. Der Motor
 * benutzt diese Tabelle NICHT - sie erlaubt dem Riegel, die alte Fehlerstelle
 * anzufahren, ohne eine Raumnummer in den Test zu schreiben.
 * `tot` = 1: Panel vom Kollisions-Riegel E5 verworfen, KEINE Wegwerf-Stelle -
 * gate_zone/gate_bit stehen dort trotzdem (das GEMESSENE Erfolgs-Bit), damit der
 * Riegel den ausgelieferten Startzustand nachstellen kann (flag(3,139) steht ab
 * Bild 0, ROOM1240 @0x0055A). */
typedef struct { uint16_t room; uint8_t leser_sub; uint8_t vorzeitig_msg;
                 uint8_t erfolg_sub; uint8_t ausloeser_msg; uint8_t item;
                 uint8_t gate_zone; uint8_t gate_bit;
                 uint8_t taken_bit; uint8_t tot; } re15_discard_panel_t;

static const re15_discard_panel_t re15_discard_panels[] = {
    { 0x10D0, 20,  9, 19,  5, 0x38, 3,  50,  52, 0 },   /* ROOM10D0 */
    { 0x10D1, 20,  9, 19,  5, 0x38, 3,  50,  52, 0 },   /* ROOM10D1 */
    { 0x11E0, 20,  9, 19,  5, 0x39, 3, 139, 138, 1 },   /* ROOM11E0 - GESTRICHEN (E5) */
    { 0x11E1, 20,  9, 19,  5, 0x39, 3, 139, 138, 1 },   /* ROOM11E1 - GESTRICHEN (E5) */
    { 0x1230, 20,  9, 19,  5, 0x37, 3, 137, 136, 0 },   /* ROOM1230 */
    { 0x1231, 20,  9, 19,  5, 0x37, 3, 137, 136, 0 },   /* ROOM1231 */
};
#define RE15_DISCARD_PANEL_COUNT 6

/* Die ausgelieferte Stueckzahl je aufgenommenem Gegenstand (Item_aot_set Byte 16).
 * Sie ist der RE1.5-Gegenwert zu RE2s Zaehlerfeld und wird beim Gebrauch
 * heruntergezaehlt (@0x80051810); bei Null wird gefragt (@0x80051824). */
#define RE15_DISCARD_MENGE_JE_AUSGABE 1   /* gemessen: alle 8 Gegenstaende
                                           * werden mit Menge 1 platziert */
