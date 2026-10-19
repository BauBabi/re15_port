/* GENERIERT von re15_port/tools/r39_johnruf/programme_bauen.py - NICHT HAND-EDITIEREN.
 * Runde 39 Spur P: SCD-Programme (Original-Bytes ROOM1041 sub11 + eingefuegte Zeilen, Listing beim
 * Werkzeugaufruf) und Text. Dossier: analysis/befunde_runde39/P_johnruf1041.md. */

#define RE15_R39P_ANKER_SUB11_OFF 0x021EAu  /* ROOM1041 sub11 */
#define RE15_R39P_ANKER_SUB11_LEN 84u
#define RE15_R39P_ANKER_SUB11_FNV 0x0EE013E3u

/* k_r39p_p_ruf1041: 94 Byte
 *   +0000 22 02 07 01                          ROOM1041 sub11 @0x021F8 Set Set(2,7)=1 Pad-Sperre (Szenen-Rahmen)
 *   +0004 22 01 1b 01                          ROOM1041 sub11 @0x021FC Set Set(1,0x1B)=1 Balken
 *   +0008 09 0a 1e 00                          Sleep 30 (Elza ist eben durch die Tuer; RE2 ROOM1060 sub05 +0x2E `09 0a 1e 00`, wie HAEMMERN39 der Spur J)
 *   +000C 5f 32                                Xa_vol(50) = RE2-Opcode 0x5F (Verteiler @0x800a74c8[0x5F] -> 0x80058090: `lbu v1,1(v0)` / `sw v1,0x5b50(at)`, pc += 2) - LEISE (NUTZER-VORGABE "die Hilferufe sollen leise sein"; Wert = RE2 ROOM11D0.RDT @0x01F84 `5f 32`)
 *   +000E 2b 37 00 00                          Message_on 55 = Sprach-Zeile help03.wav OHNE Text (Port; Form `2b id 00 00` wie sub11 @0x02224; der Stimmen-Riegel in op_message_on laesst den vorigen Ruf ausreden) - erster Ruf (an der Stelle der Original-Meldung @0x02200 `2b 03 00 00` "gunshot" = Geraeusch)
 *   +0012 2e 01 00                             ROOM1041 sub11 @0x02204 Work_set Work_set(1,0) Elza
 *   +0015 00                                   ROOM1041 sub11 @0x02207  Nop
 *   +0016 09 0a 19 00                          ROOM1041 sub11 @0x02208 Sleep Sleep 25
 *   +001A 40 00 09 20 3e b3 38 cd              Plc_dest Elza Modus 9 (Drehen), Bit 0x20 = Form sub11 @0x0220C `40 00 09 20 3c ab 04 42`; Ziel = Tuer zur Treppe ROOM1061 (-19650,-13000) - PORT-WAHL: der Ruf kommt von oben (1041 hat im Original keinen Ruf Johns)
 *   +0022 11 00 08 00                          ROOM1041 sub11 @0x02214 Do Do
 *   +0026 02                                   ROOM1041 sub11 @0x02218  Evt_next
 *   +0027 00                                   ROOM1041 sub11 @0x02219  Nop
 *   +0028 12 04                                ROOM1041 sub11 @0x0221A Edwhile Edwhile
 *   +002A 21 05 20 00                          ROOM1041 sub11 @0x0221C Ck Ck(5,0x20)==0 (bis die Drehung steht)
 *   +002E 09 0a 19 00                          ROOM1041 sub11 @0x02220 Sleep Sleep 25
 *   +0032 2b 36 00 00                          Message_on 54 = Sprach-Zeile help02.wav OHNE Text (Port; Form `2b id 00 00` wie sub11 @0x02224; der Stimmen-Riegel in op_message_on laesst den vorigen Ruf ausreden) - zweiter Ruf "Please help!" (leise; derselbe Ruf kommt in 1131 lauter, NUTZER-VORGABE "noch mal")
 *   +0036 09 0a 5a 00                          Sleep 90 (RE2 ROOM1060 sub05 +0x58 `09 0a 5a 00`; help02 1,96 s = 59 Bilder: Ruf ist aus, bevor Xa_vol wechselt)
 *   +003A 5f 78                                Xa_vol(120) = RE2-Opcode 0x5F (Verteiler @0x800a74c8[0x5F] -> 0x80058090: `lbu v1,1(v0)` / `sw v1,0x5b50(at)`, pc += 2) - zurueck auf den Standard vor Elzas Zeile (Form RE2 ROOM1010 sub22: Xa_vol(120) vor dem Dialog)
 *   +003C 2b 28 00 00                          ROOM1041 sub11 @0x02224 Message_on 2 -> Id 40 "Elza: Oh, no, this is John! ..." (NUTZER-VORGABE; Stimme synchro/STAGE1/room1041/main40.wav per Ordnerregel)
 *   +0040 09 0a 64 00                          ROOM1041 sub11 @0x02228 Sleep Sleep 100
 *   +0044 2b 3b 00 00                          Message_on 59 = Port-Warte-Zeile ohne Text: der Stimmen-Riegel laesst Elzas Zeile ausreden (main40.wav 6,0 s), dann Kartenhinweis ROOM1041 -> ROOM1141 (RE2 Opcode 0x84 @0x800591C4; Port-Form Spur H: Faden parkt, bis der Schirm wieder zu ist)
 *   +0048 22 09 61 01                          Set(9,97)=1 Panikphase (VERTRAG 1.1; Set-Form sub11 @0x021EA `22 03 ad 01`) - Musik und Folgen (Spuren Q/R) haengen an diesem Bit
 *   +004C 22 02 07 00                          ROOM1041 sub11 @0x0222C Set Set(2,7)=0
 *   +0050 22 01 1b 00                          ROOM1041 sub11 @0x02230 Set Set(1,0x1B)=0
 *   +0054 3c 01                                ROOM1041 sub11 @0x02234 Cut_auto Cut_auto 1
 *   +0056 2e 01 00                             ROOM1041 sub11 @0x02236 Work_set Work_set(1,0)
 *   +0059 00                                   ROOM1041 sub11 @0x02239  Nop
 *   +005A 42                                   ROOM1041 sub11 @0x0223A  Plc_ret
 *   +005B 00                                   ROOM1041 sub11 @0x0223B  Nop
 *   +005C 01 00                                ROOM1041 sub11 @0x0223C Evt_end Evt_end
 */
