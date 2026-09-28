/* ERZEUGT von tools/gen_lock_se_sites.py - NICHT von Hand aendern.
 *
 * Die Stellen, an denen der Spieler an einer VERSCHLOSSENEN TUER steht, aus den
 * ausgelieferten RE1.5-Daten ABGELEITET (Bedingungen A/B/C im Generator-Kopf).
 * RE2-ERGAENZUNG: RE1.5 ist hier stumm (LAB_80043084 @0x80043084 / LAB_800430bc
 * @0x800430bc), RE2 spielt Se_on(Bank 2, Satz 0x16) @0x80051610 / @0x800516a4.
 *
 * ABDECKUNG DIESES LAUFS:
 *   206 RDTs mit Inhalt gelesen (+34 Platzhalter < 0x100 B)
 *   118 Nachrichten tragen das Wort lock/latched
 *   54 Stellen aufgenommen: 17 x Art K (elektronisch/Karte), 35 x Art M (mechanisch),
 *   2 x Art S (ohne Strom)
 *   66 verworfen (Liste am Dateiende)
 *
 * wege: Bit 0 = Text-AOT (sce=1, re15_scd_show_message), Bit 1 = Skript (Message_on)
 */

typedef struct { uint16_t room; uint8_t msg; uint8_t art; uint8_t wege; } re15_lock_se_site_t;

#define RE15_LOCK_ART_K 0   /* elektronisch / Kartenleser -> RE2_DOOR_SE_ZU_E */
#define RE15_LOCK_ART_M 1   /* mechanisch                 -> RE2_DOOR_SE_ZU_A */
#define RE15_LOCK_ART_S 2   /* ohne Strom (RE2 ROOM7020)  -> RE2_DOOR_SE_ZU_P */

