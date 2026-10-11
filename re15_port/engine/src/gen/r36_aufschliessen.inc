/* ERZEUGT von re15_port/tools/gen_r36_aufschliessen.py - NICHT von Hand aendern.
 * Runde 36 Spur G: Stellen, an denen in RE1.5 eine Tuer ERFOLGREICH aufgeschlossen wird (Nachricht
 * geht auf), und die RE2-Tonfolge dazu (Arten und Belege im Generator-Kopf und in
 * engine/src/aufschliessen_r36.c). 206 RDTs gelesen, 29 Stellen, 3 verworfen.
 * wege: Bit 0 = Text-AOT (re15_scd_show_message), Bit 1 = Skript (Message_on). */

typedef struct { uint16_t room; uint8_t msg; uint8_t art; uint8_t wege; } re15_auf_stelle_t;

#define RE15_AUF_ART_KARTE_EIN 0
#define RE15_AUF_ART_KARTE     1
#define RE15_AUF_ART_KARTE_AUF 2
#define RE15_AUF_ART_RIEGEL    3

static const re15_auf_stelle_t re15_auf_stellen[] = {
    { 0x10D0,  5, RE15_AUF_ART_KARTE_AUF, 2 },   /* msg @0x01F20 "You've opened the lock." | sub19 @0x0199E */
    { 0x10D0,  9, RE15_AUF_ART_KARTE_EIN, 2 },   /* msg @0x02052 "You've used the Blue Keycard." | sub20 @0x019C4 */
    { 0x10D1,  5, RE15_AUF_ART_KARTE_AUF, 2 },   /* msg @0x01B5C "You've opened the lock." | sub19 @0x01988 */
    { 0x10D1,  9, RE15_AUF_ART_KARTE_EIN, 2 },   /* msg @0x01C8E "You've used the Blue Keycard." | sub20 @0x019AE */
    { 0x1100,  2, RE15_AUF_ART_KARTE_AUF, 2 },   /* msg @0x00E16 "You've opened the lock." | sub02 @0x00CBA */
    { 0x1101,  2, RE15_AUF_ART_KARTE_AUF, 2 },   /* msg @0x00E16 "You've opened the lock." | sub02 @0x00CBA */
    { 0x1140,  0, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x00D94 "You've opened the lock." | sub02 @0x00C94 */
    { 0x1141, 12, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x013B6 "You've opened the lock." | sub05 @0x00FD6 */
    { 0x11E0,  5, RE15_AUF_ART_KARTE_AUF, 2 },   /* msg @0x021BE "You've opened the lock." | sub19 @0x01F86 */
    { 0x11E0,  9, RE15_AUF_ART_KARTE_EIN, 2 },   /* msg @0x022E9 "You've used the Yellow Keycard." | sub20 @0x01FAC */
    { 0x11E1,  5, RE15_AUF_ART_KARTE_AUF, 2 },   /* msg @0x021BE "You've opened the lock." | sub19 @0x01F86 */
    { 0x11E1,  9, RE15_AUF_ART_KARTE_EIN, 2 },   /* msg @0x022E9 "You've used the Yellow Keycard." | sub20 @0x01FAC */
    { 0x1230,  5, RE15_AUF_ART_KARTE_AUF, 2 },   /* msg @0x015A4 "You've opened the lock." | sub19 @0x01466 */
    { 0x1230,  9, RE15_AUF_ART_KARTE_EIN, 2 },   /* msg @0x016D0 "You've used the Red Keycard." | sub20 @0x0148C */
    { 0x1231,  5, RE15_AUF_ART_KARTE_AUF, 2 },   /* msg @0x015A4 "You've opened the lock." | sub19 @0x01466 */
    { 0x1231,  9, RE15_AUF_ART_KARTE_EIN, 2 },   /* msg @0x016D0 "You've used the Red Keycard." | sub20 @0x0148C */
    { 0x2020,  2, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x00B84 "You've opened the lock." | sub03 @0x00B2A */
    { 0x2021,  2, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x00B84 "You've opened the lock." | sub03 @0x00B2A */
    { 0x2070,  0, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x00D06 "You've opened the lock." | sub02 @0x00CE8 */
    { 0x2071,  0, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x00D06 "You've opened the lock." | sub02 @0x00CE8 */
    { 0x20A0,  2, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x0204C "You've opened the lock." | sub10 @0x01FF4 */
    { 0x20A1,  2, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x0204C "You've opened the lock." | sub10 @0x01FF4 */
    { 0x3010,  1, RE15_AUF_ART_KARTE,     2 },   /* msg @0x02350 "You've used the Green Keycard." | sub02 @0x02306 */
    { 0x3011,  1, RE15_AUF_ART_KARTE,     2 },   /* msg @0x02384 "You've used the Green Keycard." | sub02 @0x0233A */
    { 0x3040,  0, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x015D4 "You've opened the lock." | sub02 @0x014F2 */
    { 0x3041,  0, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x015D4 "You've opened the lock." | sub02 @0x014F2 */
    { 0x4000,  2, RE15_AUF_ART_KARTE,     2 },   /* msg @0x01C4D "You've used the Blue Master Keycard." | sub02 @0x0144A */
    { 0x6010,  0, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x010C6 "You've opened the lock." | sub02 @0x010A8 */
    { 0x6011,  0, RE15_AUF_ART_RIEGEL,    2 },   /* msg @0x010C6 "You've opened the lock." | sub02 @0x010A8 */
};
#define RE15_AUF_STELLEN 29

/* VERWORFEN:
 *   ROOM30C0 msg 0 (RIEGEL) "You've opened the lock.": B/C: kein Weg ohne eigenes Se_on (eigen: -)
 *   ROOM30C1 msg 0 (RIEGEL) "You've opened the lock.": B/C: kein Weg ohne eigenes Se_on (eigen: -)
 *   ROOM4001 msg 2 (KARTE) "You've used the Blue Master Keycard.": B/C: kein Weg ohne eigenes Se_on (eigen: -)
 */