static const uint8_t k_r39p_p_ruf1041[94] = {
    0x22,0x02,0x07,0x01,0x22,0x01,0x1b,0x01,0x09,0x0a,0x1e,0x00,0x5f,0x32,0x2b,0x37,
    0x00,0x00,0x2e,0x01,0x00,0x00,0x09,0x0a,0x19,0x00,0x40,0x00,0x09,0x20,0x3e,0xb3,
    0x38,0xcd,0x11,0x00,0x08,0x00,0x02,0x00,0x12,0x04,0x21,0x05,0x20,0x00,0x09,0x0a,
    0x19,0x00,0x2b,0x36,0x00,0x00,0x09,0x0a,0x5a,0x00,0x5f,0x78,0x2b,0x28,0x00,0x00,
    0x09,0x0a,0x64,0x00,0x2b,0x3b,0x00,0x00,0x22,0x09,0x61,0x01,0x22,0x02,0x07,0x00,
    0x22,0x01,0x1b,0x00,0x3c,0x01,0x2e,0x01,0x00,0x00,0x42,0x00,0x01,0x00,
};

/* k_r39p_p_ruf1131: 18 Byte
 *   +0000 09 0a 1e 00                          Sleep 30 (Elza ist eben durch die Tuer; RE2 ROOM1060 sub05 +0x2E)
 *   +0004 5f 64                                Xa_vol(100) = RE2-Opcode 0x5F (Verteiler @0x800a74c8[0x5F] -> 0x80058090: `lbu v1,1(v0)` / `sw v1,0x5b50(at)`, pc += 2) - "schon etwas lauter" (NUTZER-VORGABE; Wert = RE2 ROOM1020.RDT @0x01960 `5f 64`, dieselbe XA-Stimme 0x0B wie ROOM11D0 @0x01F7A mit 70; Wahl gemessen, Dossier 4.2)
 *   +0006 2b 36 00 00                          Message_on 54 = Sprach-Zeile help02.wav OHNE Text (Port; Form `2b id 00 00` wie sub11 @0x02224; der Stimmen-Riegel in op_message_on laesst den vorigen Ruf ausreden) - "das please help von John auch noch mal"
 *   +000A 09 0a 5a 00                          Sleep 90 (RE2 ROOM1060 sub05 +0x58; der Ruf ist vor dem Zuruecksetzen aus)
 *   +000E 5f 78                                Xa_vol(120) = RE2-Opcode 0x5F (Verteiler @0x800a74c8[0x5F] -> 0x80058090: `lbu v1,1(v0)` / `sw v1,0x5b50(at)`, pc += 2) - zurueck auf den Standard (Form RE2 ROOM1010 sub22)
 *   +0010 01 00                                Evt_end (keine Szene: Elza laeuft waehrenddessen weiter)
 */
static const uint8_t k_r39p_p_ruf1131[18] = {
    0x09,0x0a,0x1e,0x00,0x5f,0x64,0x2b,0x36,0x00,0x00,0x09,0x0a,0x5a,0x00,0x5f,0x78,
    0x01,0x00,
};

/* ROOM1041 msg 40: NUTZER-VORGABE Nachtrag 10 ("Oh, no, this is John! He is in Trouble, I have to help him!"; Schreibung nach Nachtrag 6 "trouble" klein = derselbe Wortlaut wie Spur J msg 50); Form ROOM1041 msg 2 @0x022DC (Elza = Farbe 1 `05 01`), Zeilenumbruch `08`  [Zeilenbreiten 167, 235 px] */
static const uint8_t k_r39p_msg40[75] = {
    0x04,0x00,0x05,0x01,0x21,0x48,0x56,0x3d,0x16,0x05,0x00,0x00,0x2b,0x44,0x18,0x00,
    0x4a,0x4b,0x18,0x00,0x50,0x44,0x45,0x4f,0x00,0x45,0x4f,0x00,0x26,0x4b,0x44,0x4a,
    0x1a,0x08,0x24,0x41,0x00,0x45,0x4f,0x00,0x45,0x4a,0x00,0x50,0x4e,0x4b,0x51,0x3e,
    0x48,0x41,0x18,0x00,0x25,0x00,0x44,0x3d,0x52,0x41,0x00,0x50,0x4b,0x00,0x44,0x41,
    0x48,0x4c,0x00,0x44,0x45,0x49,0x1a,0x04,0x01,0x01,0x63,
};