static const re15_lock_se_site_t re15_lock_se_sites[] = {
    { 0x10D0,  6, RE15_LOCK_ART_K, 1 },   /* msg @0x01F3B "Communication Room"It's electronically locked.There's a card reader on the right." | Zwilling Platz 0 | Aot_set Platz 0 main00 @0x0102A */
    { 0x10D1,  6, RE15_LOCK_ART_K, 1 },   /* msg @0x01B77 "Communication Room"It's electronically locked.There's a card reader on the right." | Zwilling Platz 0 | Aot_set Platz 0 main00 @0x0102A */
    { 0x10F0,  5, RE15_LOCK_ART_M, 1 },   /* msg @0x012B6 "It's a small office.It's locked." | Text | Aot_set Platz 9 main00 @0x00FFE */
    { 0x10F1,  5, RE15_LOCK_ART_M, 1 },   /* msg @0x012B6 "It's a small office.It's locked." | Text | Aot_set Platz 9 main00 @0x00FFE */
    { 0x1100,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x00DA8 "It's electronically locked." | Text | Aot_set Platz 3 sub00 @0x00AEA; Aot_set Platz 3 sub00 @0x00B28 */
    { 0x1101,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x00DA8 "It's electronically locked." | Text | Aot_set Platz 3 sub00 @0x00AEA; Aot_set Platz 3 sub00 @0x00B28 */
    { 0x1170, 12, RE15_LOCK_ART_M, 1 },   /* msg @0x01B87 "It's locked from the other side." | Zwilling Platz 5 | Aot_set Platz 5 main00 @0x01320 */
    { 0x1171,  4, RE15_LOCK_ART_M, 1 },   /* msg @0x01868 "It's locked from the other side." | Zwilling Platz 4 | Aot_set Platz 4 main00 @0x012B6 */
    { 0x1180,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x00B66 "Weapon Storage"It's electronically locked.There's a card reader on the right." | Text | Aot_set Platz 5 main00 @0x00AB4 */
    { 0x1181,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x00B66 "Weapon Storage"It's electronically locked.There's a card reader on the right." | Text | Aot_set Platz 5 main00 @0x00AB4 */
    { 0x11A0,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x0141C "It's locked." | Zwilling Platz 2 | Aot_reset Platz 2 main00 @0x00FBC */
    { 0x11A0,  2, RE15_LOCK_ART_M, 1 },   /* msg @0x0142C "It's locked from the other side." | Zwilling Platz 0,4 | Aot_set Platz 0 main00 @0x00F42; Aot_set Platz 4 main00 @0x00FEE */
    { 0x11A1,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x01430 "It's locked." | Zwilling Platz 2 | Aot_reset Platz 2 main00 @0x00FBC */
    { 0x11A1,  2, RE15_LOCK_ART_M, 1 },   /* msg @0x01440 "It's locked from the other side." | Zwilling Platz 0,4 | Aot_set Platz 0 main00 @0x00F42; Aot_set Platz 4 main00 @0x00FEE */
    { 0x11B1, 12, RE15_LOCK_ART_M, 1 },   /* msg @0x0179F "The door is locked." | Text | Aot_set Platz 8 sub00 @0x0112A */
    { 0x11E0,  6, RE15_LOCK_ART_K, 1 },   /* msg @0x021D9 "Prisons"It's electronically locked.There's a card reader on the right." | Zwilling Platz 3 | Aot_set Platz 3 main00 @0x0157A */
    { 0x11E1,  6, RE15_LOCK_ART_K, 1 },   /* msg @0x021D9 "Prisons"It's electronically locked.There's a card reader on the right." | Zwilling Platz 3 | Aot_set Platz 3 main00 @0x0157A */
    { 0x1211, 10, RE15_LOCK_ART_M, 1 },   /* msg @0x0259D "It's locked.It seems that there is someone inside..." | Text | Aot_set Platz 8 sub00 @0x01DEE */
    { 0x1230,  6, RE15_LOCK_ART_K, 1 },   /* msg @0x015BF "Weapon Storage"It's electronically locked.There's a card reader on the right." | Zwilling Platz 4 | Aot_set Platz 4 main00 @0x00B9C */
    { 0x1231,  6, RE15_LOCK_ART_K, 1 },   /* msg @0x015BF "Weapon Storage"It's electronically locked.There's a card reader on the right." | Zwilling Platz 4 | Aot_set Platz 4 main00 @0x00B9C */
    { 0x2020,  3, RE15_LOCK_ART_M, 1 },   /* msg @0x00B9F "It's locked." | Text | Aot_set Platz 5 main00 @0x008C0 */
    { 0x2021,  3, RE15_LOCK_ART_M, 1 },   /* msg @0x00B9F "It's locked." | Text | Aot_set Platz 5 main00 @0x008C0 */
    { 0x2030,  0, RE15_LOCK_ART_M, 1 },   /* msg @0x02014 "It's locked from the other side." | Zwilling Platz 2 | Aot_set Platz 2 main00 @0x01B12 */
    { 0x2031,  0, RE15_LOCK_ART_M, 1 },   /* msg @0x02014 "It's locked from the other side." | Zwilling Platz 2 | Aot_set Platz 2 main00 @0x01B12 */
    { 0x20A0,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x0203C "It's locked." | Text | Aot_set Platz 8 main00 @0x0189E */
    { 0x20A1,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x0203C "It's locked." | Text | Aot_set Platz 8 main00 @0x0189E */
    { 0x3010,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x0231C "It's locked.An ID card is required to open it." | Zwilling Platz 6 | Aot_set Platz 6 main00 @0x020B2 */
    { 0x3011,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x02350 "It's locked.An ID card is required to open it." | Zwilling Platz 6 | Aot_set Platz 6 main00 @0x020B2 */
    { 0x3020,  6, RE15_LOCK_ART_M, 1 },   /* msg @0x00F35 "It's locked." | Text | Aot_set Platz 1 main00 @0x00B5A */
    { 0x3021,  4, RE15_LOCK_ART_M, 1 },   /* msg @0x00CF3 "It's locked." | Text | Aot_set Platz 1 main00 @0x00B5A */
    { 0x3060,  0, RE15_LOCK_ART_M, 1 },   /* msg @0x01106 "It's locked." | Zwilling Platz 1 | Aot_reset Platz 1 sub00 @0x00F2C */
    { 0x3060, 10, RE15_LOCK_ART_M, 1 },   /* msg @0x012DB "It's locked from the other side." | Zwilling Platz 2 | Aot_set Platz 2 main00 @0x00EBE */
    { 0x3061,  0, RE15_LOCK_ART_M, 3 },   /* msg @0x013E6 "It's locked." | Zwilling Platz 1 | Aot_reset Platz 1 sub00 @0x01016; Message_on sub02 @0x01040 */
    { 0x3061, 10, RE15_LOCK_ART_M, 1 },   /* msg @0x015B3 "It's locked from the other side." | Zwilling Platz 2 | Aot_set Platz 2 main00 @0x00EBE */
    { 0x3090,  3, RE15_LOCK_ART_M, 1 },   /* msg @0x01579 "The door is latched shut and won't open." | Zwilling Platz 1 | Aot_reset Platz 1 sub00 @0x01442 */
    { 0x3091,  6, RE15_LOCK_ART_M, 2 },   /* msg @0x01FC6 "The door is latched shut and won't open.I'd need a key to open this..." | Text | Message_on sub02 @0x0159A */
    { 0x30C0,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x00BFF "It's locked." | Text | Aot_set Platz 2 main00 @0x00B9A */
    { 0x30C1, 10, RE15_LOCK_ART_M, 1 },   /* msg @0x010BE "It's locked." | Text | Aot_set Platz 2 main00 @0x00B9A */
    { 0x4000,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x01BA0 "The door is locked. The text on the panel reads:Door is under emergency lockdown. An ID ca" | Text | Aot_set Platz 6 main00 @0x0112C; [NICHT: Message_on sub02 @0x0143A hat eigenes RE1.5-Se_on] */
    { 0x4001, 14, RE15_LOCK_ART_M, 1 },   /* msg @0x01F8A "Security Room"It's locked." | Text | Aot_set Platz 8 main00 @0x0110C */
    { 0x4080,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x00F02 "Cell Cultivation Room"It's electronically locked." | Zwilling Platz 2 | Aot_set Platz 2 main00 @0x00AE4 */
    { 0x4081,  0, RE15_LOCK_ART_K, 1 },   /* msg @0x00F02 "Cell Cultivation Room"It's electronically locked." | Zwilling Platz 2 | Aot_set Platz 2 main00 @0x00AE4 */
    { 0x40A0,  3, RE15_LOCK_ART_K, 2 },   /* msg @0x011CA "It's securely locked. An ID card is required to open it." | Text | Message_on sub03 @0x0106E */
    { 0x40A1,  3, RE15_LOCK_ART_K, 2 },   /* msg @0x011CA "It's securely locked. An ID card is required to open it." | Text | Message_on sub03 @0x0106E */
    { 0x5070,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x00DB8 "Shelter 01"The door is tightly locked.It doesn't look like it will open unless there's an " | Zwilling Platz 1 | Aot_reset Platz 1 main00 @0x00A32 */
    { 0x5070,  2, RE15_LOCK_ART_M, 1 },   /* msg @0x00E27 "Shelter 02"The door is tightly locked.It doesn't look like it will open unless there's an " | Zwilling Platz 2 | Aot_reset Platz 2 main00 @0x00A3C */
    { 0x5071,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x00DB8 "Shelter 01"The door is tightly locked.It doesn't look like it will open unless there's an " | Zwilling Platz 1 | Aot_reset Platz 1 main00 @0x00A32 */
    { 0x5071,  2, RE15_LOCK_ART_M, 1 },   /* msg @0x00E27 "Shelter 02"The door is tightly locked.It doesn't look like it will open unless there's an " | Zwilling Platz 2 | Aot_reset Platz 2 main00 @0x00A3C */
    { 0x5080,  2, RE15_LOCK_ART_S, 1 },   /* msg @0x00996 "The door won't open until the power is restored!" | Zwilling Platz 0 | Aot_reset Platz 0 sub02 @0x007C2 */
    { 0x5081,  2, RE15_LOCK_ART_S, 1 },   /* msg @0x0098E "The door won't open until the power is restored!" | Zwilling Platz 0 | Aot_reset Platz 0 sub02 @0x007BA */
    { 0x5120,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x0172B "The door is locked because of the fire!" | Text | Aot_set Platz 4 main00 @0x0147A; Aot_set Platz 5 main00 @0x0148E */
    { 0x5121,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x0172B "The door is locked because of the fire!" | Text | Aot_set Platz 4 main00 @0x0147A; Aot_set Platz 5 main00 @0x0148E */
    { 0x6000,  6, RE15_LOCK_ART_M, 1 },   /* msg @0x0138B "It's locked from the other side." | Zwilling Platz 2 | Aot_set Platz 2 main00 @0x00DA0 */
    { 0x6001,  1, RE15_LOCK_ART_M, 1 },   /* msg @0x00FEB "It's locked from the other side." | Zwilling Platz 2 | Aot_set Platz 2 main00 @0x00DA0 */
};
#define RE15_LOCK_SE_SITE_COUNT 54

/* VERWORFEN (Nachrichten mit lock/latched im Wortlaut, die KEINE Tuer-Schloss-Stelle sind):
 *   ROOM1011 msg 15  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "Roy: My friend John should still be in the underground cell block."
 *   ROOM1030 msg 17  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "There are some words carved here... I C A N ' T E S C A P E ! ! !"The door is lo"
 *   ROOM1031 msg 17  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "There are some words carved here... I C A N ' T E S C A P E ! ! !"The door is lo"
 *   ROOM1070 msg  4  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The lockers all appear to be locked."
 *   ROOM1071 msg  4  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The lockers all appear to be locked."
 *   ROOM10D0 msg  5  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM10D0 msg  7  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "A card reader.You have to use the Blue Keycard and insert four digits tounlock t"
 *   ROOM10D1 msg  5  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM10D1 msg  7  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "A card reader.You have to use the Blue Keycard and insert four digits tounlock t"
 *   ROOM1100 msg  1  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "It's a control panel.A type of voice device is required to open the lock."
 *   ROOM1100 msg  2  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM1101 msg  1  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "It's a control panel.A type of voice device is required to open the lock."
 *   ROOM1101 msg  2  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM1140 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM1141 msg 12  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM11D0 msg  1  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The road is blocked and the knob is broken."
 *   ROOM11D1 msg  1  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The road is blocked and the knob is broken."
 *   ROOM11E0 msg  5  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM11E0 msg  7  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "A card reader.You have to use the Yellow Keycard and insert four digits tounlock"
 *   ROOM11E0 msg 10  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "If I had some kind of pliers maybe I could cut that wire and disable thesafety l"
 *   ROOM11E1 msg  5  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM11E1 msg  7  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "A card reader.You have to use the Yellow Keycard and insert four digits tounlock"
 *   ROOM11E1 msg 10  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "If I had some kind of pliers maybe I could cut that wire and disable thesafety l"
 *   ROOM1230 msg  5  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM1230 msg  7  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "A card reader.You have to use the Red Keycard and insert four digits tounlock th"
 *   ROOM1231 msg  5  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM1231 msg  7  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "A card reader.You have to use the Red Keycard and insert four digits tounlock th"
 *   ROOM2020 msg  2  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM2021 msg  2  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM2050 msg  1  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The water is blocking the entrance."
 *   ROOM2051 msg  1  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The water is blocking the entrance."
 *   ROOM2070 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM2071 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM20A0 msg  2  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM20A1 msg  2  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM3020 msg  3  B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)  "It's locked from the other side."
 *   ROOM3020 msg  4  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The car is locked, I can't get inside."
 *   ROOM3021 msg  0  B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)  "It's locked from the other side."
 *   ROOM3021 msg  1  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The car is locked, I can't get inside."
 *   ROOM3040 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM3040 msg  2  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The boxes are blocking the way. I must find a way to move them."
 *   ROOM3041 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM3041 msg  2  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The boxes are blocking the way. I must find a way to move them."
 *   ROOM3060 msg  7  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "Sherry: I'll unlock the back door from the other side!Just wait by the door, oka"
 *   ROOM3060 msg  9  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "For some reason the door isn't locked anymore..."
 *   ROOM3061 msg  7  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "Sherry: I'll unlock the back door from the other side!"
 *   ROOM3091 msg  7  B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)  "The door is latched shut and won't open."
 *   ROOM30C0 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM30C1 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM30D0 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The car is locked, I can't get inside."
 *   ROOM30D1 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "The car is locked, I can't get inside."
 *   ROOM4001 msg  0  B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)  "The door is locked. The text on the panel reads:Door is under emergency lockdown"
 *   ROOM4010 msg 40  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "There's nothing useful in this locker."
 *   ROOM4011 msg  4  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "There's nothing useful in this locker."
 *   ROOM4050 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "There's nothing useful in this locker."
 *   ROOM4051 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "There's nothing useful in this locker."
 *   ROOM5060 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "A locker used to hold containment suits.It's broken, and can't be opened."
 *   ROOM5060 msg  3  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "Security CabinetIt's locked and airtight."
 *   ROOM5061 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "A locker used to hold containment suits.It's broken, and can't be opened."
 *   ROOM5061 msg  3  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "Security CabinetIt's locked and airtight."
 *   ROOM5130 msg  1  B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)  "Shelter 01"The door is tightly locked.It doesn't look like it will open unless t"
 *   ROOM5130 msg  2  B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)  "Shelter 02"The door is tightly locked.It doesn't look like it will open unless t"
 *   ROOM5131 msg  1  B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)  "Shelter 01"The door is tightly locked.It doesn't look like it will open unless t"
 *   ROOM5131 msg  2  B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)  "Shelter 02"The door is tightly locked.It doesn't look like it will open unless t"
 *   ROOM6010 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 *   ROOM6011 msg  0  A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)       "You've opened the lock."
 */
