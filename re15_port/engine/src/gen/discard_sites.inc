/* ERZEUGT von tools/gen_discard_sites.py - NICHT von Hand aendern.
 *
 * Die Benutzungsstellen der Schluessel-Gegenstaende, aus den ausgelieferten Daten
 * ABGELEITET (Aufnahmebedingungen A/B/C siehe Generator-Kopf), nicht gewaehlt.
 *
 * ABDECKUNG DIESES LAUFS:
 *   206 RDTs mit Header gelesen (+34 Stummel <0x48 B), 0 Desync-Stopps
 *   164 Item_aot_set-Records insgesamt
 *   20 Nachrichten "...used the X..." in 18 Raeumen
 *   19 davon namentlich aufgeloest, 1 nicht (You've used the Latch Key.)
 *   17 Eintraege aufgenommen = 9 Gegenstaende x ihre Raumvarianten
 *   VERWORFEN ROOM2060 msg 5  0x40 Fuse                     B: keine Item_aot_set-Ausgabe
 *   VERWORFEN ROOM2061 msg 5  0x40 Fuse                     B: keine Item_aot_set-Ausgabe
 *   VERWORFEN ROOM3091 msg 32  (A: kein Name der 102er-Tabelle)  "You've used the Latch Key."
 *
 * AUFGENOMMENE GEGENSTAENDE (Id, Name, Ausgaben, Mengen je Ausgabe):
 *   0x30 Pliers                   2 Item_aot_set, Menge [1]
 *   0x31 Fire Extinguisher        1 Item_aot_set, Menge [1]
 *   0x36 Green Keycard            2 Item_aot_set, Menge [1]
 *   0x37 Red Keycard              4 Item_aot_set, Menge [1]
 *   0x38 Blue Keycard             2 Item_aot_set, Menge [1]
 *   0x39 Yellow Keycard           1 Item_aot_set, Menge [1]
 *   0x44 Minidisc Player w/ Disc  2 Item_aot_set, Menge [1]
 *   0x46 Red Master Keycard       2 Item_aot_set, Menge [1]
 *   0x47 Blue Master Keycard      1 Item_aot_set, Menge [1]
 */

typedef struct { uint16_t room; uint8_t msg; uint8_t item; } re15_discard_site_t;

static const re15_discard_site_t re15_discard_sites[] = {
    { 0x1090,  9, 0x31 },   /* ROOM1090: "You've used the Fire Extinguisher." */
    { 0x10D0,  9, 0x38 },   /* ROOM10D0: "You've used the Blue Keycard." */
    { 0x10D1,  9, 0x38 },   /* ROOM10D1: "You've used the Blue Keycard." */
    { 0x1100,  4, 0x44 },   /* ROOM1100: "You've used the Minidisc Player w/ Disc." */
    { 0x1101,  4, 0x44 },   /* ROOM1101: "You've used the Minidisc Player w/ Disc." */
    { 0x11E0,  9, 0x39 },   /* ROOM11E0: "You've used the Yellow Keycard." */
    { 0x11E0, 12, 0x30 },   /* ROOM11E0: "You've used the Pliers." */
    { 0x11E1,  9, 0x39 },   /* ROOM11E1: "You've used the Yellow Keycard." */
    { 0x11E1, 12, 0x30 },   /* ROOM11E1: "You've used the Pliers." */
    { 0x1230,  9, 0x37 },   /* ROOM1230: "You've used the Red Keycard." */
    { 0x1231,  9, 0x37 },   /* ROOM1231: "You've used the Red Keycard." */
    { 0x3010,  1, 0x36 },   /* ROOM3010: "You've used the Green Keycard." */
    { 0x3011,  1, 0x36 },   /* ROOM3011: "You've used the Green Keycard." */
    { 0x3050,  5, 0x46 },   /* ROOM3050: "You've used the Red Master Keycard." */
    { 0x3051,  5, 0x46 },   /* ROOM3051: "You've used the Red Master Keycard." */
    { 0x4000,  2, 0x47 },   /* ROOM4000: "You've used the Blue Master Keycard." */
    { 0x4001,  2, 0x47 },   /* ROOM4001: "You've used the Blue Master Keycard." */
};
#define RE15_DISCARD_SITE_COUNT 17

/* Die ausgelieferte Stueckzahl je aufgenommenem Gegenstand (Item_aot_set Byte 16).
 * Sie ist der RE1.5-Gegenwert zu RE2s Zaehlerfeld und wird beim Gebrauch
 * heruntergezaehlt (@0x80051810); bei Null wird gefragt (@0x80051824). */
#define RE15_DISCARD_MENGE_JE_AUSGABE 1   /* gemessen: alle 9 Gegenstaende
                                           * werden mit Menge 1 platziert */
