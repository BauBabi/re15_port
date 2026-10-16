/* ERZEUGT von tools/r36_j/szene11b0_bauen.py - NICHT von Hand aendern.
 * Runde 36 Spur J: Erweiterung der ROOM11B0-Szene sub06 + Tuer-zu-Programm + drei Nachrichten.
 * Herleitung je Zeile im Generator-Kopf und in analysis/befunde_runde36/J_szenenleon.md §2. */
#define RE15_SZL_PROG_LEN      294
#define RE15_SZL_TUERPROG_LEN  80
#define RE15_SZL_OFF_ABSCHLIESSEN   0x008A
#define RE15_SZL_OFF_LAUF_HIN       0x001C
#define RE15_SZL_OFF_LAUF_ZURUECK   0x00CA
#define RE15_SZL_OFF_RUECKGABE      0x0122
#define RE15_SZL_OFF_SCHLIESSEN     0x005C
#define RE15_SZL_OFF_SCHLOSS_TON    0x0092
#define RE15_SZL_OFF_TUER_BILD_ZU   0x0064
#define RE15_SZL_OFF_TUER_TON       0x006E
#define RE15_SZL_OFF_TUER_ZU        0x00AE
#define RE15_SZL_OFF_ZEILE          0x010A
/*  +0x0000  22 09 54 01                                                  Set(9,84)=1 als ERSTES (Einmal-Riegel) */
/*  +0x0004  2e 02 00 00                                                  Work_set(2,0)+Nop Marvin */
/*  +0x0008  2b 14 00 00                                                  Message_on 20  Marvin: "But, hold on..."  (NUTZER-VORGABE Runde 38) */
/*  +0x000C  3f 00 0f 00                                                  Plc_motion(0,15) Geste 'Hey!' (sub06 @0x014F6) */
/*  +0x0010  09 0a 28 00                                                  Sleep 40  (Zeilentakt @0x014FA) */
/*  +0x0014  3f 00 17 00                                                  Plc_motion(0,23) Abschluss (sub06 @0x01506) */
/*  +0x0018  09 0a 14 00                                                  Sleep 20  (Zeilentakt @0x0150A) */
/*  +0x001C  40 00 05 02 e2 04 bd ab                                      Plc_dest Modus 5 Marvin -> (1250,-21571) */
/*  +0x0024  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x0026  40 00 05 02 d8 f5 bd ab                                      Plc_dest Modus 5 Marvin -> (-2600,-21571) */
/*  +0x002E  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x0030  29 0a                                                        Cut_chg 10  Weg durch die Werkstatt (Cut 4 mit Licht) */
/*  +0x0032  40 00 05 02 29 e8 78 9d                                      Plc_dest Modus 5 Marvin -> (-6103,-25224) */
/*  +0x003A  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x003C  40 00 05 02 25 bf 78 9d                                      Plc_dest Modus 5 Marvin -> (-16603,-25224) */
/*  +0x0044  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x0046  29 08                                                        Cut_chg 8  die Werkstatt-Tuer (Cut 2 mit Licht) */
/*  +0x0048  40 00 05 02 84 ae 9a c0                                      Plc_dest Modus 5 Marvin -> (-20860,-16230) */
/*  +0x0050  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x0052  40 00 09 02 89 a6 9a c0                                      Plc_dest Modus 9 Marvin dreht zur Tuer (-22903,-16230) */
/*  +0x005A  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x005C  3f 00 14 00                                                  Plc_motion(0,20) Marvin greift zur offenen Tuer (PORT-WAHL Clip 20, Form sub06 @0x014F6) */
/*  +0x0060  09 0a 0f 00                                                  Sleep 15  bis zum Hand-Maximum Bild 15 (gesten_arme.py rec1 Clip 20) */
/*  +0x0064  4b 00 07 00                                                  Cut_replace(0,7)+Nop  = sub00 @0x01184 noch einmal: Tausch zurueck, Zonen zeigen Bild 0 (zu) */
/*  +0x0068  4b 02 08 00                                                  Cut_replace(2,8)+Nop  = sub00 @0x01188 noch einmal: Tausch zurueck, Zonen zeigen Bild 2 (zu) */
/*  +0x006C  29 02                                                        Cut_chg 2  dieselbe Kamera (RDT @0x00A0 == @0x0160), Tuer GESCHLOSSEN gemalt */
/*  +0x006E  36 0e 01 00 00 00 00 00 00 00 00 00                          Se_on Port-Bank 0x0E Satz 1 = RE2 DOOR1A Door_exit (@0x800141f0/@0x800141f4) */
/*  +0x007A  09 0a 0a 00                                                  Sleep 10  Rest von Clip 20 (Bild 15..24) */
/*  +0x007E  3f 00 14 00                                                  Plc_motion(0,20) noch einmal */
/*  +0x0082  43 00 80 00                                                  Plc_flg(0,0x80) rueckwaerts (ROOM11C0 @0x018E4) */
/*  +0x0086  09 0a 19 00                                                  Sleep 25  Clip 20 rueckwaerts bis Bild 0 */
/*  +0x008A  3f 00 0f 00                                                  Plc_motion(0,15) = RE2 Kendo Clip 0 (ROOM1010 sub09 @0x01C18, bytegleich) */
/*  +0x008E  09 0a 14 00                                                  Sleep 20  (@0x01C1C) */
/*  +0x0092  36 3a 0a 00 00 00 00 00 00 00 00 00                          Se_on Port-Bank 0x3A Satz 0x0A = RE2 ROOM1010 snd0 (@0x01C20) */
/*  +0x009E  09 0a 0a 00                                                  Sleep 10  (@0x01C2C) */
/*  +0x00A2  3f 00 0f 00                                                  Plc_motion(0,15) noch einmal */
/*  +0x00A6  43 00 80 00                                                  Plc_flg(0,0x80) rueckwaerts (ROOM11C0 @0x018E4) */
/*  +0x00AA  09 0a 23 00                                                  Sleep 35  (@0x01C34..@0x01C44: 5 + 5 + 25) */
/*  +0x00AE  37 03 0c ff                                                  Sca_id_set(3,0x0C)=0xff  Durchgangszelle wieder solid (@0x00EC8 u0=ff) */
/*  +0x00B2  39 03 0c 03                                                  Sca_floor_set(3,0x0C)=0x03  Band 0 (@0x00EC8 fl=03) */
/*  +0x00B6  2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06 16 00 ff ff 00 00  Aot_set Platz 8 Text msg 22 (ROOM11B1 sub00 @0x0112A, msg 12 -> 22) */
/*  +0x00CA  40 00 05 02 25 bf 78 9d                                      Plc_dest Modus 5 Marvin -> (-16603,-25224) */
/*  +0x00D2  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x00D4  29 04                                                        Cut_chg 4  zurueck durch die Werkstatt (Kamera von Cut 10, Tuer jetzt zu) */
/*  +0x00D6  40 00 05 02 29 e8 78 9d                                      Plc_dest Modus 5 Marvin -> (-6103,-25224) */
/*  +0x00DE  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x00E0  40 00 05 02 d8 f5 bd ab                                      Plc_dest Modus 5 Marvin -> (-2600,-21571) */
/*  +0x00E8  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x00EA  29 06                                                        Cut_chg 6  zurueck am Container (Cut der Original-Szene @0x016C2) */
/*  +0x00EC  40 00 05 02 e2 04 bd ab                                      Plc_dest Modus 5 Marvin -> (1250,-21571) */
/*  +0x00F4  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x00F6  40 00 05 02 4c 04 e1 b1                                      Plc_dest Modus 5 Marvin -> (1100,-19999) */
/*  +0x00FE  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x0100  40 00 09 02 f3 f9 64 b2                                      Plc_dest Modus 9 Marvin dreht zu Leon (sub06 @0x0174C) */
/*  +0x0108  18 0b                                                        Gosub sub11 (warten auf Ck(5,2)) */
/*  +0x010A  2b 15 00 00                                                  Message_on 21  Marvin: "I just want to make sure ..."  (NUTZER-VORGABE) */
/*  +0x010E  3f 00 10 00                                                  Plc_motion(0,16) Geste (sub06 @0x014FE) */
/*  +0x0112  09 0a 28 00                                                  Sleep 40  (Zeilentakt @0x014FA) */
/*  +0x0116  09 0a 32 00                                                  Sleep 50  (Zeilentakt @0x01502, Geste gehalten) */
/*  +0x011A  3f 00 17 00                                                  Plc_motion(0,23) Abschluss (sub06 @0x01506) */
/*  +0x011E  09 0a 14 00                                                  Sleep 20  (Zeilentakt @0x0150A) */
/*  +0x0122  47 05                                                        Aot_on 5  = sub06 @0x017EA (Gully -> ROOM1260) */
/*  +0x0124  01 00                                                        Evt_end  = sub06 @0x017EC */
/* Ereignis 22: die Erweiterung (im Faden von sub06 ab @0x017EA) */
static const uint8_t k_szl_prog[294] = {
    0x22, 0x09, 0x54, 0x01, 0x2e, 0x02, 0x00, 0x00, 0x2b, 0x14, 0x00, 0x00, 0x3f, 0x00, 0x0f, 0x00,
    0x09, 0x0a, 0x28, 0x00, 0x3f, 0x00, 0x17, 0x00, 0x09, 0x0a, 0x14, 0x00, 0x40, 0x00, 0x05, 0x02,
    0xe2, 0x04, 0xbd, 0xab, 0x18, 0x0b, 0x40, 0x00, 0x05, 0x02, 0xd8, 0xf5, 0xbd, 0xab, 0x18, 0x0b,
    0x29, 0x0a, 0x40, 0x00, 0x05, 0x02, 0x29, 0xe8, 0x78, 0x9d, 0x18, 0x0b, 0x40, 0x00, 0x05, 0x02,
    0x25, 0xbf, 0x78, 0x9d, 0x18, 0x0b, 0x29, 0x08, 0x40, 0x00, 0x05, 0x02, 0x84, 0xae, 0x9a, 0xc0,
    0x18, 0x0b, 0x40, 0x00, 0x09, 0x02, 0x89, 0xa6, 0x9a, 0xc0, 0x18, 0x0b, 0x3f, 0x00, 0x14, 0x00,
    0x09, 0x0a, 0x0f, 0x00, 0x4b, 0x00, 0x07, 0x00, 0x4b, 0x02, 0x08, 0x00, 0x29, 0x02, 0x36, 0x0e,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0a, 0x0a, 0x00, 0x3f, 0x00,
    0x14, 0x00, 0x43, 0x00, 0x80, 0x00, 0x09, 0x0a, 0x19, 0x00, 0x3f, 0x00, 0x0f, 0x00, 0x09, 0x0a,
    0x14, 0x00, 0x36, 0x3a, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x0a,
    0x0a, 0x00, 0x3f, 0x00, 0x0f, 0x00, 0x43, 0x00, 0x80, 0x00, 0x09, 0x0a, 0x23, 0x00, 0x37, 0x03,
    0x0c, 0xff, 0x39, 0x03, 0x0c, 0x03, 0x2c, 0x08, 0x01, 0x31, 0x00, 0x00, 0x80, 0xa8, 0xd0, 0xbc,
    0xb0, 0x04, 0xa4, 0x06, 0x16, 0x00, 0xff, 0xff, 0x00, 0x00, 0x40, 0x00, 0x05, 0x02, 0x25, 0xbf,
    0x78, 0x9d, 0x18, 0x0b, 0x29, 0x04, 0x40, 0x00, 0x05, 0x02, 0x29, 0xe8, 0x78, 0x9d, 0x18, 0x0b,
    0x40, 0x00, 0x05, 0x02, 0xd8, 0xf5, 0xbd, 0xab, 0x18, 0x0b, 0x29, 0x06, 0x40, 0x00, 0x05, 0x02,
    0xe2, 0x04, 0xbd, 0xab, 0x18, 0x0b, 0x40, 0x00, 0x05, 0x02, 0x4c, 0x04, 0xe1, 0xb1, 0x18, 0x0b,
    0x40, 0x00, 0x09, 0x02, 0xf3, 0xf9, 0x64, 0xb2, 0x18, 0x0b, 0x2b, 0x15, 0x00, 0x00, 0x3f, 0x00,
    0x10, 0x00, 0x09, 0x0a, 0x28, 0x00, 0x09, 0x0a, 0x32, 0x00, 0x3f, 0x00, 0x17, 0x00, 0x09, 0x0a,
    0x14, 0x00, 0x47, 0x05, 0x01, 0x00,
};
/*  +0x0000  37 03 0c ff                                                  Sca_id_set(3,0x0C)=0xff  Durchgangszelle wieder solid (@0x00EC8 u0=ff) */
/*  +0x0004  39 03 0c 03                                                  Sca_floor_set(3,0x0C)=0x03  Band 0 (@0x00EC8 fl=03) */
/*  +0x0008  2c 08 01 31 00 00 80 a8 d0 bc b0 04 a4 06 16 00 ff ff 00 00  Aot_set Platz 8 Text msg 22 (ROOM11B1 sub00 @0x0112A, msg 12 -> 22) */
/*  +0x001C  06 00 2e 00                                                  Strom-Zweig wie sub00 @0x0116C (Ifel_ck, Laenge 0x2E) */
/*  +0x0020  21 04 06 01                                                  Ck(4,6)==1  Strom (sub00 @0x01170) */
/*  +0x0024  4b 00 07 00                                                  Cut_replace(0,7)+Nop  = sub00 @0x01184 noch einmal: Tausch zurueck, Zonen zeigen Bild 0 (zu) */
/*  +0x0028  4b 02 08 00                                                  Cut_replace(2,8)+Nop  = sub00 @0x01188 noch einmal: Tausch zurueck, Zonen zeigen Bild 2 (zu) */
/*  +0x002C  06 00 0c 00                                                  steht der Cut auf 7 (offen) -> 0 (zu) (Ifel_ck, Laenge 0x0C) */
/*  +0x0030  23 00 0a 00 07 00                                            Cmp(work_vars[0x0A]==7)  angezeigter Cut (sub00 @0x01178, LAB_800402a0 @0x800402fc) */
/*  +0x0036  29 00                                                        Cut_chg 0  (sub00 @0x0117E umgekehrt) */
/*  +0x0038  3c 01                                                        Cut_auto 1  (sub00 @0x01180) */
/*  +0x003A  08 00                                                        Endif */
/*  +0x003C  06 00 0c 00                                                  steht der Cut auf 8 (offen) -> 2 (zu) (Ifel_ck, Laenge 0x0C) */
/*  +0x0040  23 00 0a 00 08 00                                            Cmp(work_vars[0x0A]==8)  angezeigter Cut (sub00 @0x01178, LAB_800402a0 @0x800402fc) */
/*  +0x0046  29 02                                                        Cut_chg 2  (sub00 @0x0117E umgekehrt) */
/*  +0x0048  3c 01                                                        Cut_auto 1  (sub00 @0x01180) */
/*  +0x004A  08 00                                                        Endif */
/*  +0x004C  08 00                                                        Endif */
/*  +0x004E  01 00                                                        Evt_end */
/* Ereignis 23: Tuer zu beim Raumaufbau, solange (9,84)=1 */
static const uint8_t k_szl_tuerprog[80] = {
    0x37, 0x03, 0x0c, 0xff, 0x39, 0x03, 0x0c, 0x03, 0x2c, 0x08, 0x01, 0x31, 0x00, 0x00, 0x80, 0xa8,
    0xd0, 0xbc, 0xb0, 0x04, 0xa4, 0x06, 0x16, 0x00, 0xff, 0xff, 0x00, 0x00, 0x06, 0x00, 0x2e, 0x00,
    0x21, 0x04, 0x06, 0x01, 0x4b, 0x00, 0x07, 0x00, 0x4b, 0x02, 0x08, 0x00, 0x06, 0x00, 0x0c, 0x00,
    0x23, 0x00, 0x0a, 0x00, 0x07, 0x00, 0x29, 0x00, 0x3c, 0x01, 0x08, 0x00, 0x06, 0x00, 0x0c, 0x00,
    0x23, 0x00, 0x0a, 0x00, 0x08, 0x00, 0x29, 0x02, 0x3c, 0x01, 0x08, 0x00, 0x08, 0x00, 0x01, 0x00,
};
/* msg 20 Marvin: But, hold on...  (NUTZER-VORGABE Runde 38, Form ROOM11B0 msg 12 @0x1BD0, Auslassung wie msg 4 @0x19CD `57 57 57`) */
static const uint8_t k_szl_msg20[33] = {
    0x04, 0x00, 0x05, 0x07, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x1e, 0x51,
    0x50, 0x18, 0x00, 0x44, 0x4b, 0x48, 0x40, 0x00, 0x4b, 0x4a, 0x57, 0x57, 0x57, 0x04, 0x01, 0x01,
    0x5f,
};
/* msg 21 Marvin: I just want to make sure / we don't have any unwanted followers. */
static const uint8_t k_szl_msg21[80] = {
    0x04, 0x00, 0x05, 0x07, 0x29, 0x3d, 0x4e, 0x52, 0x45, 0x4a, 0x16, 0x05, 0x00, 0x00, 0x25, 0x00,
    0x46, 0x51, 0x4f, 0x50, 0x00, 0x53, 0x3d, 0x4a, 0x50, 0x00, 0x50, 0x4b, 0x00, 0x49, 0x3d, 0x47,
    0x41, 0x00, 0x4f, 0x51, 0x4e, 0x41, 0x08, 0x53, 0x41, 0x00, 0x40, 0x4b, 0x4a, 0x3a, 0x50, 0x00,
    0x44, 0x3d, 0x52, 0x41, 0x00, 0x3d, 0x4a, 0x55, 0x00, 0x51, 0x4a, 0x53, 0x3d, 0x4a, 0x50, 0x41,
    0x40, 0x00, 0x42, 0x4b, 0x48, 0x48, 0x4b, 0x53, 0x41, 0x4e, 0x4f, 0x57, 0x04, 0x01, 0x01, 0x5f,
};
/* msg 22 = ROOM11B1 msg 12 @0x179F "The door is locked." (Bytes 1:1) */
static const uint8_t k_szl_msg22[23] = {
    0x04, 0x02, 0x30, 0x44, 0x41, 0x00, 0x40, 0x4b, 0x4b, 0x4e, 0x00, 0x45, 0x4f, 0x00, 0x48, 0x4b,
    0x3f, 0x47, 0x41, 0x40, 0x57, 0x01, 0x00,
};
