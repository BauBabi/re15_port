/* GENERIERT von re15_port/tools/r39_parkplatz/programme_bauen.py - NICHT HAND-EDITIEREN.
 * Runde 39 Spur O: Tuersperre ROOM11C0 waehrend des Gorilla-Kampfes (Programme + Text + Anker).
 * Dossier: analysis/befunde_runde39/O_parkplatz11c0.md. */

#define RE15_R39O_ANKER_MAIN00_TUER_OFF 0x01712u
#define RE15_R39O_ANKER_MAIN00_TUER_LEN 32u
#define RE15_R39O_ANKER_MAIN00_TUER_FNV 0xD9E816BCu
#define RE15_R39O_ANKER_SUB00_03_OFF 0x01768u
#define RE15_R39O_ANKER_SUB00_03_LEN 1188u
#define RE15_R39O_ANKER_SUB00_03_FNV 0x501E77BEu

/* k_r39o_p_szene: 14 Byte
 *   +0000 46 00 01 31 28 00 ff ff 00 00  Aot_reset Slot 0 (Tuer -> 11B0) -> MESSAGE msg 40, Maske 0xFFFF (Form ROOM11B0 sub01 @0x01216 `46 01 01 31 0f 00 ff ff 00 00`; RE2 ROOM7040 sub06 @0x0185A)
 *   +000A 18 02                          Gosub sub02 = die Ada-Szene UNVERAENDERT aus dem RDT (@0x0184E..@0x01B00; Pad-Sperre @0x01856 in derselben Runde, Rueckgabe @0x01ACE)
 *   +000C 01 00                          Evt_end
 */
static const uint8_t k_r39o_p_szene[14] = {
    0x46,0x00,0x01,0x31,0x28,0x00,0xff,0xff,0x00,0x00,0x18,0x02,0x01,0x00,
};

/* k_r39o_p_ende: 14 Byte
 *   +0000 46 00 02 31 30 9c 00 00 0d 9d  Aot_reset Slot 0 -> DOOR, Nutzlast = main00 @0x01712+14 Ziel (-25552,0,-25331) (Form ROOM1141 sub05 @0x00FDC `46 01 02 31 ..`; RE2 ROOM7040 sub07 @0x01ACC) — VOR sub03 @0x01C08 Aot_on(0)
 *   +000A 18 03                          Gosub sub03 = die Kampf-Ende-Szene UNVERAENDERT aus dem RDT (@0x01B02..@0x01C0A, msg 7/8/9, Aot_on(0) @0x01C08 -> ROOM11B0)
 *   +000C 01 00                          Evt_end
 */
static const uint8_t k_r39o_p_ende[14] = {
    0x46,0x00,0x02,0x31,0x30,0x9c,0x00,0x00,0x0d,0x9d,0x18,0x03,0x01,0x00,
};

/* ROOM11C0 msg 40: NUTZER-VORGABE "I cannot leave Ada alone here with those monsters!" (AUFTRAG.md Nachtrag 9), Erzaehlform `04 02 .. 08 .. 01 00` wie
 * ROOM1090 msg 7 @0x28E6 / Runde 38 F 1141 msg 41  [Zeilenbreiten 202, 139 px <= 271] */
static const uint8_t k_r39o_msg_sperre[54] = {
    0x04,0x02,0x25,0x00,0x3f,0x3d,0x4a,0x4a,0x4b,0x50,0x00,0x48,0x41,0x3d,0x52,0x41,
    0x00,0x1d,0x40,0x3d,0x00,0x3d,0x48,0x4b,0x4a,0x41,0x00,0x44,0x41,0x4e,0x41,0x08,
    0x53,0x45,0x50,0x44,0x00,0x50,0x44,0x4b,0x4f,0x41,0x00,0x49,0x4b,0x4a,0x4f,0x50,
    0x41,0x4e,0x4f,0x1a,0x01,0x00,
};
